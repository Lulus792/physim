# Speicherpflege und kompakte Nachweise – 25. Oktober 2026

Nutzerauftrag: sauberere Dokumentation mit weniger Bildern, veraltete Daten
aufräumen und neue generierte Entwicklungsdaten auf F: ablegen.

**Zwischenstand: Die 13 Testarchive sind vollständig geprüft und bereinigt;
der restliche Buildbaum wird noch verlagert.** Währenddessen die markierten Bäume `D:/Physim/build` und
`F:/Physim/build` für neue Builds freihalten. Der Builder erkennt die
Umzugssperre. Getrennte neue Artefaktstämme unter `F:/Physim/validation`
funktionieren bereits.

## Umgesetzt und geprüft

- [Speicheranleitung](development-storage.md),
  [Berichtsvorlage](templates/native-validation.md) und AGENTS-Regeln führen
  neue Builds, Testausgaben, Logs, Caches und Temporärdateien nach `F:/Physim`.
  Quellen und Testprogramme bleiben versioniert im Repository auf D:.
- Der Wrapper wurde unter Windows und WSL mit echten temporären Dateien,
  unverändert weitergegebenem Fehlercode und abgewiesenem Quellwurzelpfad geprüft.
- Zwei native Gruppen (`protocol_store`, `protocol_text`) wurden je Plattform
  gebaut und bestanden: Windows Release und Linux Debug. Beide Builds und ihre
  Testergebnisse liegen vollständig unter
  `F:/Physim/validation/storage-build-2026-10-25`. Sie verwenden den damaligen
  lokalen Arbeitsstand mit den noch offenen UI-Änderungen.
- Die gezielte Archivwiederherstellung wurde auf beiden Plattformen mit einer
  echten 498.605-Byte-Datei und Umlauten geprüft: Erstherstellung, wiederholte
  Herstellung gleicher Bytes, Erhalt abweichender bestehender Dateien und
  Abweisung eines Pfadausbruchs.
- Die Umzugssperre wurde an beiden Windows-Buildpfaden und unter WSL geprüft.
  Ein unabhängiger Artefaktstamm erreichte den bewusst provozierten
  Compilerfehler; er wurde von der Umzugssperre nicht blockiert.
- Die Layoutkatalog-Mutationsprüfung benötigt auf dem externen Laufwerk mehr
  Zeit als die frühere 60-Sekunden-Grenze: Windows zunächst 55,891 Sekunden,
  Linux beim gezielten Nachlauf 60,956 Sekunden. Das unveränderte Testprogramm
  besteht mit dem offiziellen, jetzt gezielt auf 180 Sekunden gesetzten
  Katalogbudget auch unter gleichzeitiger Datenverlagerung: Windows 73,062,
  Linux 113,606 Sekunden. Die mehr als 4.500 Datei-Mutationszyklen bleiben
  vollständig erhalten.

Die Werkzeuge und die Anleitung sind seit `179c74b`, Umzugssperre und erster
Fortschrittsbericht seit `e939259` in beiden Repositories.
Die Prüfungen benötigen keine neuen Bildschirmbilder.

## Bereits bereinigt oder verlagert

| Bestand | Umfang | Sicherung und Ergebnis |
| --- | ---: | --- |
| Veraltete Compilerobjekte, älter als 48 Stunden | 1.399 Dateien, 327.718.154 Byte | Dateiliste, Größe, Änderungszeit und SHA-256 erfasst; 287.643.780 Byte auf D: und 40.074.374 Byte auf F: entfernt. Programme, Bibliotheken und Debug-Symbole erhalten. |
| Alte Linux-Testläufe aus `/tmp/physim-*` | 189 Verzeichnisse, 4.839 Dateien, 3.338.556.533 Byte | Archiv und direkte F:-Kopien vollständig per SHA-256 geprüft; historische Pfade zeigen über Symlinks auf die Kopien. Archiv: 197.175.338 Byte. |
| Testläufe aus 13 Buildvarianten | 85.328 Dateien, 48.907.120.511 Byte | 4.487.854.239 Byte geprüfte ZIP-Archive mit Wiederherstellungsindizes. 44.241.174.841 Byte Originale auf D: und 4.684.004.788 Byte vorherige lose Kopien auf F: entfernt. |

Das besonders große Archiv `Design-analysis-linux` enthält davon 48.320
Dateien mit 26.967.948.923 Originalbytes in 2.614.260.840 Archivbytes. Sämtliche
Dateien wurden vor dem Entfernen nochmals mit ihren Archivhashes verglichen.

Der vollständige dateigenaue Wartungsbestand liegt unter
`F:/Physim/maintenance/2026-10-25`, die Testarchive unter
`F:/Physim/archive/completed-tests-2026-10-25` und die alten Linux-Läufe unter
`F:/Physim/archive/linux-test-runs-2026-10-25`. Wiederherstellung und Format sind
in der Speicheranleitung beschrieben. Archivierte Fehlerbelege behalten ihren
ursprünglichen Inhalt und Geltungsbereich.

Beim Beginn der restlichen Verlagerung waren auf D: 102.673.428.480 Byte frei.
Dieser Freiplatz enthält auch die gleichzeitig abgeschlossene Bereinigung
eines anderen Projekts. Die noch auf D: verbliebenen 94.581 regulären
Builddateien einschließlich der neuen Umzugs-Steuerdatei belegen
35.526.930.640 Byte; sie werden jetzt ebenfalls nach F: verlagert.
Der endgültige Freiplatz und die dateigenau abgeglichene Gesamtbilanz folgen
nach Abschluss dieses Schritts.

Ein kleiner, unabhängiger SDL-Linux-SDK-Stand liegt zusätzlich unter
`F:/Physim/dependencies/SDL3-linux-3.2.30`: 85 Originalheader, die bereits nativ
verwendete Bibliothek und die Lizenz, zusammen 7.459.194 Byte. Alle 87 Dateien
wurden hashgleich kopiert und in `manifest.json` erfasst. Dadurch brauchen
weitere isolierte Builds nicht den gerade verlagerten SDK-Pfad.

## Verbleibende Arbeit und Grenzen

Den restlichen Buildbaum nach F: verlagern und dessen Integrität einschließlich
der SDL-Symlinks und der eingebetteten SDL-Git-Historie kontrollieren.
Anschließend soll eine Verzeichnisverknüpfung von `D:/Physim/build` nach
`F:/Physim/build` die bisherigen Startpfade erhalten. Erst danach die
Umzugssperren entfernen und die Startpfade prüfen. Diese Schritte sind noch offen.

Unix-Laufzeitverzeichnisse bleiben kurzzeitig im Linux-Dateisystem: Der lokale
F:-Mount meldet bei `AF_UNIX.bind` Fehler 95 (`Not supported`) und behält trotz
`chmod 0700` den Modus `0777`. Große Testdaten liegen dort nicht mehr.
25 vorhandene Orca-Laufzeitverzeichnisse mit Socketdateien wurden aus der
Verlagerung ausgenommen.

Die 3,34 GB aus `/tmp` sind innerhalb von WSL freigegeben. Die gemeinsame
`D:/WSL/Alpine-Dev/ext4.vhdx` wurde weder verschoben noch verkleinert.
Quellen, lokale Änderungen, Nutzdaten, Git-Historien, eingefrorene Nachweise
im Repository und die laufenden Arbeiten anderer Projekte bleiben erhalten.
