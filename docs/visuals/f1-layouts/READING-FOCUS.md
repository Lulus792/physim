# Nativer Lesefokus und tatsächlicher Orca-Lesebeginn

Diese Erweiterung baut auf dem geprüften H-Renderer auf. Sie verändert die
Fokusanbindung, nicht dessen Gestaltung. Die konservative Gesamtwertung bleibt
UI 8/10, Dokumentationsaufgabe 8/10 und Barrierefreiheit 5/10.

## Dokument als natives Fokusziel

Der Lesebereich veröffentlicht jetzt einen eigenen, an die Inhaltsversion
gebundenen Fokusauftrag. Der UI-Thread übernimmt ihn beim tatsächlichen Aufbau
des Dokuments. Windows UIA, Linux AT-SPI und der AppKit-Adapter erhalten dazu
Fokuszustand, Anforderung und Benachrichtigungen. Ein veraltetes, verborgenes oder
suspendiertes Dokument kann den Auftrag nicht übernehmen. Eine später angenommene
Control-Aktion verwirft einen noch offenen Dokumentauftrag.

Der unabhängige Zwei-Fenster-Test legte eine Lücke zwischen UI-Übernahme und
Betriebssystemfokus offen: `window=2 keyboard=3`, danach `same=1 pending=0
published=0 keyboard=0 control=0 visible=1`. Der Auftrag war bereits verbraucht,
aber der asynchrone Fensterwechsel noch nicht abgeschlossen. AT-SPI gab deshalb
vorzeitig `false` zurück. Das logische Dokumentziel bleibt jetzt während dieses
Übergangs erhalten. Nach außen meldet der Getter Fokus weiterhin ausschließlich
bei tatsächlichem Tastaturfokus des Hostfensters. Die native Warteoperation bleibt
auf eine Sekunde begrenzt und endet bei einem ungültigen Dokument.

Der native Ausgangslauf scheitert gezielt am Dokument-`GrabFocus`
(`run-exjihh98`, 4/5). Mit der Korrektur besteht dieselbe Gruppe 5/5
(`run-y_7usyjm`). Der abschließende Stand besteht:

| Umgebung | Modelltests | Native/UI-Prüfungen |
| --- | --- | --- |
| Windows, MSVC/ASan | 4/4, `run-hxe9pzle` | 7/7, `run-osto59fy` |
| Alpine/X11, GCC 15.2/ASan/UBSan | 4/4, `run-uk25wntu` | 7/7, `run-rbvkf_c3` |

