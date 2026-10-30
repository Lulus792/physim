"""Exercise the actual SDK topic copier and native reader after relocation."""
import argparse
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
sys.dont_write_bytecode=True


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,required=True)
    parser.add_argument('--probe',type=Path,required=True)
    parser.add_argument('--work',type=Path,required=True)
    args=parser.parse_args();root=args.root.resolve();args.work=args.work.resolve();args.work.mkdir(parents=True,exist_ok=True)
    spec=importlib.util.spec_from_file_location('physim_sdk_builder',root/'tools/build.py')
    builder=importlib.util.module_from_spec(spec);spec.loader.exec_module(builder)
    topics=[('README.md',False),('third_party/README.md',False),
            ('third_party/libunibreak-8.0/README.md',False),
            ('third_party/libunibreak-8.0/LICENCE',True),
            ('third_party/libunibreak-8.0/UNICODE-LICENSE.txt',True)]
    with tempfile.TemporaryDirectory(prefix='native notices ä ',dir=args.work) as temporary:
        assert Path(temporary).resolve().is_relative_to(args.work)
        staging=Path(temporary)/'original';staging.mkdir()
        adapted='# SDK offline\n\n[Herkunft](third_party/README.md#f1-codevorschau)\n'
        (staging/'README.md').write_text(adapted,encoding='utf8')
        (staging/'third_party').mkdir();shutil.copy2(root/'third_party/README.md',staging/'third_party/README.md')
        (staging/'tests').mkdir()
        (staging/'tests/documentation_topics.json').write_text(json.dumps({'format':1,'topics':[{'path':p,'code':code} for p,code in topics]}),encoding='utf8')
        checker=[sys.executable,'-X','utf8','-B',str(root/'tests/check_documentation_anchors.py'),'--root',str(staging),'--probe',str(args.probe.resolve())]
        before=subprocess.run(checker,capture_output=True,text=True,timeout=40)
        assert before.returncode and 'Native topic cannot be parsed: third_party/libunibreak-8.0/README.md' in before.stderr,(before.returncode,before.stderr)
        builder.copy_documentation_topics(root,staging,topics)
        assert (staging/'README.md').read_text(encoding='utf8')==adapted,'Adapted SDK entry page was overwritten'
        for path,_ in topics[1:]:assert (staging/path).read_bytes()==(root/path).read_bytes(),path
        moved=staging.with_name('relocated SDK ü');staging.rename(moved)
        checker[checker.index('--root')+1]=str(moved)
        after=subprocess.run(checker,capture_output=True,text=True,timeout=40)
        assert after.returncode==0,(after.returncode,after.stdout,after.stderr)
        assert '5 pages parsed' in after.stdout,after.stdout
        print(after.stdout,end='')
    print('SDK native notices: missing-file countercheck, actual copy, adapted entry preservation and relocated offline links passed')


if __name__=='__main__':main()
