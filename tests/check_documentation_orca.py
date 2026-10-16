"""Real Orca control announcements in the normal app; run on a private X11 display.

Requires Orca, PyAT-SPI, python-dbus, Speech Dispatcher with its Python client,
espeak-ng, Speech Dispatcher's PulseAudio output, PulseAudio with pactl,
Openbox and xdotool. This checks control
announcements, not complete document/math reading or audible output: synthesis
uses a private PulseAudio server with only a null sink.

Example: xvfb-run -a dbus-run-session -- python3 tests/check_documentation_orca.py
  --app build/native/Debug-sanitized/bin/physim --work build/native/orca-controls
"""

import argparse
import errno
import hashlib
import json
import os
from pathlib import Path
import re
import pty
import subprocess
import tempfile
import threading
import time
import traceback

import dbus
import pyatspi
from gi.repository import GLib

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--app', required=True, type=Path)
parser.add_argument('--work', required=True, type=Path)
parser.add_argument('--read-document', action='store_true', help='Also require native reading focus and actual Orca Say All text')
args = parser.parse_args()
root = Path(__file__).resolve().parents[1]
app_path = args.app.resolve()
work = args.work.resolve()
work.mkdir(parents=True, exist_ok=False)
runtime = Path(tempfile.mkdtemp(prefix='physim-orca-'))
env = dict(os.environ, XDG_RUNTIME_DIR=str(runtime), XDG_CONFIG_HOME=str(work/'config'),
           XDG_DATA_HOME=str(work/'data'), XDG_CACHE_HOME=str(work/'cache'),
           GSETTINGS_BACKEND='memory', ASAN_OPTIONS='detect_leaks=0',
           UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1', LIBGL_ALWAYS_SOFTWARE='1',
           SDL_VIDEODRIVER='x11', GDK_BACKEND='x11', XDG_SESSION_TYPE='x11')
env.pop('WAYLAND_DISPLAY',None)
os.environ.pop('WAYLAND_DISPLAY',None)
os.environ.update(env)
children, files, checkpoints = [], [], []
report = dict(status='failed', app_sha256=hashlib.sha256(app_path.read_bytes()).hexdigest(),
              scope='Orca control announcements; private PulseAudio null sink, no audio audition',
              reading_command='keyboard' if args.read_document else None,
              checkpoints=checkpoints)


def launch(command, name):
    out, err = (work/(name+'.stdout')).open('wb'), (work/(name+'.stderr')).open('wb')
    files.extend([out, err])
    child = subprocess.Popen(command, cwd=root, env=env, stdout=out, stderr=err)
    children.append(child)
    return child


