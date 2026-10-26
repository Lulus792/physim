# Speicherpflege auf D: – 26. Oktober 2026

Alle Physim-Daten bleiben auf D:. Die vollständige Rückführung ist im
[Abschlussbericht](storage-cleanup-2026-10-25.md) mit 52.259 geprüften Dateien
und 36.585.973.582 Byte dokumentiert. F: wurde in diesem Nachlauf weder
abgefragt noch beschrieben. Neue Ausgaben liegen unter `D:/Physim/build`.

## Ausgeführter Nachlauf

| Prüfung oder Bereinigung | Ergebnis |
| --- | --- |
| Speicherregeln unter Windows und `Alpine-Dev` | Beide bestehen: D:-Standard, acht lexikalisch gesperrte Pfade, aufgelöstes gesperrtes Ziel, Archivpfade und Konfigurationsversion. Verbotene Orte werden im Test vor ihrer Pfadauflösung abgefangen. |
| Wrapper mit echtem Kindprozess | Temporärdatei liegt unter dem D:-Artefaktstamm; Exitcode 7 wird auf beiden Plattformen unverändert weitergereicht. |
| Alte native Compilerobjekte | 1.899 `.obj`-/`.o`-Dateien mit 678.375.480 Byte Dateigröße entfernt. Nur erzeugte Objekt-Caches, älter als 48 Stunden, ohne laufenden Build und ohne ausdrücklichen Dokumentationsverweis. |
| Erhaltene Programme, Bibliotheken und Symbole | 4.796 Dateien vor der Bereinigung per SHA-256 erfasst und danach erneut hashgleich geprüft. 42 Fixture-Objekte vorsorglich erhalten. |
| Neue Bildschirmbilder | Keine: Alle Aussagen dieses Nachlaufs lassen sich mit Text, Pfaden und Prüfsummen belegen. |

Ein weiterer eigener Nachlauf entfernt nur die ersetzten Objekt-Caches aus
`build/validation/protocol-wrap-2026-10-26/2-editor/{windows,linux}-build/obj`:
288 `.obj`-/`.o`-Dateien mit 106.613.375 Byte. Zugehörige Laufzeitarchive wurden
vollständig nachgelesen, die verbleibenden Programme, Bibliotheken und Symbole
vorher und nachher hashgleich geprüft. Quellen, Fehlerbilder, eingefrorene
Nachweise, der aktive Folgebuild und fremde Arbeiten bleiben erhalten.
Die [zweite kompakte Quittung](verification/storage-wrap-objects-2026-10-26.json)
nennt Befehle, Archivhashes und die lokale Dateiliste. Beide Nachläufe zusammen
entfernen 2.187 Objekte mit 784.988.855 Byte Dateigröße; dies ist weiterhin
keine Messung physisch freigegebener Laufwerksbytes. Für diesen Nachlauf wurden
ebenfalls keine Bilder erzeugt.

Der dritte Nachlauf entfernt die inzwischen ersetzten Objekt-Caches aus
`build/validation/protocol-wrap-2026-10-26/1-layout/{windows,linux}-build/obj`:
166 Dateien mit 27.993.818 Byte. Diese ersten Unit-Builds sind beendet; ihre
damals fehlgeschlagenen Gegenproben bleiben mit Programmen, Symbolen, Quellen
und Logs erhalten. Vorherige Prozess- und Verweisprüfung sowie 137 hashgleich
erhaltene Programm-, Bibliotheks-, Symbol- und Metadatendateien sind in der
[dritten Quittung](verification/storage-layout-objects-2026-10-26.json)
verknüpft. Zusammen sind damit 2.353 ersetzte Objekte mit 812.982.673 Byte
Dateigröße entfernt. Die aktuelle isolierte Buildkopie bleibt für weitere
Prüfungen verwendbar; F: wurde auch für diesen Nachlauf nicht angesprochen.

Die vollständige lokale Dateiquittung und rohen Prüfausgaben liegen unter
`D:/Physim/build/maintenance/2026-10-26-storage`. Der
[kompakte maschinenlesbare Nachweis](verification/storage-objects-2026-10-26.json)
verknüpft ihre Prüfsummen und genauen Befehle. Die gelöschte Dateigröße ist
wegen NTFS-Kompression und paralleler Arbeit keine Messung physisch
freigegebener Laufwerksbytes.

## Dokumentation und Aufbewahrung

Die [Speicheranleitung](development-storage.md) und
[Prüfvorlage](templates/native-validation.md) verlangen einen eigenen Zweck
für zusätzliche Bilder und eine Aufbewahrungsentscheidung. Wiederholte
Bedienfolgen werden als Soll/Ist-Zustände dokumentiert. Bereits hashgeprüfte
Laufzeitarchive können bei unverändertem Programm von mehreren Berichten
referenziert werden. Archiv und vollständige lose Wiederherstellung nicht
dauerhaft doppelt aufbewahren.

