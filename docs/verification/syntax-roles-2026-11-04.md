# Gemeinsame Syntaxrollen · 4. November 2026

Die freigegebenen zehn Rollen aus [D](../visuals/completion-typing-options/APPROVED.md)
werden jetzt im Editor, in Vorschlagsnamen/-glyphen, Titel und Signatur der
neutralen Kandidatenkarte sowie Codeblöcken der API-Karte gemeinsam gezeichnet.
Funktionen sind dunkel `#dcdcaa`, hell `#795e26`; keine pauschale `ps_`-Typfarbe.
Echte Semibold-Glyphen heben das Präfix hervor. Text, Auswahl, Einfügemarke,
Übernahme und Undo verwenden weiter ihren vorhandenen Vertrag.

Der allokationsfreie Scanner nutzt den echten Physim-Lexer für Schlüsselwörter
und Unicode-Bezeichner, den gemeinsamen Compilerbestand für 60 eingebaute Typen
und exakte Katalogidentitäten für 649 C-/615 Physim-Einträge. Der bestehende,
quellstandsgebundene Dokumentumriss liefert Deklarationsrollen. Das ist noch kein
vollständiger Scope-Provider: lokale Sichtbarkeit, Shadowing, Includes und
Typinferenz sind damit nicht bewiesen. Unbekannte Aufrufe erhalten eine lexikalische
Funktionsrolle; unbekannte Namen keine erfundene Typinformation.

## Tatsächliche Ausführung

Windows Release/MSVC und Linux Debug/GCC unter Alpine-Dev/Xvfb,
ASan/UBSan; GUI mit `detect_leaks=0`. Eigenes isoliertes Quellabbild,
163 eigene Eingaben, 1.115 erfasste Compiler-/Testdateien. Fremde AT-SPI-/Orca-
Änderungen ausgeschlossen. [Originalkommandos, Ergebnisse und SHA-256](syntax-roles-2026-11-04.json).

| Phase | Windows | Linux | Aussage |
| --- | --- | --- | --- |
| `syntax-roles-final` | 8/8 Unit `run-zk1qnkdv`; 5/5 GUI `run-txjray6c` | 8/8 Unit `run-s9tijuhw`; 5/5 GUI `run-paymmy12` | Vollständige betroffene Gruppe vor der Bereichsoperator-Korrektur. |
| `syntax-roles-ranges` | 1/1 Unit `run-5z80dbsj`; 5/5 GUI `run-w2ygczev` | 1/1 Unit `run-3cvv54f3`; 5/5 GUI `run-gv1_8b_5` | Danach nur Scanner und Syntax-Unit verändert; übrige Eingaben per Hash gleich. |
| `syntax-roles-head-proof` | 1/1 GUI `run-u1wb6920` | 1/1 GUI `run-ryf85y0v` | Nur Testnachweis verschärft: Titel tatsächlich oberhalb der Signatur. Produktionscode unverändert. |

Unit-Gruppe: `syntax`, `completion`, `preferences`, `source_outline`,
`api_reference`, `language_lexer`, `language_parser`, `language_checker`.
GUI-Gruppe: `completion_workflow`, `completion_automatic_workflow`,
`api_reference_workflow`, `language_editor_unicode_preview`,
`language_editor_multiline_preview`. `--test` und `--test-display` sind getrennte
Aufrufe, mit nachgewiesen gleichen App-Bytes je Plattform und serieller Phase.
Keinen neuen vollständigen 8+5-Lauf mit der letzten Binärdatei behaupten.

Die manuelle Completion-Gruppe führt C und Physim jeweils in Modell, Analyse und
weiterem Dokument aus; dunkel/Code 12, hell/UI und Code 22, Kontrast/Code 12.
Tatsächliche Nuklear-Zeichenbefehle bestätigen Quellrollen, Listennamen, Kartentitel,
Signatur und verwendete Semibold-Fontquelle unabhängig von der Produktionsabbildung.
C-Rollenmaske 1023, Physim 991 ohne C-Präprozessor; alle Kartenmerkmale jeweils 1.
Der echte Referenzknopf wird am Ende angeklickt und die Quellerhaltung geprüft.
Drei aktuelle native BMPs tatsächlich betrachtet: dunkel, hell/groß und Kontrast.

Preferences prüft alle zehn exakten Farben für drei Themes in Basis-, Werkbank-
und Laborpalette. Kontrastschwellen gelten auf den tatsächlichen Basis-/Werkbank-
Codeflächen, nicht zusätzlich auf generischen Labor-Wertefeldern. Die gemeinsame
RGB-Aggregathilfe ersetzt nur die bisherige Nuklear-Hilfsfunktion; andere
abgeleitete UI-Farbwerte bleiben identisch.

