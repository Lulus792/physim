# Diagramme und Tabellen aus Analysecode

`physim/report.h` verbindet C-Auswertungen mit der Ergebnisansicht der App. Der
Analyseprozess schreibt Diagramme, Tabellen und Herkunftsangaben nach
`<output_prefix>.psreport`. Nach erfolgreicher Analyse prüft ein Hintergrundthread
die Datei und zeigt **Analyseergebnis**. **Messdaten** führt zur ursprünglichen
Kanalvorschau. Bestehende Projekte behalten ihren eigenen Analysecode; neue Projekte
erhalten die erweiterte Vorlage.

## Ein Diagramm erzeugen

Im folgenden Beispiel sind `time` und `velocity` bereits ausgerichtete
Series-Handles desselben Contexts und Laufs.

```c
#include "physim/report.h"

static ps_result velocity_plot(ps_analysis_context *ctx, ps_series time,
                               ps_series velocity, const char *path) {
    ps_report *report = NULL;
    ps_result result = ps_report_create("Meine Auswertung", "Methode: zentrale Sekanten", &report);
    ps_plot_info plot = {0};
    snprintf(plot.title, sizeof plot.title, "Geschwindigkeit");
    snprintf(plot.x_label, sizeof plot.x_label, "Zeit");
    snprintf(plot.y_label, sizeof plot.y_label, "Geschwindigkeit");
    ps_report_unit_from(PS_SECOND, &plot.x_unit);
    ps_report_unit_from(PS_VELOCITY, &plot.y_unit);
    ps_plot_handle handle = {0};
    if (result == PS_OK) result = ps_report_add_plot(report, &plot, &handle);
    if (result == PS_OK)
        result = ps_report_add_series(report, handle, ctx, time, velocity,
                                      "Ableitung", PS_PLOT_LINE);
    if (result == PS_OK) result = ps_report_save(report, path);
    ps_report_destroy(report);
    return result;
}
```

Der Aufrufer bildet `path` aus seinem Ausgabepräfix plus `.psreport` und prüft die
Pfadlänge. Vorhandene Dateien werden nicht überschrieben. Das vollständige Beispiel
steht in der mitgelieferten `analysis.c`.

## Linien, Punkte und Histogramme

Ein Diagramm hat feste X-/Y-Dimensionen und Anzeigeskalen. `ps_report_add_series`
prüft Dimensionen und Samplezuordnung und konvertiert in die Achseneinheiten.
Mehrere Kurven können Rohableitung und gefilterte Geschwindigkeit vergleichen.
`PS_PLOT_SCATTER` zeichnet Punkte, etwa einen Phasenraum. `ps_report_add_curve`
kopiert selbst berechnete Punkte; diese müssen bereits in den Achseneinheiten vorliegen.

Pro Kurve werden höchstens 2048 Punkte gespeichert. Bei größeren Linienreihen liest
die API alle Werte blockweise und bewahrt je Indexabschnitt Minimum und Maximum in
ihrer ursprünglichen Reihenfolge sowie die beiden Endpunkte. Punktdiagramme werden
gleichmäßig über die Sampleindizes abgetastet. `source_count` enthält die ursprüngliche
Punktzahl; die App kennzeichnet reduzierte Vorschauen. Die vollständige Datenreihe
bleibt unabhängig mit `ps_series_export_csv` exportierbar.

`ps_report_add_histogram` zählt alle Werte in bis zu 128 gleich breiten Klassen.
Die letzte Klasse schließt das Maximum ein. Eine konstante Reihe erhält eine einzige
Klasse mit positiver Breite. Die Y-Achse zeigt absolute Anzahlen, keine
Wahrscheinlichkeitsdichte. Mehr als 2^53 Samples sind nicht zulässig,
da die Darstellung Anzahlen als Double speichert.

## Tabellen und Export

`ps_report_add_table` legt Titel und bis zu acht Spalten mit jeweils einer Einheit
fest. `ps_report_add_row` kopiert Zeilenname und endliche Zahlen. Eine Tabelle fasst
bis zu 256 Zeilen; die App bietet horizontales und vertikales Scrollen.
Unterschiedliche Größen stehen in Spalten mit eigenen Einheiten.

