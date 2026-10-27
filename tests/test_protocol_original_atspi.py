"""Independent AT-SPI full original, inactive scroll, selection and clipboard."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

from check_protocol_original import ORIGINAL

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app', type=Path, required=True)
parser.add_argument('--work', type=Path, required=True)
parser.add_argument('--inner', action='store_true')
args = parser.parse_args()
args.work.mkdir(parents=True, exist_ok=True)
if not args.inner:
    raise SystemExit(subprocess.call(['dbus-run-session', '--', '/usr/bin/python3',
        str(Path(__file__).resolve()), '--app', str(args.app.resolve()), '--work',
        str(args.work.resolve()), '--inner'], env=dict(os.environ, GSETTINGS_BACKEND='memory')))

import dbus
bus = dbus.SessionBus()
status = bus.get_object('org.a11y.Bus', '/org/a11y/bus')
dbus.Interface(status, 'org.freedesktop.DBus.Properties').Set('org.a11y.Status', 'IsEnabled', dbus.Boolean(True))
os.environ['AT_SPI_BUS_ADDRESS'] = str(dbus.Interface(status, 'org.a11y.Bus').GetAddress())
import pyatspi
from gi.repository import GLib

reader = args.work / 'reader'
expected = ORIGINAL.decode('utf8')
app_hash = hashlib.sha256(args.app.read_bytes()).hexdigest()
out = (args.work / 'app.stdout').open('wb')
err = (args.work / 'app.stderr').open('wb')
process = subprocess.Popen([str(args.app), '--workspace-state-test', str(reader), 'protocol-ui-wrap-remote'],
    stdout=out, stderr=err, env=dict(os.environ, NO_AT_BRIDGE='0', LIBGL_ALWAYS_SOFTWARE='1'))
events = []

def event(value):
    events.append(dict(type=value.type, offset=value.detail1, length=value.detail2))

pyatspi.Registry.registerEventListener(event, 'object:text-selection-changed', 'object:text-caret-moved',
                                      'object:state-changed:focused')

def wait(function, label):
    deadline = time.monotonic() + 30
    last = None
    while time.monotonic() < deadline:
        context = GLib.MainContext.default()
        while context.pending():
            context.iteration(False)
        try:
            value = function()
            if value:
                return value
        except Exception as error:
            last = repr(error)
        if process.poll() is not None:
            raise RuntimeError('App stopped before '+label+': '+(args.work / 'app.stderr').read_text())
        time.sleep(.025)
    raise RuntimeError('Timeout: '+label+'; '+str(last))

def ready(marker):
    return wait(lambda: marker in (args.work / 'app.stdout').read_text(encoding='utf8'), marker)

def original_field():
    for app in pyatspi.Registry.getDesktop(0):
        if app.name != 'Physim' or app.get_process_id() != process.pid:
            continue
        stack = [(node, 0) for node in app]
        while stack:
            node, depth = stack.pop()
            if node.name == 'Originalmeldung' and node.getRole() == pyatspi.ROLE_ENTRY:
                return node
            if depth < 8:
                stack.extend((child, depth+1) for child in node)
    return None

try:
    ready('PROTOCOL ORIGINAL REMOTE READY')
    node = wait(original_field, 'native Originalmeldung')
    text = node.queryText()
    editable = node.queryEditableText()
    assert not node.getState().contains(pyatspi.STATE_EDITABLE)
    assert text.characterCount == len(expected) and text.getText(0, -1) == expected
    assert not editable.setTextContents('must remain readonly')
    assert text.getText(0, -1) == expected
    before = dict(caret=text.caretOffset, selections=text.getNSelections(),
                  focused=node.getState().contains(pyatspi.STATE_FOCUSED))
    assert not before['focused'] and before['selections'] == 0
    hidden = text.getCharacterExtents(len(expected)-1, pyatspi.WINDOW_COORDS)
    assert hidden[2] == 0 and hidden[3] == 0, hidden
    assert text.scrollSubstringTo(len(expected)-1, len(expected), pyatspi.SCROLL_TOP_LEFT)
    def final_visible():
        box = text.getCharacterExtents(len(expected)-1, pyatspi.WINDOW_COORDS)
        return box if box[2] > 0 and box[3] > 0 else None
    visible = wait(final_visible, 'last glyph visible after inactive native scroll')
    assert text.caretOffset == before['caret'] and text.getNSelections() == before['selections']
    assert node.getState().contains(pyatspi.STATE_FOCUSED) == before['focused']
    assert text.getText(0, -1) == expected
    (reader / 'scroll-checked').write_text('ready', encoding='utf8')
    ready('PROTOCOL ORIGINAL REMOTE END')
    assert node.queryComponent().grabFocus()
    wait(lambda: node.getState().contains(pyatspi.STATE_FOCUSED), 'actual original focus')
    (reader / 'focus-applied').write_text('ready', encoding='utf8')
    ready('PROTOCOL ORIGINAL REMOTE SELECTED')
    wait(lambda: text.getNSelections() == 1 and tuple(text.getSelection(0)) == (0, len(expected)),
         'full native Unicode selection')
    assert text.getText(*text.getSelection(0)) == expected
    (reader / 'native-copy-requested').write_text('ready', encoding='utf8')
    editable.copyText(0, len(expected))
    (reader / 'selected-checked').write_text('ready', encoding='utf8')
    assert process.wait(timeout=30) == 0
    output = (args.work / 'app.stdout').read_text(encoding='utf8')
    states = [json.loads(line) for line in output.splitlines()
              if line.startswith('{"kind":"protocol_original_remote"')]
    assert 'PROTOCOL ORIGINAL REMOTE SELF-TEST: PASSED' in output
    assert [row['stage'] for row in states] == list(range(3, 10))
    assert all(row['passed'] and row['bytes'] == len(ORIGINAL) for row in states)
    assert states[-1]['native_copy'] and hashlib.sha256(args.app.read_bytes()).hexdigest() == app_hash
    (args.work / 'verification.json').write_text(json.dumps(dict(app_sha256=app_hash,
        original_bytes=len(ORIGINAL), original_characters=len(expected), independent_atspi_client=True,
        readonly_replacement_rejected=True, full_value_and_selection=True,
        inactive_scroll=dict(before=before, hidden_last_glyph=hidden, visible_last_glyph=visible,
                             caret_selection_focus_value_unchanged=True),
        native_copy_exact_clipboard_checked_by_app=True, source_unchanged=True,
        states=states, events=events, screenshots_created=len(list(reader.glob('*.png')))),
        indent=2)+'\n', encoding='utf8')
finally:
    if process.poll() is None:
        process.kill()
        process.wait()
    out.close()
    err.close()
print('Native Original AT-SPI: full 64 KiB, readonly rejection, inactive scroll, selection and native clipboard passed')
