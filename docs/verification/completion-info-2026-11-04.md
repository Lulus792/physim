# Kandidaten-Kurzinfo: Verzögerung und Bedienung · 4. November 2026

Vertrag: [D-Freigabe](../visuals/completion-typing-options/APPROVED.md) und
[Interaktionsbeschreibung](../visuals/completion-typing-options/README.md),
insbesondere Punkte 3–5. Kein vollständiger Abschluss von D oder der Oberfläche.
Der Nutzer-Mac bleibt aus; kein Zugriff und keine Erreichbarkeitsprobe. Nur D:,
kein Zugriff auf F:.

Quellcommit **`f01f25c1`** normal in origin/upstream gepusht. Alle 1.122
Compiler-/Testdateien und 170 eigenen Eingaben gegen tatsächliche Git-Blobs
geprüft, ausschließlich CRLF/LF normalisiert:
[Commitabschluss und neue offene CI-Handles](completion-info-commit-2026-11-04.json).

## Verhalten

Manuelles Öffnen wählt den ersten Vorschlag zur möglichen Übernahme, zeigt aber
noch keine Kurzinfo. Automatisches Öffnen bleibt ohne Übernahmeauswahl und ohne
Kurzinfo. Pfeile oder eine bewegte Maus über einem anderen Eintrag starten die
280-ms-Verzögerung neu. Mausvorschau allein erlaubt keine Übernahme mit Enter/Tab.
Mausrad und Scrollgriff verändern weiterhin nur den Listenausschnitt.

`i` oder Strg+Leertaste im offenen Popup schaltet die Kurzinfo sofort um, ohne
Auswahl oder Übernahmebereitschaft zurückzusetzen. Ein vorgemerkter alter Hinweis
kehrt nach dem Ausblenden nicht zurück. Der Controller bindet den Hinweis an
Editor, Katalogidentität, Cursor, Editor-Scrollposition und sämtliche Quellbytes;
auch eine gleich lange Änderung außerhalb des Präfixes verwirft ihn. Eine eigene
begrenzte Quellkopie wird bei unveränderten Bytes wiederverwendet. Keine Worker,
OS-Timer oder neuen Dienste.

Bei sichtbarer Karte scrollen Alt+BildAuf/BildAb und Alt+Pos1/Ende deren Inhalt.
Quelltext, Cursor, Editor-Scrollposition und Listenauswahl bleiben erhalten.
Der `i`-Tooltip nennt diese Befehle; sein zugänglicher Name nennt Strg+Leertaste.
Umschalt+Tab schließt die Liste und verlässt das Codefeld ohne Einrückung oder
Übernahme. Andere modifizierte Tab-Kombinationen erhalten die bisherige weitere
Ereignisverarbeitung. Bestehende 140-ms-Automatik, Undo und Referenzzugänge bleiben
erhalten. Keine erneute Gestaltung der Karte.

## Tatsächlich ausgeführte Prüfungen

[Originalkommandos, Quell-/Binärhashes, Phasen und Einzelmarker](completion-info-2026-11-04.json).
Windows Release/MSVC und Linux Debug/GCC ASan/UBSan sind getrennte echte Builds.
Der Abschlussstand heißt `completion-info-tab`; Unit und GUI laufen in separaten
Aufrufen mit nachgewiesen identischen App-Bytes je Plattform.

| Plattform | Unit | UI-Gruppe |
| --- | --- | --- |
| Windows | `run-ot7qjiy2`, 1/1 | `run-zzc2kapy`, 4/4 |
| Linux | `run-3ypvbd2x`, 1/1 | `run-7fh8kh0v`, 4/4 |

Die Gruppe enthält `completion_info_workflow`, `completion_workflow`,
`completion_automatic_workflow` und `api_reference_workflow`. C und Physim werden
jeweils tatsächlich gestartet. Der neue Ablauf prüft Modell-, Analyse- und
Dateieditor: dunkel/UI16/Code12, hell/UI22/Code22, Kontrast/UI16/Code12.
Er beobachtet echte gezeichnete Textbefehle, Geometrie und Datenzustände nach den
betroffenen Frames. Jede Verzögerungsprüfung hat Vorfristbeobachtungen, die rasche
Neuauswahl ein Beobachtungsfenster zwischen alter und neuer Frist. Zeitgrenze je
Stufe 4 s; vorhandener `completion-*`-Watchdog 60 s unverändert.

