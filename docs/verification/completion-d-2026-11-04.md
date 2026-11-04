# Vervollständigung D: Viewport und wirksame Editor-Einstellungen

Stand 4. November 2026. **Geprüfter Teilstand; vollständiges D und Gesamtziel
bleiben offen.** Maßgeblich sind ADR 022/023 und die freigegebene
[gesamte D-Spezifikation](../visuals/completion-typing-options/APPROVED.md).

## Verhalten und Speichervertrag

- Ganze Zeilen statt Vierer-Seiten: 400 logische Einheiten breite flache Liste,
  Standard fünf sichtbare Einträge, kleinere Ergebnismengen schrumpfen; verfügbare
  Editorhöhe begrenzt die Darstellung. Alle Treffer bleiben erreichbar. Mausrad
  und ziehbarer 6-Einheiten-Thumb ändern nur den unabhängigen Scrolloffset;
  ausdrückliche Pfeilwahl bringt den gewählten Eintrag ins Sichtfeld.
- Einzeilige, bei Bedarf gekürzte Namen; Rolle links, tatsächlicher Katalogtyp
  beziehungsweise Rückgabetyp rechts und Zähler der vollständigen Menge unten.
  Die getrennte neutrale Kandidatenkarte zeigt echte Katalogsignatur/Modul und
  öffnet die vorhandene API-Referenz. Sie ist keine Aufruf-Parameterhilfe.
- Einstellungen A / Editor: Vorschläge beim Tippen ein/aus und sichtbare
  Vervollständigungen 1–20, Standard 5. Ausschalten verhindert Vormerkung und
  automatische Anzeige; manuelles Strg+Leertaste bleibt wirksam. Die Oberfläche
  verwendet denselben Entwurf, Änderungsprüfung, Übernehmen, Abbrechen und
  Standardwerte-Vertrag wie die bisherigen Optionen.
- PSPREF06 ist 320 Bytes lang: automatische Anzeige bei 308, Zeilen bei 312,
  CRC über 316 Bytes am Offset 316. 01–05 bleiben lesbar; die neuen Werte
  erhalten dort 1/5. Die alte V5-CRC am Offset 308 wird nicht als Option gelesen.
  Ungültige Werte oder CRC erhalten die Ausgabe; fehlgeschlagene Speicherung
  erhält die bestehende Datei. Feldweise Zusammenführung erhält unabhängige
  Geometrie-/Workspace-/Ansichtsänderungen.

## Tatsächlich ausgeführt

[Originalkommandos, Ergebnisdateien, App-Aufrufe und SHA-256-Nachweise](completion-d-2026-11-04.json).
Getrennte Unit- und GUI-Aufrufe; der Display-Modus führt keine Units mit aus.
Windows Release/MSVC und Linux Debug/GCC mit ASan/UBSan; GUI-Leakprüfung wie im
bestehenden Harness deaktiviert. Vorhandene Compilerwarnungen bleiben sichtbar;
kein warnungsfreier Build behauptet. Isolierte Quellen enthalten eigene Änderungen,
keine fremden AT-SPI-/Orca-/Workflow-/Status-/dist-Arbeiten. Alle Ausgaben auf D:.

| Phase | Windows | Linux | Einordnung |
| --- | --- | --- | --- |
| `completion-d-viewport` | Unit 2/2, GUI 4/4 | Unit 2/2, GUI 4/4 | Erste Funktionstests bestanden; anschließende Bildprüfung fand trotzdem abgeschnittenen Referenzknopf. |
| `completion-d-clip-counter` | GUI 0/1, C-App an Stufe 5 abgelehnt | nicht ausgeführt | Absichtlich zu niedrige Karte in isolierter Quelle; neue Geometrieprüfung erkennt sie. Physim-App wird danach nicht gestartet. |
| `completion-d-final` | Unit 2/2, GUI 5/5 | Unit 2/2, GUI 5/5 | Korrigierte Karte, ergänzte große Schrift/Platzgrenze und langsame Tastaturgruppe. |

Units prüfen alle 20 Zeilenwerte mit abwechselnd aus-/eingeschalteter Automatik, unzulässige
Werte auch mit korrekter CRC, V5-Migration bei großer UI-Schrift und erneutes
V6-Schreiben; frühere 01–04-/Dock-/Fehler-/Aliasingfälle bleiben erhalten.
Die native Completion-Prüfung durchläuft C und Physim in drei Editorzielen mit
Pointer/Enter/Tab und Undo/Redo. Die automatische Gruppe prüft den bisherigen
Eingabevertrag plus ausgeschaltete Automatik mit manuellem Zugang, fünf ganze
Zeilen, alle Treffer, ein/neun Zeilen, unabhängiges Scrollen, Pfeil-Reveal und
wirkliches Thumb-Ziehen. UI-/Codeschrift 22 wird tatsächlich geladen, Hell und
Kontrast gewechselt, 20 gewünschte Zeilen auf den Editorraum begrenzt.

