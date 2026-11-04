# Physim: Parsergrundlage für lokale Vorschläge · 5. November 2026

Dies ist eine **Sprachgrundlage**, noch keine Anzeige lokaler Vorschläge in der
nativen UI und keine vollständige D-Abnahme. Maßgeblich bleibt
[die D-Freigabe](../visuals/completion-typing-options/APPROVED.md).
Der Nutzer-Mac bleibt aus; kein Zugriff oder Erreichbarkeitstest. Nur D:,
kein Zugriff auf F:. Beginn der lokalen Prüfung am 4. November, Abschluss am 5.

Quellcommit **`5d67db6e`** normal in beide Repositories gepusht; alle 1.124
erfassten Compiler-/Testdateien und 174 eigenen Eingaben mit tatsächlichen
Git-Blobs abgeglichen, nur CRLF/LF normalisiert.
[Commitabschluss und offene CI-Handles](language-scopes-commit-2026-11-05.json).

## Schnittstelle und Besitz

[`ps_lang_editor_bindings`](../../src/language/editor_bindings.h) verwendet die
gleichen Produktionen und den gleichen Lexer wie der strenge Physim-Parser.
Keine Übernahme des begrenzten Browser-C-Helfers. Der Aufrufer stellt Quelle,
Knoten- und Ausgabepuffer. Keine Allokation, Ein-/Ausgabe, Quelltextumschreibung
oder Ausführung. Ergebnisse sind Positionen in genau diesem Quellstand, kein
globaler Namenscache und kein Dokumentbesitz über den Aufruf hinaus.

Geparst wird der Originalpräfix bis zum Cursor. Eine dort unvollständige Klammer
liefert ausdrücklich `PREFIX`; daraus wird kein gültiger Compiler-AST abgeleitet.
Bei Syntaxfehlern vor dem Cursor oder erschöpften Ressourcen keine Kandidaten.
Eine leere Cursorzeile erhält anhand des nächsten echten Tokens ihre
Blockzugehörigkeit. Dieser Blick nach vorn liest ausschließlich die Einrückung;
keine späteren Deklarationen werden veröffentlicht. Kommentare/Strings laufen
durch den echten Lexer. Die bestehende Completion-Kontextprüfung bleibt für das
Unterdrücken von Vorschlägen in Kommentaren/Strings zuständig.

Die Ausgabe ist ein Deklarationsstapel aktiver lexikalischer Bereiche, einschließlich
überdeckter äußerer Bindungen. Rückwärtssuche liefert den nächsten Namen; die
eindeutige UI-Ergebnisliste muss später selbst entsprechend zusammenführen.
Variablen erscheinen erst nach abgeschlossenem Initialisierer und Trennzeichen
bzw. nach dem vollständigen eingerückten Lambda-Initialisierer. Parameter,
Schleifen- und bedingte Bindungen treten in ihren tatsächlichen Geltungsbereich.
Guard-Bindung nach dem Else-Block, Switch-Nutzlast auch in der Arm-Bedingung;
`_` ist dort kein Vorschlag. Geschlossene/fremde Blöcke hinterlassen keine Namen.
Verschachtelte Funktionen/Lambdas behalten die umgebenden sichtbaren Bindungen.

Typbereiche zeigen die tatsächlich geschriebene Annotation einschließlich
Gruppierung, Qualifikation, Array-, Optional- und Funktionstypen. Fehlende
Annotation bleibt ohne erfundenen Typ. `self` erhält den nominalen Typnamen aus
dem Struct-/Enum-Kopf und die Methodendeklaration als Quellstelle; statische
Methoden erhalten keinen eigenen `self`. Keine vollständige semantische
Typinferenz, Import-/Überladungsauflösung oder Laufzeitwertanzeige behaupten.
Die neuen Typbereichsmetadaten im internen AST ändern dessen Speicherlayout;
abhängige Ziele müssen mit den aktuellen Headern neu gebaut werden.

## Ausführung und Gegenbelege

[Originalkommandos, Eingaben, Hashes und Einzelresultate](language-scopes-2026-11-05.json).
Die native Gruppe umfasst `language_parser`, `language_checker`,
`language_editor_bindings`, `source_outline` und `syntax`. Zwei echte Toolchains:
Windows Release/MSVC und Linux Debug/GCC ASan/UBSan mit `detect_leaks=1`.
**`--no-app`**: Compiler und Unit-Verbraucher gebaut/geprüft; keine neue App-
Binärdatei und keine GUI-Abnahme dieses Stands. Ältere archivierte App-Dateien
werden nicht als neu gebaut ausgegeben.

