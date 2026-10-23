from pathlib import Path
import json,sys
proof=Path(__file__).resolve().parent;root=proof.parents[3]
sys.path.insert(0,str(root/'tests'))
from check_display_workflow import compact_log_ink
record=json.loads((proof/'file-tabs-compact-counterexample.json').read_text(encoding='utf-8'))
ink=compact_log_ink(proof/'windows-c-contrast-large-narrow-log-end.png',record['clip'],record['row'],False)
try:compact_log_ink(proof/'negative-source5-log-text.png',record['clip'],record['row'],False)
except RuntimeError:print('Compact diagnostic: corrected image accepted, blank original rejected; ink='+str(ink))
else:raise AssertionError('Blank original accepted')
