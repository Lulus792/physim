import hashlib,json,os,signal,shutil,subprocess,tempfile,time
from pathlib import Path
root=Path('/mnt/d/Physim');base=root/'build/native/Design-analysis-linux/test-results/file-tabs-source4-ext4';base.mkdir(exist_ok=True)
app=root/'build/native/Design-analysis-linux/bin/physim';digest=hashlib.sha256(app.read_bytes()).hexdigest()
case=os.environ.get('PHYSIM_EXT4_CASE','documentation_math'); deadline=75 if case=='documentation_math' else 65
base=base/case;base.mkdir(exist_ok=False)
work=Path(tempfile.mkdtemp(prefix='physim-file-tabs-final-'+case+'-'))
command=['/usr/bin/python3',str(root/'tests/check_display_workflow.py'),case,'--app',str(app),'--work',str(work),'--root',str(root),'--cc','/usr/bin/cc']
started=time.monotonic();proc=subprocess.Popen(command,cwd=root,env=dict(os.environ,ASAN_OPTIONS='detect_leaks=1'),stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True,encoding='utf-8',errors='replace',start_new_session=True)
try:
    stdout,stderr=proc.communicate(timeout=deadline)
    record=dict(source=4,name=case,status='passed' if proc.returncode==0 else 'failed',seconds=time.monotonic()-started,command=command,exit_code=proc.returncode,stdout=stdout,stderr=stderr)
except subprocess.TimeoutExpired:
    os.killpg(proc.pid,signal.SIGKILL);stdout,stderr=proc.communicate()
    record=dict(source=4,name=case,status='failed',seconds=time.monotonic()-started,command=command,exit_code=proc.returncode,reason='Original registry deadline exceeded; owned process group killed and reaped',stdout=stdout,stderr=stderr)
record.update(binary=dict(path=str(app),sha256=digest),original_work=str(work),outer_deadline_seconds=deadline,inner_app_deadline_seconds=45 if case=='documentation_math' else 15,scope='Only working data and captures moved to ext4; application and repository remain on /mnt/d')
assert hashlib.sha256(app.read_bytes()).hexdigest()==digest
(base/'result.json').write_text(json.dumps(record,ensure_ascii=False,indent=2)+'\n',encoding='utf-8');shutil.copytree(work,base/'files ä')
print(json.dumps({k:v for k,v in record.items() if k not in ('stdout','stderr')},ensure_ascii=False),flush=True)
raise SystemExit(0 if record['status']=='passed' else 1)
