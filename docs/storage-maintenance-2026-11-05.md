# Dokumentationspflege und Bereinigung – 5. November 2026

Nutzerauftrag: Dokumentation dauerhaft übersichtlicher halten und entbehrliche
Bilder, Builds und temporäre Dateien auf D: aufräumen. Grundlage `c82b6219`;
keine Änderung am Anwendungscode. Die [Pflegeregeln](development-workflow.md)
stehen jetzt zentral und sind in AGENTS, Speicheranleitung und Prüfvorlage verlinkt.
Der Arbeitsstand wird ersetzt statt immer weiter verlängert. Originalnachweise
bleiben unverändert; aktuelle Nachläufe werden im Prüfindex verlinkt.

## Tatsächlich entfernt

Nur `build/native/Design-analysis-linux/obj`: **124 alte Linux-Compilerobjekte
und ihre 124 Cache-Signaturen, zusammen 105.475.284 logische Bytes** (100,6 MiB).
Die alten Analyseprogramme sind durch neuere isolierte Prüfbauten ersetzt;
die nächste Arbeit verwendet `module-signatures-final`, nicht diesen Objektcache.
Die 99 Dateien unter `bin`/`lib` des alten Builds wurden vor und nach dem Löschen
vollständig gelesen und per SHA-256 identisch bestätigt. Programme, Bibliotheken
und darin enthaltene Debug-Informationen sind erhalten. Ein erneuter Build des
alten Verzeichnisses muss seine entfernten Objekte neu kompilieren.

Vorher geprüft: aufgelöste D:-Pfade, reguläre Dateien ohne Reparse-Verknüpfung,
einzelner Dateilink, Compiler-Cache-Signaturen, Windows-/Linux-Prozesse und
verbleibende Nutzung. Gelöscht wurden ausschließlich einzeln quittierte Dateien
mit `Remove-Item -LiteralPath`; keine Verzeichnisse pauschal geleert.

D: meldete unmittelbar vorher 4.933.767.168, nachher 4.963.205.120 freie Bytes:
beobachtete Zunahme **29.437.952 Bytes** (28,1 MiB). Logische Dateigröße entspricht
wegen NTFS-Kompression und paralleler Laufwerksnutzung nicht direkt der Änderung
des freien Platzes. Diese Werte sind Momentaufnahmen, keine VM-Platzmessung.

## Bewusst erhalten

Die zusätzliche Bildprüfung las 1.711.383.666 Bytes und bewertete 58 Laufberichte.
Alle aufbewahrungspflichtigen, fehlgeschlagenen oder unvollständigen Läufe blieben
geschützt; **keine weiteren sicher entbehrlichen Bildduplikate** gefunden und
hier keine Bilder gelöscht. Die bereits beim Parserschritt entfernten 34
bytegleichen BMPs (108.695.892 Bytes) sind getrennt im
[Parsernachweis](verification/module-signatures-2026-11-05.json) quittiert und
werden nicht als neue Bereinigung doppelt gezählt.

Aktuelle Windows-/Linux-Objektcaches, Benutzer-Builds, benötigte Programme/Symbole,
Quellen, Nutzdaten, Offline-Abhängigkeiten, Referenzbilder, Fehlerbelege und
eingefrorene Archive bleiben erhalten. Die zwei zuvor abgelehnten Löschziele
wurden nicht angerührt. F: und der ausgeschaltete Nutzer-Mac wurden nicht angesprochen;
die gemeinsame WSL-VM und fremde aktive Arbeiten bleiben erhalten.

## Reproduzierbare Nachweise

Zusätzlicher Abschluss der Dateinamen-/Fokusprüfungen: [14 identische
Mac-Extraktionsbilder](verification/filename-wrap-macos-cleanup-2026-11-05.json)
und [acht identische lokale Bildkopien](verification/signature-focus-images-cleanup-2026-11-05.json)
entfernt. Zusammen **29.123.036 Bytes logische Dateigröße**; die tatsächlich
freigegebene NTFS-Zuordnung wurde für diese einzelnen Löschungen nicht gemessen.
Original-ZIPs, Hashlisten, eindeutige neue Bilder und sämtliche kanonischen
Referenzbilder bleiben erhalten. Ein eigener verwaister Xvfb-Server des beendeten
Zwischenlaufs wurde nach PID-/Argumentprüfung beendet; fremde Server und Arbeiten
blieben erhalten. Diese Schritte zählen keine frühere Bereinigung erneut.
Die automatische Freigabeprüfung blockierte anschließend die geplante Löschung
der zwei eigenen `xauth-verified`-/`xauth-accepted`-Dateien im Fokus-Prüfordner
mit „blocked by policy“, ohne nähere Begründung. Diese Ziele bleiben unangetastet;
die Ablehnung wird nicht durch andere Werkzeuge umgangen.

Späterer, getrennter Abschluss des gehosteten Mac-Laufs42037853611:
[vier bytegleiche entpackte ARM-Bildkopien entfernt](verification/signature-outside-macos-cleanup-2026-11-05.json),
**9.850.152 Bytes**. Alle acht tatsächlichen Bilder waren zuvor geöffnet;
kanonische Intel-Bilder und beide vollständig geprüften Original-ZIPs bleiben
erhalten. Dies ändert keine eingefrorenen Originalberichte und zählt keine
frühere Bereinigung erneut.

Dateigenaue Pläne, Hashlisten, Prüfscripte und Löschquittung liegen unter
`D:/Physim/build/maintenance/docs-cleanup-2026-11-05` und sind über
[den kompakten Bericht](verification/storage-maintenance-2026-11-05.json)
mit SHA-256 gebunden. Geprüft wurden die geänderten Dokumentationslinks,
JSON-Daten und `git diff --check`. Kein neuer Anwendungsbuild und keine neue
Bildserie für diese reine Dokumentationspflege.
