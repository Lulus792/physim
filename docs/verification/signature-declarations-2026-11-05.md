# Geschriebene Physim-Funktionssignaturen

Stand: 5. November 2026. Voraussetzung für die vollständige unabhängige
Parameterhilfe nach ADR 023; noch kein sichtbarer HUD und keine fertige
C-/API-/Benutzerfunktionsauflösung.

## Änderung und Grenzen

`src/language/editor_signatures.h` und der echte Parser liefern vollständige
geschriebene Funktionsköpfe: Namen, Parameter mit originalen Typ-/Namensspannen,
Rückgabeannotation, eigene Generics, Methodenart und tatsächlicher Struct-/Enum-
Besitzer. Implizites `self` zählt nicht als Aufrufargument. Eine fehlende
Rückgabeannotation wird als fehlend gemeldet; keine Typ-/Wertinferenz.

Lokale Deklarationen tragen den wirklichen umschließenden Block, dessen Ende
an der ausgerückten Zeile und ihren Sichtbarkeitsbeginn. Modulnamen sind davon
getrennt. Die Verarbeitung veröffentlicht geprüfte Köpfe in der jeweiligen
Parserproduktion vor der Rumpfverarbeitung; beliebige partielle ASTs werden
nicht als gültig interpretiert. EOF-Präfixe dürfen vollständige Köpfe liefern,
andere Syntaxfehler und erschöpfte Parserressourcen liefern keine verwertbare
Sammlung. Zu kleine Ausgabepuffer melden die vollständigen benötigten Anzahlen;
abgeschnittene Ergebnisse müssen verworfen werden. Reine Anzahlabfrage möglich.
Quelle/Knoten/Ausgabe gehören dem Aufrufer; keine Allokation, I/O oder Worker.

Die offene C-Signaturprojektion muss tatsächliche Deklaratoren, Zeiger,
Prototypen und Scopes aus dem vorhandenen Grammatik-Snapshot verwenden. Danach
folgen API-/Benutzerauflösung und der gesamte native HUD mit automatischem und
manuellem Öffnen, aktiver Parameterdarstellung, inneren/überzähligen Argumenten,
Escape-/Fokus-/IME-/Revisionsbindung und Barrierefreiheit. Diese Aufgaben werden
durch die neue Datenschnittstelle nicht als erledigt erklärt.

## Prüfanlass

Die neue Parserbeobachtung verlangt Deklarations-/Grenztests sowie Regressionen
für Parser, Editor-Scopes und Vervollständigung. Kein erneuter Volltest von F1,
Protokoll oder Einstellungen allein wegen Kontextwechsel.

Der gehostete Vorlauf zu `c9b9c00f` ist [mit Originalarchiven erfasst](call-targets-macos-2026-11-05.json):
ARM Unit 4/4 + GUI 8/8, Intel Unit 4/4 + GUI 7/8. Die Intel-Kurzinfo-Prüfung
verlangte einen tatsächlich gerenderten Frame im alten/neuen Fristintervall.
Das kann bei langsamen Frames fehlen. Der Originalbericht protokolliert den
entscheidenden Boolean nicht getrennt; er beweist den Fehler in Stufe 15,
keine vollständige Ursachenisolierung aller denkbaren Prädikate.

Der neue Prüfer aktualisiert den tatsächlich wartenden Auftrag mit dessen alter
Frist und verlangt unverändert wartenden Zustand und neue Frist. Sichtbare
Prüfungen vor/nach der neuen Frist sowie Hover/Quellbindung bleiben erhalten.
Die bisherigen deterministischen Frist-Units bleiben bestehen. Keine Änderung
am Produktions-Timer und keine bloße Verlängerung eines Timeouts.

## Lokale Abnahme

Direkt GCC C17 mit `-Wall -Wextra -Werror`, ASan/UBSan und Leakprüfung bestanden:
Deklarationen, Methoden, Scopes, 13 EOF-/Fehlerfälle, Knoten-/Ausgabekapazitäten,
294 strikte AST-/Präfixvergleiche. Drei tatsächlich kompilierte defekte Varianten
(fehlende Köpfe, auslaufender Scope, verschluckte Ressourcenerschöpfung) scheitern
gezielt. Originale unter `build/validation/signature-declarations-2026-11-05/`.

Windows Release/MSVC und Linux Debug/GCC mit ASan/UBSan bestehen je **15
Unit-Gruppen und 6 GUI-Gruppen**. [Maschinenlesbare Ergebnisse und Originalhashes](signature-declarations-2026-11-05.json).
Der erste Windows-Lauf bestand 14/15: der neue AST-Test verglich zusätzlich
bedeutungslose Struktur-Padding-Bytes per `memcmp`. Der korrigierte Vergleich
aller Syntax-/Tokenfelder besteht mit **1.410 reinen Padding-Abweichungen** unter
Windows und null unter Linux. Die übrigen 14 Windows-Gruppen und die bereits
bestandenen 15 Linux-Gruppen wurden wegen dieser reinen Testkorrektur nicht
wiederholt; der neue Test bestand danach gezielt auf beiden Plattformen.
Die Appbytes bleiben über beide Phasen identisch. Linux-GUI 6/6 vor, Windows-GUI
6/6 nach der Testkorrektur; die geänderte Fristprüfung besteht C/Physim über je
drei Editorziele. Keine physische Tastatur-/IME-/Screenreader-Abnahme daraus.

Finaler Snapshot `signature-declarations-fields`: 284 eigene Eingaben, 1.223
Compiler-/Testeingaben, alle **1.242** verschiedenen Eingaben gegen tatsächliche
Git-Index-Blobs gelesen; alle referenzierten Archivbytes geprüft. Der zweite
Snapshot ändert ausschließlich den AST-Test. Alle lokalen Prozesse beendet.
34 neue BMPs waren bytegleich mit erhaltenen Referenzen: **108.695.892 Bytes**
freigegeben, keine neue Bildserie aufbewahrt. Berichte, Originalfehler, Referenzen
und benötigte Programme bleiben erhalten; keine SDK-/VM-/Fremddatenbereinigung.

Neuer gehosteter Mac-Nachlauf zu dieser Quelle bleibt offen: Workflow enthält
sechs Daten-Units und acht GUI-Gruppen. Ein grüner früherer Lauf oder diese
Konfiguration bestätigt die neuen Signaturen und die Testkorrektur nicht.

Nutzer-Mac ausdrücklich aus und unberührt. Nur D:; keine F:-Nutzung.
