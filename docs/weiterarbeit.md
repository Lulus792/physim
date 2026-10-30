# Wiedereinstieg in die native UI-Arbeit

Stand: 30. Oktober 2026. Diese kurze Übergabe vor neuen Untersuchungen lesen und
mit `git status` sowie den relevanten Diffs abgleichen. Designverträge stehen in
[AGENTS.md](../AGENTS.md) und der [Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Bei Widersprüchen zuerst Quellstand, Datum und betroffenen Umfang abgleichen.

## Nächster konkreter Schritt

**Historische Mac-GUI-Testeingaben auf die neue native Menüleiste portieren.**
Mehrere Toolbar-/Tastatur-/Dokumentabläufe klicken bisher auf die alten
In-Window-Menüflächen oder erwarten das eigene F10-Menü. Diese Erwartungen
passen nach der ausdrücklich gewünschten Verschiebung nicht mehr zu macOS;
keinen alten Menü-Fallback ausschließlich zum Bestehen dieser Tests aktivieren.
Das vorher fehlerhafte Dokumentationsanker-Gate ist inzwischen gezielt behoben
und unten als eigener Teilabschluss belegt.

Die Systemmenüleiste selbst nach [ADR 024](decisions/024-native-macos-menubar.md)
ist implementiert und gezielt nativ geprüft: Datei/Bearbeiten/Ansicht/Build/Hilfe
über AppKit, Einstellungen im App-Menü, gemeinsame Befehle und Zustandsprüfung.
Auf `517021f0` bestehen Intel und Apple Silicon je zwei Fälle: echter
AppKit-Menüadapter und gestartete App mit entfernten In-Window-Menüknöpfen.
Windows Release und Linux Debug ASan/UBSan bestehen je vier Menü-/Headerfälle
plus App-Start; der Testkatalog ist geprüft. Diese grünen Kampagnen bei bloßem
Kontextwechsel nicht wiederholen. Manuelle Bedienung/VoiceOver/Vollbild sowie die
gesamte Mac-GUI-Suite bleiben gesondert offen. Den Nutzer-Mac ohne neue
Online-Meldung nicht ansprechen.

Weitere verbindliche Arbeit umfasst A2 in der Szene, Mehrfachauswahl,
Kontextmenüs A, Completion D mit Parameterhilfe und die übrige native Oberfläche.
Freigabe und Implementierungs-/Plattformnachweis getrennt halten.

Physim verlinkt AppKit bereits; die bestehende Accessibility-Brücke nutzt
typisierte Objective-C-Runtime-Aufrufe aus C17. Offizielle Apple-Quellen und der
enge Nutzerumfang sind in ADR 024 festgehalten. Vollbild-/Menüleistenverhalten
macOS verwalten lassen. [Kompakter Nachweis](verification/macos-menubar-2026-10-30.json)
nennt tatsächliche lokale/gehostete Prüfungen und verbleibende Prüflücken.

**Dokumentationsanker-Gate behoben auf `ed3866ed`:** Die ursprünglichen Läufe
`41126070397`/`41126070358` scheiterten an neun nicht erreichbaren Themenzielen.
Die saubere Git-Gegenprobe bestätigt genau diese neun Fehler. Acht Entwickler-
und Designverweise sind jetzt ausdrücklich externe, revisionsfeste Repository-
Quellen; F1 kopiert ihre Adresse. libunibreak-Herkunft und beide Lizenztexte sind
als native Offline-Themen registriert. Der SDK-Installer erhält ihre Pfade auch
außerhalb von `docs/` und überschreibt seine angepasste Startseite nicht.
Der Prüfer bleibt strikt; fehlende Dateien liefern nun gezielte Fehler statt
Python-Tracebacks. Windows/Linux bestehen 188 Themen und 234 Abschnittslinks,
Paket-Gegenprobe/Umzug und je zwei echte F1-Fälle (16/22 px). Der gehostete
Mac-Lauf `41136506871` besteht auf Intel und Apple Silicon je **4/4**.
[Nachweis und Grenzen](verification/documentation-links-2026-10-30.json).
Diese gezielten Prüfungen ohne relevante Änderung nicht erneut beginnen.

Die uncommittete fremde Ergänzung in `docs/status.md` besitzt separat einen
relativen Link zum noch nicht registrierten Bereinigungsbericht vom 22.10.
Sie bleibt unverändert. Deshalb wurde der Git-Quellstand mit ausschließlich
eigenen Änderungen isoliert geprüft; kein pauschales Grün für fremdes WIP oder
die gesamte CI ableiten. Die alten Fehlerprotokolle unter
`build/validation/tree-selection-a2-2026-10-30/ci-*.log` bleiben erhalten.

## Abgeschlossen – nicht aus Kontextverlust wiederholen

| Teil | Ergebnis und Nachweis | Verbleibende Grenze |
| --- | --- | --- |
| F1-Link-Gate und Offline-Lizenzen im SDK | `ed3866ed`: 188 Themen/234 Abschnittslinks unter Windows/Linux; Herkunft und beide Lizenzen tatsächlich per F1-Tastatur geöffnet, Quellenadresse kopiert; SDK-Umzug mit fehlenden Dateien als Gegenprobe. Intel/Apple Silicon je 4/4. [Nachweis](verification/documentation-links-2026-10-30.json). | Gesamte GUI-/Inhalts-/Screenreader-Abnahme bleibt offen; fremde Status-Ergänzung nicht mit geprüft. |
| macOS-Systemmenüleiste | `d43dcd6c` + `517021f0`; je 2/2 native Fälle Intel/Apple Silicon, je 4/4 Menü-/Headerfälle plus Start unter Windows/Linux. [Kompakter Nachweis](verification/macos-menubar-2026-10-30.json). | Manuelle Vollbild-/VoiceOver-Prüfung und Portierung historischer kompletter Mac-GUI-Workflows offen. |
| Trennlinie oberhalb des Protokolls | **Behoben seit `ef1f3321`.** Dauerhaft 1 px, gemeinsame Konturfarbe; Akzent bei Hover/Fokus. [Ursprüngliche Korrektur und negative Kontrolle](verification/protocol-separator-2026-10-29.json). [Nachlauf 30.10.](verification/protocol-boundary-workspaces-2026-10-30.json): Windows Release und Linux Debug ASan/UBSan, je 12/12 Zustände in Entwickeln, Simulieren, Auswerten. | Große Schrift im Nachlauf: 22 px; keine neue macOS-/DPI-/Screenreader-Abnahme. |
| A2 für Textauswahl | Gemeinsamer Textfeldrenderer, Commit `c12e2a10`. [Umfang und native Belege](visuals/unified-workspace/TEXT-SELECTION-A2-NATIVE.md). | Belegt keine vollständige A2-Umsetzung aller Objektarten. |
| A2 für Protokoll- und Explorerlisten/-kacheln | Commit `d228b16e`. [Native Umsetzung](visuals/unified-workspace/LIST-SELECTION-A2-NATIVE.md), [Windows-/Linux-Prüfungen und sechs native Gegenproben](verification/list-selection-a2-2026-10-30.json). | Mehrfachauswahl bleibt offen. |
| A2 für Workspace- und Szenenbaum | [Umsetzung](visuals/unified-workspace/TREE-SELECTION-A2-NATIVE.md), [Nachweis 30.10.](verification/tree-selection-a2-2026-10-30.json): je zehn tatsächliche Zustände Windows/Linux, sechs native Gegenproben, zusätzlich 72 Linux-Pendellaufansichten. | Einzelauswahl; 32 px sind eine gezielte Rendererbelastung, keine neue Einstellungsoption oder reale 200%-DPI-Abnahme. |
| F1: ungültige synthetische Umschalttastenereignisse unter Linux | Commit `8840b9c9`: SDL-Ereignisse ausdrücklich initialisiert; sieben Linux-Kontextfälle bestanden. | Eigenständige Windows-Fokusbefunde bleiben offen; nicht als gleicher Fehler behandeln. |

Der Trennlinien-Nachlauf war hier eine unnötige Wiederholung eines bereits
behobenen Befunds. Er brachte zusätzliche Bereichsabdeckung, aber keine neue
Darstellungskorrektur. **Nicht noch einmal als neue Reparatur beginnen.**

## Wann erneut prüfen?

- Relevante Änderungen am Renderer, an Palette/Layout oder am betroffenen
  Eingabeweg: passende Regression ausführen und den konkreten Anlass nennen.
- Neuer konkreter Fehlerbeleg: zuerst betroffene Version und Zustand mit dem
  vorhandenen Nachweis vergleichen; nur die offene Abweichung reproduzieren.
- Eine bisher unbelegte Plattform/Eingabe-/Darstellungsdimension: gezielt diese
  Dimension ergänzen und bestehende Ergebnisse ihrer Revision zugeordnet lassen.

Ein neuer Chat oder eine Kontextkompaktierung genügt als Anlass nicht. Vor jedem
neuen Lauf kurz festhalten: **Was hat sich geändert, welche Frage ist offen und
welcher kleinste Test beantwortet sie?**

## Lokaler technischer Anschluss

Der letzte A2-Stand heißt `tree-a2-final`. Alle zugehörigen Builds,
nativen Tests, Gegenproben und Archivprüfungen sind beendet. Die abschließenden
Baumtests `tree-a2-fifth` bestehen auf Windows/Linux, die sechs Gegenproben in
`tree-a2-counter-r3` ebenfalls. Die finalen Quellen unterscheiden sich davon nur
durch die geprüfte Python-Gegenproben-Harnesskorrektur, nicht durch App-Code.

Generierte Manifeste, Archive und Kommandoprotokolle:
`build/validation/explorer-inline-2026-10-29/tree-a2-final-*` und
`build/validation/tree-selection-a2-2026-10-30/`. Der gemeinsame isolierte
Compilerbaum liegt unter `build/validation/protocol-wrap-2026-10-26/3-isolated/`.
Für dessen nächste Änderung das neue Manifest verwenden, nicht den älteren
`list-a2-final`- oder `protocol-boundary-final`-Lauf. Der letzte Eingabebestand
umfasst 101 Dateien. Der neue Menüstand `mac-menu-shell` erfasst 105 Eingaben
einschließlich Adapter-/Testdateien. Seine Windows-/Linux-Läufe sind beendet;
Manifest-/Archivnachweise liegen im gleichen Kampagnenordner.
**Neuester Anschluss ist `doc-links-final` mit 112 Eingaben und 1097 gegen Git
abgeglichenen Compiler-/Testdateien**, aufgebaut auf `mac-menu-shell`.
Die F1-Läufe `doc-links-first` sind Windows/Linux je 2/2 bestanden; danach änderte
sich nur der Mac-Menüabsatz in `design-direction.md`, kein Compilerinput oder
App-Runtime. Alle lokalen Builds, Textprüfungen und Archivläufe sind beendet.
Für weitere Änderungen `doc-links-final` als vorheriges Manifest verwenden.
A2-/Protokolltests ohne relevante Änderung weiterhin nicht wiederholen.

Die 16 neuen erfolgreichen F1-Rohbilder wurden nach bestandener Zustandsprüfung
entfernt (26.267.808 Bytes); Textberichte und Originalreferenzen bleiben erhalten.
Eine zusätzliche Bereinigung von Compilerobjekten wurde automatisch mit
`blocked by policy` abgelehnt, auch der engere Einzeldateiversuch. Es wurden
keine Objekte entfernt: 64 `.obj`-Dateien / 6.103.831 Bytes sind bestätigt erhalten.
Die Löschung nicht über eine andere Shell oder Kodierung umgehen.

Parallele lokale AT-SPI-/Orca-, Workflow- und Statusänderungen getrennt erhalten;
nur eigene geprüfte Dateien committen. Alles bleibt auf D:, F: nicht berühren.

Für spätere Mac-Tests verweist der Nutzer ausdrücklich auf NexaCloud und NexaUI:
`D:/NexaCloud/docs/MACOS_TESTING.md` sowie die aktuellen Mac-Hinweise in
`D:/NexaUI/AGENTS.md` zuerst lesen. Eigene Remote-Reste nach gesicherter,
hashgeprüfter Belegkopie gezielt aufräumen; gemeinsame Werkzeuge erhalten.
Die neueste gelesene Verfügbarkeitsnotiz (26.10.) meldet den Nutzer-Mac als aus.
Die Nachricht vom 30.10. enthält einen Anleitungshinweis, keine neue
Verfügbarkeitsmeldung. Deshalb gab es keine neue SSH-/Probe auf dem Nutzer-Mac.
Die hier belegten neuen Mac-Tests liefen auf gehosteten GitHub-Runnern.
