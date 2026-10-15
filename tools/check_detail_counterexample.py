"""Require the old F1 metadata wrapping to fail the current glyph/clip probe."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

import build


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default='clang')
    parser.add_argument('--sdl', type=Path, required=True)
    parser.add_argument('--build-dir', type=Path, default=build.ROOT/'build/native/Debug-sanitized')
    args = parser.parse_args()
    if sys.platform not in ('linux', 'darwin'):
        parser.error('Use the actual Linux or macOS font environment for this counterexample')
    directory = args.build_dir.resolve()
    proof = directory.parent/'detail-counterexample'
    proof.mkdir()  # Never overwrite an earlier proof.
    (proof/'workflow').mkdir()
    ui = (build.ROOT/'app/documentation_ui.inc').read_text(encoding='utf-8')
    start = ui.index('/* Detail text uses the same bounded UTF-8 line breaks')
    end = ui.index('static void documentation_view(', start)
    previous = ui[:start]+'''static struct nk_rect documentation_detail_label(app *a,const char *detail) {
    struct nk_context *ui=a->doc_ui;
    nk_layout_row_dynamic(ui,documentation_height(a->doc_font_ui,detail,strlen(detail),
        fmaxf(40,nk_window_get_content_region(ui).w-8))+8,1);
    struct nk_rect bounds=nk_widget_bounds(ui);
    nk_style_push_color(ui,&ui->style.text.color,a->doc_error[0]?UI_COLOR(error):UI_COLOR(muted));
    nk_label_wrap(ui,detail);nk_style_pop_color(ui);
    bounds.h=nk_widget_height(ui);return bounds;
}
'''+ui[end:]
    # Restore the previous ASCII value/unit formatting as well.
    formatter=previous.index('    char detail[192];')
    previous=previous[:formatter]+previous[formatter:].replace(r'%u\xC2\xA0px','%u px')
    probe_start=ui.index('static void documentation_detail_draw_probe(')
    probe_end=ui.index('static bool documentation_zoom_button(',probe_start)
    assert previous[probe_start:probe_end]==ui[probe_start:probe_end], 'Keep the independent probe unchanged'
    (proof/'documentation_ui.inc').write_text(previous, encoding='utf-8')
    source = (build.ROOT/'app/main.c').read_text(encoding='utf-8')
    main_source = proof/'main.c'
    main_source.write_text('/* isolated metadata '+hashlib.sha256(previous.encode()).hexdigest()+' */\n'+source,
                           encoding='utf-8')
    options = argparse.Namespace(build_dir=directory, compiler=args.compiler, sdl=args.sdl,
        config='Debug', no_app=False, sanitizers=True, jobs=4, rebuild=False)
    with build.build_lock(directory):
        builder = build.Builder(options, dict(os.environ))
        libraries = [builder.lib/('libphysim-'+name+'.a')
                     for name in ('project','batch','platform','language','zlib','core')]
        if not all(p.is_file() for p in libraries):
            raise RuntimeError('Build the current sanitized viewer before the counterexample')
        sources = [str(main_source) if name=='main' else f'app/{name}.c' for name in build.APP]
        binary = builder.executable('physim-detail-counterexample', sources, libraries, sdl=True,
            defines=('Z_PREFIX', f'PS_SOURCE_DIR="{build.ROOT.as_posix()}"'))
    command = [sys.executable, str(build.ROOT/'tests/check_display_workflow.py'), 'documentation_detail',
        '--app', str(binary), '--root', str(build.ROOT), '--work', str(proof/'workflow')]
    result = subprocess.run(command, cwd=build.ROOT, capture_output=True, timeout=120)
    (proof/'stdout').write_bytes(result.stdout)
    (proof/'stderr').write_bytes(result.stderr)
    steps_path = proof/'workflow/app-steps.json'
    steps = json.loads(steps_path.read_text(encoding='utf-8')) if steps_path.is_file() else []
    output = '\n'.join(step.get('stdout','') for step in steps)
    rejected_states = re.findall(r'DOCUMENTATION DETAIL DRAW: case=(\d+) font=(\d+) theme=(\d+) lines=(\d+) complete=0', output)
    rejected = result.returncode != 0 and bool(rejected_states) and 'DOCUMENTATION DETAIL SELF-TEST: FAILED' in output
    report = dict(command=command, exit_code=result.returncode, precise_rejection=rejected,
        source_sha256=hashlib.sha256(ui.encode()).hexdigest(),
        isolated_previous_layout_sha256=hashlib.sha256(previous.encode()).hexdigest(),
        binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(), rejected_states=rejected_states)
    (proof/'report.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    if not rejected:
        raise RuntimeError('Old metadata layout did not reach and fail the actual glyph/clip probe')
    print('DETAIL COUNTEREXAMPLE: actual previous wrapping rejected by glyph/clip probe')


if __name__ == '__main__':
    main()
