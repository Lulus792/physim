# Native lokale Physim-Vorschläge — 5. November 2026

Der echte Physim-Parser liefert jetzt sichtbare Variablen/Parameter an die native
Vervollständigung. Eigenständige Quellkopie, geschriebene Typen, nächste Bindung bei
Überdeckung, vollständiger Trefferbestand vor API-Namen. Liste, neutrale Kurzinfo,
Quellzeile und Einfügen verwenden dieselbe revisionseigene Deklaration. Lokale Namen
erhalten kein erfundenes API-Handbuch. API-Referenz und bisheriger Inspektor bleiben
erhalten; Shift+F1 prüft lokale Namen auch ohne offene Liste. Bei Prüf-/Speicherfehlern
wird keine ersatzweise falsche API-Referenz geöffnet.

Leere Argumente nach `(` / `,` lösen die vorhandene verzögerte Automatik aus. Die
Aufrufposition stammt aus der tatsächlichen Grammatik, nicht aus Kommazählen in
Strings oder Arrays. Nur der Bezeichner wird ersetzt; Klammern/weitere Argumente
und zusammenhängendes Undo bleiben erhalten. Unicode-Präfixe nutzen die realen
Physim-Bezeichnerregeln und den tatsächlichen fetten Monospace-Font. Die Rollen von
Titel/Liste/Signatur folgen den aktiven Syntaxfarben; eine lokale Variable namens
`sin` bleibt in der Signatur eine Variable. Kein eigener Worker/Provider/Sprachdienst.

## Ausgeführte Nachweise

Originalkommandos, Quell-/Artefakthashes und Ergebnisdateien:
[kompakter Bericht](completion-locals-2026-11-05.json).

| Phase | Windows Release / MSVC | Linux Debug / GCC ASan/UBSan |
| --- | --- | --- |
| `completion-locals-sdl` | Unit **10/10**, GUI **6/6** | Unit **10/10**, GUI **6/6** |
| `completion-locals-final` | gezielt GUI **2/2** | gezielt GUI **2/2** |

Erste Phase: `run-5gdvxv90` / `run-thhiuzvj` und `run-vj6fisw0` / `run-xpjl39wh`.
Abschluss: `run-0n04ipy5` / `run-mlom98ej`. Zwischen den beiden Phasen nur
Shift+F1-Fehlerbehandlung, zugehörige geschlossene-Liste-Prüfung und Bedienungsdoku
geändert. Keine erneuten unveränderten Units oder gesamte GUI-Matrix mit letzter
Binärdatei behaupten. Unit-/GUI-Appbytes der ersten Phase je Plattform identisch.

Units: Completion, Locals, Help, Info, Syntax, API-Referenz, Sprachparser, Checker,
Editor-Bindings und Explorer-Worker. Neue direkte strenge C17-Tests mit `-Wall
-Wextra -Werror -fsanitize=address,undefined`, `detect_leaks=1`: Scope-Modell samt
jedem Allokationsfehler und Parser mit **49 Präfixen + 1.024 AST-Feldvergleichen**
bestanden. GUI verwendet weiterhin `detect_leaks=0`; kein physischer Eingabenachweis.

Native neue Fixture: gültige Einrückungsgrammatik statt alter Browser-/Brace-Fixture;
Modell, Analyse, zusätzliches Dokument; Dunkel, Hell/große Schrift und Kontrast.
Tatsächliche Zeichenbefehle, vorhandene Unicode-Glyphen und echter fetter Font,
Typ/Zeile, Ausschluss fremder/späterer Namen, Überdeckung, mehr als fünf Ergebnisse,
Scrollen, leere Argumente, Ersatz mit Restwort/weiterem Argument und ein Undo geprüft.
Sechs endgültige Plattform-/Theme-Ansichten betrachtet; Interaktionen liefern Text,
keine zusätzlichen Vollbilder pro Tastendruck. Windows-Modellbild bytegleich zur
bereits betrachteten ersten positiven Aufnahme; übrige finale Ansichten betrachtet.

## Erkannte Fehler und Gegenprobe

Beide ersten App-Builds scheiterten am neuen Testcode: SDL2-Schreibweise `SDLK_z`
statt SDL3 `SDLK_Z`. Korrigiert; diese Phase ist kein nativer Erfolg.

Isoliert nur die lokale Signatur-Rollenauflösung entfernt. Windows-Gegenprobe
`run-70hxafqh`: **0/1**, Stufe **10**, `ink=1/1/0/1/1/1`; nur die geprüfte
Signaturfarbe fehlt, Liste/Präfix/Quelle/Rolle/Referenzschutz bleiben vorhanden.
Root unverändert. Danach bestandene Quellen und alle **248 Windows-/135 Linux-**
Laufzeitdateien einschließlich referenzierter Archivbytes gelesen/abgeglichen und
abweichende Bytes aus dem positiven Snapshot wiederhergestellt. Kein erneuter Lauf
identischer nachgewiesener Appbytes behauptet. Alle lokalen Handles beendet.

Sieben ersetzte erfolgreiche Rohbilder entfernt: **22.378.566 Bytes**.
Die sechs finalen Plattform-/Theme-Referenzen, das Linux-Umbruchbild und sämtliche
ursprünglichen Fehler-/Gegenprobenprotokolle bleiben erhalten.
[Geprüfte Pfade und Hashes](completion-locals-cleanup-2026-11-05.json).

## Grenzen und Anschluss

**Keine volle D-/UI-Abnahme.** Echte C-Scopes und die separate Aufruf-/Parameterhilfe
einschließlich Benutzerfunktionen fehlen. Keine Typinferenz, Laufzeitwerte,
Präprozessor-/Includeauflösung oder allgemeine semantische Syntaxklassifikation
außerhalb der belegten Deklarations-/aktuellen Abfragespannen behaupten.
Physische Eingabe/IME, AT, DPI/Monitor/Vollbild und neue macOS-Abnahme bleiben offen.
Der Nutzer-Mac ist aus und wurde nicht angesprochen; frühere gehostete Mac-Erfolge
belegen diese neuen Quellen nicht. Neue Mac-Workflowregistrierung ist kein Testlauf.

Linux bei Schriftgröße 22: Dateinamen im Baum brechen teilweise innerhalb der
Erweiterung um. Finale Analyseaufnahme erhalten; vor Layoutänderung mit dem
freigegebenen Explorer A abgleichen. F1-/Protokoll-/Settings-Matrizen nicht allein
wegen Kontextwechsel wiederholen. Fremde AX-/Orca-/Workflow-/dist-Arbeit ausgeschlossen.