Die anschließende tatsächliche [Mac-Abnahme von 4b04b2c](https://github.com/Lulus792/physim/actions/runs/38943129520)
besteht auf ARM und Intel vollständig: jeweils 19/19 portable und 26/26 GUI-Fälle,
die drei Rendergegenproben sowie externe AX. ARM: `run-0gk5aq6r` / `run-v3mkm1e6`;
Intel: `run-gfjblt7n` / `run-7241v0c8`. Der vorherige Build von c36eaf7 scheiterte
an einem entfernten, weiterhin von Tabellenzellen benötigten No-op-Setter. 4b04b2c
stellt diesen Setter wieder her; der neue Dokumentsetter bleibt eigenständig.
Diese Abnahme umfasst noch keine spätere F6-Erweiterung und keine VoiceOver-Aufgabe.

Die Modelltests umfassen Dokument, Fokus, Suspendierung und Eingabefelder. Die
UI-Gruppen erfassen native Dokument-/Edit-/Linkadapter, Tastatur, H und
Inline-Formelsatz. AT-SPI prüft zusätzlich zwei echte Fenster, tatsächlichen
Dokumentfokus und exklusive Rückgabe an ein Control. Der direkte Modellgegenlauf
für konkurrierende Aufträge scheitert zuvor an `!m->pending_document_focus`;
derselbe GCC-ASan-/UBSan-Lauf besteht nach der Korrektur einschließlich Bereinigung.

Windows-Binary-SHA256:
`0d2181fd8afd40079e391266e54356d5c8060e68e93bd7a502cfea63a44530bb`.
Linux-Binary-SHA256:
`33160d4657cd87be0eea3aa43d37c929e7f4108d8d0a30bdacf5705ae22fd0d5`.
Die neue AppKit-Fokuserweiterung benötigt eigene tatsächliche Mac-Belege; die
älteren H-/AX-Läufe beweisen diese Änderung nicht.

## Orca und die tatsächliche Synthese

Die Nachprüfung des historischen Laufs `orca-controls-final` fand eine fehlende
ALSA-Ausgabebibliothek: `Cannot open plugin alsa` und `Can't initialize audio`.
Die damals protokollierten Control-Ansagen und nativen Zustände bleiben belegt.
Die konfigurierte ALSA-Nullausgabe war jedoch kein Nachweis erfolgreicher Synthese.

Der aktuelle Prüfer startet einen eigenen PulseAudio-17.0-Server ohne
Systemkonfiguration. Er lädt ausschließlich eine Nullsenke und einen privaten
Unix-Socket. Alle Clients verwenden diesen Socket ausdrücklich. Vor Orca muss
eine reale Speech-Dispatcher-/espeak-ng-Synthese `begin` und `end` liefern;
Audio-Initialisierungsfehler lassen die Probe scheitern.

Orcas gepuffertes Dateilog führte zunächst zu einem Beobachtungs-Timeout, obwohl
das abschließend geschlossene Log den Tastaturbefehl und Artikeltext enthielt.
Eine private Terminalausgabe wird jetzt unverändert mitgelesen; Orca schreibt
dorthin zeilenweise. Es werden keine Fokusereignisse oder Sprachausgaben erzeugt
oder in die Prüfung eingespeist. Die normale App läuft ohne Selbsttestschalter.

Die echte X11-Tastenfolge öffnet F1, sucht das Pendel, öffnet den Artikel und
wechselt über die lokale Suche in den nativen Lesebereich. `KP_Add` startet Orcas
eigenen Say-All-Befehl. Der Lesebereich enthält **36.389 Zeichen**. Der Prüfer
verlangt alle sieben Control-Ansagen sowie eine tatsächliche Synthese-
Abschlussmeldung für den Einleitungsabsatz: Zeichen **39 bis 211**, aktueller
Fortschritt **211**. Der von Orca gemeldete Absatz wird mit dem Text aus der
nativen Dokumentabfrage abgeglichen.

Der abschließende Lauf `build/native/orca-document-final/results.json`
meldet `passed`, `audio_completion_probe=[begin,end]`, `audio_errors=[]` und
`orca_log_complete=true`. Der Einleitungsabsatz ist mit
`say_all_completed_paragraph={begin:39,end:211,current:211}` belegt.

Der geprüfte Windows-Stand liegt auch unter
`build/native/Debug-sanitized/bin/physim.exe`; F1 öffnet das Handbuch.

Reproduktion mit frischem Ausgabeordner:

```sh
xvfb-run -a -s '-screen 0 1280x1024x24' dbus-run-session -- \
  python3 tests/check_documentation_orca.py \
  --app build/native/F1-linux/Debug-sanitized/bin/physim \
  --work build/native/orca-reading-new --read-document
```

Benötigt werden Orca, PyAT-SPI, python-dbus, Speech Dispatcher mit Python-Client und PulseAudio-Ausgabe,
espeak-ng, PulseAudio mit pactl, Openbox und xdotool. Der Prüfer verwendet X11
explizit und übernimmt keine geerbte Wayland-Anzeige. Er beendet ausschließlich
seine eigenen Kindprozesse. Eine Hörprüfung findet an der Nullsenke nicht statt.

## Offene Abnahme

Der abgeschlossene erste Absatz beweist noch kein vollständiges Vorlesen des
gesamten Artikels. Screenreader-Linkaktivierung, Rückkehr zum Workspace,
strukturierte Mathematik, Narrator/NVDA/VoiceOver-Aufgaben und physische
Monitor-/DPI-Wechsel bleiben offen. Ebenso bleibt der vollständige Abgleich
aller 185 Anleitungen mit dem Projektstand erforderlich.