Zusätzlich: exakt unabhängige 0,280-s-Konstante, vor/an der Frist, unveränderte
Leerlauffrist, Speicherbesitz, schnelle Neuauswahl, sechs Bindungsdimensionen,
ungültige Eingaben und Abbrechen in der reinen Unit. Separater abschließender
C17-Build mit `-Wall -Wextra -Werror`, ASan/UBSan und `detect_leaks=1`: Exit 0.
Linux-GUI verwendet weiterhin `detect_leaks=0`; daraus keinen GUI-Leaknachweis
ableiten. Registrierungs-/Harnessprüfung tatsächlich Exit 0; ihre absichtlichen
Mock-Fehler sind keine ausgeführten Produktfehler oder Plattformabnahmen.

## Vorherbefunde und Korrekturen

- `completion-info-before`: Windows-Unit 1/1, neuer UI-Fall 0/1. Der alte erste
  manuelle Öffnungszustand meldete bereits sichtbare Info. Dies ist eine
  Zustandsgegenprobe vor dem Zeichnen, kein Nachweis tatsächlich gezeichneter
  Glyphen in diesem Fehlerframe. Linux wurde in dieser Phase nicht ausgeführt.
- `completion-info-integrated`: beide Plattformen Unit 1/1, UI 2/4; manuelle
  Completion und API-Referenz bestanden. Umschalt+Tab erreichte Nuklears
  `ALLOW_TAB`-Pfad und fügte vier Leerzeichen ein. Der neue echte Tastendruckfall
  erkannte die Quelländerung. Jetzt wird dieser konkrete Befehl zum Verlassen
  des Editors konsumiert, einschließlich Freigabe des eigenen Tastaturbesitzes.
- Der automatische Prüfer maß nach dem sofortigen Sichtbar-Schalten noch die
  Geometrie des vorherigen Frames. Nun höchstens ein Umschaltbefehl und danach
  begrenztes Warten auf tatsächlich gezeichnete Kartenbounds; bestehende Daten-,
  Undo-, Viewport- und Referenzassertionen bleiben erhalten.
- Der neue Prüfer berücksichtigt die reale Reihenfolge: Ablauf vor Zeichnung,
  Zeichenbeobachter danach. Direkte Änderungen warten eine, SDL-Aktionen zwei
  tatsächliche Zeichengenerationen. Keine festen Wartepausen als Ersatz für
  beobachtete Zustände. Frühere Gegenprobe nicht nachträglich als neu ausgeführt
  ausgeben.

## Bilder, Speicher und Grenzen

Der neue Timing-Ablauf erzeugt keine Bilder. Drei vorhandene Abschlussaufnahmen
aus dem aktuellen Windows-Completion-Lauf tatsächlich angesehen: C-Modell dunkel,
C-Analyse hell/groß und Physim-Datei im Kontrasttheme. Beschreibung, vollständige
Karte und Referenzzugang bleiben sichtbar. Diese Bilder sind keine physische
Tastatur-, Screenreader- oder DPI-Abnahme.

Zwölf ersetzte Erfolgs-BMPs aus dem bestandenen manuellen Teil der ersten
Integration entfernt: **38.363.256 Bytes**. Aufgelöste einzelne D:-Pfade,
Nichtverwendung, Original-/Ersatzhashes und vorhandene Abschlussbilder vor dem
Löschen geprüft. Fehlerbilder, aktuelle Ansichten, Quellen, Archive, Nutzdaten,
Abhängigkeiten und VM erhalten. Keine pauschale Ordner-/Objektbereinigung.
[Einzeldateien und Ersatzbelege](completion-info-cleanup-2026-11-04.json).

Die gehosteten Mac-Prüfungen des **vorherigen** Quellstands `25dd37dc` sind separat
gesichert: Intel-Editor 5/5, Einstellungen 16/16, F1 5/6; ARM jeweils abgebrochen.
F1 `docs-keyboard-h` erreicht bei 60,089 s Stufe 26; Ursache offen. Keine neue
Mac-Abnahme der Verzögerung ableiten. Alle drei Archive/Digests/Einträge gelesen:
[getrennter Mac-Bericht](completion-help-macos-2026-11-04.json).

Weiter offen: echte lokale C-/Physim-Scopes, separate Aufrufsignatur und weitere
Codekontexte sowie physische Eingabe/IME, Assistive-Technologien, DPI, mehrere
Monitore, Vollbild und aktuelle macOS-Abnahme. Die neue Tastaturbedienung allein
belegt keine vollständige Barrierefreiheit. Keine volle D-/UI-Punktzahl.