Die Abschluss-Unit enthält 36 konkrete Cursorpräfixe und 1.024 deterministische
Byteeingaben. Jeder AST-Einzelfeldwert wird gegen den unabhängigen strengen
Parse-Einstieg verglichen, ohne undefinierte Struktur-Paddingbytes zu vergleichen.
Quellbytes bleiben unverändert; Namens-/Typbereiche bleiben innerhalb des
Präfixes. Zusätzlich strenger GCC-C17-Build mit `-Wall -Wextra -Werror`,
ASan/UBSan und `detect_leaks=1`, tatsächlicher Exit 0.

| Stand | Windows | Linux |
| --- | --- | --- |
| Parsergrundlage, 32 Präfixe | 5/5, `run-tl4vac0i` | 5/5, `run-1yrw9_ea` |
| Lambda-/Kapazitätskorrektur, 34 Präfixe | 5/5, `run-a_cflptz` | 5/5, `run-c90ctmc8` |
| Abschluss mit Leerzeilenzugehörigkeit, 36 Präfixe | 5/5, `run-efq_y_cx` | 5/5, `run-nurnwodj` |

Abschluss `language-scopes-affinity` eingefroren; Quell-/Laufzeit-/referenzierte
Archivbytes tatsächlich gelesen. Alle lokalen Build-/Test-/Archivhandles beendet.

Drei bei der Erweiterung tatsächlich scheiternde Vorherprüfungen erhalten:

- Ein fertig geschriebenes Lambda ließ den Namen seiner speichernden Variablen
  fehlen. Der neue `closure`-Fall scheiterte; die Deklaration wird nun auch nach
  dem eingerückten Initialisierer veröffentlicht, weiterhin nicht in dessen Körper.
- Ein exakt ausreichender Knotenpuffer wurde als erschöpft gemeldet. Vorher
  Exit 1, danach Exit 0; tatsächliche Kapazitäts-/Tiefenüberschreitungen bleiben
  gesondert `LIMIT` und liefern keine Teilkandidaten.
- Eine leere, nicht eingerückte Zeile zwischen fortgesetzten eingerückten
  Anweisungen verlor die lokalen Namen. Vorher Exit 1, nach reiner Layout-
  Vorschau Exit 0; geschlossene innere und spätere Bindungen bleiben ausgeschlossen.

Eine getrennte Kopie deaktiviert ausschließlich die Rücknahme der Bindungen beim
Blockende. Dieselbe Unit lehnt den dadurch entstehenden Fremdfunktions-Leak mit
Exit 1 ab. Produktionsquelle unverändert; Original-/Mutant-/Binärhashes gesichert.
Alle Phasen bleiben getrennt, kein nachträglich erfundener neuer Gesamtlauf.
Keine Bilder erzeugt und keine Objekt-/Abhängigkeits-/VM-Verzeichnisse gelöscht.

## Anschluss und offene Abnahme

Native Zusammenführung vor API-Namen, Quellstellen-Kurzinfo, Einfügen/Undo,
leere Argumentauslöser, echte C-Deklaratoren/-Scopes und separate Aufrufhilfe
einschließlich Benutzerfunktionen bleiben erforderlich. Ebenso physische
Eingabe/IME, Screenreader, DPI/Monitor/Vollbild und aktuelle Mac-Abnahme.
Die Browservorlage ersetzt diese Arbeiten nicht.

Zum vorherigen Quellstand `f01f25c1` sind neue gehostete Originalberichte gelesen:
Intel-Editor **6/6**, ARM-Editor **5/6** (automatischer Ablauf Stufe 71, Ursache
offen), Intel-F1 **6/6**. Archive/Digests/alle Einträge geprüft. Einstellungen
und ARM-F1 waren im Snapshot noch queued. Dies ist keine Mac-Abnahme der neuen
Parsergrundlage. Frühere F1-Timeouts werden durch den neuen Erfolg nicht
nachträglich erklärt oder gelöscht.
[Getrennter Mac-Bericht](completion-info-macos-2026-11-05.json).
Zuerst den konkreten ARM-Automatikbefund untersuchen, danach die Scopes nativ
zusammenführen und C ergänzen. Keine geschlossenen F1-/Protokoll-/Settings-
Matrizen allein wegen Kontextwechsel wiederholen. Keine volle D-/UI-Punktzahl.
