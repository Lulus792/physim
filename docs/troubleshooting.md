# Fehler finden und beheben

## Der Build schlägt fehl

Öffne **Protokoll / Ctrl+L** und beginne beim ersten Fehler. Ein Folgefehler kann
durch denselben fehlenden Namen oder Syntaxfehler entstehen. Klicke eine
Quelldiagnose an, korrigiere die Stelle und drücke erneut F5.
Für Experimente wird ein C17-Compiler benötigt, auch im portablen Paket.
Unter Windows findet Physim die Visual Studio C++ Build Tools mit Windows SDK
automatisch; unter Linux verwendet es `cc`. `PHYSIM_CC` wählt einen anderen
Compiler. Der Projektbuild benötigt keine CMake-Installation.
Nach einem SDK-Update müssen Experiment und Analyse neu gebaut werden; aktuell
gilt API/ABI 3. Ein altes Binärmodul kann trotz unveränderter Quellen inkompatibel sein.

[Toolchain einrichten und SDK prüfen](build.md)

## Der Lauf startet nicht oder bricht ab

Prüfe **Build erfolgreich**, den positiven Zeitschritt und das Protokoll.
Ein Callback-Fehler oder nichtendliche Messwerte stoppen den Runner. Häufige
Ursachen sind Division durch null, instabile Integration, ungültige Einheiten,
überschrittene Kanal-/Szenenkapazität oder eine eingestellte Laufgrenze.
Verkleinere bei instabiler Integration den Zeitschritt und prüfe deine Modellgleichung.
Eine kleinere Schrittweite behebt keinen falschen Einheitenfaktor.

Schreibe in C eine konkrete Erklärung nach `context->error`, bevor dein Callback
einen Fehler zurückgibt. In der Physim-Sprache liefern Laufzeitfehler Quelldiagnosen.
Eine Pause zählt beim realen Zeitlimit weiter. Prüfe deshalb bei einem unerwarteten
Abbruch auch **Laufgrenzen** und bei Serien deren eigenes Zeitlimit.

## Die Szene ist leer oder ein Objekt fehlt

Stelle die Kamera auf **Standard**, aktiviere die passenden Formschalter und
drücke unter Szeneneinträgen **Alle einblenden**. Alpha 0 macht ein Objekt unsichtbar.
Positionen und Größen sind in Metern, Y zeigt nach oben. Für Boxen sind die Größen
volle Ausdehnungen. Prüfe Szenengrenzen und eindeutige, von null verschiedene IDs.
`build_scene` baut jeden Snapshot neu; behalte keinen Zeiger auf die übergebene Szene.

[Formen und Koordinaten](api.md)

## Daten und Diagramm sehen unterschiedlich aus

Die Vorschau großer Läufe wird reduziert. Eine vollständige CSV kann daher mehr
Zeilen haben als die angezeigte Kurve. Der Export eines Ergebnisplots enthält
dessen Plotpunkte, der Reihenexport die vollständige Reihe. Mit **Alles zeigen**
setzt du außerdem einen eventuell aktiven Zoom zurück.
Sensorwerte müssen gemeinsam mit ihrem Status gelesen werden: 0 = nicht fällig,
1 = gültig, 2 = ausgefallen. Ein gespeicherter Zahlenwert bei Status 0 oder 2 ist
keine gültige neue Messung. Filtere Zeit, Wert und Unsicherheit mit derselben Auswahl.

[Reihen filtern und Zeitraster angleichen](series.md)

## Analyse oder Export meldet einen Fehler

Prüfe Kanalnamen einschließlich Groß-/Kleinschreibung, Einheiten und Reihenlängen.
`time` ist die Zeitachse. Gleich lange Reihen aus verschiedenen Läufen sind nicht
automatisch gepaart. Resampling verlangt einen passenden gemeinsamen Zeitbereich.
Ausgabepräfix bedeutet Ordner plus Dateinamensanfang; der Ordner muss existieren.
Vorhandene Ausgabedateien werden nicht überschrieben. Verwende einen neuen Präfix.
Behalte Handles nur solange ihr Context beziehungsweise Bericht lebt.

[Eigene Auswertung](series.md)

[Berichte und Lebensdauer](reports.md)

## Was bedeutet der Rückgabewert in C?

- `PS_OK`: erfolgreich; die Ausgabe ist verwendbar.
- `PS_INVALID`: ungültige Argumente, Dimensionen, Handles oder Wertebereiche.
- `PS_IO`: Datei-/Ein-/Ausgabefehler; Pfad und Schreibbarkeit prüfen.
- `PS_MEMORY`: Speicher konnte nicht reserviert werden.
- `PS_VERSION`: Versionsvertrag passt nicht; Module neu bauen.
- `PS_CORRUPT`: Daten verletzen den Dateivertrag.
- `PS_EOF`: Ende oder nicht gefunden, abhängig von der Funktion; kein normaler Messwert.
- `PS_RECOVERED`: Teil eines unvollständigen Laufs ist lesbar; Ergebnis als unvollständig behandeln.
- `PS_SINGULAR`: Gleichungssystem oder Transformation ist numerisch singulär.
- `PS_LIMIT`: Kapazität, Speicherquote, Iterationen oder Schrittbudget ausgeschöpft.
- `PS_NUMERIC`: Rechnung liefert unzulässige oder nichtendliche Zahlen.

`ps_result_string(result)` liefert eine lesbare Erklärung. Prüfe den Vertrag der
jeweiligen Funktion: Manche liefern bei `PS_LIMIT` eine Diagnose oder benötigte
Kapazität. Gehe nicht pauschal davon aus, dass alle Ausgaben gültig sind.

## Änderungen sind nach einem Abbruch verschwunden

Öffne dasselbe Projekt und prüfe den Wiederherstellungsdialog. Autosave sichert
Editoränderungen periodisch, nicht jede Eingabe sofort. Bewusstes Speichern und
die Quellkopien eines Laufs erfüllen unterschiedliche Zwecke.
[Wiederherstellung und Konflikte](autosave.md)
