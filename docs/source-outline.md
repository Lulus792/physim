# Quelltextgliederung in der Werkbank

Der Bereich **IM DOKUMENT** enthält Deklarationen aus der aktuell geöffneten
Quelldatei. Die Liste folgt der Quelltextreihenfolge. Funktionen, Bindungen und
Typen haben unterschiedliche Symbole. Ein Klick oder die Tastaturaktivierung
markiert den Namen im Editor und bringt ihn in den sichtbaren Bereich. Danach
kannst du direkt weiterschreiben; der Mauszeiger darf über der Liste bleiben.

Die Liste berücksichtigt ungespeicherte Änderungen und Undo/Redo. Navigation
ändert weder Quelldatei noch Dirty-Zustand. Die vollständige Liste ist innerhalb
des Kontextbereichs scrollbar; sie verdrängt die API-Karte nicht nach unten.
Ausgeblendeter Kontext und persönliche Dockgrößen bleiben erhalten.

## Daten und Grenzen

- Für **Physim** wird der vorhandene Compiler-Parser verwendet. Angezeigt werden
  Funktionen, globale `let`-/`var`-Bindungen sowie Strukturen und Enumerationen,
  einschließlich ihrer generischen Formen. Lokale Variablen und Typmitglieder
  sind keine eigenen Einträge auf Dateiebene. Bei unvollständiger Syntax zeigt
  die Oberfläche einen Hinweis und keine veraltete erfolgreiche Gliederung.
- Für **C** ist dies eine Navigation durch geschriebene Deklarationen auf
  Dateiebene: globale Bindungen, Funktionen/Prototypen, Aggregate und Typedefs.
  Kommentare, Zeichenketten, Funktionskörper und Makrodefinitionszeilen werden
  übersprungen. Funktionszeiger und Funktionen, die einen Funktionszeiger
  zurückgeben, sind berücksichtigt. Das ist keine semantische Compileranalyse:
  Makros werden nicht expandiert und Präprozessorbedingungen nicht ausgewertet.
  Daher können Deklarationen aus mehreren geschriebenen `#if`-Zweigen erscheinen.
  Compilerabhängige Deklarator-Erweiterungen benötigen eine spätere Integration
  mit dem tatsächlichen Compiler. Ein leerer oder begrenzter Index ist kein
  Nachweis, dass eine Datei erfolgreich kompiliert.
- Andere Textdateien erhalten einen verständlichen Hinweis. Die Gliederung ist
  in der Werkbank für Experimentcode und zusätzliche Quelldokumente eingebunden;
  die Auswertungsoberfläche behält ihren bisherigen Aufbau.
- Höchstens 1 MiB Quelltext und 512 sichtbare Indexeinträge; für Physim höchstens
  65.536 Syntaxknoten. Erreichte Grenzen und Speicherfehler werden angezeigt.
  Es gibt keine Dateizugriffe oder Compilerprozesse beim Navigieren.

Der Index enthält Bytebereiche in einer eigenen Kopie des tatsächlichen
Editorinhalts. Unveränderte Frames vergleichen die Bytes und parsen nicht neu.
Ein Sprung prüft vor Aktivierung nochmals die aktuelle Quelle und wandelt die
Bytebereiche für Nuklears UTF-8-Zeichenpositionen um. Es gibt keinen Rückgriff
auf eine alte gespeicherte oder erfolgreich gebaute Version.

## Prüfungen

`source_outline` prüft unabhängig erwartete Deklarationen und Positionen für C
und Physim, UTF-8, Kommentare/Strings, lokale Variablen, Funktionszeiger,
unvollständige Quellen, Größen- und Anzeigegrenzen. `source_outline_workflow`
bedient tatsächliche SDL-/Nuklear-Editoren: C dunkel, Physim hell und C mit
großer Schrift, jeweils Experiment und zusätzliche Datei. Geprüft werden
Klicksprung, Auswahl, Fokus, Tippen mitten im Namen, aktualisierter Index,
Dirty-Zustand, Undo und unveränderte Dateien auf dem Datenträger. Zusätzlich
prüft der Bildleser die tatsächliche Auswahlfarbe; helle unsichtbare Markierungen
auf dunklem Code fallen damit durch.

Die dabei gefundenen Fehler sind separat reproduziert: Der erste native Lauf
verliert den Editorfokus durch Panel-Hover. Der reine Nuklear-Test
`editor_typing_undo` scheitert vor der Korrektur am Undo nach dem Einfügen mitten
im Text. ASCII, Mehrbytezeichen und Ersetzungsmodus werden danach geprüft.
Konkrete endgültige Läufe und Bilder werden in der Werkbank-Validierung verlinkt.

## Ausgeführter Stand am 20. Oktober 2026

- Windows MSVC/ASan: abschließende Gliederung **1/1 Workflow**, darin drei
  App-Läufe mit jeweils Experiment und Zusatzdatei (`run-79if5yzw`); zusätzlich
  **6/6 aktuelle Werkbank-App-Läufe**. Die Tastaturaktivierung verwendet den
  nativen Accessibility-Fokusauftrag und echte SDL-Enter-/Textereignisse. Das
  ersetzt keine vollständige manuelle Tabfolge oder Screenreader-Abnahme.
- Linux Alpine-Dev GCC/ASan/UBSan, Xvfb/Openbox/Mesa: **2/2 aktuelle Workflows**
  mit denselben drei Gliederungs- und sechs Werkbank-App-Läufen (`run-rmhwz9w2`).
- Kernprüfungen je Plattform **2/2**, inklusive exakter 1-MiB-Grenze für beide
  Sprachen: Windows `run-r7q0qvi2`, Linux `run-pc6lzrjg`.
- Frühere Regression vor der letzten Vereinheitlichung der nativen Fokuszeichnung:
  Windows **3/3** Gliederung/Ersetzen/Completion; Linux **4/4**
  Gliederung/Werkbank/Ersetzen/API. Diese älteren Regressionen waren vor der
  zusätzlichen Tastaturaktivierungsprobe und sind entsprechend abgegrenzt.
- Vier Standardansichten bestehen den begrenzten Bildprüfer für Schriftmaß,
  Flächenfarben und Trennlinien. Der Auswahl-Gegenfall enthält null passende
  Auswahlpixel im Codebereich; der korrigierte echte Windows-Lauf 1.161.
- Die reguläre Windows-EXE und ihr PDB wurden aktualisiert; `--smoke` bestand.
  54 neue native PNGs sind ohne Retusche aus den tatsächlichen BMPs konvertiert.

Verbindliche Hashes, konkrete Ergebnisdateien und Binärstände:
[source-outline-validation.json](visuals/develop-designs/source-outline-validation.json).
Das Manifest kennzeichnet auch die vorhandenen separaten, uncommitteten
F1-/AT-SPI-Arbeiten im gemeinsamen Buildstand. macOS, physischer DPI-Wechsel und
vollständige Screenreader-Bedienung wurden hier nicht nachgewiesen.

Nach der Sicherung wurden 63 ersetzte erfolgreiche Rohbilder entfernt,
zusammen 287.846.070 Byte (rund 275 MiB). Aktuelle Rohbilder und negative Belege
bleiben erhalten. [Bereinigungsprotokoll](visuals/develop-designs/native-outline/cleanup.json).