def launch_orca():
    # Orca opens --debug-file with default buffering. A private terminal makes
    # that file line-buffered; copy its real output verbatim for live observers.
    master,slave=pty.openpty()
    log=(work/'orca.debug').open('wb',buffering=0);files.append(log)
    try:
        child=subprocess.Popen(['orca','--speech-system','speechdispatcherfactory','--debug-file','/dev/stderr'],
            cwd=root,env=env,stdout=slave,stderr=slave)
    except BaseException:
        os.close(master)
        raise
    finally:os.close(slave)
    children.append(child)
    capture=dict(eof=False,error=None)
    def copy_output():
        try:
            while data:=os.read(master,65536):log.write(data)
            capture['eof']=True
        except OSError as error:
            if error.errno==errno.EIO:capture['eof']=True  # Linux terminal slave closed.
            else:capture['error']=repr(error)
        except Exception as error:capture['error']=repr(error)
        finally:os.close(master)
    reader=threading.Thread(target=copy_output,daemon=True);reader.start()
    report['debug_capture']='line-buffered private terminal; unmodified Orca output'
    return child,reader,capture


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
    # No system configuration: only a null sink and an explicit private socket.
    pulse_socket=runtime/'pulse.sock'
    pulse_config=work/'pulse.pa'
    pulse_config.write_text('load-module module-null-sink sink_name=physim_null\n'
        f'load-module module-native-protocol-unix socket={pulse_socket} auth-anonymous=1\n'
        'set-default-sink physim_null\n')
    env['PULSE_SERVER']='unix:'+str(pulse_socket)
    launch(['pulseaudio','-n','--daemonize=no','--use-pid-file=no','--exit-idle-time=-1',
            '--file='+str(pulse_config)],'pulse')
    wait(pulse_socket.exists,'private PulseAudio socket')
    sinks=subprocess.check_output(['pactl','--server='+env['PULSE_SERVER'],'list','short','sinks'],env=env,text=True)
    assert len(sinks.splitlines())==1 and sinks.split()[1]=='physim_null',sinks
    report['audio_sink']=sinks.strip()
    config = work/'speech-config'
    (config/'modules').mkdir(parents=True)
    (config/'modules/espeak-ng.conf').write_bytes(Path('/etc/speech-dispatcher/modules/espeak-ng.conf').read_bytes())
    (config/'speechd.conf').write_text('LogLevel 4\nDefaultLanguage "de"\nAudioOutputMethod "pulse"\n'
                                    'AddModule "espeak-ng" "sd_espeak-ng" "espeak-ng.conf"\n'
                                    'DefaultModule "espeak-ng"\n')
    (work/'speech-log').mkdir()
    socket = runtime/'speechd.sock'
    env['SPEECHD_ADDRESS'] = 'unix_socket:'+str(socket)
    launch(['speech-dispatcher', '-s', '-t', '0', '-C', str(config), '-S', str(socket),
            '-P', str(runtime/'speechd.pid'), '-L', str(work/'speech-log')], 'speechd')
    wait(socket.exists, 'speech socket')
    import speechd
    audio_events=[]
    audio_client=speechd.SSIPClient('physim-audio-proof',address=env['SPEECHD_ADDRESS'],autospawn=False)
    try:
        audio_client.set_output_module('espeak-ng')
        audio_client.set_language('de')
        audio_client.speak('Sprachausgabe bereit.',callback=lambda kind,**kwargs:audio_events.append(kind),
                           event_types=[speechd.CallbackType.BEGIN,speechd.CallbackType.END])
        wait(lambda:speechd.CallbackType.END in audio_events,'real synthesis completion through private null sink',timeout=20)
        report['audio_completion_probe']=list(audio_events)
    finally:audio_client.close()
    orca,orca_reader,orca_capture=launch_orca()
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
    if args.read_document:
        key('Tab')
        checkpoint('Pendel & Integratoren')
        reading=focused('Pendel & Integratoren')
        assert reading.getRole() == pyatspi.ROLE_DOCUMENT_TEXT
        native_text=reading.queryText().getText(0,-1)
        assert 'Berechne dasselbe nichtlineare Pendel' in native_text
        report['native_document_characters']=len(native_text)
        report['native_document_sha256']=hashlib.sha256(native_text.encode()).hexdigest()
        key('KP_Add')
        wait(lambda:'SAY ALL PRESENTER: say_all. Script:' in (work/'orca.debug').read_text(errors='replace'),
             'actual Orca Say All command dispatch',timeout=30)
        def spoken_reading():
            debug_text=(work/'orca.debug').read_text(errors='replace')
            return ('SAY ALL PRESENTER: say_all. Script:' in debug_text and
                'Berechne dasselbe nichtlineare Pendel' in debug_text.split('SAY ALL PRESENTER: say_all. Script:',1)[1])
        wait(spoken_reading,'actual document text after Say All dispatch',timeout=45)
        def completed_reading():
            debug_text=(work/'orca.debug').read_text(errors='replace')
            for record in re.finditer(r"SAY ALL PROGRESS CALLBACK: Completed SAY ALL: [^\n]+\]\s+'(.*?)' \((\d+)-(\d+), current: (\d+)\)",debug_text,re.S):
                text,begin,end,current=record.groups();begin,end,current=map(int,(begin,end,current))
                if 'Berechne dasselbe nichtlineare Pendel' in text and current==end and \
                   ' '.join(text.split())==' '.join(native_text[begin:end].split()):
                    return dict(begin=begin,end=end,current=current)
            return None
        report['say_all_completed_paragraph']=wait(completed_reading,'actual synthesis completion of the native introductory paragraph',timeout=45)
        report['scope']='Control announcements and completed native Say All introductory paragraph; no full-document completion or audio audition'
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
    if 'orca_reader' in globals():
        orca_reader.join(timeout=5)
        report['orca_log_complete']=not orca_reader.is_alive() and orca_capture['eof'] and not orca_capture['error']
        report['orca_log_error']=orca_capture['error']
    for file in files:
        file.close()
    debug = (work/'orca.debug').read_text(errors='replace') if (work/'orca.debug').exists() else ''
    speech = [line.split('SPEECH OUTPUT: ', 1)[1] for line in debug.splitlines() if 'SPEECH OUTPUT: ' in line]
    required = ['Physim | Dokumentation', 'Alle Inhalte durchsuchen', 'Modellbereich einblenden (geschlossen)',
                'Pendel & Integratoren', 'Aktuelles Dokument durchsuchen', 'Vorheriger Treffer', 'Nächster Treffer']
    report['speech_output'] = speech
    report['missing_announcements'] = [name for name in required if not any(name in utterance for utterance in speech)]
    report['speech_backend_available'] = bool(re.search(r'Speech server is <orca\.speechdispatcherfactory\.SpeechServer', debug))
    audio_log=(work/'speech-log/speech-dispatcher.log').read_text(errors='replace') if (work/'speech-log/speech-dispatcher.log').exists() else ''
    report['audio_errors']=[line for line in audio_log.splitlines() if "Can't initialize audio" in line or
                            'Audio event but server audio not set up' in line][:5]
    if args.read_document:
        say_all = debug.split('SAY ALL PRESENTER: say_all. Script:', 1)[-1] if 'SAY ALL PRESENTER: say_all. Script:' in debug else ''
        report['say_all_started'] = bool(say_all)
        report['say_all_text_present'] = 'Berechne dasselbe nichtlineare Pendel' in say_all
        if not report['say_all_started'] or not report['say_all_text_present']:
            report['missing_announcements'].append('Actual document text after Orca Say All')
    report['status'] = ('passed' if report.get('native_steps_completed') and not report['missing_announcements']
                        and report['speech_backend_available'] and 'end' in report.get('audio_completion_probe',[])
                        and not report['audio_errors'] and report.get('orca_log_complete') else 'failed')
    (work/'results.json').write_text(json.dumps(report, ensure_ascii=False, indent=2)+'\n')
print('Orca F1 controls:', report['status'], work, flush=True)
raise SystemExit(0 if report['status'] == 'passed' else 1)
