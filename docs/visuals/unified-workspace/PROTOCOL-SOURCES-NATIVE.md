# Gemeinsames Protokoll: native Datenquellen

Stand: 25. Oktober 2026. **Geprüfte Datenbasis, noch keine Umsetzung oder
visuelle Abnahme von Protokoll C.** Die Navigator-Anordnung aus
[ADR 011](../../decisions/011-selected-workspace-additions.md) bleibt verbindlich.
Die bisherigen Protokolloberflächen verwenden weiterhin ihre bestehende Anzeige.
Herkunftsnavigation, Filter, Auswahl und Detail-/Originalbereich müssen als
nächster zusammenhängender Schritt daran angeschlossen werden.

## Umsetzung

- `app/protocol_store.*` hält bis zu 200 Sitzungsmeldungen mit stabilen IDs
  und höchstens 4 MiB behaltenem Speicher. Titel, Kontext, Quellposition,
  Operation, Argument und Fehlercode werden kopiert. Modellzeiten bleiben
  ausdrücklich optional. Ein Schweregrad löst keine Prozessaktion aus.
- Compilerdiagnosen erhalten ihren tatsächlichen Warnungs-/Fehlerschweregrad
  aus dem GCC-/Clang-/MSVC-Parser. Die Diagnose verweist zusätzlich auf die
  vollständige Ausgabe des zugehörigen Jobs. Deren Rohbytes bleiben einschließlich
  CR, LF, Tabs, Unicode, NUL und ungültigem UTF-8 erhalten. Die alte zeilenweise
  Diagnoseliste ersetzt diese Ausgabe nicht.
- Build, Parameterabfrage und Auswertung liefern ihre gelesenen Originalbytes
  an dieselbe Historie. Prozessende und die vorhandenen Abbruchaktionen setzen
  den tatsächlichen Ausgang. Vor dem Schließen wird verbleibende Pipe-Ausgabe
  gelesen. Erfolg, Fehler und Abbruch werden ausdrücklich vom Host gemeldet.
- Modellmeldungen übernehmen Level, Modellzeit und Originaltext aus
  `ps_log_record`; der Host ergänzt den wirklichen Laufkontext. Strukturierte
  Diagnosen behalten ihre übermittelten Felder. Ersetzt eine strukturierte
  Diagnose eine alte Textdiagnose, wird deren vorheriger Historieneintrag entfernt.
- `app/protocol_text.*` ist ausschließlich ein Anzeigeadapter. Gültiges UTF-8
  bleibt auch über mehrere Lesevorgänge erhalten. Nicht darstellbare Bytes
  erscheinen als `\xHH`, ohne eine nicht übermittelte Zeichencodierung zu raten.
  Gespeicherte Rohbytes werden nicht geändert. Fremde Compilerbytes beschädigen
  damit die bisherige UTF-8-Protokollanzeige nicht mehr.
- Kapazitätsverluste werden gezählt. Fehlgeschlagenes Anhängen erhält den
  bisherigen Text und kennzeichnet ausgelassene Bytes am Eintrag und global.
  Die neue Oberfläche muss diese Angaben noch sichtbar machen. Kopieren und
  Export sind in diesem Schritt ebenfalls noch nicht angeschlossen.

Der Speicher gehört dem UI-Thread. Vor einer Verdrängung werden geliehene
Eingaben kopiert; das schützt auch Eingaben aus derselben Historie. Löschen gibt
Speicher frei und verwendet frühere IDs nicht erneut. Während einer Kopiermutation
kann der vorübergehende Speicher höher als das Retentionsbudget sein.

## Ausgeführte Prüfungen

Die [eingefrorenen Belege](protocol-sources-2026-10-25/manifest.json) enthalten
Originalausgaben, Kommandos, Binärprüfsummen und 314 geprüfte Projekt-/
Schrifteingaben. Die Treiber prüfen diese Eingaben und das Programm vor/nach
jeder Gruppe. Grundlage ist `a0fabf2` im gemeinsamen lokalen Arbeitsstand.
Die erfassten parallel vorhandenen Accessibility-Änderungen sind keine Bestandteile
dieses Protokoll-Commits; dies ist kein isolierter Clean-Checkout-Nachweis.
Der [Abgleich mit dem Git-Index](protocol-sources-staged-2026-10-25.json)
erfasst zusätzlich die Quellprüfsummen des Parser-Unit-Tests außerhalb dieser
App-Eingabeliste. Sieben Dateien werden beim Stagen ausschließlich von CRLF
nach LF normalisiert; die übrigen gestagten Inhalte sind bytegleich.

