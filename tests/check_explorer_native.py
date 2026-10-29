"""Independent UIA/AT-SPI values, long Unicode edits and native picker close."""
import argparse,hashlib,json,os,subprocess,sys,time
from pathlib import Path
parser=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):parser.add_argument('--'+name,type=Path,required=True)
parser.add_argument('--probe',type=Path);parser.add_argument('--inner',action='store_true');parser.add_argument('--inline',action='store_true');parser.add_argument('--preview',action='store_true');args=parser.parse_args()
args.app=args.app.resolve();args.work=args.work.resolve();args.work.mkdir(parents=True,exist_ok=True)
if sys.platform=='linux' and not args.inner:
    raise SystemExit(subprocess.call(['dbus-run-session','--',sys.executable,str(Path(__file__).resolve()),'--app',str(args.app),'--work',str(args.work),'--inner',*(['--inline'] if args.inline else []),*(['--preview'] if args.preview else [])],env=dict(os.environ,GSETTINGS_BACKEND='memory')))
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
    command=[str(args.app),'--workspace-state-test',str(app_work),('explorer-a-preview-reader' if args.preview else 'explorer-a-reader'+('-inline' if args.inline else ''))+('-large' if size=='large' else '')]
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
        wait(lambda:('EXPLORER PREVIEW READER READY' if args.preview else 'EXPLORER READER READY') in output(),'reader ready')
        if args.preview:
            source=app_work/'files/main.c';expected=source.read_text(encoding='utf8');raw_hash=digest(expected)
            if sys.platform=='linux':
                field=named('Dateivorschau');text=field.queryText();assert text.characterCount==len(expected) and text.getText(0,-1)==expected
                assert not field.queryEditableText().setTextContents('must remain readonly')
                assert text.getText(0,-1)==expected
                assert not field.getState().contains(pyatspi.STATE_FOCUSED)
                assert text.scrollSubstringTo(len(expected)-1,len(expected),pyatspi.SCROLL_BOTTOM_RIGHT)
                wait(lambda:all(v>0 for v in text.getCharacterExtents(len(expected)-1,pyatspi.WINDOW_COORDS)[2:]),'last wrapped glyph visible')
                assert text.setSelection(0,0,len(expected)) if text.getNSelections() else text.addSelection(0,len(expected))
                wait(lambda:text.getNSelections()==1 and text.getText(*text.getSelection(0))==expected,'full original selection')
                record['observations'].append(dict(action='readonly/select/scroll',characters=len(expected),sha256=raw_hash,last_glyph=text.getCharacterExtents(len(expected)-1,pyatspi.WINDOW_COORDS)))
            else:
                native='\\\\?\\'+str(source.resolve())
                wait(lambda:probe('readonly-file','Dateivorschau',native,False)[0],'full readonly preview')
                assert probe('scroll-text-end','Dateivorschau','ENDE λ🌍')[0]
                assert probe('focus','Dateivorschau')[0]
                assert probe('select-text','Dateivorschau',expected)[0]
                wait(lambda:probe('readonly-selected-file','Dateivorschau',native,False)[0],'full original selection')
                record['observations'].append(dict(action='readonly/select/scroll',utf8_bytes=len(expected.encode('utf8')),sha256=raw_hash))
            assert source.read_text(encoding='utf8')==expected;record['preview_bytes']=len(expected.encode('utf8'));record['preview_sha256']=raw_hash
        else:
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
            record['utf8_limit_verified']=4095;record['oversize_rejected']=4096
        close_name='Zur Werkbank →' if args.inline or args.preview else 'Dateiauswahl schließen'
        if sys.platform=='linux':
            assert named(close_name).queryAction().doAction(0)
            wait(lambda:named(close_name) is None,'browser left through native action')
        else:
            assert probe('invoke',close_name)[0]
            wait(lambda:probe('absent',close_name,keep=False)[0],'browser left through native action')
        (app_work/'explorer-reader-complete').write_text('native clients completed\n',encoding='utf8')
        code=process.wait(timeout=20)
        assert code==0 and ('EXPLORER PREVIEW READER SELF-TEST: PASSED' if args.preview else 'EXPLORER READER SELF-TEST: PASSED') in output()
        record['passed']=True
    except Exception as error:
        record['error']=str(error);raise
    finally:
        if process.poll() is None:
            process.terminate()
            try:process.wait(timeout=5)
            except subprocess.TimeoutExpired:process.kill();process.wait()
        stdout.close();stderr.close()
        (args.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,platform=sys.platform,embedded=args.inline or args.preview,preview=args.preview,physical_keyboard_focus_proven=False,cases=cases),ensure_ascii=False,indent=2)+'\n',encoding='utf8')
    assert hashlib.sha256(args.app.read_bytes()).hexdigest()==binary
print('Explorer native clients: normal/large '+('readonly wrapped preview, full original selection, final glyph scroll and native close passed' if args.preview else 'path field, full4095-byte Unicode value,4096-byte rejection and native close passed'))
