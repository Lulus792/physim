# Protokoll C – nativer Arbeitsstand vom 25. Oktober 2026

**Nachlauf vom 26. Oktober:** Der [Fokusbericht](PROTOCOL-FOCUS-NATIVE.md)
ergänzt verschärfte Gegenproben, die Adapterkorrektur und den weiterhin offenen
direkten Mausfokus. Der folgende Text dokumentiert den Prüfstand vom 25. Oktober.

Die ausgewählte Navigator-Anordnung C ist im lokalen Arbeitsstand begonnen.
Sie ist **noch nicht abgenommen und noch nicht als Quelländerung veröffentlicht**.
Der letzte reguläre Windows-Lauf scheitert bei der Mausziehung der Höhenleiste.
Der entsprechende Linux-Lauf besteht. Frühere erfolgreiche Windows-Läufe
heben diesen offenen Befund nicht auf.

Dieser Bericht hält den Stand ohne weitere Bildschirmbilder fest. Die
[kompakte Ergebnisliste](../../verification/protocol-c-2026-10-25.json) nennt
Quellmanifest, Binärhashes, Originalberichte, Prüfsummen und tatsächliche
Einzelresultate. Die vollständigen lokalen Belege liegen unter
`D:/Physim/build/validation/protocol-ui-2026-10-25`.

## Implementierter lokaler Umfang

- Gemeinsamer Protokollbereich mit Herkunfts-/Severity-Filtern, Unicode-Suche,
  Meldungsliste, Details, explizitem Quellsprung und TXT-Ausgabe der Originale.
- Natürliche Filterbreiten, getrennte Schriftrollen und Herkunftssymbole aus
  den freigegebenen SVG-Pfaden; schmale Anordnung mit Details unter der Liste.
- Höhensteuerung mit Tastatur und Maus, Herauslösen und Rückkehr an den
  vorgesehenen Platz. Das native Fenster übernimmt zunächst die tatsächliche
  Größe des Ursprungsbereichs.
- Dieselbe Anfangsgröße wird für die sechs herauslösbaren Simulationsbereiche
  verwendet. Szenenbaum und Inspektor behalten dort ihre vorhandenen Überschriften.
- Simulationshöhen berücksichtigen den gemeinsamen Protokollbereich, Messfläche
  und Zeitleiste. Größere Schrift und kleine Fenster benötigen weiterhin einen
  eigenen Abgleich mit der freigegebenen Vorlage.

Die Implementierung enthält auch neue Test-Fixtures. Deren ausdrücklich als
Prüfmeldungen angelegte Datensätze sind kein Nachweis echter Compiler- oder
Laufzeitmeldungen. Der bestehende Anschluss dieser Datenquellen ist im
[vorherigen Bericht](PROTOCOL-SOURCES-NATIVE.md) beschrieben; sein neuer
Bildschirmablauf muss mit Navigator C erneut geprüft werden.

## Tatsächlich ausgeführte Prüfungen

Windows: Release, SDL, NVIDIA/OpenGL. Linux: `Alpine-Dev`, Debug mit ASan/UBSan,
Xvfb/Openbox und Software-OpenGL. `detect_leaks=0`; keine Aussage über Leaks.
Alle neuen Ausgaben liegen auf D: beziehungsweise `/mnt/d/Physim/build`.

| Prüfung | Windows | Linux | Geltungsbereich |
| --- | --- | --- | --- |
| `protocol_store`, `protocol_text`, `preferences`, `layout_catalog` | 4 bestanden | 4 bestanden | Phase 13; spätere Änderungen betreffen diese Prüflinge nicht |
| `docking_workflow` | bestanden | bestanden | Phase 14; sechs Bereiche, C dunkel und Phys hell; reale Projekte, Lauf und Rückkehr |
| `protocol_navigator` im regulären Katalog | fehlgeschlagen | bestanden | Phase 15; Windows stoppt im ersten Modus, Linux besteht alle acht Varianten |
| `test_native_test_runner.py` | bestanden | bestanden | Phase 15; Testläufer und Katalog, keine zusätzliche UI-Abnahme |

Linux prüft dunkel, hell, hohen Kontrast, große Schrift, 1080×740,
1150-Pixel-Grenze, leere Meldungshistorie sowie ein ausgeblendetes Fenster.
Der volle dunkle Ablauf prüft 70 Schritte, unter anderem Filter, unveränderte
Editorbytes, Quellsprung, TXT-Fehlerfall und Herauslösen/Rückkehr. Die leere
Meldungshistorie verwendet ein geladenes Prüfprojekt; sie belegt nicht die
gesondert freigegebene Gesamtleeransicht ohne Projekt.

Die Originalberichte enthalten die tatsächlichen Befehle. Beispielsweise startet
der registrierte Prüfer das Programm mit `--workspace-state-test <Prüfordner>
protocol-ui-dark`. Zum Wiederholen einen neuen Prüfordner auf D: verwenden;
eingefrorene Belege nicht überschreiben. Nach Quelländerungen neu bauen und
einen neuen Nachweis anlegen. Die Quellsicherung umfasst 324 Dateien des damaligen
lokalen Stands einschließlich fremder Änderungen; sie ist kein Git-Release.

## Offener Windows-Befund und nächste Prüfung

Im Lauf `run-d1dabwlf` bestehen die fokussierten Tastenaktionen: Shift+Auf
setzt 340 auf 380, Pos1 auf 150, Ende auf 500 und Eingabe zurück auf 340.
Die anschließende synthetische Mausziehung soll die Höhe auf 370 setzen und
eine aktive Größenänderung melden. In Schritt 45 bleiben Höhe 340 und
`resize=0`. Timeout ist nicht die Ursache dieses konkreten Laufabbruchs.
Die Ursache zwischen Eingabeweg und Produkt ist noch nicht belegt.

Als Nächstes diese Interaktion am isolierten nativen Prüfprojekt mit realer
Mauseingabe kontrollieren und bei Bedarf den Beginn der Größenänderung gezielt
protokollieren. Erwartete Bewegungswerte und Fokusvoraussetzungen beibehalten.
Danach die bestehenden Diagnose-, Logging- und `protocol_sources`-Abläufe mit
den neuen Auswahl-/Quellsprungaktionen abgleichen und tatsächlich ausführen.

Physische Tastatur-/Mausbedienung, Screenreader, Dateidialog/Abbrechen,
Zwischenablage und macOS sind für diesen Stand nicht nachgewiesen. Die
Überschriften weiterer herausgelöster Entwickeln-Bereiche und die vollständige
Designgleichheit bleiben ebenfalls offen. Ein bestandener Teiltest ersetzt
keine Gesamtbewertung von Entwickeln, Simulieren, Auswerten und F1.

## Aufbewahrung

Die Originalberichte, fehlgeschlagenen Läufe, Quellen- und Laufzeitarchive sowie
Referenz-/Fehlerbilder bleiben erhalten. Nur 236 wiederherstellbare
Compiler-Zwischendateien dieser abgeschlossenen Prüfbuilds wurden nach
Hashprüfung der erhaltenen Programme entfernt; siehe
[Speicherbericht](../../storage-cleanup-2026-10-25.md).
