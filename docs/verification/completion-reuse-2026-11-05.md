# C-Syntax bei unveränderter Quelle wiederverwenden

Stand: 5. November 2026. **Cursorwechsel und erneutes Öffnen der Liste verwenden
die unveränderte Syntax wieder. Große Textänderungen bleiben langsam.**
[Nachweis mit Originalberichten, Quellhashes und Grenzen](completion-reuse-2026-11-05.json).
Die [vorherige Baseline](completion-perf-2026-11-05.md) bleibt unverändert.
Quellcommit **`9292b67e`** in beiden Repositories;
[Commitabgleich und gehosteter Mac-Anschluss](completion-reuse-commit-2026-11-05.json).

## Änderung und Besitz

`ps_c_bindings_source` besitzt die ursprünglichen Quellbytes und höchstens
65.536 flache Grammatikknoten. Nach dem Aufbau lebt kein Tree-sitter-Objekt
weiter. Bei einem anderen Cursor entstehen Scope-/Aufrufdaten mit frischem
privatem Index; auch die alten C-Parameterflags werden nicht zwischen Aufrufen
geteilt. Geänderte Quellbytes, Sprache oder Editorbesitzer erzwingen einen
neuen Snapshot. Es gibt weder eine Trefferkürzung noch einen neuen Worker.
Die bisherigen Quell-, Knoten- und Parserbudgets bleiben bestehen.

Das Vorschlagsmodell überträgt einen wiederverwendeten Syntaxbesitz erst bei
erfolgreicher Veröffentlichung. Bei Speicherfehlern bleiben Quelltext,
Typen, IDs, Ergebnisse und bisheriger Cache erhalten. Geschriebene Typen,
Originalspannen und vollständige Treffer bleiben die Grundlage; keine neue
Präprozessor-, Include-, Typinferenz- oder Signaturauflösung.

In der Oberfläche löscht das Ausblenden weiterhin sichtbare Auswahl,
Einfügezustand, Kurzinfo und ausstehende UI-Anfragen. Ein einzelner interner,
eigener Snapshot darf für denselben Editor erhalten bleiben. Vor erneutem
Anzeigen werden Besitzer, Sprache, sämtliche Quellbytes und Cursor geprüft.
Editorwechsel, Dokumentauswahl/-entfernung, Projektwechsel und App-Ende geben
den Cache frei. Die Codefarben verwenden lokale Daten weiterhin nur bei
aktiver, zur Quelle passender Liste. Das freigegebene Design ändert sich nicht.

## Tatsächlich geprüft

Windows Release und Linux Debug mit ASan/UBSan bestehen jeweils **Unit 8/8 und
GUI 6/6**. Nach Verstärkung des C-Tests zusätzlich je **1/1**, ohne Änderung der
geprüften App-Datei. Die Linux-Units und der direkte strenge C17-Lauf verwenden
`detect_leaks=1`; die GUI behält ihre bestehende Font-/Runtime-Konfiguration
mit `detect_leaks=0`. Daraus keine gesamte GUI-Leakfreiheit ableiten.

- **887** Vergleiche mit jeweils frisch erzeugtem Modell: Cursor-/Scopewechsel,
  echte Einfüge-/Löschfolgen, gleicher Textpuffer, Unicode/UCN, Funktionsparameter,
  ältere C-Parametersyntax, Kommentare, Strings und Fehlererholung.
- **123/123** Fehler im neuen C-Modellaufbau, **7/7** beim wiederverwendenden
  Cursorwechsel und **87/87** bei einer Textänderung erhalten den alten Zustand
  und erlauben den anschließenden Versuch. Die vorhandene Scope-Suite bestätigt
  35 Quellen, 182 Cursor-/Dokumentpaare, 67 Speicherfehler und 1.024 fehlerhafte
  Eingaben; 38 Aufrufkontexte bleiben bestanden.
- Native C- und Physim-Listen über Modell, Analyse und eigene Datei, jeweils
  dunkel/12 px, hell/22 px und hoher Kontrast/12 px: Escape entfernt tatsächlich
  die sichtbaren Treffer; Strg+Leertaste öffnet mit demselben geprüften Snapshot
  erneut. Quellkarte, Ersetzen/Undo, Automatik, Fristrennen und API-Abschirmung
  bleiben bestanden. Neue Marker werden vom externen Prüfer verlangt.
- Mess-CLI mit dem neuen `edit`-Modus: **54 Fälle pro Windows/Linux**, Linux
  tatsächlich GCC/ASan/UBSan/Leakprüfung. Er fügt am Cursor `v` ein und löscht es
  wieder; Eingabepufferaufbau bleibt außerhalb der gemessenen Backendzeit.

Die erste absichtlich falsche Inhaltsprüfung wurde vom ursprünglichen Test
**nicht** erkannt: Wechselnde Besitzer und eine Umbenennung mit gleicher
Grammatikform verdeckten den Fehler. Die verstärkte Probe ändert bei gleichem
Besitzer, Cursor, Puffer und Textumfang `double value` in `return value` und
verlangt, dass die Deklaration verschwindet. Danach werden sowohl die ausgelassene
Byteprüfung als auch ein ignorierter Cursor mit echten Assertions abgewiesen.
Beide manipulierten Kopien kompilieren; die Produktion wird dafür nicht verändert.
Nur diese absichtlich vor dem Aufräumen abbrechenden Negativprogramme deaktivieren
Leakprüfung. Positive Prüfungen behalten sie bei.

