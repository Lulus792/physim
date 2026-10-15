"""Independent AT-SPI table client: Unicode, sparse cells and lifecycle."""
import argparse,json,os,subprocess,time
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--fixture',required=True);p.add_argument('--work',type=Path,required=True)
p.add_argument('--inner',action='store_true');a=p.parse_args();a.work.mkdir(parents=True,exist_ok=True)
if not a.inner:
    raise SystemExit(subprocess.call(['dbus-run-session','--','/usr/bin/python3',str(Path(__file__).resolve()),
        '--fixture',str(Path(a.fixture).resolve()),'--work',str(a.work.resolve()),'--inner'],env=dict(os.environ,GSETTINGS_BACKEND='memory')))
import dbus
bus=dbus.SessionBus();status=bus.get_object('org.a11y.Bus','/org/a11y/bus')
dbus.Interface(status,'org.freedesktop.DBus.Properties').Set('org.a11y.Status','IsEnabled',dbus.Boolean(True))
os.environ['AT_SPI_BUS_ADDRESS']=str(dbus.Interface(status,'org.a11y.Bus').GetAddress())
import pyatspi
from gi.repository import GLib

out=(a.work/'fixture.stdout').open('wb');err=(a.work/'fixture.stderr').open('wb')
process=subprocess.Popen([a.fixture],stdin=subprocess.PIPE,stdout=out,stderr=err,env=dict(os.environ,NO_AT_BRIDGE='0'))
events=[]
def event(e):events.append(dict(type=e.type,detail=e.detail1,path=str(e.source.path)))
pyatspi.Registry.registerEventListener(event,'object:state-changed','object:bounds-changed','object:children-changed')
def wait(fn,label):
    deadline=time.monotonic()+15;last=None
    while time.monotonic()<deadline:
        context=GLib.MainContext.default()
        while context.pending():context.iteration(False)
        assert process.poll() is None,(process.returncode,(a.work/'fixture.stderr').read_text())
        try:
            result=fn()
            if result:return result
        except Exception as error:last=repr(error)
        time.sleep(.02)
    raise AssertionError(label+': '+str(last))
def command(value):process.stdin.write(value);process.stdin.flush()
def window():
    for app in pyatspi.Registry.getDesktop(0):
        if app.name=='Physim' and app.get_process_id()==process.pid:
            return next((w for w in app if w.name=='Physim AT-SPI A'),None)
def showing(node):return node.getState().contains(pyatspi.STATE_SHOWING)
try:
    w=wait(window,'fixture window');command(b't')
    document=wait(lambda:next((n for n in w if n.name=='Tabellenprobe θ🌍'),None),'table document')
    node=wait(lambda:next((n for n in document if n.getRole()==pyatspi.ROLE_TABLE),None),'table')
    table=node.queryTable();assert (table.nRows,table.nColumns)==(3,3)
    assert node.childCount==9 and node.parent==document
    assert [table.getColumnHeader(i).name for i in range(3)]==['θ🌍','B','C']
    long_cell=table.getAccessibleAt(1,0);assert long_cell.name=='x'*1536
    empty=[table.getAccessibleAt(2,i) for i in (1,2)]
    assert empty[0]!=empty[1] and [n.name for n in empty]==['','']
    for i,n in enumerate(empty,1):
        item=n.queryTableCell();assert tuple(item.position)==(1,2,i)
        assert tuple(item.getRowColumnSpan())==(2,i,1,1)
        assert item.table==node and item.columnHeaderCells==[table.getColumnHeader(i)]
        assert n.parent==node and n.getIndexInParent()==6+i and node[6+i]==n
    assert not showing(empty[1]) and empty[1].queryComponent().getExtents(pyatspi.WINDOW_COORDS)[2]==0
    assert empty[1].queryComponent().scrollTo(pyatspi.SCROLL_ANYWHERE)
    wait(lambda:showing(empty[1]),'offscreen cell revealed')
    assert table.getAccessibleAt(2,2)==empty[1]
    assert empty[1].queryComponent().getExtents(pyatspi.WINDOW_COORDS)[2]>0
    assert long_cell.name=='x'*1536
    cell_path=str(empty[1].path);screen_before=empty[1].queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    window_before=w.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    command(b'm')
    wait(lambda:empty[1].queryComponent().getExtents(pyatspi.DESKTOP_COORDS).x!=screen_before.x,'moved cell bounds')
    screen_after=empty[1].queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    window_after=w.queryComponent().getExtents(pyatspi.DESKTOP_COORDS)
    assert screen_after.x-window_after.x==screen_before.x-window_before.x
    assert screen_after.y-window_after.y==screen_before.y-window_before.y
    # Drain the actual move, then an unchanged publication must stay quiet.
    def drain():
        context=GLib.MainContext.default()
        while context.pending():context.iteration(False)
    time.sleep(.1);drain()
    quiet_before=sum(e['path']==cell_path for e in events)
    time.sleep(.2);drain()
    assert sum(e['path']==cell_path for e in events)==quiet_before,'unchanged cell emitted events'
    command(b'h');wait(lambda:not showing(empty[1]),'hidden cell')
    assert empty[1].name=='' and not empty[1].queryComponent().scrollTo(pyatspi.SCROLL_ANYWHERE)
    hidden_box=empty[1].queryComponent().getExtents(pyatspi.WINDOW_COORDS)
    assert (hidden_box.width,hidden_box.height)==(0,0),repr(hidden_box)
    command(b's');wait(lambda:showing(empty[1]),'reshown cell')
    assert table.getAccessibleAt(2,2)==empty[1]
    table_path=str(node.path)
    raw=dbus.bus.BusConnection(os.environ['AT_SPI_BUS_ADDRESS'])
    refs=dbus.Interface(raw.get_object('org.a11y.atspi.Registry','/org/a11y/atspi/accessible/root'),'org.a11y.atspi.Accessible').GetChildren()
    provider=next(str(name) for name,path in refs if dbus.Interface(raw.get_object(name,path),'org.freedesktop.DBus.Properties').Get('org.a11y.atspi.Accessible','Name')=='Physim')
    command(b't')
    wait(lambda:any(n.path!=table_path for n in document if n.getRole()==pyatspi.ROLE_TABLE),'replacement table')
    for path,iface,method,signature,args in [
        (table_path,'Table','GetAccessibleAt','ii',(dbus.Int32(0),dbus.Int32(0))),
        (cell_path,'TableCell','GetRowColumnSpan','',())]:
        try:raw.call_blocking(provider,path,'org.a11y.atspi.'+iface,method,signature,args,timeout=5)
        except dbus.DBusException as error:assert error.get_dbus_name()=='org.a11y.atspi.Error.Defunct'
        else:raise AssertionError('old table object remained usable')
    wait(lambda:any(e['path']==cell_path and e['type'].startswith('object:state-changed:defunct') for e in events),'cell defunct event')
    assert any(e['path']==cell_path and e['type'].startswith('object:bounds-changed') for e in events)
    (a.work/'results.json').write_text(json.dumps(dict(unicode_headers=True,full_1536_character_name=True,
        distinct_virtual_cells=True,coordinates=True,offscreen_scroll=True,hidden_retained=True,
        stale_rejected=True,host_move=True,quiet_publication=True,events=events),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    command(b'q');assert process.wait(timeout=10)==0;raw.close()
finally:
    if process.poll() is None:process.kill();process.wait()
    out.close();err.close()
print('AT-SPI tables: Unicode, full names, virtual cells, headers, scroll, hidden state and stale objects passed')
