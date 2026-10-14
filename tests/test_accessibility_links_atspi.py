"""Independent AT-SPI client reads embedded F1 links, focuses and follows them."""
import argparse,json,os,subprocess,time
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--app',type=Path,required=True);p.add_argument('--work',type=Path,required=True)
p.add_argument('--inner',action='store_true');a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
if not a.inner:
    raise SystemExit(subprocess.call(['dbus-run-session','--','/usr/bin/python3',str(Path(__file__).resolve()),
        '--app',str(a.app.resolve()),'--work',str(a.work.resolve()),'--inner'],env=dict(os.environ,GSETTINGS_BACKEND='memory')))
import dbus,pyatspi
from gi.repository import GLib
bus=dbus.SessionBus();status=bus.get_object('org.a11y.Bus','/org/a11y/bus')
dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
os.environ['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress())
out=(a.work/'app.stdout').open('wb');err=(a.work/'app.stderr').open('wb')
process=subprocess.Popen([str(a.app),'--workspace-state-test',str(a.work/'reader'),'docs-keyboard-links-remote'],
    stdout=out,stderr=err,env=dict(os.environ,NO_AT_BRIDGE='0',LIBGL_ALWAYS_SOFTWARE='1'))
events=[]
def event(e):events.append(dict(type=e.type,detail=e.detail1))
pyatspi.Registry.registerEventListener(event,'object:state-changed:focused','object:state-changed:showing','object:bounds-changed','object:children-changed')
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
        time.sleep(.02)
    raise AssertionError(label+': '+str(last))
def window():
    for app in pyatspi.Registry.getDesktop(0):
        if app.name=='Physim' and app.get_process_id()==process.pid:
            for w in app:
                if w.name=='Physim | Dokumentation':return w
def marker(text):return text in (a.work/'app.stdout').read_text()
try:
    wait(lambda:marker('LINKS READY FOCUS'),'F1 links ready')
    w=wait(window,'native F1 window')
    document=wait(lambda:next((n for n in w if n.getRole()==pyatspi.ROLE_DOCUMENT_TEXT),None),'native document')
    text=document.queryText();content=text.getText(0,-1);hypertext=document.queryHypertext()
    assert hypertext.getNLinks()==2
    links=[hypertext.getLink(i) for i in range(2)]
    names=[]
    for i,link in enumerate(links):
        node=link.getObject(0);names.append(node.name)
        assert node.getRole()==pyatspi.ROLE_LINK and link.nAnchors==1 and link.isValid()
        assert content[link.startIndex:link.endIndex]==node.name
        assert node.queryHyperlink().startIndex==link.startIndex
        bounds=link.get_index_range();assert (bounds.start_offset,bounds.end_offset)==(link.startIndex,link.endIndex)
        assert hypertext.getLinkIndex(link.startIndex)==i
        assert hypertext.getLinkIndex(link.endIndex)==-1
        assert link.getURI(0)==('x-guide.md' if i==0 else 'c-guide.md')
    assert names==['C-Lernweg','C'] and links[0].endIndex<=links[1].startIndex
    raw=dbus.bus.BusConnection(os.environ['AT_SPI_BUS_ADDRESS'])
    refs=dbus.Interface(raw.get_object('org.a11y.atspi.Registry','/org/a11y/atspi/accessible/root'),'org.a11y.atspi.Accessible').GetChildren()
    provider=next(str(name) for name,path in refs if dbus.Interface(raw.get_object(name,path),
        'org.freedesktop.DBus.Properties').Get('org.a11y.atspi.Accessible','Name')=='Physim')
    cache=dbus.Interface(raw.get_object(provider,'/org/a11y/atspi/cache'),'org.a11y.atspi.Cache').GetItems()
    path=next(str(item[0][1]) for item in cache if int(item[7])==94 and str(item[6])==document.name)
    ht=dbus.Interface(raw.get_object(provider,path),'org.a11y.atspi.Hypertext')
    link_ref=ht.GetLink(dbus.Int32(0));hl=dbus.Interface(raw.get_object(link_ref[0],link_ref[1]),'org.a11y.atspi.Hyperlink')
    old_ref=ht.GetLink(dbus.Int32(1));old_link=dbus.Interface(raw.get_object(old_ref[0],old_ref[1]),'org.a11y.atspi.Hyperlink')
    properties=dbus.Interface(raw.get_object(link_ref[0],link_ref[1]),'org.freedesktop.DBus.Properties').GetAll('org.a11y.atspi.Hyperlink')
    assert int(properties['NAnchors'])==1 and int(properties['StartIndex'])==links[0].startIndex
    for operation in (lambda:ht.GetLink(dbus.Int32(-1)),lambda:ht.GetLink(dbus.Int32(2)),lambda:hl.GetURI(dbus.Int32(1))):
        try:operation()
        except dbus.DBusException as error:assert error.get_dbus_name()=='org.freedesktop.DBus.Error.InvalidArgs'
        else:raise AssertionError('invalid link or anchor index accepted')
    target=links[1].getObject(0)
    assert not target.getState().contains(pyatspi.STATE_SHOWING),'table link must start offscreen'
    assert target.queryComponent().grabFocus()
    wait(lambda:target.getState().contains(pyatspi.STATE_FOCUSED) and target.getState().contains(pyatspi.STATE_SHOWING),'focused link revealed')
    assert target.queryComponent().getExtents(pyatspi.WINDOW_COORDS)[2]>0
    rectangle=text.getRangeExtents(links[1].startIndex,links[1].endIndex,pyatspi.WINDOW_COORDS)
    assert rectangle[2]>0 and rectangle[3]>0,'link range has no actual text geometry'
    (a.work/'reader/link-focus-done').write_text('verified',encoding='utf-8')
    wait(lambda:marker('LINKS READY INVALID'),'invalid link phase')
    assert links[0].getObject(0).queryAction().doAction(0)
    wait(lambda:any('nicht in der mitgelieferten' in n.name for n in w),'actual link error')
    assert links[0].isValid() and text.getText(0,-1)==content
    (a.work/'reader/link-invalid-done').write_text('verified',encoding='utf-8')
    wait(lambda:marker('LINKS READY INVOKE'),'valid link phase')
    assert target.queryAction().doAction(0)
    wait(lambda:document.queryText().getText(0,-1)!=content,'actual native navigation')
    try:
        old_link.GetObject(dbus.Int32(0))
    except dbus.DBusException as error:assert error.get_dbus_name()=='org.a11y.atspi.Error.Defunct'
    else:raise AssertionError('obsolete hyperlink remained callable')
    (a.work/'reader/link-invoke-done').write_text('verified',encoding='utf-8')
    assert process.wait(timeout=20)==0
    assert marker('DOCUMENTATION LINKS SELF-TEST: PASSED')
    assert all((a.work/'reader'/name).is_file() for name in ('docs-native-link-focus.bmp','docs-native-link-failed.bmp','docs-native-link-open.bmp'))
    assert any(e['type'].startswith('object:state-changed:focused') for e in events)
    (a.work/'results.json').write_text(json.dumps(dict(independent_client=True,ordered_links=names,
        scalar_ranges=True,uris=True,offscreen_focus=True,actual_geometry=True,invalid_navigation_rejected=True,
        native_navigation=True,obsolete_links_rejected=True,events=events),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    raw.close()
finally:
    if process.poll() is None:process.kill();process.wait()
    out.close();err.close()
print('Actual F1 AT-SPI links: ordered scalar ranges, URI, offscreen focus, failed/successful navigation and obsolete objects passed')
