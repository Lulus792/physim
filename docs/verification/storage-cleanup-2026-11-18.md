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
