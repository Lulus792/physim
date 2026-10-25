# Speicherpflege und kompakte Nachweise – 25. Oktober 2026

Nutzerauftrag: sauberere Dokumentation mit weniger Bildern und veraltete Daten
aufräumen. Der zunächst gewünschte Wechsel auf F: wurde am selben Tag widerrufen:
**Alle Physim-Daten bleiben auf D:. F: wird nach der Rückführung nicht mehr
verwendet oder für Physim abgefragt.**

**Abgeschlossen: Alle 52.259 vollständigen Dateien mit 36.585.973.582 Byte sind
geprüft zurück auf D:.** Zwölf abgebrochene Kopien mit 8.709.073 Byte wurden
nach Nachweis ihrer vollständigen D:-Originale entfernt. Der unabhängige
Abschlussabgleich erfasst alle 52.271 Inventareinträge; keine abweichenden
Dateiversionen. `F:/Physim` wurde nach Entfernung aller Dateien als leeres
Verzeichnis entfernt. Seitdem finden die Physim-Prüfschritte ausschließlich auf
D: statt. `D:/Physim/build` bleibt ein echtes, transparent NTFS-komprimiertes
Verzeichnis. Die Umzugssperre ist nach den Prüfungen aufgehoben.

## Umgesetzt und geprüft

- [Speicheranleitung](development-storage.md),
  [Berichtsvorlage](templates/native-validation.md) und AGENTS-Regeln führen
  neue Builds, Testausgaben, Logs, Caches und Temporärdateien nach `D:/Physim/build`.
  Quellen und Testprogramme bleiben versioniert im Repository auf D:.
- Der Wrapper wurde unter Windows und WSL mit echten temporären Dateien,
  unverändert weitergegebenem Fehlercode und abgewiesenem Quellwurzelpfad geprüft.
- Zwei native Gruppen (`protocol_store`, `protocol_text`) wurden je Plattform
  gebaut und bestanden: Windows Release und Linux Debug. Beide Builds und ihre
  Testergebnisse liegen vollständig unter
  `D:/Physim/build/validation/storage-build-2026-10-25` nach der Rückführung.
  Ausgeführt wurden sie noch am damaligen F:-Ort. Sie verwenden den damaligen
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
| Alte Linux-Testläufe aus `/tmp/physim-*` | 189 Verzeichnisse, 4.839 Dateien, 3.338.556.533 Byte | Archiv und direkte D:-Kopien vollständig per SHA-256 geprüft; historische Symlinks auf D: umgestellt. Archiv: 197.175.338 Byte. |
| Testläufe aus 13 Buildvarianten | 85.328 Dateien, 48.907.120.511 Byte | 4.487.854.239 Byte geprüfte ZIP-Archive mit Wiederherstellungsindizes. 44.241.174.841 Byte Originale auf D: und 4.684.004.788 Byte vorherige lose Kopien auf F: entfernt. |

Das besonders große Archiv `Design-analysis-linux` enthält davon 48.320
Dateien mit 26.967.948.923 Originalbytes in 2.614.260.840 Archivbytes. Sämtliche
Dateien wurden vor dem Entfernen nochmals mit ihren Archivhashes verglichen.

Der vollständige dateigenaue Wartungsbestand liegt unter
`D:/Physim/build/maintenance/2026-10-25`, die Testarchive unter
`D:/Physim/build/archive/completed-tests-2026-10-25` und die alten Linux-Läufe unter
`D:/Physim/build/archive/linux-test-runs-2026-10-25` nach der Rückführung.
Wiederherstellung und Format sind
in der Speicheranleitung beschrieben. Archivierte Fehlerbelege behalten ihren
ursprünglichen Inhalt und Geltungsbereich.

Vor der Rückführung wurden 52.271 reguläre Dateien mit 36.594.682.655 Byte unter
`F:/Physim` inventarisiert, darunter zwölf abgebrochene Kopierdateien des zuvor
gestoppten Hinwegs. Es gab dort keine Verzeichnisverknüpfungen oder VM-Datenträger.
Auf D: waren zu diesem Zeitpunkt 127.182.774.272 Byte frei; parallele Arbeiten
anderer Projekte beeinflussen den freien Laufwerksplatz ebenfalls.

Ein kleiner, unabhängiger SDL-Linux-SDK-Stand liegt zusätzlich unter
`D:/Physim/build/dependencies/SDL3-linux-3.2.30` nach der Rückführung:
85 Originalheader, die bereits nativ
verwendete Bibliothek und die Lizenz, zusammen 7.459.194 Byte. Alle 87 Dateien
wurden hashgleich kopiert und in `manifest.json` erfasst. Dadurch brauchen
weitere isolierte Builds einen festen D:-SDK-Pfad.

## Rückführung nach D:

