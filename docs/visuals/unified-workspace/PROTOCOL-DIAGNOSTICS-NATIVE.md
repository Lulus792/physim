# Protokoll C – echte Diagnosen im Navigator, 26. Oktober 2026

Der native Diagnoseablauf besteht unter Windows und Linux jeweils alle sechs
Fälle. Eine echte Compilerdiagnose aus `include/helper α.h` wird im Navigator
ausgewählt; erst **Quellstelle ansehen** öffnet die Datei und markiert Zeile 2.
Der Test repariert anschließend die Datei, baut das Projekt und prüft den echten
gespeicherten Lauf. Die vier Fälle für C-/Physim-Experimente und Analysen verwenden
jetzt ebenfalls die sichtbare Meldungsauswahl und die ausdrückliche Quellaktion.
Der zusätzliche Fremddateifall bleibt eine getrennte Pfad-/Metadatenprüfung.

Dies ist ein weiterer Teilnachweis. Er schließt weder den
[offenen direkten Mausfokus](PROTOCOL-FOCUS-NATIVE.md) noch die vollständige
Design- oder Plattformabnahme. Der [bisherige Bericht](PROTOCOL-C-NATIVE.md)
behält seinen datierten Geltungsbereich.

## Gegenstand und Prüfweg

Der alte Test erwartete ein `diagnostic_bounds`-Rechteck der ersetzten Anzeige
und einen impliziten Zeilen-Quellsprung. Der neue Ablauf bedient Schweregradfilter,
Meldungsliste und Quellaktion über native SDL-Ereignisse. Bei 1080 × 740 erreicht
er die Bedienelemente durch die vorhandenen inneren und äußeren Scrollbereiche.
Er schreibt weder Auswahl noch Scrollposition direkt in die Navigatorzustände.

Zusätzliche Prüfungen:

- Die Zeilenauswahl lässt den bisherigen Arbeitsbereich unverändert.
- Die Quelldatei und ihre markierte Fehlerzeile stimmen mit der tatsächlichen
  Compilerdiagnose überein; der fehlgeschlagene Build startet keinen Lauf.
- Einzelne Diagnose → vollständige Prozessausgabe → einzelne Diagnose erhält
  Originalbytes, Größe und CRC32. Der größere Text ist der zugehörige echte
  Compilertranskript-Datensatz, kein Beispieltext.
- Nach der Reparatur wird die Datei gespeichert. Der erfolgreiche Build startet
  einen echten Lauf mit mindestens drei lesbaren Messpunkten.
- Die strukturierten Laufzeit-/Analysediagnosen behalten Originalmeldung,
  Fehlercode, Operation, Argument, Quellposition und gültige Sidecar-Prüfsumme.

Der Python-Prüfer akzeptiert die tatsächlich aufgezeichnete Windows-Sequenz.
Drei gezielt veränderte Gegenbelege werden abgewiesen: fehlende Quellaktion,
veränderte Originalprüfsumme nach Rückkehr und unvollständige Prozessausgabe.
Diese Gegenproben prüfen die Aussagekraft des Prüfers; sie sind keine weiteren
nativen Programmläufe.

Zusätzlich wurden die 31 eigenen Quell- und Teständerungen auf eine isolierte
Kopie des veröffentlichten Git-Stands gelegt. Andere uncommittete Änderungen
blieben ausgeschlossen. Dort bestehen unter Windows und Linux jeweils der
vollständige App-Build, vier gezielte Unit-Gruppen sowie vier UI-Gruppen:
`docking_workflow`, `protocol_navigator`, `protocol_sources` und
`diagnostic_workflow`. Das grenzt die nachgewiesene Integration von den übrigen
lokalen Arbeiten ab; es ist weiterhin keine Gesamtfreigabe der Oberfläche.

## Nachweise und Aufbewahrung

Die [kompakte Ergebnisliste](../../verification/protocol-diagnostics-2026-10-26.json)
nennt Quellstand, Programme, Befehle, Exitcodes und Hashes der Originalberichte.
Die vollständigen Daten liegen unter
`D:/Physim/build/validation/protocol-ui-2026-10-25/20-native` und den darin
referenzierten Ergebnisordnern. Windows verwendet MSVC Release und NVIDIA/OpenGL,
Linux `Alpine-Dev`, GCC Debug mit ASan/UBSan und Xvfb/Openbox/Software-OpenGL.
Leak-Erkennung ist dort deaktiviert; kein Leak-Nachweis wird behauptet.

Im Compiler-Hilfsdateifall ersetzt ein PNG des Quellsprungs die bisherigen drei
BMP-Zustandsbilder. Inhalt und Rückkehr der Originalanzeige werden als native
Zustandswerte dokumentiert. Zusätzliche Bilder werden nur auf ausdrücklichen
Prüfaufruf mit `PHYSIM_TEST_ALL_CAPTURES` erzeugt. Vorhandene eingefrorene
Referenzen und Belege offener Fehler bleiben erhalten.

Der ausgewählte [Windows-Quellsprung](protocol-native-2026-10-26/compiler-source.png)
stammt aus diesem isolierten Lauf. Er zeigt sowohl die korrekt markierte
Fehlerzeile als auch den nachfolgend beschriebenen offenen Originaltextbefund.

## Neuer offener Darstellungsbefund: lange Originalzeile

Die Quellsprungbilder zeigen auf beiden Plattformen eine lange technische
Originalzeile bis zum rechten Feldrand. Der vollständige Inhalt bleibt im
Protokollspeicher und in der Kopier-/TXT-Quelle erhalten. Die normale sichtbare
Erreichbarkeit ihres Endes ist damit nicht nachgewiesen.

Im eingebundenen Nuklear-Editor bricht `nk_textedit_layout_row` nur an echten
Zeilenumbrüchen um. `NK_EDIT_NO_HORIZONTAL_SCROLL` erzeugt keinen weichen Umbruch;
es verhindert vielmehr die horizontale Nachführung des Cursors. Ein weicher
Umbruch braucht deshalb einen passenden nativen Zeichen-, Auswahl- und
Accessibility-Pfad. Zusätzliche Umbruchbytes im Originaltext wären ungeeignet,
weil sie die auswählbare Originalmeldung verändern würden.

Die [freigegebene CSS-Referenz](../design-options/options.css) verwendet für das
Original-`pre` ausdrücklich `white-space: pre-wrap` und `overflow-wrap: anywhere`
bei `.8rem` Schrift und Zeilenhöhe `1.85`. Der native Textbereich muss dieses
Verhalten noch erhalten; ein dauerhaft horizontaler Ersatz wäre nicht identisch
mit der Vorlage.

Als Nächstes diesen Fall mit einem gezielten langen Originaltext prüfen und
das sichtbare Zeilenende sowie Auswahl/Kopie der unveränderten Bytes belegen.
Der bestandene Diagnose-Datenablauf ersetzt diese Darstellungsprüfung nicht.