## Fehler und Gegenproben erhalten

- `syntax-roles-ready`: beide Runner vor nativen Tests abgebrochen, weil der
  Generator die ausgegliederte Typnamensliste noch im Checker suchte. Generator
  auf dieselbe Registry umgestellt; Katalogbestand unverändert geprüft.
- `syntax-roles-registry`: je 7/8 Unit; nur Preferences-Link scheitert an
  `nk_rgb`, nachdem die Unit erstmals abgeleitete Paletten benutzt. Pure
  RGB-Aggregathilfe behebt die Kopplung. Windows-Diagnose `run-w86_o9yy` besteht
  die C-App und alle C-/Physim-Farbmerkmale, scheitert aber später am Physim-
  Referenzklick auf veraltete Layoutkoordinaten. Fixture wartet nun begrenzt auf
  die tatsächlich neue Symbol-/Kartendarstellung. Keine Produktursache behaupten.
- `syntax-roles-palette`: danach beide Plattformen tatsächlich 8/8 + 5/5.
- Isolierter Renderer-Gegenfall `run-wpt_xh7w`: ausschließlich Funktionsfarbe auf
  Typfarbe umgebogen, Lexer/Palette unverändert. Windows lehnt die C-App wirklich
  ab: Quellmaske 895 statt 1023, Liste/Titel/Signatur/Präfix jeweils 0. Physim
  und Linux hierfür nicht ausgeführt. Mutant-Bytes und ursprüngliche Quellen erhalten.
- Echter Bereichsoperatorfehler: `1...5` wurde als `1`, `.`, `.`, `.5` zerlegt.
  Neue Unit lehnt alte Produktion mit Exit 1 ab; keine künstliche Mutation.
  Korrektur behandelt `...`/`..<` als einen Textoperator, anschließend Exit 0.
  Direkter C17-/ASan-/UBSan-Lauf mit `-Wall -Wextra -Werror` nutzte geprüfte Kopien
  eingefrorener Bibliotheken, keine Ausgaben des gleichzeitig laufenden Builds.
  Die spätere registrierte Unit bestätigt auch die echten Compiler-Tokenarten.

Alle lokalen Handles beendet, Quell-/Runtimearchive einschließlich referenzierter
Bytes gelesen. Letzter eingefrorener Anschluss: `syntax-roles-head-proof`.
Nach konkreter Pfad-, Hash-, Schutz- und Nutzungsprüfung nur 36 ersetzte erfolgreiche
Completion-BMPs aus drei eigenen älteren Phasen gelöscht: **115.089.768 Bytes**.
Aktuelle zwölf manuelle Bilder, Fehler, andere GUI-Gruppen, ausdrücklich benötigte
ältere Referenzen, Quellen, Archive, Nutzdaten, Abhängigkeiten und VM erhalten.
Dies sind neue, im JSON einzeln benannte Pfade; frühere Bereinigungen nicht mitzählen.

## Grenzen und nächster Schritt

Vollständige Kandidatenzusammenfassung/Parametertexte, eigener Dokumentbesitz,
echte lokale C-/Physim-Scopes und separate Aufrufhilfe bleiben offen.
Die neue gemeinsame Signaturzeichnung ist keine eigenständige Parameterhilfe.
F1-Rich-Codeblöcke ohne Sprachmetadaten und weitere Beispielkontexte sind noch
nicht allgemein auf den Scanner umgestellt. Semibold-Präfixe nutzen den vorhandenen
ASCII-Markerfont; spätere Unicode-Scopevorschläge brauchen passende Glyphenabdeckung.
Physische Eingabe/IME, Screenreader, DPI/Monitor/Vollbild und aktuelle Mac-Abnahme
bleiben offen. Keine volle D-/UI-Punktzahl oder aktuelle Mac-Abnahme behaupten.

Der Nutzer-Mac ist aus und wurde nicht kontaktiert. Auf dem **vorherigen** Stand
`913d60ed` sind die gehosteten Intel-Editorfälle tatsächlich **5/5**, Settings
**16/16**; beide ZIPs/Digests und Originalergebnisse gelesen. Beide ARM-Jobs
wurden abgebrochen, nicht bestanden. [Getrennter Quellstandnachweis](completion-d-macos-intel-2026-11-04.json).
Diese Ergebnisse beweisen keine macOS-Ausführung der neuen Syntaxänderungen.
