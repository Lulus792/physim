# Entwurf: ein gemeinsames Protokoll

**25. Oktober 2026 – Protokoll C freigegeben:** Der Nutzer hat aus den neuen
Varianten ausdrücklich **C · Navigator** ausgewählt.
[Verbindliche Auswahl](../design-options/APPROVED.md) ·
[ausgewählte Referenz](../design-options/index.html?approved=1&topic=protocol).
Die Anordnung dieses älteren Entwurfs wird durch C ersetzt; die folgenden
gemeinsamen Anforderungen an Meldungen, Filter, Originaltexte und Daten gelten
weiter. Keine erneute Variantenfreigabe vor der nativen Umsetzung erforderlich.

Stand: 24. Oktober 2026. Neuer Nutzerauftrag: Fehler, Warnungen und weitere
Protokollzustände gestalten. **Interaktive Designreferenz; noch keine native
Umsetzung oder ausdrückliche Designabnahme dieses neuen Entwurfs.** Die frühere
Freigabe des Grafikexports bleibt unverändert.

[Beispiele öffnen](index.html?example=protocol#develop). In der gemeinsamen
Vorschau auch über **Ansicht → Protokoll anzeigen**, die Statusleiste und
**Simulieren → Ereignisse → Protokoll öffnen** erreichbar.

## Aufbau und Verhalten

- Ein gemeinsamer unterer Dockbereich über die gesamte Fensterbreite. Standort,
  Filter und Auswahl bleiben beim Wechsel Entwickeln/Simulieren/Auswerten erhalten.
  Der bisherige kleine Werkbank-Protokollkörper wird währenddessen ausgeblendet
  und beim Schließen wieder sichtbar. Native Integration soll denselben
  `PS_DOCK_LOG` verwenden, keine zweite unabhängige Meldungshistorie schaffen.
- Kopf mit Gesamtsummen und Quellenwahl **Diese Sitzung / Designbeispiele**.
  Beispiele sind unabhängige Situationen und werden niemals als aktuelle Fehler
  oder echte Messdaten ausgegeben. Sie verändern weder Modell noch Dateien.
- Filter **Alle / Fehler / Warnungen / Info / Debug**, jeweils mit Anzahl.
  Bereichsfilter und Suche werden mit dem Schweregrad UND-verknüpft. Die Suche
  umfasst Titel, Text, Kontext, Quellstelle und Originalmeldung. Kopfzahlen zählen
  die gesamte gewählte Quelle; die Fußzeile nennt sichtbare/gesamte Meldungen.
- Links eine scrollende Liste; rechts die ausgewählte Meldung mit Ursache,
  Auswirkung, Kontext, verfügbarer Quellstelle, Modellzeit und Originaltext.
  Bei höchstens 900 px wird daraus eine Liste mit eigener Detailansicht und
  **← Alle Meldungen** samt Fokusrückgabe. Kein horizontaler Zwangsscroll.
- Rot mit ⊗ und „Fehler“, Gelb mit △ und „Warnung“, Blau mit ⓘ und „Info“,
  neutral mit ◇ und „Debug“. Erfolgreicher Abschluss darf zusätzlich ein grünes
  Häkchen haben; er bleibt eine Info, kein zusätzlicher Logger-Schweregrad.
- `Originalmeldung anzeigen` erhält Zeilenumbrüche, Tabs, Unicode und lange
  Zeichenfolgen. Kein Abschneiden wesentlicher Fehlertexte. Kein HTML-Rendering
  aus Meldungstext: Inhalte werden als Text eingefügt.
- Quellsprung nur mit tatsächlich verfügbarer Datei/Position. Das Buildbeispiel
  markiert ausdrücklich eine **Beispielstelle** (`gravity_m_s2`) ohne den
  fehlerhaften Beispielcode in `main.c` einzusetzen. Meldungen ohne Ort zeigen
  „Nicht übermittelt“ und keinen erfundenen Sprung.
- Kopieren einer Meldung mit Rückmeldung; bei verweigerter Zwischenablage stehen
  Originaltext und TXT-Download zur Verfügung. Download umfasst die aktuell
  gefilterten Meldungen und kennzeichnet Designbeispiele auch in der Datei.
- Standardhöhe 340 px, Minimum 150 px, Maximum min(500 px, 55 % Fensterhöhe),
  mindestens 150 px. Derselbe [Resizing-Kern](../shared/resize.js): volle dünne
  Trennkante, Pfeile 10 px, Umschalt 40 px, Pos1/Ende Grenzen, Enter/Doppelklick
  Standard, Escape/Pointerabbruch/Fokusverlust/Resize verwirft laufendes Ziehen.
  Die obere Trennkante bleibt auch ohne Zeigerkontakt in der gemeinsamen
  Konturfarbe sichtbar; Zeigerkontakt oder Tastaturfokus hebt sie mit der
  gemeinsamen Akzentfarbe hervor.
  **Abgeschlossen:** Die Korrektur ist seit Commit `ef1f3321` vorhanden.
  Der [Nachlauf vom 30. Oktober](../../verification/protocol-boundary-workspaces-2026-10-30.json)
  bestätigt die ruhende 1-px-Linie in Entwickeln, Simulieren und Auswerten auf
  Windows und Linux, je vier Theme-/Schriftzustände. Nur bei relevantem Anlass
  erneut prüfen; siehe [Wiedereinstieg](../../weiterarbeit.md).
  Kopf, Filter und Fuß bleiben stehen; Inhalt scrollt. Bei großer Schrift oder
  sehr niedriger Höhe scrollt der ganze Protokollinhalt, damit Meldungen und
  Aktionen erreichbar bleiben. Schließen gibt Fokus zurück.

## Abgedeckte Meldungsfälle

| Fall | Gestaltung / Auswirkung |
| --- | --- |
| Compilerfehler | Dateikontext, Originaldiagnose, Korrekturhinweis, optionaler Quellsprung; Buildfehler klar benannt |
| Compilerwarnung | Gelb; ein Build kann trotzdem erfolgreich sein |
| Ungültiges numerisches Modellergebnis | Fehler mit Lauf und Modellzeit; tatsächlichen Abbruchgrund von einer bloßen Fehlermeldung trennen |
| Modellwarnung | Freier Modelltext mit Annahme/Empfehlung; keine erfundene automatische Prüfung |
| Fehler beim Laden eines Messlaufs | Betroffener Lauf und Dateikontext; andere geladene Inhalte erhalten |
| Erfolgreicher Abschluss | Info mit Häkchen; kein zusätzlicher modaler Dialog |
| Debug / mehrzeiliger Text | Neutral, Originaltext vollständig, gesondert filterbar |
| Unterdrückte Meldungsflut | Anzahl ausgelassener Meldungen sichtbar; Vollständigkeit nicht vortäuschen |
| Keine Meldung / kein Suchtreffer | Unterschiedliche Leertexte; Download ohne sichtbare Meldung deaktiviert |
| Laden / Build / Abbrechen | Fortschrittsmeldung, Abschluss bzw. Unterbrechung in derselben Sitzung |

Die aktuelle Browser-Sitzung protokolliert echte **Demo**-Buildübergänge und
übernommene Vorschau-Messläufe. Fehler beim Demo-Build öffnen das Protokoll in
der Sitzungsansicht automatisch, ohne den Tastaturfokus aus dem Editor zu ziehen.
In der bewusst geöffneten Beispielansicht bleibt die Quellentrennung erhalten.
Die letzten 200 Sitzungsmeldungen werden gehalten und als solche bezeichnet.
Native Speicher-/Flutbegrenzungen müssen separat an vorhandene Verträge anbinden.

## Vertrag für den umsetzenden Chat

1. [Inhaltsinventar](CONTENT-INVENTORY.md) mitlesen. Vorhandene native Datenquellen
   weiterverwenden: `include/physim/log.h`, `include/physim/diagnostic.h`,
   `app/compiler_diagnostic.*`, `app/main.c`, `app/design_ui.inc`,
   `app/laboratory_tail.inc`, `include/physim/run_stream.h`.
2. `ps_log_record` enthält **level, time_s, message**. Keine Dateiposition, keinen
   Laufnamen, keine Wanduhruhrzeit und keinen getrennten Titel voraussetzen.
   Der Host darf Herkunft/Lauf zuordnen. Originalzeit nicht erfinden; fehlt sie,
   auslassen oder eindeutig als Empfangszeit kennzeichnen. Die Uhrzeiten der
   Designbeispiele sind illustrativ. `ps_diagnostic` hat dagegen optionale
   Quellposition, Operation und Argument; null bedeutet unbekannt.
3. GCC/Clang- und MSVC-Diagnosen unterstützen. Der derzeitige Compilerparser
   liefert Datei, Zeile, Spalte und Nachricht, jedoch kein eigenes Severity-Feld.
   Für echte Filter Schweregrad strukturiert erhalten/normalisieren und Rohtext
   aufbewahren; nicht jede Diagnose pauschal als Fehler behandeln.
4. **Logger-Level Fehler bedeutet allein keinen Abbruch.** Erst der tatsächliche
   Runner-/Build-/Analysezustand bestimmt die Auswirkung. Generische Modelltexte
   nicht mit frei erfundenen Ursachen oder Reparaturversprechen ergänzen.
5. Warnungen und Fehler müssen auch bei geschlossener Ereignisgruppe erkennbar
   bleiben. Gemeinsamer Zähler/Status verweist auf dieselbe ausgewählte Meldung.
   Wiederholungen nicht still verlieren; vorhandene Omission-Zähler übernehmen.
   Folgebewegung einer laufenden Liste darf eine bewusst gelesene Stelle nicht
   wegziehen; native Umsetzung braucht „neue Meldungen“-Hinweis/Rückkehr zum Ende.
6. Laufende/erfolgreiche/abgebrochene/fehlgeschlagene Jobs, mehrere Dateien,
   mehrzeilige UTF-8-Texte, fehlende Positionen, lange Pfade, viele Meldungen und
   nicht verfügbare Quelldateien im nativen Design prüfen. Ein nacktes `PS_LOG_ERROR`
   darf keine Simulation stoppen und keine Messdaten löschen.
7. Reale Speichern-/Kopieren-/Dateidialogfehler und Plattform-Fokus behandeln.
   Rohdaten, Quellcode und vorhandene Läufe beim Lesen/Filtern nicht ändern.
   Bestehende native Resizing-/Docking- und Accessibility-Verträge erhalten.

## Prüfung und Grenzen

Reproduzierbar: `node tools/check_protocol_design.cjs` mit Playwright/Edge.
Versionierte Browserbelege und Ergebnis: [protocol-review/](protocol-review/).
Details der tatsächlich ausgeführten Abnahme stehen in
[VALIDATION.md](protocol-review/VALIDATION.md). Bestehende gemeinsame Vorschau,
Resizing und Export werden zusätzlich auf Regression geprüft.

Dies ist eine HTML-Referenz auf Windows. Keine native Prüfung von Screenreadern,
physischen Displays/Monitorwechseln, macOS oder Linux und keine neue Behauptung
über die Fehlerbehandlung des nativen Runners. Ein leerer nativer Start bleibt leer;
die Beispielmeldungen gehören ausschließlich in eine bewusst gestartete Vorschau.
