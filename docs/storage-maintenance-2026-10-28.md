# Speicherpflege vom 28. Oktober 2026

Nach dem bestandenen Projektdialog-Nachlauf wurden aus genau dem ersetzten,
erfolgreichen Windows-Testlauf `run-49y85y9c` insgesamt **3.808 SDK-Objekt- und
Präprozessordateien** entfernt. Freigegeben: **589.485.744 Bytes**, etwa 562 MiB.
Es handelt sich ausschließlich um generierte `build/Debug/sdk-*.obj` und
`sdk-*.obj.i` aus synthetischen Testprojekten unter `D:/Physim/build`.

Die aktuelle Windows- und Linux-Matrix besteht mit je 68 Projektabläufen;
der alte Lauf wird für weitere Builds nicht benötigt. Der Bereiniger prüfte
vorher dessen erfolgreiches Ergebnis, laufende Prozesse, jeden vollständigen
Zielpfad, Reparse Points und SHA-256. Er entfernte einzelne Dateien mit
PowerShell `Remove-Item -LiteralPath`; Verzeichnisse wurden nicht pauschal geleert.

**332 Programme, vollständige PDB-Dateien und Messläufe** wurden vorher und
nachher gehasht und blieben unverändert. Quellen, Projektbeschreibungen,
Ergebnisberichte, Bilder und eingefrorene Quell-/Laufzeitarchive bleiben erhalten.
Die SDK-Quellen und Offline-Abhängigkeiten sind weiterhin vorhanden;
ein bewusster erneuter Build des alten Testprojekts kann den Cache neu erzeugen.
Aktuelle Builds, fremde Arbeiten und VM-/WSL-Datenträger wurden nicht bereinigt.
F: wurde nicht verwendet oder abgefragt.

Der [kompakte Bereinigungsnachweis](verification/storage-sdk-cache-2026-10-28.json)
verweist mit Hash auf das lokale Einzeldateiinventar und die ausgeführte Anweisung.
Der [Projektdialog-Nachweis](verification/project-wizard-b-2026-10-28.json)
dokumentiert Quellen und tatsächlich bestandene Prüfungen. Für die visuelle
Dokumentation sind nur zwei unterschiedliche Bildzustände mit zusammen
326.415 Bytes übernommen; Bedienabläufe und Einzelergebnisse stehen als Text/JSON.
