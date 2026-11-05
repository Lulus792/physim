"""Build and measure the C completion backend without SDL or an application window.

Each invocation uses a new work directory under the repository's build directory.
Wall times measure backend calls only, with a warm OS/process. No UI/frame-rate
acceptance is implied. Default allocator timings and tracked requested heap bytes
are separate observations, not an estimate of instrumentation-free memory cost.
"""
from pathlib import Path
from types import SimpleNamespace
import argparse
import hashlib
import json
import math
import os
import platform
import statistics
import subprocess
import sys

import build

SOURCES = ['tools/profile_completion.c', 'app/completion_locals.c', 'app/completion.c',
           'app/c_bindings.c', 'app/c_syntax.c', 'app/c_grammar.c', 'src/memory.c',
           'src/language/lexer.c', 'src/language/parser.c']


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--work', type=Path, required=True)
    parser.add_argument('--input', type=Path)
    parser.add_argument('--cursor', type=int, help='Byte offset, required with --input')
    parser.add_argument('--iterations', type=int, default=21)
    parser.add_argument('--config', choices=('Debug', 'Release'), default='Release')
    parser.add_argument('--compiler')
    parser.add_argument('--sanitizers', action='store_true')
    args = parser.parse_args()
    if (args.input is None) != (args.cursor is None):
        parser.error('--input and --cursor must be given together')
    if not 1 <= args.iterations <= 1000:
        parser.error('--iterations must be between 1 and 1000')
    work = build.artifact_paths.checked(args.work)
    if not work.is_relative_to((build.ROOT/'build').resolve()):
        parser.error('--work must be inside this repository\'s build directory')
    cases = []
    if args.input:
        data = build.artifact_paths.checked(args.input).read_bytes()
        if not 0 <= args.cursor <= len(data):
            parser.error('--cursor must be inside the source or at its end')
        if not data:
            parser.error('The cursor-refresh workload requires a nonempty source')
        cases.append(('input', data, args.cursor))
    else:
        data = (build.ROOT/'examples/pendulum/main.c').read_bytes()
        marker = b'ps_v3('
        at = data.rfind(marker)
        if at < 0:
            parser.error('The pendulum source no longer contains the expected call')
        cases.append(('pendulum', data, at+len(marker)))
        for count in (64, 512, 2048, 4096, 6144):
            data = ('void step(double dt){\n'+''.join(
                f' double value{i:05d}=dt+{i};\n' for i in range(count))+' consume(  );\n}\n').encode()
            cases.append((f'locals-{count}', data, data.index(b'consume(')+len(b'consume(')))
    if any(len(data) > 262144 for _, data, _ in cases):
        parser.error('Source exceeds the backend\'s 256KiB limit')
    work.mkdir(parents=True, exist_ok=False)
    native = work/'native'
    source_dir = work/'sources'
    source_dir.mkdir()
    temporary = work/'tmp'
    temporary.mkdir()
    # Also constrain compiler subprocess scratch when run without with-artifacts.
    for name in ('TMPDIR', 'TMP', 'TEMP'):
        os.environ[name] = str(temporary)
    metadata = dict(platform=platform.platform(), processor=platform.processor(),
        logical_cpus=os.cpu_count(), config=args.config, sanitizers=args.sanitizers,
        temporary_directory=str(temporary),
        timer='QPC (Windows), CLOCK_MONOTONIC (POSIX); wall time inside backend call',
        scope='Warm process/OS. No UI/frame/GPU/physical input acceptance.',
        percentiles='nearest rank; p95 at ceil(0.95*N)-1; p50 is median',
        modes=dict(syntax='Grammar plus flat node output; output allocation outside timer',
                   bindings='Syntax plus scope/call collector; output allocation outside timer',
                   cold='Fresh model per sample; destroy preceding model outside timer',
                   cursor='Same source, alternating adjacent UTF-8 boundaries; every sample recollects; syntax may be reused',
                   edit='Alternate inserting/deleting ASCII v at cursor; source bytes/revision and cursor change; input buffers outside timer/allocator metrics',
                   cached='Same owner, source and cursor; primed before timing'),
        memory='Tracked requested-live bytes, excluding tracker headers; includes caller output buffers. 64MiB guard. Not RSS. Default allocator memory metrics are null.',
        commands=[], records=[])
    # Include exact working-tree inputs, not merely a Git revision label. The
    # existing builder also records command/dependency signatures in native/.
    inputs = {build.ROOT/p for p in SOURCES+[__file__]}
    for folder in ('include', 'src/language', 'app',
                   'third_party/tree-sitter-0.27.1/lib', 'third_party/tree-sitter-c-0.24.2/src'):
        inputs.update(p for p in (build.ROOT/folder).rglob('*')
                      if p.is_file() and (p.suffix in ('.h', '.inc') or
                                         folder.startswith('third_party/') and p.suffix == '.c'))
    metadata['input_sha256'] = {p.relative_to(build.ROOT).as_posix(): digest(p) for p in sorted(inputs)}
    (work/'metadata.json').write_text(json.dumps(metadata, indent=2)+'\n', encoding='utf8')
    settings = SimpleNamespace(build_dir=native, compiler=args.compiler, no_app=True,
                               config=args.config, sanitizers=args.sanitizers, jobs=4)
    with build.build_lock(native):
        builder = build.Builder(settings, build.compiler_environment())
        program = builder.executable('physim-completion-profile', SOURCES, [])
        metadata.update(compiler=builder.cc, executable_sha256=digest(program))
        for index, (name, data, cursor) in enumerate(cases):
            path = source_dir/(name+'.c')
            path.write_bytes(data)
            for mode in ('syntax', 'bindings', 'cold', 'cursor', 'cached', 'edit'):
                # Alternate ordering between workloads to make drift visible;
                # this is not a paired same-process allocator-overhead proof.
                for allocator in (('default', 'tracked') if index%2 else ('tracked', 'default')):
                    command = [str(program), str(path), str(cursor), mode, str(args.iterations), allocator]
                    run = subprocess.run(command, env=builder.env, capture_output=True, text=True, timeout=180)
                    metadata['commands'].append(dict(command=command, exit_code=run.returncode,
                                                       stdout=run.stdout, stderr=run.stderr))
                    with (work/'commands.jsonl').open('a', encoding='utf8') as log:
                        log.write(json.dumps(metadata['commands'][-1])+'\n')
                    if run.returncode:
                        raise RuntimeError(f'Profile failed ({run.returncode}): {command}\n{run.stderr}')
                    rows = [json.loads(line) for line in run.stdout.splitlines()]
                    if len(rows) != args.iterations or any(r['status'] != 0 for r in rows):
                        raise RuntimeError('Incomplete or failed backend measurement')
                    if mode == 'cached' and allocator == 'tracked' and any(r['allocations'] for r in rows):
                        raise RuntimeError('The identical-snapshot cache allocated')
                    if mode in ('cursor', 'edit') and allocator == 'tracked' and any(not r['allocations'] for r in rows):
                        raise RuntimeError('A revision/cursor-refresh sample hit the result cache')
                    times = sorted(r['seconds']*1000 for r in rows)
                    if any(not math.isfinite(t) or t < 0 for t in times):
                        raise RuntimeError('Invalid wall time')
                    summary = dict(p50_ms=statistics.median(times), p95_ms=times[math.ceil(.95*len(times))-1],
                                   max_ms=max(times), counts=sorted({r['count'] for r in rows}),
                                   syntax_statuses=sorted({r['syntax_status'] for r in rows}))
                    metadata['records'].append(dict(name=name, source_bytes=len(data), source_sha256=digest(path),
                        cursor=cursor, mode=mode, allocator=allocator, summary=summary, rows=rows))
                    with (work/'measurements.jsonl').open('a', encoding='utf8') as log:
                        log.write(json.dumps(metadata['records'][-1])+'\n')
                    print(name, mode, allocator, json.dumps(summary), flush=True)
    # Keep original commands and raw samples even on failure. A report.json is
    # written only after the complete workload has finished successfully.
    (work/'report.json').write_text(json.dumps(metadata, indent=2)+'\n', encoding='utf8')


if __name__ == '__main__':
    main()
