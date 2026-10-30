"""Native Explorer A owners stay independent while the main workspace changes."""
import argparse,hashlib,json,os,re,subprocess
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
for name in ('app','work'):p.add_argument('--'+name,type=Path,required=True)
a=p.parse_args();a.app=a.app.resolve();a.work.mkdir(parents=True,exist_ok=True)
binary=hashlib.sha256(a.app.read_bytes()).hexdigest();cases=[]
css=(Path(__file__).resolve().parents[1]/'docs/visuals/design-options/options.css').read_text(encoding='utf8')
compact=re.search(r'\.browser-places \.navigation-list button\{([^}]*)\}',css)[1]
def value(body,name):return re.search(r'(?:^|;)'+re.escape(name)+r':([^;]+)',body)[1]
compact_padding=float(value(compact,'padding')[:-3])*14
body=re.search(r'(?:^|\})body\{([^}]*)\}',css)[1]
line_height=float(re.search(r'/([\d.]+)',value(body,'font'))[1])
compact_pitch=float(value(compact,'font-size')[:-3])*14*line_height
compact_gap=float(value(compact,'gap')[:-2])
icon=float(value(re.search(r'svg\.icon\{([^}]*)\}',css)[1],'width')[:-2])
base_button=re.search(r'(?:^|\})button\{([^}]*)\}',css)[1]
minimum=float(value(base_button,'min-height')[:-2]);border=float(value(base_button,'border').split()[0][:-2])
for mode in ('dark','light','contrast','large','small','large-small'):
    work=a.work/mode;work.mkdir();command=[str(a.app),'--workspace-state-test',str(work),'explorer-a-windows-'+mode]
    result=subprocess.run(command,capture_output=True,timeout=90,env=dict(os.environ,PHYSIM_TEST_TRACE='1'))
    (work/'stdout.bin').write_bytes(result.stdout);(work/'stderr.bin').write_bytes(result.stderr)
    rows=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"independent_picker_windows"')]
    places=[json.loads(line) for line in result.stdout.decode('utf8',errors='replace').splitlines() if line.startswith('{"kind":"independent_picker_places"')]
    cases.append(dict(mode=mode,command=command,exit_code=result.returncode,measurements=rows,places=places))
    (a.work/'verification.json').write_text(json.dumps(dict(binary_sha256=binary,cases=cases),indent=2)+'\n')
    assert result.returncode==0 and b'INDEPENDENT PICKER WINDOWS SELF-TEST: PASSED' in result.stdout,(mode,result.stderr.decode('utf8',errors='replace'))
    assert len(rows)==1 and all(rows[0][key] for key in ('native_handles_distinct','main_navigation','main_edit_and_save','detached_input_and_return','independent_results','reinvoke_preserves_form','parent_close_scoped','main_change_scoped'))
    assert rows[0]['live_font_cycles']==2 and rows[0]['wide_resize_stage']==2
    assert len(places)==(1 if 'small' in mode else 0)
    for entry in places:
        assert len(entry['items'])==3;f=entry['scale'];pitch=compact_pitch*f
        height=max(minimum*f,max(max(icon*f,item['ink'][3]+pitch-entry['font_height'])+2*(compact_padding+border)*f for item in entry['items']))
        for item in entry['items']:
            button,ink=item['button'],item['ink']
            assert abs(ink[0]-button[0]-(compact_padding+border+icon+compact_gap)*f)<.05,'Compact place inset differs from approved CSS'
            assert abs(button[3]-height)<.05,'Compact place row differs from approved CSS'
    assert hashlib.sha256(a.app.read_bytes()).hexdigest()==binary
print('Independent A: distinct OS owners, simultaneous B/main selections, main navigation/edit/save, scoped results/close/project change, repeat invocation, late reply, resize/live fonts and shutdown with child open passed')
