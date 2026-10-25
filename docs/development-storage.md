# Lokale Entwicklungsdaten und Prüfnachweise

Auf dem Windows-Entwicklungsrechner bleiben Quellen, Testprogramme und kleine
versionierte Nachweise in `D:\Physim`. Seit dem erneuten Nutzerentscheid am
25. Oktober 2026 bleiben auch sämtliche generierten Physim-Daten auf D:.
`D:\Physim\build` ist ein echtes Verzeichnis. Darunter liegen `native` für
Programme und Zwischenprodukte, `validation` für Prüfläufe, `tmp` für temporäre
Dateien, `cache` für Caches, `archive` für alte geprüfte Läufe und `maintenance`
für Bereinigungsprotokolle. F: wird nach der Rückführung nicht mehr für Physim
verwendet. Das ist eine lokale Speicherregel, keine Voraussetzung für andere Rechner.

Entwicklungsbefehle erhalten die Umgebung nur für ihren eigenen Prozessbaum:

```powershell
python tools/with-artifacts.py --check
python tools/with-artifacts.py -- python tools/build.py --config Release --no-app --test --test-filter protocol_store
```

Unter `Alpine-Dev`, aus `/mnt/d/Physim`:

```sh
python3 tools/with-artifacts.py --check
python3 tools/with-artifacts.py -- python3 tools/build.py --config Debug --no-app --test --test-filter protocol_store
```

Der Wrapper setzt `PHYSIM_ARTIFACT_ROOT`, `TMPDIR`, `TMP`, `TEMP` und
`XDG_CACHE_HOME` und `PYTHONPYCACHEPREFIX`. Der direkte Builder verwendet dann standardmäßig
`<root>/native/<Konfiguration>`; Ergebnisse liegen im jeweiligen
Buildordner unter `test-results`. Ein explizites `--build-dir` hat Vorrang.
Andere Werkzeuge mit eigenen Ausgabeoptionen müssen ebenfalls einen Pfad unter
D: erhalten. Beliebige Programme, die feste Pfade verwenden, werden durch
Umgebungsvariablen allein nicht umgeleitet.

Ohne `--root` verwendet der Wrapper das `build`-Verzeichnis dieses Repositorys,
unter WSL entsprechend `/mnt/d/Physim/build`. Auch der direkte Builder verwendet
diesen Standard. Ein explizites `PHYSIM_ARTIFACT_ROOT` bezeichnet denselben Stamm.
Die ignorierte lokale Datei `.physim-storage.json` legt auf diesem Rechner
`artifact_root: "build"` und `blocked_roots: ["F:/", "/mnt/f"]` fest
(Formatversion 1). Wrapper und Builder weisen solche Pfade vor ihrer Verwendung
ab; das betrifft beim Builder auch SDL- und Installationspfade. Diese Prüfung
ersetzt keine Prüfung der Ausgabeoptionen fremder Programme.

Während eines Umzugs sperrt eine Datei `.physim-storage-migration` den betroffenen
Buildbaum für den direkten Builder. Während der Rückführung keine neuen Builds
in diesem Baum starten. Die Sperre erst nach
vollständigem Umzug und Integritätsprüfung entfernen.

Unix-Laufzeitverzeichnisse mit Sockets bleiben kurzzeitig im nativen
Linux-Dateisystem und werden nach dem Lauf entfernt. `XDG_RUNTIME_DIR` wird
deshalb nicht auf das Windows-Laufwerk gesetzt. Große Bild-, Log- oder
Compilerdateien gehören nicht in diese Laufzeitverzeichnisse.

## Nachweise mit weniger Bildern

Für neue native Prüfungen die [Berichtsvorlage](templates/native-validation.md)
verwenden und nur die tatsächlich benötigten Abschnitte ausfüllen.

