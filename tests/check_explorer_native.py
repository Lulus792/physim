"""Independent UIA/AT-SPI values, long Unicode edits and native picker close."""
import argparse,hashlib,json,os,subprocess,sys,time
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):parser.add_argument('--'+name,type=Path,required=True)
parser.add_argument('--probe',type=Path);parser.add_argument('--inner',action='store_true');args=parser.parse_args()
args.app=args.app.resolve();args.work=args.work.resolve();args.work.mkdir(parents=True,exist_ok=True)
if sys.platform=='linux' and not args.inner:
    raise SystemExit(subprocess.call(['dbus-run-session','--',sys.executable,str(Path(__file__).resolve()),'--app',str(args.app),'--work',str(args.work),'--inner'],env=dict(os.environ,GSETTINGS_BACKEND='memory')))
if sys.platform=='linux':
    import dbus
    from gi.repository import GLib
    session=dbus.SessionBus();status=session.get_object('org.a11y.Bus','/org/a11y/bus')
    dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
    os.environ['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress())
    import pyatspi
else:assert sys.platform=='win32' and args.probe and args.probe.is_file()
def digest(value):return hashlib.sha256(value.encode('utf8')).hexdigest()
binary=hashlib.sha256(args.app.read_bytes()).hexdigest();cases=[]
for size in ('normal','large'):
    work=args.work/size;work.mkdir();app_work=work/'reader'
    command=[str(args.app),'--workspace-state-test',str(app_work),'explorer-a-reader'+('-large' if size=='large' else '')]
    stdout=(work/'app.stdout').open('wb');stderr=(work/'app.stderr').open('wb')
    process=subprocess.Popen(command,stdout=stdout,stderr=stderr,env=dict(os.environ,PHYSIM_TEST_TRACE='1',NO_AT_BRIDGE='0'))
    record=dict(size=size,command=command,passed=False,observations=[]);cases.append(record)
    def output():return (work/'app.stdout').read_text(encoding='utf8',errors='replace')
    def wait(fn,label):
        end=time.monotonic()+30;last=None
        while time.monotonic()<end:
            if sys.platform=='linux':
                context=GLib.MainContext.default()
                while context.pending():context.iteration(False)
            try:
                value=fn()
                if value:return value
            except Exception as error:last=repr(error)
            if process.poll() is not None:raise RuntimeError('App stopped before '+label+': '+str(process.returncode))
            time.sleep(.03)
        raise RuntimeError('Timeout '+label+': '+str(last))
    def tree():
        for app in pyatspi.Registry.getDesktop(0):
            if app.name!='Physim' or app.get_process_id()!=process.pid:continue
            found=[];stack=[(node,0) for node in app]
            while stack:
                node,depth=stack.pop();found.append(node)
                if depth<10:stack.extend((child,depth+1) for child in node)
            return found
        return []
    def named(label):return next((node for node in tree() if node.name==label),None)
    def probe(action,label,value=None,keep=True):
        cmd=[str(args.probe.resolve()),str(process.pid),action,label]
        if value is not None:cmd.append(value)
        result=subprocess.run(cmd,capture_output=True,timeout=15)
        observation=dict(action=action,label=label,exit_code=result.returncode,stdout=result.stdout.decode('utf8',errors='replace'),stderr=result.stderr.decode('utf8',errors='replace'))
        if value is not None:observation.update(argument_bytes=len(value.encode('utf8')),argument_sha256=digest(value))
        if keep:record['observations'].append(observation)
        return result.returncode==0,observation
    def read_value(value):
        if sys.platform=='linux':return named('Ort').queryText().getText(0,-1)==value
        path=work/'expected.txt';path.write_text(value,encoding='utf8')
        native='\\\\?\\'+str(path.resolve())
        okay,observation=probe('value-file','Ort',native,False)
        if okay:record['observations'].append(observation)
        return okay
    def set_value(value,accepted=True):
        if sys.platform=='linux':
            result=bool(named('Ort').queryEditableText().setTextContents(value))
            record['observations'].append(dict(action='SetTextContents',accepted=result,utf8_bytes=len(value.encode('utf8')),sha256=digest(value)))
        else:result,_=probe('edit-value','Ort',value)
        assert result==accepted,'Native value acceptance differs'
        if accepted:wait(lambda:read_value(value),'full native value')
    try:
        wait(lambda:'EXPLORER READER READY' in output(),'reader ready')
        initial=(app_work/'files').resolve().as_posix();wait(lambda:read_value(initial),'initial4096-capacity path field')
        prefix='D:/λ🌍/';value=prefix+'p'*(4095-len(prefix.encode('utf8')))
        assert len(value.encode('utf8'))==4095
        set_value(value)
        if sys.platform=='linux':
            assert named('Ort').queryText().characterCount==len(value)
            medium=prefix+'p'*(3000-len(prefix.encode('utf8')));set_value(medium)
            assert named('Ort').queryEditableText().insertText(1020,'Ω',2)
            expected=medium[:1020]+'Ω'+medium[1020:];wait(lambda:read_value(expected),'long native insertion')
            assert named('Ort').queryEditableText().deleteText(1020,1021)
            wait(lambda:read_value(medium),'long native deletion');record['observations'].append(dict(action='InsertText/DeleteText',unicode_offset=1020,retained_utf8_bytes=3000))
            set_value(value)
        set_value(value+'p',False);wait(lambda:read_value(value),'rejected value preserves current text')
        set_value(initial)
        if sys.platform=='linux':
            assert named('Dateiauswahl schließen').queryAction().doAction(0)
            wait(lambda:named('Dateiauswahl schließen') is None,'picker closed through native action')
        else:
            assert probe('invoke','Dateiauswahl schließen')[0]
            wait(lambda:probe('absent','Dateiauswahl schließen',keep=False)[0],'picker closed through native action')
        (app_work/'explorer-reader-complete').write_text('native clients completed\n',encoding='utf8')
        code=process.wait(timeout=20)
        assert code==0 and 'EXPLORER READER SELF-TEST: PASSED' in output()
        record['passed']=True;record['utf8_limit_verified']=4095;record['oversize_rejected']=4096
    except Exception as error:
        record['error']=str(error);raise
    finally:
        if process.poll() is None:
            process.terminate()
            try:process.wait(timeout=5)
            except subprocess.TimeoutExpired:process.kill();process.wait()
        stdout.close();stderr.close()
        (args.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,platform=sys.platform,physical_keyboard_focus_proven=False,cases=cases),ensure_ascii=False,indent=2)+'\n',encoding='utf8')
    assert hashlib.sha256(args.app.read_bytes()).hexdigest()==binary
print('Explorer native clients: normal/large path field, full4095-byte Unicode value,4096-byte rejection and native close passed')
