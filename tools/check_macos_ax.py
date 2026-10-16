"""Build a separate public AX client and verify complete F1 reading on macOS."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import subprocess
import sys
import time

import build


def check(args):
    work=args.work.resolve();work.mkdir(parents=True)
    report=dict(status='unverified',platform=sys.platform,architecture=platform.machine(),
                independent_client=True,permission_prompt=False)
    report_path=work/'results.json'
    def save():report_path.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    save()
    if sys.platform!='darwin':
        report.update(status='unavailable',reason='requires macOS');save();return 77
    options=argparse.Namespace(build_dir=args.build_dir.resolve(),compiler=args.compiler,sdl=args.sdl.resolve(),
        config='Debug',no_app=False,sanitizers=True,jobs=4,rebuild=False)
    with build.build_lock(options.build_dir):
        builder=build.Builder(options,dict(os.environ))
        client=builder.executable('accessibility-macos-probe',['tests/accessibility_macos_probe.c'],[])
    report['client_sha256']=hashlib.sha256(client.read_bytes()).hexdigest()
    trust=subprocess.run([str(client),'--trust'],capture_output=True,timeout=10)
    (work/'trust.stdout').write_bytes(trust.stdout);(work/'trust.stderr').write_bytes(trust.stderr)
    report['trusted']=trust.returncode==0
    if trust.returncode==77:
        report.update(status='unavailable',reason='external Accessibility client is not trusted');save()
        print('MAC AX EXTERNAL: UNAVAILABLE; trusted=false, no permission changes requested')
        return 0 if args.allow_unavailable else 77
    if trust.returncode:raise RuntimeError(f'AX trust probe failed: exit {trust.returncode}')
    application=builder.bin/'physim'
    report['application_sha256']=hashlib.sha256(application.read_bytes()).hexdigest();save()
    out=(work/'app.stdout').open('wb');err=(work/'app.stderr').open('wb')
    process=subprocess.Popen([str(application),'--workspace-state-test',str(work/'reader'),'docs-keyboard-reading-remote'],
                             stdout=out,stderr=err,env=builder.env,cwd=build.ROOT)
    report['application_pid']=process.pid;save()
    try:
        deadline=time.monotonic()+45
        while 'READING READY' not in (work/'app.stdout').read_text(encoding='utf-8'):
            if process.poll() is not None:raise RuntimeError(f'Application exited before readiness: {process.returncode}')
            if time.monotonic()>=deadline:raise TimeoutError('F1 source readiness')
            time.sleep(.02)
        transcript=work/'ax-document.txt'
        probe=subprocess.run([str(client),str(process.pid),str(transcript)],capture_output=True,timeout=30)
        (work/'client.stdout').write_bytes(probe.stdout);(work/'client.stderr').write_bytes(probe.stderr)
        if probe.returncode:
            detail=probe.stderr.decode('utf-8',errors='replace').strip()[:2000]
            raise RuntimeError(f'External AX client failed: exit {probe.returncode}; {detail}')
        expected=(build.ROOT/'docs/project-audit.json').read_bytes()+b'\n\n'
        actual=transcript.read_bytes();assert len(expected)>256*1024 and actual==expected,'External AX text differs from the complete source'
        utf16=len(expected.decode('utf-8').encode('utf-16-le'))//2
        output=probe.stdout.decode('utf-8')
        identity=re.search(r'MAC AX CLIENT: pid=(\d+) application_pid=(\d+)',output)
        assert identity and int(identity[1]) not in (process.pid,os.getpid()) and int(identity[2])==process.pid
        report['client_pid']=int(identity[1]);report['client_pid_is_separate']=True
        assert re.search(rf'MAC AX READING: bytes={len(expected)} utf16={utf16} complete=1 substring=1 initial_geometry=1 readonly=1 reveal_requested=1',output)
        if process.wait(timeout=20):raise RuntimeError('Application did not complete the native reveal')
        app_output=(work/'app.stdout').read_text(encoding='utf-8')
        assert 'READING END' in app_output and 'DOCUMENTATION READING SELF-TEST: PASSED' in app_output
        assert (work/'reader/docs-reading-end.bmp').is_file()
        report.update(status='passed',bytes=len(actual),utf16_units=utf16,complete_source=True,
                      exact_substring=True,actual_initial_geometry=True,readonly=True,native_reveal=True)
        save();print('MAC AX EXTERNAL: PASSED; full source, UTF-16 count, exact substring, native geometry and completed reveal')
        return 0
    except Exception as error:
        report.update(status='failed',error=str(error));save();raise
    finally:
        # Cleanup only after a verified assertion, terminal result or operation timeout.
        if process.poll() is None:process.terminate();process.wait(timeout=10)
        out.close();err.close()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler',default='clang');parser.add_argument('--sdl',type=Path,required=True)
    parser.add_argument('--build-dir',type=Path,default=build.ROOT/'build/native/Debug-sanitized')
    parser.add_argument('--work',type=Path,default=build.ROOT/'build/native/external-ax')
    parser.add_argument('--allow-unavailable',action='store_true',help='Record missing AX permission as unavailable, never as passed')
    args=parser.parse_args();raise SystemExit(check(args))


if __name__=='__main__':main()
