# Speicheraufnahme und gezielte Bereinigung – 18. November 2026

Anlass ist der ausdrückliche Auftrag, sämtliche Physim-Daten und Builds auf
entbehrliche Daten zu prüfen. Grundlage: Git-HEAD
`01ccc27a94ca611c9e9cdd8ccde0da426aee2378`; der Arbeitsbaum enthielt fremde,
nicht abgeschlossene Änderungen. Die Metadatenaufnahme ist kein inhaltlicher
Einzelnachweis für jede Datei und keine vollständige Freigabe aller Buildbäume.

## Bestandsaufnahme

Am 18. November 2026, 14:08 MEZ, wurden unter `D:/Physim` rekursiv 542.908
reguläre Dateien gezählt: 88.253.270.487 logische und 42.904.799.152 zugeordnete
Bytes. NTFS-Zuordnung und Hardlinks wurden berücksichtigt; 16 Reparse-Punkte
wurden nicht verfolgt, Fehler: 0. Die Aufnahme erfasst auch Git und Quellen und
ist eine Momentaufnahme. Das vollständige lokale Ergebnis liegt unter
`build/maintenance/storage-inventory-20261118-140849.json`, SHA-256
`4e757c65cea4a0571c198fe14a47f488a661a255c89353851e58956b89be3f69`.

`build` belegt 29.403.580.216 Bytes, darin `validation` 15.747.659.456,
`native` 8.257.188.624 und `archive` 5.240.261.440 Bytes. Der separate ältere
Baum `build-clang-ninja` belegt 7.583.088.488 Bytes; allein dessen `language-ui`
enthält 133.687 Dateien mit 5.203.014.802 logischen Bytes. Historische
SDK-Prüflogs in diesem Baum sind in der Projektdokumentation referenziert.
Programme, Debugsymbole, Testdaten und Referenzen bedürfen einer gesonderten
Inhalts- und Verwendungsprüfung; ihre Dateiendung oder Reproduzierbarkeit allein
ist kein Löschbeleg. Andere alte Buildbäume sind ebenfalls noch nicht einzeln
auf ihren Nachweiszweck geprüft.

Die gemeinsam mit NexaUI und NexaCloud genutzte WSL-VHDX maß 82.163.269.632
Bytes und ist kein Physim-Alleinbestand. Während des Nachlaufs war fremde
NexaUI-Arbeit in WSL aktiv. Die VHDX und fremde Arbeit wurden erhalten; F: und
der ausgeschaltete Nutzer-Mac wurden nicht angesprochen.

## Tatsächlich entfernt

Aus den zwei abgeschlossenen Prüfständen
`build/validation/explorer-inline-2026-10-29/cursor-units2-{windows,linux}/obj`
wurden **84 Compilerobjekte und zugehörige Cache-Signaturen mit zusammen
23.410.082 logischen Bytes** einzeln entfernt. Die beiden dokumentierten Läufe
enthielten je zehn erfolgreiche Testfälle. Die ursprünglichen Programme und
Symbole wurden vor der Löschung gegen alle 31 Einträge der zwei unveränderten,
hashgeprüften Laufzeitarchive geprüft und nach der Löschung erneut gehasht.
Ergebnisberichte, Logs, Archive und Programme bleiben erhalten. Beim erneuten
Build dieser alten Prüfstände müssen die Objekte neu erzeugt werden.

Vorher und nachher meldete D: 6.826.500.096 beziehungsweise 6.834.315.264 freie
Bytes, eine beobachtete Zunahme um 7.815.168 Bytes. Wegen NTFS-Kompression und
paralleler Laufwerksnutzung ist diese Differenz kein isolierter Messwert für die
Löschung. Freier Platz innerhalb der gemeinsamen VM wurde nicht gemessen.

Der dateigenaue Plan und die Löschquittung liegen lokal unter `build/maintenance`:

- `cleanup-cursor-units-2026-11-18-plan.json`, SHA-256
  `ebeaedc0583d1eb3c5ac99c933eeb9bf5192e02a40fdc54c9629da0172c5279b`.
- `cleanup-cursor-units-2026-11-18-removed.jsonl`, SHA-256
  `10532244a795b303a225aca73e72fc51fd78e2dcca1a3dc05c3ecf0632b9925b`.
- Planer `.py`, SHA-256 `b574c9beeab70b2f17329268d99620462de9af43775f97c9075729f27386d55f`,
  und Löscher `.ps1`, SHA-256 `e1776e5e4c34823dcbe394d6433e05f625fd975bcca94a91397c01960b3ae47d`,
  jeweils mit demselben Präfix. Der Planer prüft Archive, Laufzeitdateien,
  Dateityp, Linkzahl, Zielpfade und Hashes; der Löscher prüft diese Dateihashes
  und Verzeichnisgrenzen erneut. Keine rekursive Verzeichnislöschung.

Ausdrücklich eingefrorene Belege, offene Fehler, Nutzdaten, Offline-Abhängigkeiten,
aktive Buildstände und die früher automatisch abgelehnten Löschziele blieben
erhalten. Die Inventur und dieser gezielte Schritt ersetzen keine vollständige
inhaltliche Prüfung sämtlicher 542.908 Dateien. Es wurden keine neuen Produkt-
oder Plattformtests ausgeführt; die Löschung ändert keine Quellen.

