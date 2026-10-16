"""Independent AT-SPI values, caret and selection drive both actual F1 fields."""
import argparse,json,os,subprocess,time
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--app',type=Path,required=True);p.add_argument('--work',type=Path,required=True)
p.add_argument('--inner',action='store_true');a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
if not a.inner:
    raise SystemExit(subprocess.call(['dbus-run-session','--','/usr/bin/python3',str(Path(__file__).resolve()),
        '--app',str(a.app.resolve()),'--work',str(a.work.resolve()),'--inner'],env=dict(os.environ,GSETTINGS_BACKEND='memory')))
import dbus
bus=dbus.SessionBus();status=bus.get_object('org.a11y.Bus','/org/a11y/bus')
dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
os.environ['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress())
import pyatspi
from gi.repository import GLib
out=(a.work/'app.stdout').open('wb');err=(a.work/'app.stderr').open('wb')
process=subprocess.Popen([str(a.app),'--workspace-state-test',str(a.work/'reader'),'docs-keyboard-edit-remote'],
    stdout=out,stderr=err,env=dict(os.environ,NO_AT_BRIDGE='0',LIBGL_ALWAYS_SOFTWARE='1'))
events=[]
def event(e):events.append(dict(type=e.type,offset=e.detail1,length=e.detail2))
pyatspi.Registry.registerEventListener(event,'object:text-changed','object:text-caret-moved','object:text-selection-changed','object:state-changed:focused')
def wait(fn,label):
    deadline=time.monotonic()+20;last=None
    while time.monotonic()<deadline:
        context=GLib.MainContext.default()
        while context.pending():context.iteration(False)
        try:
            value=fn()
            if value:return value
        except Exception as error:last=repr(error)
        assert process.poll() is None,(process.returncode,(a.work/'app.stderr').read_text())
        assert time.monotonic()<deadline,label+': '+str(last)
        time.sleep(.02)
    raise AssertionError(label+': '+str(last))
def field(name,role=pyatspi.ROLE_ENTRY):
    for app in pyatspi.Registry.getDesktop(0):
        if app.name!='Physim' or app.get_process_id()!=process.pid:continue
        for window in app:
            if window.name=='Physim | Dokumentation':
                for node in window:
                    if node.name==name and node.getRole()==role:return node
    return None
try:
    for phase,name,value in [('SEARCH','Aktuelles Dokument durchsuchen','ps_rng_seed θ🌍'),('GLOBAL','Alle Inhalte durchsuchen','ps_rng_seed')]:
        wait(lambda:'EDIT READY '+phase in (a.work/'app.stdout').read_text(),'app ready '+phase)
        node=wait(lambda:field(name),'native field '+name)
        assert node.getRole()==pyatspi.ROLE_ENTRY and node.getState().contains(pyatspi.STATE_EDITABLE),(phase,node.getRole(),node.getState().getStates(),node.queryComponent().getExtents(pyatspi.WINDOW_COORDS))
        assert node.queryComponent().grabFocus()
        wait(lambda:node.getState().contains(pyatspi.STATE_FOCUSED),'actual field focus')
        assert node.queryEditableText().setTextContents(value)
        wait(lambda:node.queryText().getText(0,-1)==value,'actual native value')
        wait(lambda:'EDIT APPLIED '+phase in (a.work/'app.stdout').read_text(),'app applied '+phase)
        text=node.queryText();assert text.characterCount==len(value)
        if phase=='SEARCH':
            start=value.index('θ');assert text.addSelection(start,len(value))
            wait(lambda:text.getNSelections()==1 and tuple(text.getSelection(0))==(start,len(value)), 'actual Unicode selection')
            assert text.caretOffset==len(value)
            (a.work/'reader/selection-applied').write_text('ready',encoding='utf-8')
        else:
            (a.work/'reader/global-focus-checked').write_text('ready',encoding='utf-8')
    wait(lambda:'EDIT READY RETURN' in (a.work/'app.stdout').read_text(),'keyboard return to local search')
    node=wait(lambda:field('Aktuelles Dokument durchsuchen'),'local field after Ctrl+F')
    wait(lambda:node.getState().contains(pyatspi.STATE_FOCUSED),'native focus after Ctrl+F')
    (a.work/'reader/return-focus-checked').write_text('ready',encoding='utf-8')
    for phase,name,marker in [('PREVIOUS','Vorheriger Treffer','previous-focus-checked'),('NEXT','Nächster Treffer','next-focus-checked'),('PREVIOUS RETURN','Vorheriger Treffer','previous-return-focus-checked')]:
        wait(lambda:'EDIT READY '+phase in (a.work/'app.stdout').read_text(),'keyboard button '+phase)
        node=wait(lambda:field(name,pyatspi.ROLE_PUSH_BUTTON),'native button '+name)
        wait(lambda:node.getState().contains(pyatspi.STATE_FOCUSED),'button focus '+phase)
        (a.work/'reader'/marker).write_text('ready',encoding='utf-8')
    assert process.wait(timeout=20)==0
    output=(a.work/'app.stdout').read_text()
    assert 'DOCUMENTATION EDIT SELF-TEST: PASSED' in output
    assert all((a.work/'reader'/name).is_file() for name in ('docs-native-search.bmp','docs-native-selection.bmp','docs-native-global-search.bmp'))
    assert any(e['type'].startswith('object:text-changed:insert') for e in events)
    assert any(e['type'].startswith('object:text-selection-changed') for e in events)
    (a.work/'results.json').write_text(json.dumps(dict(independent_client=True,actual_fields=True,unicode_selection=True,
        actual_replacement=True,tab_exit=True,keyboard_return_focus=True,events=events),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
finally:
    if process.poll() is None:process.kill();process.wait()
    out.close();err.close()
print('Actual F1 AT-SPI fields: named entries, value delivery, Unicode caret/selection, real text replacement and Tab exit passed')
