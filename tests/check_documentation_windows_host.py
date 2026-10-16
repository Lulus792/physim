"""Observe real Win32 host focus while the native F1 pane matrix runs.

This observer never activates windows or injects OS input. The app's existing
matrix sends its SDL test keys; Win32 foreground/GUI-thread state is independent.
"""
import argparse
import ctypes
from ctypes import wintypes as w
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import time

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app',type=Path,required=True)
parser.add_argument('--work',type=Path,required=True)
args=parser.parse_args()
if sys.platform!='win32':parser.error('This observer requires Windows')
work=args.work.resolve()/'host-audit';work.mkdir(parents=True,exist_ok=False)
app=args.app.resolve()
u=ctypes.WinDLL('user32',use_last_error=True)
u.GetForegroundWindow.restype=w.HWND
u.GetWindowThreadProcessId.argtypes=[w.HWND,ctypes.POINTER(w.DWORD)]
u.GetWindowThreadProcessId.restype=w.DWORD
u.GetWindowTextW.argtypes=[w.HWND,w.LPWSTR,ctypes.c_int]
u.IsWindowVisible.argtypes=[w.HWND]
class GuiThread(ctypes.Structure):
    _fields_=[('cbSize',w.DWORD),('flags',w.DWORD),('active',w.HWND),
              ('focus',w.HWND),('capture',w.HWND),('menu',w.HWND),
              ('move',w.HWND),('caret',w.HWND),('rect',w.RECT)]
u.GetGUIThreadInfo.argtypes=[w.DWORD,ctypes.POINTER(GuiThread)]
callback=ctypes.WINFUNCTYPE(w.BOOL,w.HWND,w.LPARAM)
u.EnumWindows.argtypes=[callback,w.LPARAM]

def snapshot(pid):
    found=[]
    @callback
    def visit(hwnd,param):
        owner=w.DWORD();thread=u.GetWindowThreadProcessId(hwnd,ctypes.byref(owner))
        if owner.value==pid and u.IsWindowVisible(hwnd):
            name=ctypes.create_unicode_buffer(256);u.GetWindowTextW(hwnd,name,256)
            if name.value.startswith('Physim |'):
                info=GuiThread();info.cbSize=ctypes.sizeof(info)
                queried=bool(u.GetGUIThreadInfo(thread,ctypes.byref(info)))
                found.append(dict(title=name.value,window=int(hwnd),
                    foreground=u.GetForegroundWindow()==hwnd,active=info.active==hwnd,
                    focus=info.focus==hwnd,focus_present=bool(info.focus),thread_info=queried))
        return True
    if not u.EnumWindows(visit,0):raise ctypes.WinError(ctypes.get_last_error())
    return sorted(found,key=lambda node:node['window'])

report=dict(status='failed',app_sha256=hashlib.sha256(app.read_bytes()).hexdigest(),observations=[])
process=None;started=time.monotonic();last=None
try:
    with (work/'app.stdout').open('wb') as out,(work/'app.stderr').open('wb') as err:
        process=subprocess.Popen([str(app),'--workspace-state-test',str(work/'reader'),'docs-keyboard-panes'],
            cwd=Path(__file__).resolve().parents[1],stdout=out,stderr=err,
            env=dict(os.environ,PHYSIM_TEST_TRACE='1',PHYSIM_TEST_SMALL='1'))
        while process.poll() is None:
            state=snapshot(process.pid)
            if state!=last:
                report['observations'].append(dict(seconds=round(time.monotonic()-started,3),windows=state));last=state
            if time.monotonic()-started>90:raise TimeoutError('Native pane matrix exceeded its observer deadline')
            time.sleep(.02)
    output=(work/'app.stdout').read_text(encoding='utf-8')
    report['exit_code']=process.returncode
    records=re.findall(r'DOCUMENTATION PANES: case=(\d+) .* complete=(\d+)',output)
    assert process.returncode==0 and 'DOCUMENTATION PANES SELF-TEST: PASSED' in output,'Native pane matrix failed'
    assert records==[(str(index),'1') for index in range(24)],'Incomplete native pane matrix'
    assert any(node['title']=='Physim | Dokumentation' and all(node[key] for key in ('foreground','active','focus','thread_info'))
               for event in report['observations'] for node in event['windows']),'No independent Win32 document focus observed'
    report['status']='passed'
except Exception as error:
    report['error']=str(error)
finally:
    if process is not None and process.poll() is None:
        process.terminate()
        try:process.wait(timeout=10)
        except subprocess.TimeoutExpired:process.kill();process.wait()
    if process is not None:report['exit_code']=process.returncode
    (work/'results.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('Independent Win32 F1 host focus:',report['status'],work)
raise SystemExit(0 if report['status']=='passed' else 1)
