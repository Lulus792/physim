"""Compare native detail geometry with the selected C reference's CSS rules."""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import re
import subprocess

MODES = ('dark', 'light', 'contrast', 'large', 'narrow', 'long')

def require(value, message):
    if not value:
        raise RuntimeError(message)

def expected_css(root):
    css = (root / 'docs/visuals/design-options/options.css').read_text(encoding='utf8')
    css = css[:css.index('@media')]
    rules = {selector.strip(): body for selector, body in re.findall(r'([^{}]+)\{([^{}]*)\}', css)}
    def declaration(selector, name):
        value = re.search(r'(?:^|;)'+re.escape(name)+r':([^;]+)', rules[selector])
        require(value, 'Missing reference declaration '+selector+' '+name)
        return value[1].strip()
    def number(selector, name, unit=''):
        value = declaration(selector, name)
        require(not unit or value.endswith(unit), 'Unexpected CSS unit')
        return float(value[:-len(unit)] if unit else value)
    root_em = number(':root', 'font-size', 'px')
    line = float(declaration('body', 'font').split('/')[1].split()[0])
    body_size = number('.log-impact', 'font-size', 'rem')*root_em
    badge_font = number('.badge', 'font-size', 'rem')*root_em
    badge_padding = float(declaration('.badge', 'padding').split()[0][:-3])*root_em
    summary_padding = float(declaration('.log-detail details summary', 'padding').split()[0][:-2])
    return dict(meta_font=number('small,.small', 'font-size', 'rem')*root_em,
        title_line=number('.log-detail h3', 'font-size', 'rem')*root_em*line,
        impact_line=body_size*line, property_line=body_size*line,
        badge_height=badge_font*number('.badge', 'line-height')+2*badge_padding+2,
        title_margin=float(declaration('.log-detail h3', 'margin').split()[0][:-2]),
        impact_margin=float(declaration('.log-impact', 'margin').split()[0][:-2]),
        detail_margin=float(declaration('.log-detail details', 'margin').split()[0][:-2]),
        property_gap=number('.property-list', 'gap', 'px'),
        column_gap=number('.property-list>div', 'gap', 'px'),
        summary_height=body_size*line+2*summary_padding)

def verify(output, expected):
    rows = [json.loads(line) for line in output.splitlines() if line.startswith('{"kind":"protocol_typography"')]
    require(len(rows) == 1, 'Missing native detail measurements')
    row = rows[0]
    require(row['passed'] and 'PROTOCOL TYPOGRAPHY SELF-TEST: PASSED' in output, 'Native detail contract failed')
    scale = row['scale']
    def close(actual, reference, name):
        require(math.isclose(actual, reference, abs_tol=.06), name+': '+str((actual, reference)))
    close(row['meta_font'], expected['meta_font'], 'metadata font')
    for key in ('title_line', 'impact_line', 'property_line'):
        close(row[key], expected[key]*scale, key)
    close(row['badge_height'], expected['badge_height']*scale, 'badge height')
    header, title, impact, summary = (row[key] for key in ('header', 'title', 'impact', 'summary'))
    close(title[1]-header[1]-header[3], expected['title_margin']*scale, 'title margin')
    close(impact[1]-title[1]-title[3], expected['impact_margin']*scale, 'impact top margin')
    properties = row['properties']
    require(len(properties) >= 2, 'Missing actual property rows')
    close(properties[0]['row'][1]-impact[1]-impact[3], expected['impact_margin']*scale, 'impact bottom margin')
    for index, prop in enumerate(properties):
        label, value = prop['label'], prop['value']
        close(value[0]-label[0]-label[2], expected['column_gap']*scale, 'property column gap')
        close(label[2], value[2], 'equal property columns')
        if index:
            previous = properties[index-1]['row']
            close(prop['row'][1]-previous[1]-previous[3], expected['property_gap']*scale, 'property row gap')
    last = properties[-1]['row']
    close(summary[1]-last[1]-last[3], expected['detail_margin']*scale, 'details top margin')
    close(summary[3], expected['summary_height']*scale, 'summary height')
    return row

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app', type=Path, required=True)
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--work', type=Path, required=True)
    args = parser.parse_args()
    args.work.mkdir(parents=True, exist_ok=True)
    expected = expected_css(args.root)
    program = hashlib.sha256(args.app.read_bytes()).hexdigest()
    cases = []
    for mode in MODES:
        work = args.work / mode
        work.mkdir()
        command = [str(args.app), '--workspace-state-test', str(work), 'protocol-ui-type-'+mode]
        result = subprocess.run(command, capture_output=True, timeout=90, env=dict(os.environ, PHYSIM_TEST_TRACE='1'))
        (work / 'stdout.bin').write_bytes(result.stdout)
        (work / 'stderr.bin').write_bytes(result.stderr)
        require(result.returncode == 0, 'Native typography process failed: '+mode)
        state = verify(result.stdout.decode('utf8', errors='replace'), expected)
        require(hashlib.sha256(args.app.read_bytes()).hexdigest() == program, 'Program changed during test')
        cases.append(dict(mode=mode, command=command, exit_code=result.returncode, state=state))
        (args.work / 'verification.json').write_text(json.dumps(dict(app_sha256=program, expected_css=expected, cases=cases), indent=2)+'\n', encoding='utf8')
    print('Native C detail typography: six theme/size/wrap states match CSS font roles, line boxes and block/column gaps')

if __name__ == '__main__':
    main()