Das lokale Inventar, der wiederaufnehmbare Rückführer und die dateigenauen
SHA-256-Belege liegen unter `D:/Physim/build/maintenance/2026-10-25-return`.
Jede Zielkopie wird geprüft und ihre Quittung auf D: geschrieben, bevor das
F:-Original entfernt wird. Bei abweichenden bestehenden D:-Dateien bleiben beide
Versionen erhalten; die zurückgeholte Version liegt dann unter
`build/recovered-from-F`. Ein Wiederanlauf prüft auch die Zielkopien bereits
abgeschlossener Schritte erneut. Verzeichnisse auf F: werden nur entfernt, wenn
sie leer sind.

Zuordnung für historische Pfadangaben:

| Früherer Ort | Heutiger Ort nach Rückführung |
| --- | --- |
| `F:/Physim/build/<Pfad>` | `D:/Physim/build/<Pfad>` |
| `F:/Physim/archive/<Pfad>` | `D:/Physim/build/archive/<Pfad>` |
| `F:/Physim/validation/<Pfad>` | `D:/Physim/build/validation/<Pfad>` |
| `F:/Physim/dependencies/<Pfad>` | `D:/Physim/build/dependencies/<Pfad>` |
| `F:/Physim/maintenance/<Pfad>` | `D:/Physim/build/maintenance/<Pfad>` |
| `F:/Physim/cache/<Pfad>`, `tmp/<Pfad>` | entsprechende Gruppe unter `D:/Physim/build` |

Eingefrorene Manifeste werden nicht umgeschrieben. Die alten F:-Pfade sind
historische Daten, keine aktuellen Kommandos. Auch damalige Hilfsskripte und
Programme können feste F:-Pfade enthalten; weitere Prüfungen verwenden neue
D:-Steuerdateien und frisch auf D: gebaute Programme.

Der erste Rücklauf hat 768 Dateien mit 6.112.054.188 Byte überprüft zurückgeführt.
Eine Pfad-Assertion stoppte danach vor dem Entfernen weiterer Originale.
Eine vollständige Nachprüfung der geplanten Zielpfade fand keinen Pfadausbruch.
Der Wiederanlauf vergleicht kanonische Windows-Pfade ohne optionales
Extended-Length-Präfix und protokolliert bei einer Ablehnung den genauen Pfad.
Die Ursache des ersten Assertion-Fehlers ist damit noch nicht abschließend belegt.

Der unabhängige Abschlusslauf hat anschließend alle 52.271 Quittungen erfolgreich
mit den D:-Dateien verglichen. Dabei wurde F: nicht mehr angesprochen. Die
Dateihashes des Inventars und der Quittungen stehen im
[kompakten maschinenlesbaren Abschluss](verification/storage-2026-10-25.json).

Beim Umstellen der Linux-Links fiel ein fehlendes leeres Zielverzeichnis auf.
Der dateibasierte Kopierer hatte leere Verzeichnisse nicht mit angelegt.
Aus dem hashgeprüften ursprünglichen Linux-Archiv wurden deshalb 578 leere
Verzeichnisse auf D: wiederhergestellt; alle 4.839 Dateien wurden zusätzlich
mit dem ursprünglichen Archivindex abgeglichen. Danach ließen sich alle
189 Links atomar auf D: umstellen. Die 25 Orca-Socketverzeichnisse blieben erhalten.

Die 87 Dateien des eigenständigen SDL-SDKs stimmen mit dessen Manifest überein.
Beide SDL-Bibliothekslinks im bisherigen SDK lösen auf D: auf; `git fsck --full`
für den erhaltenen SDL-Quellstand endet mit Exitcode 0. Die alte Umzugssperre ist
als `retired-migration-marker.txt` mit Hashquittung im Wartungsordner erhalten,
anstatt den Buildbaum weiter zu blockieren.

## Prüfung der D:-Speicherregeln

Die lokalen Logs liegen im Rückführungsordner; für diese Prüfung wurden keine
Bildschirmbilder erzeugt. Unter Windows und `Alpine-Dev` ausgeführt:

- `tests/test_artifact_paths.py --work <D:-Prüfordner>`: bestanden. Standardpfad,
  acht verbotene Pfadschreibweisen vor jeder Pfadauflösung, verbotene aufgelöste
  Ziele, Archiv-Ein-/Ausgabepfade und unbekannte Konfigurationsversion geprüft.
- `tools/with-artifacts.py --check`: Stamm, Temporär- und Cachepfade liegen
  unter `D:/Physim/build` beziehungsweise `/mnt/d/Physim/build`. Ein Kindprozess
  erzeugt dort eine echte temporäre Datei und gibt seinen Fehlercode 7 korrekt
  an den Aufrufer zurück.
- F:-Argumente für Wrapper und Builder (`--build-dir`, `--sdl`, `--install`):
  mit Exitcode 2 vor der Pfadverwendung abgewiesen, auf beiden Plattformen.
- `tests/test_bootstrap_build.py --work <D:-Prüfordner>`: mit echtem Compiler
  unter WSL bestanden. Unter Windows scheiterte der erste Lauf beim atomaren
  Ersetzen von `test-header-probe.exe` mit WinError 5; der unveränderte Nachlauf
  bestand. Ursache der einmaligen Sperre nicht belegt; beide Logs erhalten.