Settings verwendet tatsächliche Tabs und Aktivierung der beiden neuen Felder:
Entwurf 0/9 bei aktivem 1/5, Übernehmen, erneut gelesene Datei, Standardwerte 1/5
nur im Entwurf und Escape ohne Veränderung der gespeicherten 0/9. Die langsam
getaktete Gruppe erhält ihre 250-ms-Frames und unveränderte Schlussprüfung.
Die Referenzaktion wird zusätzlich auf vollständigen Platz innerhalb der
Kandidatenkarte geprüft. Der negative Höhenmutant und Originalfehler bleiben
als Belege getrennt von den erfolgreichen Ergebnissen erhalten.

## Zusätzlicher tatsächlicher Reflow-Befund

Der nachträglich erweiterte Test erhält einen gewählten späten Treffer beim
Wechsel von neun auf eine Zeile. `completion-d-reflow-before` führt tatsächlich
nur die C-App unter Windows aus und scheitert: 649 Treffer, gewählt 648,
Ausschnittsbeginn 640, eine sichtbare Zeile. Die Bedingung wird in Schritt 64
verletzt; die App-Diagnose nennt den bereits vorgemerkten Folgeschritt 69.
Keine mutierte Produktionsquelle, kein erfolgreicher Physim-/Linux-Lauf für
diesen Gegenfall behauptet. Die Korrektur fordert bei geänderter Viewporthöhe
explizit die Auswahl-Reveal an; ohne verbleibende Scrollspanne wird Drag beendet.
`completion-d-refined` prüft danach gezielt Completion-Unit und beide GUI-Gruppen;
die seit `completion-d-final` unveränderten Preferences-/Settings-Quellen werden
nicht bloß wegen dieses isolierten Completion-Fixes erneut geprüft. Der gezielte Abschluss besteht Windows/Linux je Unit **1/1** und GUI **2/2**,
`run-wzwnoqh0` / `run-3m9ciqz6` und `run-vrm9pyps` / `run-qghdxkv9`.
Die letzte
vollständige 2+5-Gruppe und der spätere gezielte Abschluss bleiben getrennt.

## Visuelle Prüfung und Grenzen

Das native Windows-Bild der ersten Phase zeigt den abgeschnittenen Knopf.
Die korrigierte Aufnahme zeigt die vollständige Karte mit Signaturfläche und
freiem Referenzlink; kein neuer gefüllter Vollbreitenknopf. Das ersetzt keine
vollständige D-Abnahme. Die interaktive lokale HTML-Datei konnte nicht geöffnet
werden: Computer Use meldete eine URL-Sicherheitsrichtlinie gegen `file:`.
Kein Browser-Umweg. Versionierte APPROVED.md, CSS und `review/color-dark.png`
wurden gelesen/betrachtet; vorhandene Referenz ist keine aktuelle Browserbedienung.

Noch erforderlich: gemeinsame freigegebene Syntaxrollen einschließlich gelber
Funktionen, passende Rollenglyphen/-farben und vollständige Kurztexte, echte
C-/Physim-Scopes mit Quellen und Revisionen sowie separate Aufrufsignatur mit
Benutzerfunktionen, aktivem Argument und eigener Escape-Lebenszeit. Physische
IME-/OS-Eingabe, Screenreader, DPI-/Monitor-/Vollbild- und aktuelle Mac-Abnahme
stehen weiter aus. Der Nutzer-Mac ist aus; keinerlei Zugriff oder Probe.
Gehostete CI-Resultate anderer Quellstände gelten nicht für diesen Stand.

Nächster konkreter Schritt: gemeinsame Syntaxrollen und Kandidaten-Kurzinfo
vervollständigen; danach/verbunden echte sprachabhängige Scope- und Aufrufdaten.
Vor weiteren Builds den gespeicherten Anschluss in docs/weiterarbeit.md lesen;
geschlossene Protokoll-/F1-/Settings-Prüfungen nicht wegen Kontextwechsel neu starten.

## Speicherpflege

23 ersetzte erfolgreiche BMPs aus den ersten beiden positiven Layoutphasen
entfernt: **73.529.574 Bytes**. Jede Datei vorab über aufgelösten D:-Pfad,
Eigentümerlauf, Namen, Reparse-/Schreibschutz, Größe und SHA-256 geprüft; eigene
Testprozesse beendet und alle Löschungen nachkontrolliert. Der ursprüngliche
Windows-Kartenfehler, beide Gegenfälle, zwölf aktuelle manuelle Completion-Bilder,
ausdrücklich benötigte ältere Referenzen, Quellen/Runtime-Archive, Nutzdaten,
Abhängigkeiten und VM bleiben erhalten. Ergebnis-/Beweisdateien unverändert.
