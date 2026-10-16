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
    parser.add_argument('--proof-dir', type=Path, help='Fresh evidence directory; never overwritten')
    parser.add_argument('--bounded-control', action='store_true',
                        help='Require the corrected wrapper to pass the same historical environment')
    args = parser.parse_args()
    if sys.platform not in ('linux', 'darwin'):
        parser.error('Use the actual Linux or macOS font environment for this counterexample')
    directory = args.build_dir.resolve()
    proof = args.proof_dir.resolve() if args.proof_dir else directory.parent/(
        'detail-bounded-control' if args.bounded_control else 'detail-counterexample')
    proof.mkdir()  # Never overwrite an earlier proof.
    (proof/'workflow').mkdir()
    ui = (build.ROOT/'app/documentation_ui.inc').read_text(encoding='utf-8')
    start = ui.index('/* Detail text uses the same bounded UTF-8 line breaks')
    end = ui.index('static void documentation_view(', start)
    previous = ui[:start]+'''static struct nk_rect documentation_detail_label(app *a,const char *detail) {
    struct nk_context *ui=a->doc_ui;
    /* The historical label used the reading font for both measuring/drawing.
     * H now pushes a compact control font around this call. Restore the old
     * drawing font as well, or this is a different, accidentally safer layout. */
    nk_style_push_font(ui,a->doc_font_ui);
    nk_layout_row_dynamic(ui,documentation_height(a->doc_font_ui,detail,strlen(detail),
        fmaxf(40,nk_window_get_content_region(ui).w-8))+8,1);
    struct nk_rect bounds=nk_widget_bounds(ui);
    nk_style_push_color(ui,&ui->style.text.color,a->doc_error[0]?UI_COLOR(error):UI_COLOR(muted));
    nk_label_wrap(ui,detail);nk_style_pop_color(ui);
    bounds.h=nk_widget_height(ui);nk_style_pop_font(ui);return bounds;
}
'''+ui[end:]
    if args.bounded_control:
        bounded=ui[start:end]
        font='const struct nk_user_font *font=documentation_chrome_font(a);'
        assert bounded.count(font)==1, 'Review the bounded label font'
        bounded=bounded.replace(font,'const struct nk_user_font *font=a->doc_font_ui;')
        previous=ui[:start]+bounded+ui[end:]
    # Restore the previous ASCII value/unit formatting as well.
    formatter=previous.index('    char detail[192];')
    previous=previous[:formatter]+previous[formatter:].replace(r'%u\xC2\xA0px','%u px')
    compact='    bool compact=true;'
    assert previous.count(compact)==1, 'Review the metadata formatting policy'
    previous=previous.replace(compact,'    bool compact=documentation_font_size(a)>=24;')
    probe_start=ui.index('static void documentation_detail_draw_probe(')
    probe_end=ui.index('static bool documentation_zoom_button(',probe_start)
    assert previous[probe_start:probe_end]==ui[probe_start:probe_end], 'Keep the independent probe unchanged'
    (proof/'documentation_ui.inc').write_text(previous, encoding='utf-8')
    # The old case 9 had a 404-pixel metadata row, not H's 312-pixel row.
    # Wrapping failures depend on available width. Reconstruct the historical
    # article rectangle/padding from 2b31a7d^, while retaining the current probe.
    window=(build.ROOT/'app/documentation_window.inc').read_text(encoding='utf-8')
    context='    ps_doc_layout layout=ps_documentation_layout('
    assert window.count(context)==1
    # The pre-H reader had no right context pane. Otherwise its tab labels lie
    # inside the historical article rectangle and contaminate the draw probe
    # (65 observed bytes for 51 expected, even in the bounded control).
    window=window.replace(context,'    a->doc_context_hidden=true;\n'+context)
    placement='    a->doc_left_slot=nk_rect(layout.left.x,layout.left.y,layout.left.w,layout.left.h);'
    assert window.count(placement)==1
    window=window.replace(placement,'''    float side=(w<1000?220.f:270.f)*(documentation_font_size(a)/16.f);
    side=fminf(side,(float)w*.4f);
    layout.article=(ps_doc_rect){side+16,8,(float)w-side-24,(float)h-16};
'''+placement)
    padding='nk_vec2(w<1000?4:12,12)'
    assert window.count(padding)==1, 'Review the historical article padding'
    window=window.replace(padding,'nk_vec2(14,14)')
    (proof/'documentation_window.inc').write_text(window,encoding='utf-8')
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
    complete_states=re.findall(r'DOCUMENTATION DETAIL DRAW: case=(\d+) font=\d+ theme=\d+ lines=\d+ complete=1',output)
    controlled=result.returncode==0 and list(map(int,complete_states))==list(range(36)) and 'DOCUMENTATION DETAIL SELF-TEST: PASSED' in output
    report = dict(command=command, exit_code=result.returncode, precise_rejection=rejected,
        mode='bounded-control' if args.bounded_control else 'historical-counterexample',
        bounded_control_passed=controlled if args.bounded_control else None,
        restored_reading_font=True,
        restored_historical_article_geometry=True,
        draw_records=len(re.findall(r'DOCUMENTATION DETAIL DRAW:',output)),
        self_test_passed='DOCUMENTATION DETAIL SELF-TEST: PASSED' in output,
        source_sha256=hashlib.sha256(ui.encode()).hexdigest(),
        isolated_previous_layout_sha256=hashlib.sha256(previous.encode()).hexdigest(),
        isolated_window_sha256=hashlib.sha256(window.encode()).hexdigest(),
        binary_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(), rejected_states=rejected_states)
    (proof/'report.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    if args.bounded_control:
        if not controlled:
            raise RuntimeError('Corrected wrapper failed the identical historical font/rectangle/format control')
        print('DETAIL CONTROL: corrected wrapper passed all 36 historical font/rectangle/format states')
        return
    if not rejected:
        raise RuntimeError('Old metadata layout did not reach and fail the actual glyph/clip probe '
            f'(exit={result.returncode}, draw_records={report["draw_records"]}, '
            f'self_test_passed={report["self_test_passed"]})')
    print('DETAIL COUNTEREXAMPLE: actual previous wrapping rejected by glyph/clip probe')


if __name__ == '__main__':
    main()
