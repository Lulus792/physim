"""Independent native project-card selection, reveal and modal isolation."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import time

NAMES=('Pendel','Wurf mit Luftwiderstand','Kugelstoß & Medien','Box auf Ebene',
       'Feder–Masse–Dämpfer','Wurf mit Unsicherheit','Boxstoß','Auftrieb')
parser=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):parser.add_argument('--'+name,type=Path,required=True)
parser.add_argument('--probe',type=Path)
parser.add_argument('--inner',action='store_true')
args=parser.parse_args();args.app=args.app.resolve();args.work=args.work.resolve();args.work.mkdir(parents=True,exist_ok=True)
if sys.platform=='linux' and not args.inner:
    raise SystemExit(subprocess.call(['dbus-run-session','--',sys.executable,str(Path(__file__).resolve()),
        '--app',str(args.app),'--work',str(args.work),'--inner'],env=dict(os.environ,GSETTINGS_BACKEND='memory')))
if sys.platform=='linux':
    import dbus
    session=dbus.SessionBus();status=session.get_object('org.a11y.Bus','/org/a11y/bus')
    dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
    os.environ['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress())
    import pyatspi
    from gi.repository import GLib
else:
    assert sys.platform=='win32' and args.probe and args.probe.is_file()

binary=hashlib.sha256(args.app.read_bytes()).hexdigest();cases=[]
for size in ('normal','large'):
    work=args.work/size;work.mkdir();reader=work/'reader'
    out=(work/'app.stdout').open('wb');err=(work/'app.stderr').open('wb')
    command=[str(args.app),'--workspace-state-test',str(reader),'manager-design-remote'+('-large' if size=='large' else '')]
    process=subprocess.Popen(command,stdout=out,stderr=err,env=dict(os.environ,PHYSIM_TEST_TRACE='1',NO_AT_BRIDGE='0'))
    record=dict(size=size,command=command,passed=False,observations=[])
    def output():return (work/'app.stdout').read_text(encoding='utf8',errors='replace')
    def wait(fn,label):
        deadline=time.monotonic()+30;last=None
        while time.monotonic()<deadline:
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
    def selected_events():return [json.loads(line) for line in output().splitlines() if line.startswith('{"kind":"wizard_remote_selected"')]
    def probe(action,label,group=None):
        command=[str(args.probe.resolve()),str(process.pid),action,label]
        if group:command.append(group)
        result=subprocess.run(command,capture_output=True,timeout=15)
        state=dict(command=command,exit_code=result.returncode,stdout=result.stdout.decode('utf8',errors='replace'),stderr=result.stderr.decode('utf8',errors='replace'))
        record['observations'].append(state)
        if result.returncode or 'INDEPENDENT WINDOWS UIA: PASSED' not in state['stdout']:raise RuntimeError('Native UIA action failed: '+json.dumps(state))
        rows=[json.loads(line) for line in state['stdout'].splitlines() if line.startswith('{')]
        return rows[-1] if rows else True
    def tree():
        for application in pyatspi.Registry.getDesktop(0):
            if application.name!='Physim' or application.get_process_id()!=process.pid:continue
            found=[];stack=[(node,0) for node in application]
            while stack:
                node,depth=stack.pop();found.append(node)
                if depth<10:stack.extend((child,depth+1) for child in node)
            return found
        return []
    def named(name):return next((node for node in tree() if node.name==name),None)
    try:
        wait(lambda:'PROJECT WIZARD REMOTE READY' in output(),'remote ready')
        environment=next(json.loads(line) for line in output().splitlines() if line.startswith('{"kind":"wizard_remote_environment"'))
        record['environment']=environment
        assert (environment['width'],environment['height'],environment['ui_size'])==(1080,740,22 if size=='large' else 16)
        if sys.platform=='linux':
            legacy=next((node for node in tree() if node.name==NAMES[0] and node.getRole()==pyatspi.ROLE_PUSH_BUTTON),None)
            if legacy:
                record['observations'].append(dict(legacy_card_name=legacy.name,legacy_role=legacy.getRoleName(),role_id=legacy.getRole()))
                raise RuntimeError('Template card is a push button; native single-selection radio group is missing')
            group=wait(lambda:named('Vorlage'),'radio group')
            members=list(group)
            record['observations'].append(dict(members=[dict(name=node.name,role=node.getRoleName(),role_id=node.getRole()) for node in members]))
            assert len(members)==8 and {node.name for node in members}==set(NAMES)
            assert all(node.getRole()==pyatspi.ROLE_RADIO_BUTTON for node in members)
            assert not any(node.name=='Messläufe durchsuchen' for node in tree()),'Background edit leaked into modal publication'
            cards={node.name:node for node in members}
            # AT-SPI radio buttons expose CHECKABLE/CHECKED, rather than the
            # selectable-child contract of a list. UIA uses SelectionItem below.
            # https://gnome.pages.gitlab.gnome.org/gtk/atspi2/enum.Role.html
            def checked_names():return [node.name for node in members if node.getState().contains(pyatspi.STATE_CHECKED)]
            assert all(node.getState().contains(pyatspi.STATE_CHECKABLE) for node in members)
            assert checked_names()==[NAMES[0]]
            last=cards[NAMES[7]];initial_showing=last.getState().contains(pyatspi.STATE_SHOWING)
            record['observations'].append(dict(last_initial_showing=initial_showing))
            if size=='large':
                initial_box=last.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
                assert not initial_showing and initial_box[2]==0 and initial_box[3]==0
        else:
            probe('absent','Messläufe durchsuchen')
            initial=probe('control-state',NAMES[7],'Vorlage')
            if size=='large':assert initial['offscreen']
        for ordinal,index in enumerate((7,1,6,2,5,3,4,0),start=2):
            name=NAMES[index]
            if sys.platform=='linux':
                node=cards[name];assert node.queryComponent().grabFocus()
                wait(lambda:node.getState().contains(pyatspi.STATE_FOCUSED) and node.getState().contains(pyatspi.STATE_SHOWING),'native focus/reveal '+name)
                assert node.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)[2]>0
                assert node.queryAction().doAction(0)
            else:
                probe('focus',name,'Vorlage')
                wait(lambda:not probe('control-state',name,'Vorlage')['offscreen'],'UIA reveal '+name)
                probe('select',name,'Vorlage')
            wait(lambda:len(selected_events())>=ordinal and selected_events()[-1]['template']==index,'frontend selection '+name)
            if sys.platform=='linux':
                assert checked_names()==[name]
                record['observations'].append(dict(selected=name,focused=node.getState().contains(pyatspi.STATE_FOCUSED),showing=node.getState().contains(pyatspi.STATE_SHOWING)))
            else:probe('radio-state',name,'Vorlage')
        if sys.platform=='linux':
            cancel=wait(lambda:named('Abbrechen'),'cancel')
            assert cancel.queryComponent().grabFocus()
            wait(lambda:cancel.getState().contains(pyatspi.STATE_FOCUSED) and cancel.getState().contains(pyatspi.STATE_SHOWING),'cancel reveal')
            assert cancel.queryAction().doAction(0)
            wait(lambda:(node:=named('Datei')) and node.getState().contains(pyatspi.STATE_FOCUSED),'actual opener focus return')
            record['opener_native_focus']=True
        else:
            probe('invoke','Abbrechen')
            wait(lambda:probe('control-state','Datei'),'background restored')
            record['opener_focus_observation']=probe('control-state','Datei')
            record['windows_scope']='Selection interfaces and reveal; no physical foreground-focus proof'
        (reader/'wizard-reader-complete').write_text('native reader finished\n',encoding='utf8')
        assert process.wait(timeout=15)==0
        assert 'PROJECT WIZARD REMOTE SELF-TEST: PASSED' in output()
        assert hashlib.sha256(args.app.read_bytes()).hexdigest()==binary
        record['passed']=True
    except Exception as error:
        record['error']=repr(error)
        raise
    finally:
        if process.poll() is None:
            process.terminate()
            try:process.wait(timeout=5)
            except subprocess.TimeoutExpired:process.kill();process.wait()
        out.close();err.close();record['app_exit_code']=process.returncode;cases.append(record)
        (args.work/'verification.json').write_text(json.dumps(dict(app_sha256=binary,platform=sys.platform,cases=cases),ensure_ascii=False,indent=2)+'\n',encoding='utf8')
print('Independent project wizard: complete radio groups, selection, offscreen reveal, modal isolation, cancel and source integrity passed')