Ein Prüfbericht enthält Auftrag und Akzeptanzkriterien, Quell-/Binärhash,
Plattform und Werkzeuge, genaue Befehle, Exitcode sowie Soll und Ist je
Prüfung. Geometrie, Farben, Fokuskennungen und Zustandswechsel lassen sich als
JSON dokumentieren. Ein Screenshot bleibt für die visuelle Gesamtwirkung
und für Unterschiede nötig, die diese Messwerte nicht vollständig erfassen.

Pro dargestelltem Designzustand genügt normalerweise eine gezielte Aufnahme.
Weitere Bilder brauchen einen eigenen Zweck, beispielsweise einen Fehler,
eine Gegenprobe oder ein anderes Theme. Erfolgreiche Tastatur- und
Filteraktionen werden als Zustandsfolge dokumentiert. Fehlerbelege und
Vergleichsreferenzen bleiben erhalten, bis der Befund geprüft geschlossen ist.
Alte erfolgreiche Rohbilder dürfen nach Abnahme entfernt werden, wenn ihre
Aussage bereits im kompakten Bericht und den ausgewählten Referenzen steckt.

## Archivierte alte Prüfläufe wiederherstellen

Die lokalen Archive unter `D:\Physim\build\archive\completed-tests-2026-10-25`
enthalten Originaldateien abgeschlossener Läufe, einschließlich alter
Fehlerbelege. `files.zip` enthält die Bytes, `index.jsonl` Dateinamen,
Größen, Änderungszeiten und SHA-256-Werte. `verified.json` sichert Archiv
und Index. Ein `ARCHIVED.json` im früheren Ergebnisordner nennt das Archiv.
Ein archivierter Lauf ist kein neu bestandener Test.

Zum Prüfen eines alten Befunds gezielt den benötigten Lauf oder eine einzelne
Datei wiederherstellen, beispielsweise:

```powershell
python tools/restore-test-archive.py D:/Physim/build/archive/completed-tests-2026-10-25/Release --destination D:/Physim/build/validation/restored --prefix run-BEISPIEL
```

`run-BEISPIEL` durch den tatsächlichen Pfad aus `index.jsonl` ersetzen.
Unter WSL die Pfade mit `/mnt/d/Physim/build` beginnen. Das Werkzeug prüft die
Prüfsummen und schreibt bestehende, abweichende Dateien nicht um. Es stellt
nur den angegebenen Lauf/Teilpfad wieder her. Für einen alten Nachweis
gegebenenfalls die archivierten Originale vor dessen historischem Prüfskript
wiederherstellen; eingefrorene Manifeste bleiben unverändert.

Historische Manifeste und `ARCHIVED.json` können noch den früheren F:-Ort
nennen. Die Rückführung ordnet `F:/Physim/build/<Pfad>` wieder
`D:/Physim/build/<Pfad>` zu; andere Gruppen wie `archive`, `validation`,
`dependencies` und `maintenance` liegen jetzt unter `D:/Physim/build/<Gruppe>`.
Das Wiederherstellungswerkzeug erhält den heutigen Archivordner als Argument.
Alte Steuerdateien mit fest eingebauten F:-Pfaden sind historische Belege und
dürfen nicht als aktuelle Anleitungen ausgeführt werden. Einzelheiten und der
Abschlussstatus stehen im [Wartungsbericht](storage-cleanup-2026-10-25.md).

Vor einer Bereinigung Eigentümer, laufende Prozesse, Referenzen und Inhalt
prüfen. Ein kurzer Bericht nennt Umfang, Pfade, erhaltene Belege, tatsächlich
freigegebenen Speicher und Verifikationen. Kopierte Daten erst nach
Prüfsummenvergleich am Ursprung entfernen. Freier Platz innerhalb der VM ist
gesondert vom freien Platz auf D: anzugeben; das Entfernen von Linux-Dateien
verkleinert die VHDX nicht automatisch.

`D:\WSL\Alpine-Dev\ext4.vhdx` sowie fremde Projekte und deren laufende Arbeiten
gehören nicht zur Physim-Bereinigung.