Der [Originaltext-Nachlauf](visuals/unified-workspace/PROTOCOL-ORIGINAL-FLOW-NATIVE.md)
zeigt die Anwendung dieser Regel: Zustandsfolgen und Gegenproben als JSON,
zwei ausgewählte unveränderte native Bilder für die Dokumentation und
Laufzeitarchive nur für gegenüber dem bisherigen Manifest geänderte Dateien.
Die erfolgreichen und fehlgeschlagenen Ergebnisse bleiben getrennt erkennbar.

Referenzbilder, offene Fehlerbelege, eingefrorene Nachweise, Quellen,
Abhängigkeiten, Nutzdaten und fremde Arbeiten bleiben geschützt. Die gemeinsame
WSL-VM wurde nicht verändert. Die UI- und Screenreader-Abnahme bleibt eine
eigene Aufgabe; aus dieser Speicherprüfung folgt keine neue UI-Abnahme.

Ein Windows-Fehler im ersten zusätzlichen Testaufruf kam vom Hilfsprogramm:
Es wollte seine noch geöffnete Temporärdatei entfernen. Nach explizitem
Schließen des Handles besteht derselbe Test; Erstlog und Nachlauf sind
erhalten. Ein Bereinigungsvorlauf stoppte vor jeder Löschung an einem
WSL-Bibliothekslink. Die Dateierhebung überspringt jetzt sämtliche Links und
Reparse-Punkte ohne Auflösung; der Link und sein Ziel blieben erhalten.

## Weiterer gezielter Nachlauf

Die erneute Speicherprüfung unter Windows und `Alpine-Dev` besteht mit je
drei ausgeführten Befehlen: D:-Standard, Abweisung der gesperrten Orte vor
Zugriff und eine echte Temporärdatei unter dem Artefaktstamm einschließlich
unverändertem Kindprozess-Exitcode. F: wurde dabei nicht angesprochen.
Die bereits abgeschlossene Rückführung wurde nicht nochmals ausgeführt.

Aus dem ersetzten Protokoll-Buildstand
`build/validation/protocol-ui-2026-10-25/4-native/{windows,linux}-build/obj`
wurden ausschließlich 218 `.obj`-/`.o`-Dateien mit 89.076.918 Byte entfernt.
Die aufgelösten Ziele lagen innerhalb des Physim-Buildbaums; die ausgewählten
Verzeichnisse enthielten keine Links. Es liefen keine Prozesse dieses alten
Prüfstands, und es gab keine ausdrücklichen versionierten Verweise auf seine
Objektverzeichnisse. Alle 3.147 übrigen Dateien des Prüfstands wurden vorher
und nachher per SHA-256 verglichen und blieben unverändert. Dazu gehören
Programme, Symbole, Quellen und vorhandene Erfolgs- und Fehlernachweise.
Die aktuelle Buildkopie und fremde aktive Arbeiten blieben unberührt.

Die [kompakte Quittung](verification/storage-protocol-objects-2026-10-26.json) nennt
Befehle, Quellhashes, Prüfergebnisse und den Hash der lokalen Dateiliste.
Zusammen mit den drei oben dokumentierten Objektbereinigungen sind
2.571 ersetzte Objekte mit 902.059.591 Byte Dateigröße entfernt; eine physische
Freispeichermessung ist damit nicht verbunden. Neue Bilder: **0**.
Ein erster PowerShell-Aufruf scheiterte an der Aufrufsyntax vor dem Start
des Bereinigungsprogramms; der korrigierte Aufruf wurde mit Exitcode 0 beendet.

Im anschließenden [Zeiger-/Fokusnachlauf](visuals/unified-workspace/PROTOCOL-POINTER-NATIVE.md)
verknüpft ein flacher Archivindex alle 206 aktuellen Laufzeitdateien mit ihren
geprüften Originalbytes. Die Prüfung entpackt keine weiteren Programmkopien.
Zwei abgeschlossene, nur für eine Gegenprobe wiederhergestellte Kopien mit
5.647.872 Byte wurden anschließend gezielt entfernt. Die Programme bleiben in
nachgelesenen, hashgeprüften Archiven erhalten; Fehlerlogs, Quellen und aktuelle
Builds bleiben unverändert. Diese zusätzliche Zahl gehört nicht zur obigen
Objektdatei-Zwischensumme und ist keine physische Freispeichermessung.

Der folgende [F1-Reihenfolgenachlauf](visuals/f1-layouts/UIA-SCROLL-ORDER.md)
benötigt ebenfalls keine Bilder. Nach Abschluss seiner gezielten Gegenprobe
wurden nochmals zwei nur dafür wiederhergestellte Programmkopien mit
5.650.944 Byte entfernt. Ihre Bytes sind in nachgelesenen Archiven erhalten;
die aktuelle Laufzeit mit 206 Dateien hat einen eigenen flachen Archivindex.
Quellen, Symbole, aktuelle Builds, VM und Rohspuren des ursprünglichen Fehlers
bleiben erhalten. Der neue Bericht nennt die genauen Wiederherstellungspositionen.