**SVG · Diagramm** exportiert das ausgewählte Diagramm mit Achsen, Legende und
Einheiten. **PNG · Diagramm** erzeugt ein Bild mit standardmäßig 2400 × 1700 Pixeln, weißem
Hintergrund, Titel, Achsen, Einheiten und einer Legende für bis zu acht Kurven.
Die Bildgröße bleibt unabhängig von Fenstergröße und Bildschirmauflösung.
Unter **PNG-Größe (Pixel)** stehen 1200 × 850, 2400 × 1700, 3600 × 2550 und
4800 × 3400 zur Wahl. Die Auswahl gilt für alle PNG-Exporte in der aktuellen
App-Sitzung; beim nächsten Start ist wieder 2400 × 1700 voreingestellt. Titel,
Ränder, Legende und Diagramminhalt behalten bei allen Größen dieselben Proportionen.
Eine größere PNG-Datei enthält mehr Bildpixel, keine zusätzlichen Messpunkte.
Große gemeinsame Zahlenanteile erscheinen als Achsenoffset. Reduzierte
Berichtsdaten sind im Bild gekennzeichnet. **CSV · Diagrammdaten** exportiert
die im Bericht gespeicherten Punkte bzw.
Histogrammklassen samt ursprünglicher Punktzahl. **Tabelle als CSV** exportiert alle
Ergebniszeilen. Dateien entstehen mit neuem Namen im `runs`-Ordner; der genaue Pfad
steht im Protokoll. Bestehende Dateien werden nicht überschrieben.
SVG ist skalierbar; PNG eignet sich zum Einfügen in Berichte und Präsentationen.
Der PNG-Schreiber komprimiert verlustfrei mit statisch eingebundener zlib.
Die Dateigröße hängt vom Diagramminhalt ab; die Bildpixel bleiben unverändert.
Die Verarbeitung erfolgt zeilenweise ohne zusätzlichen vollständigen Bildpuffer.
Die Ausgabegröße wird vor der Allokation gegen die
Texturgrenze der Grafikkarte geprüft; eine nicht unterstützte Größe meldet einen
fehlgeschlagenen Export. Eine kleinere Auswahl kann dann verwendet werden.

## Diagrammausschnitte untersuchen

In Ergebnisdiagrammen und gespeicherten Messdaten zoomt das Mausrad um die Position
des Mauszeigers. Mit gedrückter linker Maustaste lässt sich der Ausschnitt verschieben.
„Zoom +“ und „Zoom -“ vergrößern beziehungsweise verkleinern um die Mitte.
„Alles zeigen“ stellt beide Achsen auf den vollständigen Bereich der Vorschau zurück.
Der Ausschnitt bleibt innerhalb des Datenbereichs; der maximale Zoom beträgt
eine Million. Nicht mehr unterscheidbare Achsengrenzen begrenzen ihn gegebenenfalls früher.
Bei großen gemeinsamen Zahlenanteilen zeigt die App einen Achsenoffset separat,
damit die Tickbeschriftungen auch bei starker Vergrößerung unterscheidbar bleiben:
der tatsächliche Wert ist Offset plus Achsenwert.

Jedes Diagramm und jeder Messkanal merkt sich während der Sitzung seinen Ausschnitt.
Das Öffnen eines Berichts beziehungsweise eines anderen Datensatzes setzt die zugehörigen
Ansichten zurück. Die laufende Simulation zeigt weiterhin automatisch den gesamten
Live-Verlauf. Mausradbewegungen außerhalb der Diagrammfläche bleiben normales Scrollen.

Zoom verändert weder Werte noch Statistik oder Histogrammklassen.
Bei vergrößerter Ansicht heißen die Exportaktionen ausdrücklich „CSV · Alle Daten“,
„SVG · Ganzes Diagramm“ und „PNG · Ganzes Diagramm“. Diese Exporte enthalten den vollständigen
Berichtsplot. Zusätzlich speichern **PNG · Ausschnitt** und **SVG · Ausschnitt** die gerade sichtbaren
Achsengrenzen mit Titel, Einheiten und Legende. PNG verwendet die gewählte Pixelgröße;
SVG bleibt skalierbar und zeigt separate Achsenoffsets.
Die Knöpfe sind bei unvergrößerter Ansicht deaktiviert. Linien und Balken werden an
den Grenzen abgeschnitten; die Quelldaten bleiben vollständig erhalten.
Die Vorschau bleibt gegebenenfalls reduziert; Zoom rekonstruiert keine
zuvor ausgelassenen Punkte. Sensorpunkte mit ungültigem Status bleiben ausgeblendet.

## Lebensdauer und Dateivertrag

