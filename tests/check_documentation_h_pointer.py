"""Independent native H pointer tasks in the normal app, on a private X11 display.

Run: xvfb-run -a dbus-run-session -- python3 tests/check_documentation_h_pointer.py
  --app build/native/Debug-sanitized/bin/physim --work build/native/h-pointer-new
Requires Openbox, xdotool, python-dbus and PyAT-SPI. No app self-test hooks.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import time
import traceback
import dbus
from gi.repository import GLib

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app',required=True,type=Path)
parser.add_argument('--work',required=True,type=Path)
args=parser.parse_args()
root=Path(__file__).resolve().parents[1]
work=args.work.resolve();work.mkdir(parents=True,exist_ok=False)
env=dict(os.environ,XDG_RUNTIME_DIR=tempfile.mkdtemp(prefix='physim-h-pointer-'),
         XDG_CONFIG_HOME=str(work/'config'),XDG_DATA_HOME=str(work/'data'),XDG_CACHE_HOME=str(work/'cache'),
         GSETTINGS_BACKEND='memory',SDL_VIDEODRIVER='x11',GDK_BACKEND='x11',
         ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1',LIBGL_ALWAYS_SOFTWARE='1')
os.environ.update(env)
children=[];files=[]
report=dict(status='failed',app_sha256=hashlib.sha256(args.app.resolve().read_bytes()).hexdigest(),clicks=[],checks=[])

def launch(command,name):
    out=(work/(name+'.stdout')).open('wb');err=(work/(name+'.stderr')).open('wb');files.extend((out,err))
    child=subprocess.Popen(command,cwd=root,env=env,stdout=out,stderr=err);children.append(child);return child

def wait(predicate,label,seconds=35):
    deadline=time.monotonic()+seconds;last=None
    while time.monotonic()<deadline:
        context=GLib.MainContext.default()
        while context.pending():context.iteration(False)
        try:
            value=predicate()
            if value:return value
        except Exception as error:last=repr(error)
        time.sleep(.05)
    raise AssertionError(label+': '+str(last))

def xrun(*command):
    return subprocess.check_output(['xdotool',*map(str,command)],env=env,text=True,timeout=8).strip()

def frame():
    for application in pyatspi.Registry.getDesktop(0):
        if application.name=='Physim' and application.get_process_id()==app.pid:
            for window in application:
                if window.name=='Physim | Dokumentation':return window
    return None

def find(name,group=None,role=None):
    window=frame()
    if window is None:return None
    if group:
        window=next((node for node in window if node.name==group),None)
        if window is None:return None
    return next((node for node in window if node.name==name and (role is None or node.getRole()==role)),None)

def checked(name,group):
    node=find(name,group)
    if node is None:return False
    node.clearCache()
    return node.getRole()==pyatspi.ROLE_RADIO_BUTTON and node.getState().contains(pyatspi.STATE_CHECKED)

def focused(name,group=None,role=None):
    node=find(name,group,role)
    if node is None:return False
    node.clearCache()
    return node.getState().contains(pyatspi.STATE_FOCUSED)

def select_all(name):
    length=find(name).queryText().characterCount
    xrun('key','--clearmodifiers','ctrl+a')
    if length:
        wait(lambda:find(name).queryText().getNSelections()>0 and
             tuple(find(name).queryText().getSelection(0))==(0,length),'actual selection in '+name)

def click(name,group=None,role=None):
    def synchronized():
        geometry=dict(line.split('=',1) for line in xrun('getwindowgeometry','--shell',document_window).splitlines())
        native=frame().queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
        return tuple(native)==tuple(int(geometry[key]) for key in ('X','Y','WIDTH','HEIGHT'))
    wait(synchronized,'native screen bounds match the actual X11 window after placement')
    node=wait(lambda:find(name,group,role),'native target '+name)
    rectangle=node.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    assert rectangle.width>0 and rectangle.height>0,(name,rectangle)
    xrun('mousemove','--sync',rectangle.x+rectangle.width//2,rectangle.y+rectangle.height//2)
    # The instrumented renderer must observe a held press in an actual frame;
    # xdotool's default 12 ms click can finish between two ASan frames.
    xrun('mousedown','1')
    time.sleep(.15)
    xrun('mouseup','1')
    report['clicks'].append(dict(name=name,group=group,bounds=[rectangle.x,rectangle.y,rectangle.width,rectangle.height]))
    return node

try:
    launch(['openbox'],'openbox')
    subprocess.run(['python3',str(root/'tools/wait-window-manager.py')],env=env,check=True,timeout=35)
    bus=dbus.SessionBus();status=bus.get_object('org.a11y.Bus','/org/a11y/bus')
    dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
    env['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress());os.environ['AT_SPI_BUS_ADDRESS']=env['AT_SPI_BUS_ADDRESS']
    import pyatspi
    app=launch([str(args.app.resolve())],'physim')
    main=wait(lambda:xrun('search','--all','--onlyvisible','--pid',app.pid,'--name',r'^Physim \| Experiment Studio$'),'one main window')
    assert len(main.splitlines())==1,main
    xrun('windowactivate','--sync',main);xrun('key','F1');wait(frame,'F1 native tree')
    document_window=wait(lambda:xrun('search','--all','--onlyvisible','--pid',app.pid,'--name',r'^Physim \| Dokumentation$'),'one F1 window')
    assert len(document_window.splitlines())==1,document_window
    xrun('windowactivate','--sync',document_window)
    wait(lambda:frame().getState().contains(pyatspi.STATE_ACTIVE),'F1 foreground state')
    click('Alle Inhalte durchsuchen')
    wait(lambda:focused('Alle Inhalte durchsuchen'),'pointer focuses global search')
    xrun('type','--clearmodifiers','--delay','80','Pendel')
    click('Pendel & Integratoren',role=pyatspi.ROLE_PUSH_BUTTON)
    wait(lambda:find('Pendel & Integratoren',role=pyatspi.ROLE_DOCUMENT_TEXT),'actual pendulum document')
    click('Physim','Dokumentationssprache')
    wait(lambda:checked('Physim','Dokumentationssprache') and not checked('C','Dokumentationssprache'),'global Physim choice')
    click('Beispiel','Kontextansicht')
    wait(lambda:checked('Beispiel','Kontextansicht') and not checked('Modell','Kontextansicht'),'example text tab')
    click('C','Dokumentationssprache')
    wait(lambda:checked('C','Dokumentationssprache') and not checked('Physim','Dokumentationssprache') and checked('Beispiel','Kontextansicht'),
         'global C choice retains example tab')
    click('Modell','Kontextansicht')
    wait(lambda:checked('Modell','Kontextansicht') and not checked('Beispiel','Kontextansicht'),'model text tab')
    report['checks'].append('actual OS pointer switches both language segments and both context tabs with exclusive native radio state')
    before=find('Modell','Kontextansicht').queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    click('Themennavigation ausblenden (geöffnet)')
    wait(lambda:find('Themennavigation einblenden (geschlossen)'),'left side closed')
    after=find('Modell','Kontextansicht').queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    assert (before.x,before.width)==(after.x,after.width)
    click('Modellbereich ausblenden (geöffnet)')
    wait(lambda:find('Modell','Kontextansicht') is None,'right side closed')
    reader=find('Pendel & Integratoren',role=pyatspi.ROLE_DOCUMENT_TEXT)
    assert reader is not None and reader.queryComponent().getExtents(pyatspi.DESKTOP_COORDS).width>700
    click('Themennavigation einblenden (geschlossen)')
    click('Modellbereich einblenden (geschlossen)')
    wait(lambda:checked('Modell','Kontextansicht') and checked('C','Dokumentationssprache'),'both sides reopen with retained choices')
    report['checks'].append('independent pointer toggles preserve opposite native bounds, usable article and selected tabs')
    xrun('key','--clearmodifiers','ctrl+f')
    wait(lambda:focused('Aktuelles Dokument durchsuchen'),'local search before pane navigation')
    select_all('Aktuelles Dokument durchsuchen')
    xrun('type','--clearmodifiers','--delay','40','theta')
    wait(lambda:find('Aktuelles Dokument durchsuchen').queryText().getText(0,-1)=='theta','actual local query edit')
    xrun('key','--clearmodifiers','F6')
    wait(lambda:focused('Modell','Kontextansicht'),'F6 moves from article to context')
    xrun('key','--clearmodifiers','shift+F6')
    wait(lambda:focused('Pendel & Integratoren',role=pyatspi.ROLE_DOCUMENT_TEXT),
         'Shift+F6 returns to native article')
    assert find('Aktuelles Dokument durchsuchen').queryText().getText(0,-1)=='theta'
    report['checks'].append('real F6/Shift+F6 skips the topic catalogue and preserves the local query')
    xrun('key','--clearmodifiers','F6')
    wait(lambda:focused('Modell','Kontextansicht'),'context before global filter')
    xrun('key','--clearmodifiers','F6')
    wait(lambda:focused('Alle Inhalte durchsuchen'),'header before empty results')
    select_all('Alle Inhalte durchsuchen')
    xrun('type','--clearmodifiers','--delay','40','qzx987654')
    wait(lambda:find('Alle Inhalte durchsuchen').queryText().getText(0,-1)=='qzx987654','actual global filter edit')
    xrun('key','--clearmodifiers','F6')
    wait(lambda:focused('Pendel & Integratoren',role=pyatspi.ROLE_DOCUMENT_TEXT),'empty results skip directly to article')
    assert find('Pendel & Integratoren',role=pyatspi.ROLE_PUSH_BUTTON) is None
    xrun('key','--clearmodifiers','shift+F6')
    wait(lambda:focused('Alle Inhalte durchsuchen'),'reverse skips empty results')
    select_all('Alle Inhalte durchsuchen')
    xrun('key','--clearmodifiers','BackSpace')
    xrun('key','--clearmodifiers','F6')
    wait(lambda:focused('Pendel & Integratoren',role=pyatspi.ROLE_PUSH_BUTTON),'restored results select current article')
    report['checks'].append('real filter edits are committed before F6; empty results are skipped in both directions')
    report['status']='passed'
except Exception:
    report['error']=traceback.format_exc()
    try:
        report['windows']=xrun('search','--all','--onlyvisible','--pid',app.pid)
        report['geometry']=[xrun('getwindowgeometry','--shell',window) for window in report['windows'].splitlines()]
        report['pointer']=xrun('getmouselocation','--shell')
        report['foreground']=xrun('getwindowfocus')
        report['native_root']=list(frame().queryComponent().getExtents(pyatspi.DESKTOP_COORDS))
        report['tree']=[dict(name=node.name,role=node.getRoleName(),bounds=list(node.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)),
                             text=node.queryText().getText(0,-1) if node.getRole()==pyatspi.ROLE_ENTRY else None) for node in frame()]
    except Exception as error:report['diagnostic_error']=repr(error)
finally:
    for child in reversed(children):
        if child.poll() is None:
            child.terminate()
            try:child.wait(timeout=8)
            except subprocess.TimeoutExpired:child.kill();child.wait()
    for file in files:file.close()
    (work/'results.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
print('Normal native H pointer tasks:',report['status'],work,flush=True)
raise SystemExit(0 if report['status']=='passed' else 1)