Alle **1.239** erfassten Compiler-/Test-/Besitzereingaben wurden gegen tatsächliche
Git-Index-Blobs gelesen; Vendor bytegenau, sonst ausschließlich CRLF/LF normalisiert.
Die stärkere Testversion ist im finalen Snapshot `completion-reuse-strong` enthalten.
Erste vollständige native Resultate behalten ihren früheren Testumfang von 885
Vergleichen; die gezielten Nachläufe belegen die endgültigen 887. Geschlossene
F1-/Protokoll-/Settings-Matrizen wurden nicht aus Kontextgründen wiederholt.

## Laufzeit und Speicher

Verglichen werden dieselben sechs Quelldateibytes wie zuvor. Ein voriger Backend-
Stand aus `8bddf0fa` und die neue Version verwenden denselben aktuellen C-Messtreiber.
Pro Quelle und Modus wechseln separate Prozesse, je 21 Stichproben mit normalem
und anschließend instrumentiertem Allocator. Das ist **kein** Vergleich innerhalb
desselben Prozesses und begründet keinen präzisen Beschleunigungsfaktor.

| Quelle | Cursor vorher p95 ms | Cursor nachher p95 ms | Textänderung nachher p95 ms |
| --- | ---: | ---: | ---: |
| Echtes Pendelbeispiel | 6,58 | 1,34 | 5,71 |
| 64 lokale Variablen | 0,96 | 0,08 | 0,86 |
| 512 lokale Variablen | 11,07 | 2,24 | 10,16 |
| 2.048 lokale Variablen | 39,00 | 8,27 | 25,23 |
| 4.096 lokale Variablen | 59,56 | 14,17 | 60,19 |
| 6.144 lokale Variablen | 92,73 | 22,95 | 104,16 |

Ein vorheriger vollständiger Nachlauf lag im größten Cursorfall bei 10,20 ms p95;
der alternierende Vergleich zeigt weiter Streuung. Die ursprüngliche 16,7-ms-
Prüfmarke ist damit **nicht zuverlässig erfüllt**, ebenso wenig für große
Textänderungen. Ein frischer Aufbau und geänderte Quellen parsen weiterhin voll.
Diese Grenze bleibt im Arbeitsstand offen; kein FPS-/Gesamt-UI-/Linux-/Mac-
Performanceversprechen aus diesen Windows-Backendzahlen ableiten.

Cursorwechsel benötigen in allen sechs Messquellen **sieben** Allokationen,
vorher im größten Fall 61.513. Dafür steigt der im größten Beispiel gehaltene
Zustand von **1.801.397 auf 6.166.734 angeforderte Bytes**. Die gemessene Spitze
bei Textänderung beträgt **23.198.338 Bytes**, ohne Ablehnung durch den äußeren
64-MiB-Messschutz. Angeforderte Größen einschließlich interner Parserverwaltung,
ohne äußere Messheader; keine Aussage über RSS. Dieser bewusste Speicherpreis
des einzelnen Flat-Caches bleibt im Nachweis sichtbar.

## Aufbewahrung und Anschluss

Originale unter `build/validation/completion-reuse-2026-11-05/`. Nach Abschluss
aller lokalen Prozesse wurden **33 bytegleiche BMP-Duplikate / 105.498.954 Bytes**
entfernt; Referenzen und Originalresultate bleiben erhalten. Eine abweichende
Linux-API-Aufnahme bleibt erhalten; der Sichtvergleich zeigt dieselbe Ansicht
mit abweichender Caret-Blinkphase. Keine neue Bildserie für jeden Eingabeschritt.
Kein SDK-/Toolchain-/VM-Bestand gelöscht und keine früher abgelehnte Löschung umgangen.
Ausgaben und temporäre Daten bleiben auf D:, F: unberührt.

Der private Mac bleibt ausdrücklich **aus und unberührt**. Der gehostete Nachlauf
`41968549203` zu `9292b67e` ist geschlossen: ARM `127051148217` und Intel
`127051147984` je **8/8**, beide Original-ZIPs/Digests/alle Einträge gelesen,
C/Physim je drei Editorziele mit neuem Cache-Wiederöffnungsmarker.
[Gemeinsamer Mac-Abschluss](completion-reuse-macos-2026-11-05.json).
Keine ganze Mac-/physische/Performance- oder neue Mac-Allokationsfehler-Abnahme;
die nachfolgende Parameterhilfe-Datenarbeit ist nicht darin enthalten.
Als Nächstes bleiben große Editierfolgen und sichere inkrementelle Auswertung
offen, anschließend die eigene Parameterhilfe und die übrigen freigegebenen
UI-Bereiche. Physische IME-, Screenreader-, Fokus- und DPI-Abnahme bleiben getrennt.
