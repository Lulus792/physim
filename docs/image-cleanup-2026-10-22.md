# Bereinigung alter Testbilder am 22. Oktober 2026

Auf ausdrücklichen Nutzerauftrag wurden 371 nicht mehr benötigte, Git-ignorierte
BMP-/PNG-Testbilder entfernt: **703.551.911 Bytes (670,96 MiB)**.
Abgeschlossen um 20:09 Uhr Europe/Berlin.

Die entfernten Bilder lagen ausschließlich unter `build/native/` in älteren
Testläufen von `F1-review`, `Debug-sanitized`, `F1-H-fidelity`, `F1-linux` und
`F1-reading-focus`. Für jeden entfernten Fall lag ein neuerer erfolgreicher
Fall derselben Buildkonfiguration vor. Alle protokollierten Fälle des jeweils
bereinigten Laufs waren erfolgreich. Bilder und Ergebnisprotokolle waren älter
als der 22. Oktober 2026, 00:00 Uhr Europe/Berlin.

Vor der Auswahl wurden versionierte und nicht ignorierte Projektdateien nach
Laufkennungen durchsucht. Sämtliche so referenzierten Läufe blieben geschützt.
Ebenso erhalten blieben fehlgeschlagene Läufe, aktuelle Bilder, die jüngsten
erfolgreichen Fälle, besondere Gegenproben und alle Bilder außerhalb der
ausgewählten Testläufe. Dokumentationsbilder, Vergleichsreferenzen, eingefrorene
Nachweise, Nutzdaten, Abhängigkeiten und Programme wurden nicht entfernt.

Vor dem dateigenauen Löschen wurden Projektgrenze, Dateityp, Git-Ignorierung,
Verknüpfungen, Größe, Änderungszeit, SHA-256 und Ergebnisprotokolle geprüft.
Es fand keine rekursive Verzeichnislöschung statt. Bei der Prüfung liefen weder
Physim-Builds noch Physim-Tests unter Windows oder in `Alpine-Dev`.

Nachprüfung: Alle 371 ausgewählten Bilder fehlen wie vorgesehen. Die zuvor
erfassten 4.615 versionierten bzw. nicht ignorierten Projektdateien waren
SHA-256-identisch; auch der Git-Index war unverändert. Diese Wartungsnotiz
wurde erst danach angelegt. Anwendungscode wurde nicht geändert; neue
Produkt- oder Plattformtests waren für das Entfernen der Ausgaben nicht nötig.

Die lokale, dateigenaue Löschliste mit Hashes, erfolgreichen Ersatzläufen und
Integritätsprüfung liegt unter `build/image-cleanup-20261022/`, insbesondere
`verified-files.json`, `completed.json` und `integrity-before.json`.
Die vorherige [Bereinigung von Compiler-Zwischendateien](storage-cleanup-2026-10-22.md)
ist ein separater Vorgang.

## Ergänzung: überzählige identische Bildkopien

Auf den berechtigten Nutzereinwand zur verbliebenen Bildmenge wurde die erste
Auswahl überprüft. Der Schutz kompletter Läufe anhand einer erwähnten Laufkennung
war zu pauschal. Er beweist nicht, dass jedes dort erzeugte Rohbild benötigt wird.

Bei der genaueren Prüfung wurden 23.189 BMP-/PNG-Dateien unter `build/native/`
inventarisiert. Nach einem Stichprobenvergleich wurden 15.886 mögliche
Duplikatdateien mit zusammen 37,46 GiB vollständig per SHA-256 gelesen.
Entfernt wurden anschließend **4.405 zusätzliche überzählige Kopien**, zusammen
**10.579.759.052 Bytes (9,85 GiB)**. Für jede entfernte Datei bleibt eine
SHA-256-identische Kopie erhalten; deren Pfad steht im lokalen Löschmanifest.
Bereits gemeinsam über dieselbe Dateiidentität gespeicherte Exemplare wurden
nicht als zusätzlich freigegebener Speicher gezählt.

Entfernungskriterien: ignorierte BMP-/PNG-Ausgabe, vor dem 22. Oktober erzeugt,
erfolgreicher zugehöriger Fall und ausschließlich erfolgreiche protokollierte
Fälle im selben Lauf, keine ausdrückliche Bildpfadreferenz im versionierten oder
nicht ignorierten Projektbestand sowie eine erhaltene identische Bildkopie.
Besondere Negativ-, Gegenbeispiel-, Baseline-, Vorher-/Beweisverzeichnisse und
Dateien des aktuellen Tages blieben geschützt. Fehlgeschlagene oder nicht
zuordenbare Ausgaben wurden nicht entfernt. Alle Ergebnisprotokolle bleiben
erhalten. Verknüpfungen und außerhalb des Projektbaums liegende Ziele wurden
ausgeschlossen; das Löschen erfolgte wieder ausschließlich dateigenau.

Die lokale Dokumentation liegt unter `build/image-cleanup-20261022-duplicates/`:
`candidates.json` enthält die erkannten Bildgruppen und Bildpfadreferenzen,
`verified-files.json` die Löschliste mit Hash und erhaltenem Kopiepfad und
`completed.json` die abschließende Integritätsprüfung.

Die Nachprüfung um 20:19 Uhr Europe/Berlin bestätigt alle 4.405 Löschungen,
2.506 erhaltene Kopien mit erneut vollständig geprüften SHA-256-Hashes und
1.486 weiterhin vorhandene ausdrücklich referenzierte Bildpfade. Alle 4.616
vorher erfassten versionierten bzw. nicht ignorierten Dateien und der Git-Index
waren vor dieser Ergänzung unverändert.

Beide Schritte entfernten insgesamt **4.776 Bilder mit 11.283.310.963 Bytes
(10,51 GiB)**. Daraus folgt ausdrücklich keine Bedarfsbestätigung für sämtliche
verbliebenen Bilder. Die zweite Auswahl entfernt nachgewiesene überzählige
Kopien; der Bedarf aller übrigen, unterschiedlichen historischen Ausgaben wurde
damit nicht vollständig bewertet.
