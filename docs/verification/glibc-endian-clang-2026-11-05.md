# Ubuntu-Clang-Nachlauf zur Endian-Korrektur

Quelle `df9927c6`, Workflow `42076708745`, Job `127418724879`: Der native
ASan-/UBSan-Build unter Ubuntu 24.04 kompiliert und linkt nach der Korrektur.
Die tatsächlich gestarteten Gruppen bestehen **26/26 Datenprüfungen** und
**44/48 UI-Prüfungen**. Der gesamte Job endet deshalb mit einem Fehler.
Das ist ein erfolgreicher Nachweis der zuvor fehlenden Endian-Deklarationen,
aber keine vollständige Linux-UI-Abnahme.

Das Originalarchiv wurde anhand des offiziellen SHA-256 geprüft. Alle 2.365
Einträge wurden gelesen und einzeln gehasht; keine Bilder wurden extrahiert.
Quellstand, Originalpfade, Prüfsummen und Ergebnisgruppen stehen im
[strukturierten Nachweis](glibc-endian-clang-2026-11-05.json).

| Offener Fall | Tatsächlicher erster Befund |
| --- | --- |
| `floating_workflow` | Stufe 34, bevor Stufe 35 beginnt; 750 Messpunkte geladen. |
| `spring_workflow` | Ebenfalls Stufe 34; 726 Messpunkte geladen. |
| `documentation_font_demand` | Stufe 3 bei der Fehlerprobe für Einstellungen; UI/Code jeweils 16, sieben Varianten. |
| `accessibility_links_atspi` | Erwartete zugängliche Fehlermeldung nach ungültigem Link fehlt. |

Die Ursachen dieser vier Fehler sind noch nicht nachgewiesen. Als Nächstes die
gesicherten Originalausgaben mit den zugehörigen aktuellen Abläufen abgleichen;
keinen Fehler durch Abschalten eines Falls oder bloßes Erhöhen eines Timeouts
übergehen. Der GCC-UI-Job `127418724267` und der Debian-Paketjob `127418724841`
waren beim Lesen noch aktiv und werden unter denselben Handles weiterverfolgt.
