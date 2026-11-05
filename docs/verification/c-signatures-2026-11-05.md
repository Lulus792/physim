# Geschriebene C-Funktionssignaturen

Stand: 5. November 2026. Lokaler Daten- und Regressionsschritt abgeschlossen. Voraussetzung
für die vollständige Parameterhilfe nach ADR 023; keine bereits sichtbare neue
Signaturanzeige und keine fertige API-/Benutzerfunktionsauflösung.

## Daten und Grenzen

`app/c_signatures.h` und `app/c_signatures.inc` projizieren echte vollständige
Deklaratoren aus dem bestehenden unveränderlichen C-Grammatik-Snapshot. Der
Aufruf `ps_c_bindings_source_signatures` parst nicht erneut. Es entstehen keine
Worker, Präprozessor-/Includeauswertung oder erfundenen Typen/Werte.

Die Sammlung unterscheidet Funktionen und Prototypen, geschriebene einfache
Funktionszeiger und aufrufbare Parameter wirklicher Definitionen. Bei Funktionen,
die selbst Funktionszeiger zurückgeben, gehört die Parameterliste zur tatsächlich
aufgerufenen Funktion. Der vollständige ursprüngliche Deklarator bleibt erhalten.
Felder, Zeiger auf Zeiger, Arrays und Typaliasauflösung werden nicht zu direkt
aufrufbaren Funktionen umgedeutet; innere Prototypparameter werden nicht zu lokalen
Namen. Grammatikdaten bestätigen keine Compilerakzeptanz beliebiger C-Programme.

Parameternamen, geschriebene Typen und Headerpositionen bleiben Originalspannen;
unbenannte Parameter bleiben unbenannt. Ein einzelnes schlichtes `void` bedeutet
null Parameter. Ein leerer Prototyp `f()` hat unbekannte erwartete Parameter und
wird nicht als `f(void)` ausgegeben. Ellipsis ist vom festen Parameterbestand
getrennt. Alte C-Definitionen erhalten Typen nur aus tatsächlichen eindeutigen,
gleich geschriebenen Typdeklarationen; fehlende oder doppelte Annotationen bleiben
unbekannt, kein erfundenes `int`. Keine Normalisierung alternativer UCN-Schreibweisen.

Sichtbarkeit enthält Modul-, Block- und For-Bereiche, Deklarationsreihenfolge und
den tatsächlichen Definitionsrumpf für Callback-Parameter. Ein halboffenes
Blockintervall enthält den Cursor direkt vor `}`, endet nach diesem Zeichen und
schließt den Cursor dahinter aus. Nur Modul-EOF/noch offene Bereiche schließen ihr
EOF ein. Alte Parameterannotation-Deklarationen werden auch bei einem beschädigten
erkennbaren Funktionskopf privat gehalten und nicht als globale Callbacks angeboten.

Scratch bleibt je Abfrage privat und wird auf jedem Fehlerpfad freigegeben. Der
Snapshot bleibt unverändert und weiter benutzbar. Anzahlabfragen sind möglich;
zu kleine Ausgabepuffer melden alle erforderlichen Anzahlen, keine stillschweigend
gekürzte Signatur. Andere Fehler liefern keine verwendbaren Teilsammlungen.

## Prüfungen und tatsächlich gefundene Fehler

Der strenge GCC-C17-Direktlauf mit `-Wall -Wextra -Werror`, ASan/UBSan und aktiver
Leakprüfung besteht mit 33 Deklarationsfällen, 205 Präfixen über jede Byte-Grenze,
775 Cursor-/Namensvergleichen und drei injizierten Scratch-Allokationsfehlern.
Originale unter `build/validation/c-signatures-2026-11-05/`.

Die Scope-Gegenprüfung verwendet die vorhandene Variablensammlung: dieselbe echte
Grammatik/Strukturgrundlage, eine getrennte Sichtbarkeitsauswertung. Sie fand den
zu frühen Bereichsabschluss direkt vor `}`. Die Rekonstruktion einer unvollständigen
Kommaliste fand außerdem einen falschen Basis-Typbereich, der die frühere
Deklaration enthielt. Ein zusätzlicher beschädigter alter Funktionskopf wies die
falsche globale Veröffentlichung seiner Callback-Annotation nach. Alle drei
Produktionsursachen sind korrigiert; ursprüngliche Befunde bleiben erhalten.

Beim ersten absichtlich scheiternden Scope-Test wurde der Test-Snapshot beim
frühen Rücksprung noch nicht freigegeben. Der zusätzliche LSan-Befund betraf diesen
Testpfad. Die gezielten Negativmodi geben den Snapshot vor der erwarteten Ablehnung
frei; Leakprüfung bleibt aktiv. Keine pauschale Sanitizer-Unterdrückung.

Der endgültige Stand besteht unter Windows Release/MSVC und Linux Debug/GCC mit
ASan/UBSan jeweils **Unit 12/12 + GUI 6/6**. [Ergebnisse, Befunde und Originalhashes](c-signatures-2026-11-05.json).
Der erste native Stand bestand ebenfalls 12/12 + 6/6; die danach zusätzlich
konkret reproduzierte Korrektur für beschädigte alte Funktionsköpfe war der Anlass
für den zweiten Lauf. Beide Quellstände sind getrennt gesichert. Vier tatsächliche
Negativprogramme weisen Scope-, Kommalisten-, falsche Nullparameter- und alte
Annotationsfehler gezielt ab, mit aktiver ASan/UBSan-/Leakprüfung und sauberem
Aufräumen ihrer Test-Snapshots.

Finaler Snapshot `c-signatures-final`: 287 eigene / 1.226 Compiler-/Testeingaben,
alle **1.245** verschiedenen Eingaben gegen tatsächliche Git-Index-Blobs gelesen.
Alle referenzierten Archivbytes geprüft; keine laufenden lokalen Prozesse mehr.
68 neue Test-BMPs aus beiden Phasen waren bytegleich mit geschützten Referenzen:
**217.391.784 Bytes** freigegeben. Originalfehler, Berichte, Referenzen, Quellen und
benötigte Programme erhalten; keine SDK-/VM-/Fremddatenbereinigung. Keine neue
Bildserie und keine neue visuelle/physische Gesamtabnahme.

Neuer gehosteter Mac-Nachlauf zu dieser C-Quelle steht noch aus. Der Workflow
konfiguriert sieben Daten-Units vor acht GUI-Gruppen; das ist kein Laufnachweis.

## Anschluss

Anschließend tatsächliche API-/Benutzersignaturen beider Sprachen mit Scopes,
Überdeckungen und ehrlichen unbekannten/mehrdeutigen Ergebnissen verbinden. Der
vollständige unabhängige native HUD nach ADR 023 bleibt beauftragt: Syntaxrollen,
aktive A2-/Fett-/Unterkantenmarkierung, innere und überzählige Argumente,
Automatik/Strg+Umschalt+Leertaste, unabhängige Liste/Escape, Fokus/IME/Revision und
Barrierefreiheit. Ebenso offen: große Textänderungen und übrige freigegebene UI.

Der vorherige Physim-Schritt `7a3e7950` ist [auf gehostetem ARM und Intel mit je
Unit 6/6 + GUI 8/8 abgeschlossen](signature-declarations-macos-2026-11-05.json).
Er beweist diese neue C-Projektion nicht. Nutzer-Mac aus und unberührt; nur D:.