## Nachlauf: alte MSBuild-Nachverfolgung

Anschließend wurden im Physim-Bestand
`build-clang-ninja/language-ui/<Testprojekt>/build` **50.048 Dateien mit
193.157.127 logischen Bytes** aus 1.088 alten Testprojekten entfernt. Die Auswahl
war auf reguläre `*.tlog`-Dateien in MSBuild-`*.tlog`-Verzeichnissen beschränkt:
erzeugte Compiler-, Linker- und CustomBuild-Abhängigkeits-/Befehlsnachverfolgung
für inkrementelle Builds. Erzeugte Projekte, Simulationsergebnisse, Testlogs,
Programme, Bibliotheken und PDB-Dateien waren nicht Teil des Löschplans. Ein
erneuter MSBuild-Lauf muss die entfernte Nachverfolgung neu aufbauen.

Vorher wurden Pfade, Dateityp, Linkzahl und SHA-256 einzeln erfasst und vor jedem
Löschen überprüft; Verknüpfungen wurden nicht verfolgt. Der Abschlussvergleich
bestätigt 50.048 geplante und 50.048 quittierte Einträge mit identischen Inhalten
und 193.157.127 logischen Bytes; jeder entfernte Pfad wurde unmittelbar nach dem
Entfernen auf Abwesenheit geprüft. Windows-Prozessliste und Alpine-Prozessliste
zeigten vorab keinen Physim-Buildprozess. Es wurden keine Verzeichnisse gelöscht.

D: meldete vor diesem Nachlauf 6.815.805.440 und nachher 7.066.652.672 freie
Bytes, eine beobachtete Zunahme von 250.847.232 Bytes. Dieser Unterschied ist
wegen paralleler Laufwerksaktivität kein isolierter Löschmesswert. Die plan- und
quittungsidentischen lokalen JSONL-Dateien liegen unter `build/maintenance` und
heißen `language-tlogs-2026-11-18-plan.jsonl` und
`language-tlogs-2026-11-18-removed.jsonl`; beide haben
SHA-256 `dd06ecb572fbd2f6721d7bfd73ac419ec220cd13ca0d0ed4240018ecb86cb46`.
Die lokalen Werkzeuge `plan-language-tlogs-2026-11-18.py` und
`remove-language-tlogs-2026-11-18.py` haben die SHA-256-Werte
`6ca2678a3664fbe08c097f565254fd47e29ada04ec115d487449d2ab031f1c15` und
`e010c2ccdb50b9a4fa79ffb12b95a7bbecf5476fb56690b8fab5c9b0ba64e9cc`.

Eine zwischenzeitlich begonnene Prüfsummenaufnahme zur möglichen Kompression
wurde auf Nutzerhinweis vor jeder Kompression beendet; eigene unvollständige
Hilfsdateien wurden entfernt. Nach Nutzerpräzisierung, dass Kompression zusätzlich
zum Aussortieren erwünscht ist, folgte die unten beschriebene begrenzte Maßnahme.
Weitere Physim-Buildbäume, insbesondere ihre Ergebnis- und Symbolbestände,
bleiben einzeln auf Zweck und Entbehrlichkeit zu prüfen. Der Nachlauf behauptet
keine vollständige inhaltliche Abnahme.

## Zusätzlich: Debugsymbole platzsparend aufbewahrt

Im alten Physim-Testbaum `build-clang-ninja/language-ui` wurden ausschließlich
5.440 vorhandene `.pdb`-Dateien transparent mit NTFS komprimiert. Der Aufruf
`compact.exe /C /A /S /Q '*.pdb'` lief im genannten Verzeichnis und endete mit
Exitcode 0. Vorher wurden alle Dateien und Verzeichnisse dort auf Reparse-Punkte
geprüft, die Symboldateien zusätzlich auf regulären Dateityp und einfache
Linkzahl. Jedes Symbol wurde vor und nach dem Lauf vollständig mit SHA-256
gelesen: 5.440 Dateien, 3.005.054.976 logische Bytes, alle Pfade und Inhalte
identisch. Keine Symboldatei wurde entfernt.

`compact` meldete 1.409.998.848 gespeicherte Bytes für diese Symbole. D: meldete
vorher 7.064.854.528 und nachher 8.579.051.520 freie Bytes, eine beobachtete
Zunahme von 1.514.196.992 Bytes; parallele Nutzung beeinflusst diese Differenz.
Das lokale Hashmanifest `build/maintenance/compress-symbols-2026-11-18.jsonl`
hat SHA-256 `fc4e590d31f37a952ca94b0b0013c5a718f3a5677865bdafefcfb2504881ed22`,
das Prüfprogramm `compress-symbols-check-2026-11-18.py` hat SHA-256
`c70d880df638966c861325b7467ffce6e65b376e52279ec69c97fa21416582b3`.
Die Kompression ist eine zusätzliche Speichermaßnahme und kein Ersatz für die
noch offene Prüfung der übrigen Bestände.
