"""Build a Windows counterexample without replacing the verified app.

Use a completed Release build and an immutable source snapshot. Both the
counterexample and the unchanged app run the same native window fixture.
Generated sources, runtime bytes and logs are retained in a fresh proof folder.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import zipfile


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def archive(path, files):
    members = {item.name: digest(item) for item in files}
    if len(members) != len(files):
        raise RuntimeError('Archive members must have distinct names')
    with zipfile.ZipFile(path, 'x', compression=zipfile.ZIP_DEFLATED) as target:
        for item in files:
            target.write(item, item.name)
    with zipfile.ZipFile(path) as target:
        for name, expected in members.items():
            assert hashlib.sha256(target.read(name)).hexdigest() == expected
    return members


def replace_once(text, old, new):
    if text.count(old) != 1:
        raise RuntimeError('Source changed; review counterexample marker: ' + old)
    return text.replace(old, new)


def main():
    repository = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source-root', type=Path, default=repository)
    parser.add_argument('--build-dir', type=Path, required=True)
    parser.add_argument('--sdl', type=Path, required=True)
    parser.add_argument('--proof-dir', type=Path, required=True)
    parser.add_argument('--report', type=Path)
    parser.add_argument('--fault', choices=('foreign-input', 'footer-wrap', 'font-reflow'), required=True)
    args = parser.parse_args()
    if sys.platform != 'win32':
        parser.error('This counterexample verifies the Windows Release build')
    source, directory, proof = (path.resolve() for path in
                                 (args.source_root, args.build_dir, args.proof_dir))
    if directory not in proof.parents:
        parser.error('Keep generated compiler sources inside the chosen build folder')
    sys.path.insert(0, str(source / 'tools'))
    import build  # The pinned snapshot supplies its own compiler recipe.

    def ref(path):
        try:
            return path.relative_to(repository).as_posix()
        except ValueError:
            return path.as_posix()

    options = argparse.Namespace(build_dir=directory, config='Release', compiler=None,
        no_app=False, sdl=args.sdl.resolve(), sanitizers=False, jobs=4, rebuild=False)
    report_path = args.report.resolve() if args.report else proof / 'report.json'
    # Hold the normal builder lock through both probes so a second build cannot
    # replace the production executable between its snapshot and its readback.
    with build.build_lock(directory):
        proof.mkdir()
        normal = directory / 'bin/physim.exe'
        normal_files = [normal, normal.with_suffix('.pdb')]
        production_members = archive(proof / 'production.zip', normal_files)
        originals = [source / name for name in ('app/project_manager_ui.inc',
            'app/project_wizard_ui.inc', 'app/design_ui.inc', 'app/main.c',
            'app/project_window_tests.inc', 'tests/check_project_window.py')]
        manager = originals[0].read_text(encoding='utf8')
        generated = []
        stage = 8 if args.fault == 'foreign-input' else 10
        if args.fault == 'foreign-input':
            marker = 'if(!a->manager_window || window!=a->manager_window)return false;'
            manager = replace_once(manager, marker,
                'if(a->manager_window && window!=a->manager_window)return true;\n    ' + marker)
        else:
            marker = 'group=fminf(total,available)' if args.fault == 'footer-wrap' else 'if(previous_height>0 && fabsf(previous_height-a->manager_fonts[4].height)>.01f && a->manager_focus_known)a->manager_focus_reveal=true;'
            replacement = 'group=total' if args.fault == 'footer-wrap' else '/* Deliberately omit reveal after live font reflow. */'
            renderer = replace_once(originals[1].read_text(encoding='utf8'), marker, replacement)
            renderer_path = proof / 'counter_wizard.inc'
            renderer_path.write_text(renderer, encoding='utf8')
            generated.append(renderer_path)
            manager = replace_once(manager, '#include "project_wizard_ui.inc"',
                '#include "' + renderer_path.as_posix() + '"')
        manager_path = proof / 'counter_manager.inc'
        manager_path.write_text(manager, encoding='utf8')
        design_path = proof / 'counter_design.inc'
        design_path.write_text(replace_once(originals[2].read_text(encoding='utf8'),
            '#include "project_manager_ui.inc"', '#include "' + manager_path.as_posix() + '"'), encoding='utf8')
        main_path = proof / 'counter_main.c'
        main_path.write_text(replace_once(originals[3].read_text(encoding='utf8'),
            '#include "design_ui.inc"', '#include "' + design_path.as_posix() + '"'), encoding='utf8')
        generated.extend((manager_path, design_path, main_path))
        source_members = archive(proof / 'source.zip', generated + originals + [Path(__file__).resolve()])
        builder = build.Builder(options, build.compiler_environment())
        libraries = [builder.lib / ('physim-' + name + '.lib')
                     for name in ('project', 'batch', 'platform', 'language', 'zlib', 'core')]
        if not all(path.is_file() for path in libraries):
            raise RuntimeError('Complete the normal Release build before this probe')
        inputs = [str(main_path) if name == 'main' else 'app/' + name + '.c'
                  for name in build.APP] + build.LINEBREAK + build.GRAPHEME
        counter = builder.executable('physim-project-' + args.fault + '-counter',
            inputs, libraries, sdl=True, defines=('Z_PREFIX', 'PS_SOURCE_DIR="' + source.as_posix() + '"'))
        cases = []
        for variant, app in (('counter', counter), ('positive', normal)):
            command = [sys.executable, str(source / 'tests/check_project_window.py'),
                       '--app', str(app), '--work', str(proof / variant)]
            result = subprocess.run(command, cwd=source, capture_output=True,
                                    timeout=600, env=builder.env)
            log = proof / (variant + '.log')
            log.write_bytes(result.stdout + result.stderr)
            cases.append(dict(variant=variant, command=command, exit_code=result.returncode,
                binary=ref(app), binary_sha256=digest(app), log=ref(log), log_sha256=digest(log)))
            if variant == 'counter':
                assert result.returncode != 0 and ('Independent project window stage%d failed' % stage).encode() in result.stderr, result.stderr[-3000:]
                if args.fault == 'font-reflow':
                    assert b'font_stage=1' in result.stderr, 'Expected failure after the first live font change'
            else:
                assert result.returncode == 0, result.stderr[-3000:]
        for path in normal_files:
            assert digest(path) == production_members[path.name], 'Production runtime changed'
        runtime_members = archive(proof / 'runtime.zip', [counter, counter.with_suffix('.pdb')])
        record = dict(fault=args.fault, expected_failed_stage=stage, invocation=sys.argv,
            source_archive=ref(proof / 'source.zip'), source_members=source_members,
            production_archive=ref(proof / 'production.zip'), production_members=production_members,
            runtime_archive=ref(proof / 'runtime.zip'), runtime_members=runtime_members,
            cases=cases, production_binary_unchanged=True, all_archive_bytes_read_back=True)
        with report_path.open('x', encoding='utf8') as stream:
            json.dump(record, stream, indent=2)
            stream.write('\n')
    print('PROJECT WINDOW COUNTEREXAMPLE: %s rejected at stage%d; unchanged production passed' % (args.fault, stage))


if __name__ == '__main__':
    main()