| Prüfung | Windows | Linux / Alpine-Dev |
| --- | --- | --- |
| Speicher, Besitz, IDs, FIFO, Grenzen und Alias-Eingaben | ASan: Exit 0 | ASan/UBSan, Leak-Prüfung: Exit 0 |
| Diagnoseparser und Severity | ASan: Exit 0 | ASan/UBSan: Exit 0 |
| UTF-8 an jeder Chunk-Grenze, ungültige Bytes, Abschluss und Kapazität | ASan: Exit 0 | ASan/UBSan: Exit 0 |
| `protocol_sources`: nicht abbrechende Modellfehler und echte Prozessausgaben | Exit 0 | Exit 0 |
| `logging_workflow`: C, Physim und Meldungsflut | Exit 0 | Exit 0 |
| `diagnostic_workflow`: Compilerheader, Quellsprung/Reparatur, fremde Datei, Lauf-/Analysediagnosen | Exit 0 | Exit 0 |
| `workbench_workflow`: bestehende Werkbankzustände, Themes, Schriftgrößen | **Offen: Fokus 0, Timeout** | Exit 0 |

Die Windows-App wurde als Release gebaut; ihre gezielten C-Tests verwenden ASan.
Die Linux-App wurde mit ASan/UBSan unter Xvfb/Openbox ausgeführt; Leak-Erkennung
ist für die App deaktiviert und für die gezielten Speichertests aktiviert.
macOS wurde in diesem Schritt nicht gebaut oder getestet.

Die beiden neuen Modellfixtures senden `Nonfatal error: α\n\toriginal text`,
führen den Schritt erfolgreich aus und behalten den Wert 142,25. Der Test prüft
aktiven Runner, echte Historie, zwei unveränderte Messpunkte und das gespeicherte
`.pslog`. Die Fixtures werden nur ausdrücklich als Tests gestartet; der
Produktstart lädt keine Beispielmeldungen.

Zwei weitere App-Fälle starten einen echten Kindprozess mit allen Bytewerten.
Nach Exit 7 und tatsächlichem Abbruch müssen jeweils sämtliche **153.645 Bytes**,
CRC32 **ec3a22d6**, unverändert vorhanden sein. Python berechnet die erwarteten
Bytes unabhängig. Anzeige, Herkunft, Projektkontext und Ausgang werden mitgeprüft.

Negative Belege:

- Der Windows-Vorherlauf zeigt `utf8=0`, obwohl Modellwert und nicht abbrechende
  Fehlermeldung stimmen. Die aktuellen C-/Physim-Fälle zeigen jeweils `utf8=1`.
- Ein während der Implementierung gefundener unsigned Unterlauf beim Anhängen
  unter Speicherknappheit ist korrigiert. Bei zwei 1-MiB-Einträgen und einem
  weiteren 3-MiB-Anhang reproduziert die gezielte Rücknahme dieses Grenzvergleichs
  einen ASan-Heap-Buffer-Overflow. Aktueller Code weist den Anhang mit `PS_LIMIT`
  ab und erhält beide Originale. Mutation, Kommando und Ausgabe sind eingefroren.

Der Windows-Werkbank-Nachlauf ist **nicht bestanden**: aktuell blieb das erste
dunkle Fenster mit Tastaturfokus 0 an Stufe 0 stehen, Timeout nach 45 Sekunden.
Ein vorheriger Lauf scheiterte entsprechend beim folgenden hellen Fenster.
Beide Ausgaben bleiben erhalten. Timeout und Fokusanforderung wurden nicht
gelockert; daraus folgt keine vollständige Tastaturabnahme.

## Fortsetzung und Grenzen

Als Nächstes Anordnung C im vorhandenen `PS_DOCK_LOG` umsetzen: globaler unterer
Bereich, Herkunft links, Meldungen mittig, Details rechts, gemeinsame Filter/
Suche/Auswahl über alle Arbeitsbereiche. Beim Öffnen die kleine Werkbankanzeige
ersetzen und beim Schließen wiederherstellen. Originaltext, tatsächliche
Quellsprünge, Auslassungen, Kopier-/Dateifehler, Folgebewegung, kleine Fenster
und große Schrift müssen nativ bedient und geprüft werden. Die Datenbasis
ist kein Ersatz für diese sichtbare Umsetzung.

Die allgemeine Design-/Fokus-/Screenreader-/DPI-Abnahme und die weiteren
ausgewählten Arbeitsbereichs-/Dialogvarianten bleiben offen. Der mögliche Fehler
bei mehreren Diagramm-Tastenereignissen innerhalb desselben Frames wurde in
diesem Schritt nicht bearbeitet.

Speicherpflege: 195 ersetzte erfolgreiche Linux-Analyse-Rohbilder wurden nach
Verwendungsprüfung entfernt, insgesamt 949.274.910 Bytes innerhalb der WSL-
Dateisystembelegung. Der [Löschbeleg](protocol-sources-2026-10-25/cleanup-old-linux.json.gz)
enthält Pfade, Größen und Prüfsummen. Aktuelle Bilder, Vorher-Kontrollen und alle
fehlgeschlagenen Fälle blieben erhalten. Der gemeinsame WSL-Datenträger wurde
nicht entfernt; dies behauptet keine entsprechend große Freigabe auf Windows D:.