Ein Bericht besitzt bis zu 16 Diagramme mit je acht Kurven und acht Tabellen.
Seine Handles gelten nur während seiner Lebensdauer und nur für ihn. Elemente
werden angehängt und können nicht einzeln entfernt werden. Mutierende Funktionen
lassen den Bericht bei Fehlern unverändert. Daten und Titel werden kopiert;
Quellreihen und Context dürfen anschließend geschlossen werden. Lesefunktionen
mit Suffix `_read` liefern Kopien ohne interne Zeiger. Zusätzlich liefert
`ps_report_curve_view` einen unveränderlichen Zeiger ohne Kopie für die Anzeige.
Dieser bleibt bis zum Zerstören des Berichts gültig, auch wenn weitere Kurven,
Diagramme oder Tabellen angehängt werden. Der Aufrufer darf ihn weder verändern
noch freigeben. Änderungen und Zerstörung müssen gegen gleichzeitige Leser
synchronisiert werden; die Oberfläche verwendet Views nur während ihres
synchronen Zeichenaufrufs. Bei Fehlern bleibt der Ausgabezeiger unverändert.

Der Reader akzeptiert höchstens 8 MiB Payload. Format 1 beginnt mit acht Bytes
`PSRPT17\n`, gefolgt von Little-Endian-UInt32 für Version, Payloadlänge und CRC32.
Die Payload codiert Titel/Herkunft, Plot-/Tabellenanzahl und die Einträge in dieser
Reihenfolge. Strings sind UTF-8 mit UInt32-Bytelänge, ohne NUL im Inhalt; Zahlen
sind IEEE-754-Binary64, Anzahlen UInt32 bzw. UInt64. Rohe C-Strukturen oder Zeiger
werden nicht serialisiert. Das Format ist eine Entwicklungsversion.

Vor Übernahme prüft der Reader Version, CRC, Grenzen, Dimensionen, endliche Zahlen,
UTF-8 und das exakte Dateiende. Ein abgebrochener Schreibvorgang kann eine Teil-Datei
hinterlassen; diese wird verworfen. Ein ungültiger Bericht ersetzt kein bereits
geladenes Ergebnis. Herkunftsangaben sind Text: Die App lädt daraus keine Dateien
und führt weder Pfade noch Links aus. Analysecode bleibt im separaten Runner.

Die Vorlage dokumentiert Eingabedatei, Analysequellcode-Snapshot, Compiler,
Buildzeit und Recovery-Status. Das bestehende Analysemanifest enthält zusätzlich
die Laufmetadaten. Ein über alle Artefakte kryptographisch abgesichertes Manifest
ist noch offen. Der Runner schreibt zusätzlich `<präfix>.inputs.csv` mit Modul und
allen Eingängen, Dateigrößen und FNV-1a-Fingerabdrücken. Diese sind keine
kryptographischen Nachweise. Frühere Berichte lassen sich über
[Läufe & Berichte](runs.md) ohne Neubau direkt in der App öffnen.


## Maskierte Kurven und Segmentgrenzen

`ps_report_add_series` übernimmt Gültigkeitsmasken automatisch. Ungültige Punkte
beeinflussen weder Achsengrenzen noch Statistik oder Histogramme. Linien verbinden
nur zusammenhängende gültige Beobachtungen; einzelne gültige Punkte bleiben als
Punktmarker sichtbar. Die begrenzte Vorschau erhält gültige Extrema und markiert
Segmentwechsel auch dann, wenn dazwischenliegende Zeilen bei der Reduktion
entfallen. Für sämtliche Originalzeilen verwende den Reihenexport.

Für eigene Kurvendaten bietet `ps_report_add_curve_masked` eine Flagfolge neben
dem unveränderten `ps_curve_data`: 0 = fehlend, 1 = gültig und verbunden, 3 = gültig
mit neuem Segment. `NULL` bezeichnet vollständig gültige, zusammenhängende Daten.
`ps_report_curve_mask` leiht diese Flags unveränderlich mit derselben Lebensdauer
wie `curve_view`. Die vorhandenen C-Strukturen und ABI 3 bleiben unverändert.

GUI, SVG und PNG beachten diese Flags. Der Plot-CSV ergänzt bei maskierten
Diagrammen `valid` und `segment_start`; fehlende x/y-Felder bleiben leer.
Ein Diagramm ohne gültige Punkte hat keine numerischen Achsengrenzen. Die App
zeigt dies ausdrücklich; ein PNG-/SVG-Export ohne Grenzen liefert einen Fehler.
Der Bericht selbst kann weiterhin gespeichert und wieder geöffnet werden.

Normale Berichte werden unverändert als Format 1 gespeichert. Sobald eine Kurve
fehlende Punkte oder Segmentanfänge enthält, verwendet der Writer Format 2.
Der Reader akzeptiert beide Formate und prüft Größenlimits, CRC, Koordinaten und
Flags, bevor er einen Bericht veröffentlicht.
