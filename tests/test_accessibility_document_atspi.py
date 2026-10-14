"""Independent AT-SPI client reads and reveals an entire real F1 source page."""
import argparse,json,os,subprocess,time
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--app',type=Path,required=True);p.add_argument('--root',type=Path,required=True)
p.add_argument('--work',type=Path,required=True);p.add_argument('--inner',action='store_true')
a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
if not a.inner:
    command=['dbus-run-session','--','/usr/bin/python3',str(Path(__file__).resolve()),'--app',str(a.app.resolve()),
             '--root',str(a.root.resolve()),'--work',str(a.work.resolve()),'--inner']
    raise SystemExit(subprocess.call(command,env=dict(os.environ,GSETTINGS_BACKEND='memory')))
import dbus
bus=dbus.SessionBus();status=bus.get_object('org.a11y.Bus','/org/a11y/bus')
dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
os.environ['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress())
import pyatspi
from gi.repository import GLib
out=(a.work/'app.stdout').open('wb');err=(a.work/'app.stderr').open('wb')
process=subprocess.Popen([str(a.app),'--workspace-state-test',str(a.work/'reader'),'docs-keyboard-reading-remote'],
                         stdout=out,stderr=err,env=dict(os.environ,NO_AT_BRIDGE='0',LIBGL_ALWAYS_SOFTWARE='1'))
def wait(fn,label,seconds=20):
    deadline=time.monotonic()+seconds;last=None
    while time.monotonic()<deadline:
        context=GLib.MainContext.default()
        while context.pending():context.iteration(False)
        try:
            result=fn()
            if result:return result
        except Exception as error:last=repr(error)
        if process.poll() is not None:raise AssertionError(f'app exited {process.returncode}: '+(a.work/'app.stderr').read_text())
        time.sleep(.02)
    raise AssertionError(label+': '+str(last))
def document():
    for app in pyatspi.Registry.getDesktop(0):
        if app.name!='Physim' or app.get_process_id()!=process.pid:continue
        for window in app:
            if window.name!='Physim | Dokumentation':continue
            for child in window:
                if child.getRole()==pyatspi.ROLE_DOCUMENT_TEXT:return child
    return None
try:
    wait(lambda:'READING READY' in (a.work/'app.stdout').read_text(),'actual F1 source ready')
    reader=wait(document,'actual F1 native document');text=reader.queryText()
    expected=(a.root/'docs/project-audit.json').read_bytes()+b'\n\n'
    expected_text=expected.decode('utf-8');actual=text.getText(0,-1)
    assert len(expected)>256*1024 and actual.encode('utf-8')==expected,'Native text lost or changed the full source'
    assert text.characterCount==len(expected_text) and text.caretOffset==-1 and text.getNSelections()==0
    (a.work/'atspi-document.txt').write_bytes(actual.encode('utf-8'))
    count=text.characterCount
    assert text.getText(count-128,count)==expected_text[-128:]
    def initial_geometry():
        rectangle=text.getCharacterExtents(0,pyatspi.WINDOW_COORDS)
        return rectangle if rectangle[2]>0 and rectangle[3]>0 else None
    first=wait(initial_geometry,'first native glyph')
    assert first[2]>0 and first[3]>0,'F1 exposes no real initial text geometry'
    raw=dbus.bus.BusConnection(os.environ['AT_SPI_BUS_ADDRESS'])
    references=dbus.Interface(raw.get_object('org.a11y.atspi.Registry','/org/a11y/atspi/accessible/root'),'org.a11y.atspi.Accessible').GetChildren()
    provider=next(str(name) for name,path in references if dbus.Interface(raw.get_object(name,path),
        'org.freedesktop.DBus.Properties').Get('org.a11y.atspi.Accessible','Name')=='Physim')
    # The independent cache identifies the document object; no app-private node
    # identifier or assumed synthetic path is used by this client.
    cache=dbus.Interface(raw.get_object(provider,'/org/a11y/atspi/cache'),'org.a11y.atspi.Cache').GetItems()
    path=next(str(item[0][1]) for item in cache if int(item[7])==94 and str(item[6])==reader.name)
    proxy=dbus.Interface(raw.get_object(provider,path),'org.a11y.atspi.Text')
    last=len(expected_text.rstrip())-1
    assert proxy.ScrollSubstringTo(dbus.Int32(last),dbus.Int32(last+1),dbus.UInt32(2)),'Native reveal rejected'
    assert process.wait(timeout=20)==0
    output=(a.work/'app.stdout').read_text()
    assert 'READING END' in output and 'DOCUMENTATION READING SELF-TEST: PASSED' in output
    assert (a.work/'reader/docs-reading-end.bmp').is_file()
    (a.work/'results.json').write_text(json.dumps(dict(bytes=len(expected),characters=count,independent_client=True,
        complete_source=True,actual_initial_geometry=True,actual_scroll=True),indent=2)+'\n')
    raw.close()
finally:
    if process.poll() is None:process.kill();process.wait()
    out.close();err.close()
print('Actual F1 AT-SPI reading: complete source, Unicode scalar count, real glyph geometry and native reveal passed')
