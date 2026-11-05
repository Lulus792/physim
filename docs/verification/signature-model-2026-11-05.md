# Gemeinsamer Datenkern der Parameterhilfe

Stand: 5. November 2026. Der C-/Physim-Modellkern ist nativ gebaut und geprüft.
Er ist **noch nicht mit dem sichtbaren Parameter-HUD verbunden**. Die vollständige
UI-/API-Abnahme bleibt offen; insbesondere sind Spezialverträge noch auszubauen.

`ps_signature_help` besitzt seine Quelle, Modulmetadaten und Anzeigetexte selbst.
Es übernimmt den tatsächlichen Aufrufpfad des bestehenden Locals-Modells und
leiht dessen unveränderlichen C-Snapshot ausschließlich während der Abfrage aus.
C-Ziele werden am ursprünglichen Callee-Namensort aufgelöst, nicht am Cursor in
einem inzwischen inneren Argumentbereich. Gefundene gewöhnliche Deklarationen
verhindern einen falschen Rückfall auf gleichnamige API-Funktionen.

Für Physim verwendet das Modell die echten Modul-/Präfixsignaturen und Bindings.
Lokale Funktionen, spätere Modulfunktionen, Methoden mit tatsächlicher schriftlicher
Empfängerannotation, `self` und statische Methoden sind getrennt berücksichtigt.
Ein unbekannter oder mehrdeutiger Aufruf erzeugt keine erfundene Signatur.
Generische Namen bleiben die geschriebenen Namen; Initialisierer werden nicht
zur Typbestimmung ausgeführt oder nachgeahmt.

Formale API-Metadaten werden aus dem vorhandenen Compilerregister erzeugt und
zusätzlich gegen dessen tatsächlich kompilierte Typen, Labels, Empfängerslots und
Rückgabetypen geprüft. Beispielnotation wie `Array(repeating: value, count: n)`
wird nicht als Typbeweis für `value` und `n` verwendet. Der bestehende
Completion-Katalog und die erzeugte Sprachreferenz behalten ihre bisherigen Bytes.

## Lebenszeit und vollständige Parameter

Gleicher Owner, Sprache, Quelltext und Aufrufort erlauben die Wiederverwendung
des Ergebnisses. Ein Cursorwechsel zwischen Argumenten desselben unveränderten
Aufrufs benötigt im Modell keine weitere Allokation oder Parseabfrage. Innere
Aufrufe und andere Gültigkeitsbereiche lösen erneut auf. Ein ungültiger Präfix
blendet das Ergebnis aus; Rückkehr an eine gültige Stelle stellt es wieder her.
Änderungen an Quelle, Owner oder Sprache verwenden diese alte Auflösung nicht.

Alle Parameter bleiben erhalten, unabhängig von den fünf sichtbaren Completion-
Zeilen. Überzählige Argumente, variadische Enden und unbestimmte leere C-Prototypen
markieren keinen falschen letzten Parameter. Fehlende alte C-Typannotationen und
unbenannte Parameter bleiben als solche erkennbar. Speicherfehler erhalten den
vorherigen Besitzstand; die UI muss zusätzlich den Bezug zur aktuellen Eingabe
prüfen und veraltete Ergebnisse verbergen. Anzeigen sind auf 1 MiB begrenzt,
Quell-/Parsergrenzen entsprechen den vorhandenen Editorabfragen.

## Tatsächliche Nachweise

Prüfanlass: neues Modell, API-Datengenerierung und App-Buildintegration. Windows
Release und Linux Debug mit ASan/UBSan bestehen zunächst je **10/10 Datenprüfgruppen**
samt App-Link. Anschließend wurde der belegte erneute Parse-/Allokationspfad beim
Cursorwechsel durch den Cache ersetzt und die neue Modellgruppe auf beiden
Plattformen gezielt erneut bestanden. Bestehende unveränderte GUI-Prüfungen wurden
nicht wiederholt; es entstanden keine neuen Bildschirmbilder.

38 Verhaltensfälle, vollständige 16-Parameter-Funktionen beider Sprachen,
verschachtelte Ziel-/Bereichswechsel und ungültige Präfixe sind geprüft. Dazu
**60 C-, acht Physim- und vier weitere Fehlerpfade mit geliehenem Quell-/Modulcache**:
Allokation tatsächlich abgewiesen, alter Zustand erhalten, anschließend erfolgreich
wiederholt. Der vollständige formale Katalogdurchlauf löst **370 C- und 396 Physim-
Aufrufe** auf; die Empfängertypen der Prüfungen stammen aus erfolgreichen echten
Compilerprüfungen. **190 Spezial-/Property-/Dokumentationseinträge** bleiben ausdrücklich
unaufgelöst. Fünf defekte Gegenprogramme werden sauber abgewiesen: verlorene
Maskierung, falscher Nachschlageort, falsche letzte Parametermarkierung,
erfundene Überladungswahl und erneutes Parsen bei unverändertem Aufruf.

Der neue Code samt Tests ist direkt strikt C17 mit `-Wall -Wextra -Werror`,
ASan/UBSan und aktiver Leak-Prüfung ausgeführt. Die dabei sichtbare vorhandene
positionsabhängige Capture-Initialisierung im Checker wurde auf gleichwertige
benannte Felder umgestellt; die Compiler-Regression ist Teil der nativen Matrix.
Originalstände, Befehle, Hashes, Grenzen und Bereinigung stehen einmal im
[strukturierten Nachweis](signature-model-2026-11-05.json).

Der volle Referenz-Linkcheck im Arbeitsverzeichnis scheitert bereits mit dem
unveränderten Generator an fremdem `docs/status.md`-WIP: Link auf die ungetrackte
interne Wartungsnotiz außerhalb der gebündelten F1-Themen. Vorher-/Nachher-Fehler
und identische erzeugte Sprachreferenz sind belegt. Der isoliert geprüfte eigene
Quellstand enthält diese fremde Änderung nicht; sie wurde nicht überschrieben.

## Noch zu erledigen

Den separaten HUD vollständig nach ADR 023 anschließen: aktuelle Syntaxrollen,
aktive Parametermarkierung, Umbruch, Position zur Liste, unabhängiges Schließen,
Strg+Umschalt+Leertaste, Escape, Fokus, IME, Auswahl, Revision und Accessibility.
Die 190 Einträge nach echten Spezialaufrufen, Überladungen/Optionen und nicht
aufrufbaren Eigenschaften unterscheiden. Compilerintrinsics außerhalb des Katalogs
wie Konvertierungen/`print`/`assert` gesondert aus tatsächlichen Verträgen erschließen.
Funktionswert-Annotationen und Import-/Aliasfälle sind ebenfalls noch keine volle
Auflösung. Schriftlich nicht bekannte Empfängertypen werden nicht geraten.
Ausgelassene Rückgabeannotationen beim HUD nach der tatsächlichen Sprachregel
kennzeichnen. Große Textänderungen und vollständige UI-/physische Abnahme bleiben offen.
