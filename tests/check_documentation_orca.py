"""Real Orca control announcements in the normal app; run on a private X11 display.

Requires Orca, PyAT-SPI, python-dbus, Speech Dispatcher with its Python client,
espeak-ng, Openbox and xdotool. This checks control announcements, not complete
document/math reading or audible output: synthesis uses an ALSA null sink.

Example: xvfb-run -a dbus-run-session -- python3 tests/check_documentation_orca.py
  --app build/native/Debug-sanitized/bin/physim --work build/native/orca-controls
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import time
import traceback

import dbus
import pyatspi
from gi.repository import GLib

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app', required=True, type=Path)
parser.add_argument('--work', required=True, type=Path)
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
app_path = args.app.resolve()
work = args.work.resolve()
work.mkdir(parents=True, exist_ok=False)
runtime = Path(tempfile.mkdtemp(prefix='physim-orca-'))
env = dict(os.environ, XDG_RUNTIME_DIR=str(runtime), XDG_CONFIG_HOME=str(work/'config'),
           XDG_DATA_HOME=str(work/'data'), XDG_CACHE_HOME=str(work/'cache'),
           GSETTINGS_BACKEND='memory', ASAN_OPTIONS='detect_leaks=0',
           UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1', LIBGL_ALWAYS_SOFTWARE='1')
os.environ.update(env)
children, files, checkpoints = [], [], []
report = dict(status='failed', app_sha256=hashlib.sha256(app_path.read_bytes()).hexdigest(),
              scope='Orca control announcements; synthesis to ALSA null, no audio audition',
              checkpoints=checkpoints)


def launch(command, name):
    out, err = (work/(name+'.stdout')).open('wb'), (work/(name+'.stderr')).open('wb')
    files.extend([out, err])
    child = subprocess.Popen(command, cwd=root, env=env, stdout=out, stderr=err)
    children.append(child)
    return child


def wait(predicate, label, timeout=45):
    deadline, last = time.monotonic()+timeout, None
    while time.monotonic() < deadline:
        context = GLib.MainContext.default()
        while context.pending():
            context.iteration(False)
        try:
            value = predicate()
            if value:
                return value
        except Exception as error:
            last = repr(error)
        time.sleep(.05)
    raise AssertionError(label+': '+str(last))


def xrun(*command):
    return subprocess.check_output(['xdotool', *map(str, command)], env=env, text=True, timeout=8).strip()


def key(chord):
    xrun('key', '--clearmodifiers', chord)


def f1_window():
    for application in pyatspi.Registry.getDesktop(0):
        if application.name == 'Physim' and application.get_process_id() == app.pid:
            for window in application:
                if window.name == 'Physim | Dokumentation':
                    return window
    return None


def focused(name):
    window = f1_window()
    if window is None:
        return False
    window.clearCache()
    if not window.getState().contains(pyatspi.STATE_ACTIVE):
        return False
    for node in window:
        node.clearCache()
        if node.name == name and node.getState().contains(pyatspi.STATE_FOCUSED):
            return node
    return False


def checkpoint(name, text=None):
    node = wait(lambda: focused(name), 'native focused control '+name)
    if text is not None:
        wait(lambda: node.queryText().getText(0, -1) == text, 'native search value')
    checkpoints.append(dict(name=name, role=node.getRoleName(), text=text))
    print('Native focus:', name, flush=True)
    time.sleep(1)


try:
    launch(['openbox'], 'openbox')
    subprocess.run(['python3', str(root/'tools/wait-window-manager.py')], env=env, check=True, timeout=35)
    bus = dbus.SessionBus()
    status = bus.get_object('org.a11y.Bus', '/org/a11y/bus')
    dbus.Interface(status, 'org.freedesktop.DBus.Properties').Set('org.a11y.Status', 'IsEnabled', dbus.Boolean(True))
    address = str(dbus.Interface(status, 'org.a11y.Bus').GetAddress())
    env['AT_SPI_BUS_ADDRESS'] = address
    os.environ['AT_SPI_BUS_ADDRESS'] = address
    config = work/'speech-config'
    (config/'modules').mkdir(parents=True)
    (config/'modules/espeak-ng.conf').write_bytes(Path('/etc/speech-dispatcher/modules/espeak-ng.conf').read_bytes())
    (config/'speechd.conf').write_text('LogLevel 4\nDefaultLanguage "de"\nAudioOutputMethod "alsa"\n'
                                    'AudioALSADevice "null"\nAddModule "espeak-ng" "sd_espeak-ng" "espeak-ng.conf"\n'
                                    'DefaultModule "espeak-ng"\n')
    (work/'speech-log').mkdir()
    socket = runtime/'speechd.sock'
    env['SPEECHD_ADDRESS'] = 'unix_socket:'+str(socket)
    launch(['speech-dispatcher', '-s', '-t', '0', '-C', str(config), '-S', str(socket),
            '-P', str(runtime/'speechd.pid'), '-L', str(work/'speech-log')], 'speechd')
    wait(socket.exists, 'speech socket')
    orca = launch(['orca', '--speech-system', 'speechdispatcherfactory', '--debug-file', str(work/'orca.debug')], 'orca')
    wait(lambda: bus.name_has_owner('org.gnome.Orca.Service'), 'Orca initialized')
    app = launch([str(app_path)], 'physim')
    main = wait(lambda: xrun('search', '--all', '--onlyvisible', '--pid', app.pid,
                            '--name', r'^Physim \| Experiment Studio$'), 'main window')
    assert len(main.splitlines()) == 1, main
    xrun('windowactivate', '--sync', main)
    key('F1')
    wait(f1_window, 'F1 tree')
    help_window = wait(lambda: xrun('search', '--all', '--onlyvisible', '--pid', app.pid,
                                   '--name', r'^Physim \| Dokumentation$'), 'F1 X11 window')
    assert len(help_window.splitlines()) == 1, help_window
    xrun('windowactivate', '--sync', help_window)
    xrun('windowfocus', '--sync', help_window)
    key('ctrl+shift+r')
    checkpoint('Modellbereich einblenden (geschlossen)')
    key('shift+Tab')
    checkpoint('Alle Inhalte durchsuchen')
    xrun('type', '--clearmodifiers', '--delay', '80', 'Pendel')
    checkpoint('Alle Inhalte durchsuchen', 'Pendel')
    key('Tab')
    checkpoint('Modellbereich einblenden (geschlossen)')
    key('Tab')
    checkpoint('Pendel & Integratoren')
    key('Return')
    key('ctrl+f')
    checkpoint('Aktuelles Dokument durchsuchen', 'Pendel')
    key('Tab')
    checkpoint('Vorheriger Treffer')
    key('Tab')
    checkpoint('Nächster Treffer')
    report['native_steps_completed'] = True
except Exception:
    report['error'] = traceback.format_exc()
finally:
    # Stop Orca first so its buffered speech log is complete while the app and
    # accessibility bus still exist. Only this test's child processes are stopped.
    ordered = ([orca] if 'orca' in globals() else []) + list(reversed(children))
    for child in ordered:
        if child.poll() is None:
            child.terminate()
            try:
                child.wait(timeout=8)
            except subprocess.TimeoutExpired:
                child.kill()
                child.wait()
    for file in files:
        file.close()
    debug = (work/'orca.debug').read_text(errors='replace') if (work/'orca.debug').exists() else ''
    speech = [line.split('SPEECH OUTPUT: ', 1)[1] for line in debug.splitlines() if 'SPEECH OUTPUT: ' in line]
    required = ['Physim | Dokumentation', 'Alle Inhalte durchsuchen', 'Modellbereich einblenden (geschlossen)',
                'Pendel & Integratoren', 'Aktuelles Dokument durchsuchen', 'Vorheriger Treffer', 'Nächster Treffer']
    report['speech_output'] = speech
    report['missing_announcements'] = [name for name in required if not any(name in utterance for utterance in speech)]
    report['speech_backend_available'] = bool(re.search(r'Speech server is <orca\.speechdispatcherfactory\.SpeechServer', debug))
    report['status'] = ('passed' if report.get('native_steps_completed') and not report['missing_announcements']
                        and report['speech_backend_available'] else 'failed')
    (work/'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2)+'\n')
print('Orca F1 controls:', report['status'], work, flush=True)
raise SystemExit(0 if report['status'] == 'passed' else 1)
