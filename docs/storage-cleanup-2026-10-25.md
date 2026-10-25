# Speicherpflege und kompakte Nachweise – 25. Oktober 2026

Nutzerauftrag: sauberere Dokumentation mit weniger Bildern, veraltete Daten
aufräumen und neue generierte Entwicklungsdaten auf F: ablegen.

**Zwischenstand: Die große Archivierung und die Verlagerung des Buildbaums
laufen noch.** Währenddessen die markierten Bäume `D:/Physim/build` und
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

Die Werkzeuge und die Anleitung sind seit `179c74b` in beiden Repositories.
Die nachfolgende Umzugssperre und dieser Fortschrittsbericht ergänzen den Stand.
Die Prüfungen benötigen keine neuen Bildschirmbilder.

## Bereits bereinigt oder verlagert

| Bestand | Umfang | Sicherung und Ergebnis |
| --- | ---: | --- |
| Veraltete Compilerobjekte, älter als 48 Stunden | 1.399 Dateien, 327.718.154 Byte | Dateiliste, Größe, Änderungszeit und SHA-256 erfasst; 287.643.780 Byte auf D: und 40.074.374 Byte auf F: entfernt. Programme, Bibliotheken und Debug-Symbole erhalten. |
| Alte Linux-Testläufe aus `/tmp/physim-*` | 189 Verzeichnisse, 4.839 Dateien, 3.338.556.533 Byte | Archiv und direkte F:-Kopien vollständig per SHA-256 geprüft; historische Pfade zeigen über Symlinks auf die Kopien. Archiv: 197.175.338 Byte. |
| Abgeschlossene Läufe aus `Debug-sanitized` und `Debug` | 3.715 Dateien, 276.012.410 Byte | Geprüfte ZIP-Archive mit Wiederherstellungsindex; lose Kopien auf F: entfernt. |
| Testläufe aus `Design-analysis-linux` auf dem Windows-Laufwerk | 48.320 Dateien, 26.967.948.923 Byte | Archiv mit 2.614.260.840 Byte vollständig gegengeprüft. Entfernen der erneut verglichenen Originale läuft noch. |

Der vollständige dateigenaue Wartungsbestand liegt unter
`F:/Physim/maintenance/2026-10-25`, die Testarchive unter
`F:/Physim/archive/completed-tests-2026-10-25` und die alten Linux-Läufe unter
`F:/Physim/archive/linux-test-runs-2026-10-25`. Wiederherstellung und Format sind
in der Speicheranleitung beschrieben. Archivierte Fehlerbelege behalten ihren
ursprünglichen Inhalt und Geltungsbereich.

Auf D: waren zuletzt bereits mehr als 15 GB frei. Das ist eine Momentaufnahme:
ein anderes Projekt verlagert gleichzeitig Daten. Der endgültige Freiplatz und
die vollständige Bilanz werden nach Abschluss des Umzugs erfasst.

## Verbleibende Arbeit und Grenzen

Die weiteren Testarchive vollständig prüfen, den restlichen Buildbaum nach F:
verlagern und dessen Integrität einschließlich der SDL-Symlinks kontrollieren.
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
