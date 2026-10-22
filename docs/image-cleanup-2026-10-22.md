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
