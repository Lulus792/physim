# Funktionsköpfe beim unvollständigen Tippen

Stand: 5. November 2026. Lokaler Parserschritt abgeschlossen; gemeinsamer
Signaturresolver und sichtbarer Parameter-HUD nach ADR 023 bleiben offen.

## Tatsächliche Änderung

Die getrennte Abfrage `ps_lang_editor_module_signatures` liefert vollständige
geschriebene Modulfunktions- und Methodenköpfe auch hinter einer gerade
unvollständigen Eingabe. Beispiel: In `step` wird `later(` getippt, `later` steht
weiter unten. Die bisherige vollständige Parserabfrage verwirft bei diesem Fehler
ihre Sammlung; die neue Abfrage erreicht den späteren Kopf trotzdem.

Sie verwendet den echten Lexer und die echten Funktions-/Typ-Produktionen.
Eine Syntaxwiederaufnahme erfolgt an Moduldeklarationen oder der zuvor tatsächlich
gelesenen Struct-/Enum-Methodeneinrückung. Falls ein unvollständiger Aufruf das
folgende `func` bereits als Lambda-Anfang verbraucht hat, wird dessen originale
Zeile mit dem Lexer wieder besucht. Die aktuelle Deklaration darf dabei niemals
erneut begonnen werden, auch wenn ein beschädigter Struct-/Enum-/Importkopf
noch keinen Knoten erzeugt hat. Der gespeicherte Deklarationsfortschritt sichert das.

Nur vollständig gelesene Köpfe werden veröffentlicht. `PREFIX` bedeutet hier
wiederaufgenommene Syntaxfehler, keine gültige AST-/Compilerabnahme. Lexikalische
Fehler oder erschöpfte Ressourcen verwerfen die Ausgabe. Lokale Funktionen sind
bewusst nicht Teil dieser Dokumentabfrage; ihre tatsächlichen Gültigkeitsbereiche
liefert weiter die Cursor-Präfixabfrage. Originalspannen, Methodenbesitzer und
geschriebene Typen bleiben erhalten; keine Quelltextumschreibung, Typinferenz,
Allokation, I/O oder Worker. Strikter Compiler und bisherige Präfix-API behalten
ihre Fehler- und AST-Verträge.

## Ausgeführte Prüfung

[Originalhashes und maschinenlesbare Ergebnisse](module-signatures-2026-11-05.json):

- GCC C17 `-Wall -Wextra -Werror`, ASan/UBSan und Leakprüfung: 13 Syntax-/Namespace-
  Fälle, 70 Präfix-/Kopfvergleiche und drei beschädigte Namespace-/Importköpfe.
- Windows Release/MSVC und Linux Debug/GCC: erster gemeinsamer Parserstand je
  **17/17 Daten- und 6/6 GUI-Gruppen**. Bestehende 294 AST-/Präfixvergleiche und
  C-/Physim-Aufruf-/Variablenprüfungen bestanden.
- Nach der zusätzlichen, ausschließlich in der neuen Dokumentabfrage wirksamen
  Fortschrittssicherung je **5/5 gezielte Datenprüfungen** am endgültigen Stand:
  neue Abfrage, bisherige Signaturen/AST, Aufrufziele, strikter Parser und
  Variablenbereiche. Die GUI verwendet die neue Abfrage noch nicht; deren Matrix
  wurde für diese Sicherung nicht wiederholt. Apphashes/Quellstände sind getrennt.
- Vier tatsächlich kompilierte Gegenfälle scheitern gezielt: alte API mit reinem
  Namensadapter, fehlender Zeilenbesuch, fehlender Besitzer und verschlucktes
  Ressourcenlimit. Keine Sanitizer-Unterdrückung.
- Die erste neue Wiederaufnahme überschritt bei einem beschädigten Struct-Kopf
  drei Sekunden. Der begrenzte Gegenlauf wurde beendet und das Fehlen eines
  verbleibenden Linux-Prozesses geprüft. Für diesen erzwungen beendeten Fall wird
  kein sauberer LSan-Abschluss behauptet; die korrigierte Abfrage besteht alle
  drei entsprechenden Fälle regulär.

Finaler Snapshot `module-signatures-final`: 288 eigene / 1.227 Compiler-/Test-
Eingaben, alle **1.246** verschiedenen Eingaben gegen tatsächliche Git-Index-Blobs
gelesen. Alle referenzierten Archivbytes geprüft; lokale Prozesse beendet.
34 neue BMPs waren bytegleich mit erhaltenen Referenzen: **108.695.892 Bytes**
freigegeben. Keine neue Bildserie; Fehler, Originalberichte und Referenzen bleiben.
Details unter `build/validation/module-signatures-2026-11-05/`.

## Noch offen

Aktueller Mac-Quellnachlauf steht aus; der Workflow enthält jetzt acht Daten-
Units vor acht GUI-Gruppen. Der vorherige C-Schritt `f3ed4068` ist getrennt auf
ARM/Intel [je 7/7 + 8/8 mit Originalarchiven abgeschlossen](c-signatures-macos-2026-11-05.json).
Keine gesamte UI-/Plattform-/Performance-/physische Abnahme. Nutzer-Mac aus.

Als Nächstes beide Sprachdaten mit der tatsächlichen API-/Benutzerauflösung und
der unabhängigen Parameteranzeige verbinden. Insbesondere C-Überdeckung im
eigenen unvollständigen Initialisierer darf nicht auf einen gleichnamigen
API-Eintrag zurückfallen. Alle Interaktionen und Darstellungsanforderungen aus
ADR 023 bleiben erhalten. Vor dem nächsten UI-Ausbau folgen auf Nutzerwunsch
Dokumentationspflege und eine geprüfte Bereinigung ersetzter Artefakte auf D:.
