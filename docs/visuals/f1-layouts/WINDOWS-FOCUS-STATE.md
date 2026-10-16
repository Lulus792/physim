# Windows: tatsächlicher Fokus und gespeichertes Fokusziel

Die Untersuchung setzt auf dem H-/F6-Stand c01ac79 auf. Die anschließenden
Projektanweisungen und Designstudien bleiben erhalten. Die konservative
Gesamtwertung UI 8/10, Dokumentationsaufgabe 8/10 und Barrierefreiheit 5/10
ändert sich durch diese begrenzte Korrektur nicht.

## Nachgewiesener Providerfehler

Das gemeinsame Modell behält die logische Control-ID beim Verlust des
Hostfokus. Das ist für die Rückkehr zur Lesestelle erforderlich. Der UIA-Provider
verwendete diese ID jedoch auch für `IRawElementProviderFragmentRoot::GetFocus`
und seinen Fokusereignis-Zustand. Dadurch konnte `GetFocus` ein Suchfeld liefern,
während dessen `HasKeyboardFocus` bereits falsch war. Der
[Microsoft-Vertrag für GetFocus](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-irawelementproviderfragmentroot-getfocus)
verlangt ohne Eingabefokus ein Null-Ergebnis.

Die neue native COM-Gegenprobe durchläuft vier veröffentlichte Hostzustände
aktiv/inaktiv/aktiv/inaktiv bei gleicher logischer Feld-ID. Sie vergleicht
`GetFocus` mit der tatsächlichen Fokus-Property. Auf dem unkorrigierten Provider
scheitern exakt die beiden inaktiven Zustände: `run-kdditl34`, **0/1**,
`uia-host-contract-negative.log`. Die COM-Referenzen werden auch bei diesem
erwarteten Misserfolg freigegeben.

Der Provider ermittelt jetzt das aktuell fokussierte Control anhand des sichtbaren
Hosts, des veröffentlichten Tastaturfokus und der Fokus-Property des Elements.
`GetFocus` und der Ereignis-Zustand verwenden denselben Wert. Eine logische ID
allein erzeugt keine neue Fokusmeldung; bei Rückkehr des Hosts kann auch dieselbe
Control-ID wieder einen Fokusübergang bilden. Die tatsächliche Ansage dieses
Ereignisses durch einen Screenreader ist weiterhin gesondert zu prüfen.

## Tatsächlich ausgeführte Windows-Prüfungen

MSVC/ASan auf dem vorhandenen Windows-Rechner:

- `run-4ba7h3ez`: **4/5**. Native COM-Prüfung einschließlich Gegenprobe,
  externe Dokument- und Linkprüfung sowie die 24-Zustände-Bereichsmatrix bestehen.
  Der externe Feldablauf scheitert zunächst beim lokalen Suchfeldfokus.
- `run-uhtva4rq`: gezielte externe Feldprüfung **1/1**; beide Suchfelder,
  Unicode-Auswahl, Ctrl+F-Rückkehr und die drei Tab-Buttonphasen bestehen.
- `run-1431iz7h`: native COM-Prüfung und neuer unabhängiger Win32-Beobachter
  **2/2**. Der Beobachter liest Vordergrundfenster sowie aktive/fokussierte Fenster
  des tatsächlichen GUI-Threads während aller 24 nativen Bereichszustände.
- `run-sa5e04rt`: externe Feldprüfung mit abschließender optionaler
  Prozess-/Konsolendiagnose **1/1**. Alle Fokusphasen besitzen den Zielprozess
  im Vordergrund; die Prüfkonsole ist unsichtbar.

Der Beobachter aktiviert keine Fenster und sendet keine OS-Tasten. Die bestehende
App-Matrix erzeugt ihre SDL-Testereignisse; der Win32-Zustand wird außerhalb der
App abgefragt. Eine direkte Vorprobe auf dem alten Binary e4c796a besteht ebenfalls
(`windows-host-audit-3is7jpam/results.json`). Die damals beobachtete verweigerte
Windows-Aktivierung ist durch den aktuellen erfolgreichen Ablauf daher nicht
nachträglich ursächlich erklärt oder als dauerhaft behoben anzusehen.

Die Vermutung einer sichtbaren Prüfkonsole ist in den aktuellen erfolgreichen
Läufen nicht bestätigt. `PHYSIM_UIA_FOCUS_TRACE=1` ergänzt nun pro Feldprüfung,
ob die Prozessabfrage gelang, der Zielprozess im Vordergrund liegt und eine
Prüfkonsole sichtbar beziehungsweise selbst im Vordergrund ist. Diese Diagnose
ändert keine Aktivierung und keine Fokusanforderung.

Die Katalogprüfung besteht unter Windows und Alpine-Dev. Dabei wurde auch die
fehlende Zuordnung des bereits registrierten Linux-Tabellen-Providerfalls in der
Katalogprüfung korrigiert; der tatsächliche Fall und seine Bedingungen bleiben erhalten.

## Belege, Grenzen und Speicherpflege

Der geprüfte Windows-Build besitzt SHA-256
`64f2ce8fab3a1d7c11fe6b67df79fe90cf0675d9c4027e6132644b9d6e5a182f`.
EXE und die zugehörige, vom selben Build erzeugte PDB liegen auch unter
`build/native/Debug-sanitized/bin`. Die vorherigen Dateien wurden separat in
`build/native/physim-before-uia-host-focus` gesichert; die Laufzeit-DLLs stimmen
zwischen privatem und üblichem Startordner überein.
Die isolierten Win32-Ergebnisse stehen in `uia-host-os-final/host-audit/results.json`
und im genannten nativen Testlauf. Alle Pfade beziehen sich auf `build/native`.

Nach der Abnahme wurden ausschließlich die 48 automatisch erzeugten BMPs dieser
zwei erfolgreichen OS-Fokus-Audits entfernt: 78.803.424 Bytes, rund 75,2 MiB.
Ihre Prüfer lesen die OS-Messdaten, keine Bildpixel. JSON, stdout/stderr und alle
anderen Dateien bleiben erhalten, ebenso Fehlerbelege, Referenzbilder, Programme
und Debug-Symbole. Einzelpfade und Bytezahlen dokumentiert
`uia-host-audit-image-cleanup.json`.

Die native COM-Zustandsprobe ist modellgesteuert; der getrennte Win32-Beobachter
belegt reale Hostzustände. Beides ersetzt keine vollständige praktische Narrator-,
NVDA- oder VoiceOver-Aufgabe. Sporadische Windows-Aktivierungsfehler, die Intel-
Zeitlimits, physische DPI-/Monitorwechsel und der Inhaltsabgleich aller 185 Seiten
bleiben offen. Es wird keine plattformübergreifende Höchstwertung behauptet.
