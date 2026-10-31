"""Reject loss of caller GL texture/unpack state using the actual font-bank test."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

import build
import native_tests


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler')
    parser.add_argument('--sdl',type=Path,required=True)
    parser.add_argument('--build-dir',type=Path,required=True)
    parser.add_argument('--config',choices=('Debug','Release'),default='Release')
    parser.add_argument('--sanitizers',action='store_true')
    parser.add_argument('--proof-dir',type=Path)
    args=parser.parse_args();directory=args.build_dir.resolve()
    proof=args.proof_dir.resolve() if args.proof_dir else directory/'font-bank-state-counterexample'
    proof.relative_to(directory);proof.mkdir()
    original=(build.ROOT/'app/graphics.c').read_text(encoding='utf8')
    line='    g->PixelStorei(GL_UNPACK_ALIGNMENT,unpack);g->BindTexture(GL_TEXTURE_2D,(GLuint)previous);'
    assert original.count(line)==1,'Review the texture upload restoration transaction'
    case=next(c for c in native_tests.catalog() if c.name=='font_bank_ui')
    options=argparse.Namespace(build_dir=directory,compiler=args.compiler,sdl=args.sdl,config=args.config,
        no_app=False,sanitizers=args.sanitizers,jobs=4,rebuild=False)
    environment=build.compiler_environment()
    rows=[]
    for kind,replacement,diagnostic in (
        ('binding',line.replace('(GLuint)previous','0'),'binding==(GLint)original'),
        ('unpack',line.replace('GL_UNPACK_ALIGNMENT,unpack','GL_UNPACK_ALIGNMENT,4'),'unpack==8')):
        folder=proof/kind;folder.mkdir();work=folder/'workflow';work.mkdir()
        mutant=original.replace(line,replacement);source=folder/'graphics.c';source.write_text(mutant,encoding='utf8')
        with build.build_lock(directory):
            builder=build.Builder(options,environment)
            libraries=[builder.lib/(('physim-'+n+'.lib') if builder.msvc else ('libphysim-'+n+'.a')) for n in case.libraries]
            if not all(p.is_file() for p in libraries):
                raise RuntimeError('Build the current UI libraries before the counterexample')
            sources=[str(source) if p=='app/graphics.c' else p for p in case.sources]
            binary=builder.executable('physim-font-bank-counterexample-'+kind,sources,libraries,sdl=True,defines=case.defines)
        command=[str(binary),str(work)]
        result=subprocess.run(command,cwd=build.ROOT,env=environment,capture_output=True,timeout=60)
        (folder/'stdout').write_bytes(result.stdout);(folder/'stderr').write_bytes(result.stderr)
        text=result.stderr.decode('utf8',errors='replace')
        precise=result.returncode==1 and diagnostic in text and 'Font bank ' in text
        rows.append(dict(kind=kind,command=command,exit_code=result.returncode,precise_rejection=precise,
            source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),normalized_source_sha256=hashlib.sha256(mutant.encode()).hexdigest(),binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest()))
    report=dict(original_source_sha256=hashlib.sha256(original.encode()).hexdigest(),rows=rows,
        shared_source_unchanged=True,scope='Injected upload GL error with caller texture nonzero and unpack alignment8; not GPU exhaustion or negative-process leak acceptance')
    (proof/'report.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    if not all(r['precise_rejection'] for r in rows):
        raise RuntimeError('A GL state mutation did not reach its specific font-bank assertion')
    print('FONT BANK STATE COUNTEREXAMPLES: texture binding and unpack loss both rejected')


if __name__=='__main__':
    main()