- Aus einem zurückgeholten Release-Archiv dieselbe 71.904-Byte-JSON-Datei mit
  Umlaut im Pfad unter Windows und WSL wiederhergestellt und per SHA-256 geprüft.
  Wiederholung erkennt die vorhandenen Bytes als identisch. Die temporären
  Wiederherstellungskopien wurden danach entfernt.

Die vollständige `test_native_test_runner.py`-Regression stoppte zunächst auf
beiden Plattformen am bereits bestehenden Katalogabgleich: `protocol_sources`
war im Workflow-Prüfer bekannt, fehlte aber im Integrationskatalog. Der vorhandene
Workflow ist jetzt mit seinen tatsächlichen Argumenten und Ausgabeprüfungen
registriert. Der unveränderte Testläufer besteht danach vollständig unter
Windows und WSL (Exitcode 0). Das prüft den Katalog sowie Fehlerbehandlung und
Berichte des Testläufers; es ist keine neue Ausführung des nativen Bildschirmablaufs.

Die ausschließlich für diese Speicherprüfung erzeugten synthetischen Quellen,
Compilerprodukte und Testläufer-Fixtures sind nach Abschluss ihrer Prozesse in
`synthetic-fixtures.zip` gesichert: 4.095.585 Originalbytes in 727.964 Archivbytes.
Jede Archivdatei und jede lose Ursprungskopie wurde vor dem Entfernen erneut per
SHA-256 verglichen. Das Archiv mit Dateiverzeichnis erhält auch den fehlgeschlagenen
Windows-Compilerstand. Ergebnis-JSON und rohe Logs bleiben direkt lesbar.

## Verbleibende Arbeit und Grenzen

### Anschließende Bereinigung der abgeschlossenen Protokoll-Prüfbuilds

Weitere 236 Compilerobjekte (`.obj`/`.o`, zusammen 90.374.607 Byte Dateigröße)
aus `build/validation/protocol-ui-2026-10-25/4-native/{windows,linux}-build/obj`
wurden entfernt. Davor wurden die erhaltenen Programme und Debug-Symbole mit
den vollständig nachgelesenen Laufzeitarchiven der Phase 15 abgeglichen.
Quellarchive, Bibliotheken, Testberichte, Referenzbilder und Belege offener
Fehler bleiben erhalten. Die betreffenden Builds und Tests waren beendet;
die laufende NexaCloud-Arbeit wurde nicht verändert.

Der [kompakte Bereinigungsnachweis](verification/storage-followup-2026-10-25.json)
nennt Umfang und Hash der dateigenauen lokalen Löschquittung. Die Dateigröße
ist keine Messung freigegebener physischer Laufwerksbytes: NTFS-Kompression
und parallele Arbeiten beeinflussen diesen Wert. Für diese Dokumentation
wurden keine neuen Bildschirmbilder erzeugt. Der
[native Protokoll-Teilbericht](visuals/unified-workspace/PROTOCOL-C-NATIVE.md)
führt Ergebnisse und offene Befunde als Tabelle mit verknüpften JSON-Belegen.
F: wurde dabei weder abgefragt noch beschrieben.

Der erneute Nachlauf von `test_artifact_paths.py` besteht unter Windows und
`Alpine-Dev`; `with-artifacts.py --check` bestätigt auf beiden Plattformen
D:-Pfade für Ausgaben, temporäre Dateien und Caches. 99 lokale Dokumentationslinks,
die 324 Dateien der Quellsicherung, sechs Ergebnisgruppen und die Löschquittung
wurden zusätzlich geprüft. Die vorhandenen eingefrorenen Nachweise bleiben
unverändert.

### Weiter geltende Grenzen

Neue Builds und Nachweise bleiben auf D:. Alte Programme und Steuerdateien mit
eingebauten F:-Pfaden bleiben historische Belege; für weitere native Prüfungen
neue D:-Steuerdateien und auf D: gebaute Programme verwenden. Die Rückführung
ist keine erneute native UI-, Fokus- oder Screenreader-Abnahme.

Unix-Laufzeitverzeichnisse bleiben kurzzeitig im Linux-Dateisystem: Der vor der
Rückführung geprüfte F:-Mount meldete bei `AF_UNIX.bind` Fehler 95
(`Not supported`) und behielt trotz `chmod 0700` den Modus `0777`.
Das ist ein historischer Befund, keine erneute Abfrage von F:.
Große Testdaten liegen unter D:/Physim/build.
25 vorhandene Orca-Laufzeitverzeichnisse mit Socketdateien wurden aus der
Verlagerung ausgenommen.

Die 3,34 GB aus `/tmp` sind innerhalb von WSL freigegeben. Die gemeinsame
`D:/WSL/Alpine-Dev/ext4.vhdx` wurde weder verschoben noch verkleinert.
Quellen, lokale Änderungen, Nutzdaten, Git-Historien, eingefrorene Nachweise
im Repository und die laufenden Arbeiten anderer Projekte bleiben erhalten.
