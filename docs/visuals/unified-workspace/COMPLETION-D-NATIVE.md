# Vervollständigung D · nativer Umsetzungsstand

Maßgeblich bleiben [ADR 022](../../decisions/022-automatic-completion.md),
[ADR 023](../../decisions/023-selected-completion-d.md) und die
[vollständige Freigabe](../completion-typing-options/APPROVED.md).
**D ist noch nicht vollständig nativ umgesetzt.** Die Eingabegrundlage ist
ergänzt. Fünfer-Viewport und wirksame Editor-Preferences sind als weiterer
Teilstand ergänzt; eine vollständige D-Designabnahme steht weiterhin aus.

## Ergänzte Eingabegrundlage

- Nach zwei Präfixzeichen bzw. beim bestehenden Physim-Memberzugriff nach Punkt
  wird eine Anzeige für 140 ms nach der letzten Eingabe vorgemerkt. Die Anfrage
  besitzt eine Kopie der vollständigen Quellbytes, Editor und Cursorposition.
  Änderungen verwerfen diese Kopie, auch gleich lange Änderungen außerhalb des
  Präfixes. Reine Leerlaufframes starten keine zusätzliche Quelltextsuche.
- Automatisch geöffnete Vorschläge sind zunächst nicht zur Übernahme ausgewählt.
  Enter erzeugt weiterhin eine neue Zeile, Tab die vorhandene Einrückung.
  Pfeile, Strg+N/P oder Klick wählen ausdrücklich; erst dann übernehmen Enter/Tab.
  Strg+Leertaste behält seinen bisherigen manuellen Zugang mit Erstauswahl.
  Weitere Eingabe verwirft eine alte automatische Auswahl und eine vorgemerkte
  Übernahme des vorherigen Textstands. Automatische Listen schließen unter zwei
  Nicht-Member-Präfixzeichen; die manuelle leere Kataloganzeige bleibt erhalten.
- Escape unterdrückt das Popup für den aktuellen Bezeichner auch beim Weitertippen.
  Manuelles Öffnen überstimmt die Unterdrückung. Cursor-, Ziel-, Fokus- und
  Auswahländerungen verwerfen vorgemerkte Anfragen. Die bestehenden Dokument-
  Auswahl-/Schließaktionen setzen den Zustand ebenfalls zurück.
- SDL-Kompositionsereignisse pausieren Vorschläge und manuelles Öffnen. Erst
  bestätigte Texteingabe darf erneut eine Anzeige vormerken. Kommentare und
  Strings werden mit den bisherigen sprachabhängigen Kontextregeln geprüft;
  C-Zeilenkommentare mit Backslash-LF/CRLF werden jetzt korrekt fortgesetzt.
- Katalog, Sprach-/Memberregeln, Bezeichnerersetzung, Referenzzugang und Undo
  bleiben erhalten. Der Katalog ist synchron und lokal; kein neuer Dienst oder
  Provider-Worker. Automatisches Anzeigen schreibt keinen Namen in den Editor.

[Windows-/Linux-Prüfungen, Gegenbeispiele und Grenzen](../../verification/completion-auto-2026-11-04.md).

## Liste und Einstellungen: Anschluss vom 4. November 2026

Die flache Liste ist 400 logische Einheiten breit. Standardmäßig fünf ganze
Zeilen, bei weniger Treffern entsprechend weniger; der verfügbare Editorraum
begrenzt die Höhe. Die gesamte Ergebnismenge bleibt erhalten. Ein unabhängiger
Scrolloffset, Mausrad und ziehbarer 6-Einheiten-Thumb erschließen weitere Treffer;
Pfeilnavigation bringt die Auswahl ins Sichtfeld. Scrollen allein übernimmt
keinen Eintrag. Einzeilige Namen, rechte Typ-/Rückgabetypangabe und Gesamtzähler
sind ergänzt. Die getrennte neutrale Kandidatenkarte zeigt reale Katalogdaten
und den bisherigen Referenzzugang. Zusammenfassung, parameterbezogene Inhalte
und gemeinsame Syntaxglyphen sind dort noch zu ergänzen; keine Aufrufhilfe daraus
ableiten. Bei Platzmangel kann die Karte seitlich schmaler werden.

Einstellungen A / Editor enthält „Vorschläge beim Tippen“ und „Sichtbare
Vervollständigungen“ (1–20, Standard 5). Ausschalten deaktiviert Vormerkung und
automatische Anzeige; Strg+Leertaste bleibt wirksam. PSPREF06 speichert die
Werte, liest 01–05 weiterhin mit den neuen Defaults. Feldweises Zusammenführen,
Änderungsprüfung, Übernehmen, Abbrechen und Standardwerte verwenden denselben
Vertrag wie die bisherigen Einstellungen. Geometrie und Nutzdaten bleiben
unabhängig. [Ausführungsstand, Gegenfall und Grenzen](../../verification/completion-d-2026-11-04.md).

## Weiterhin verbindlich umzusetzen

1. Gemeinsame freigegebene Syntaxrollen und Palette für Editor, Liste, neutrale
   Kandidateninfo und Aufrufsignatur, insbesondere gelbe Funktionen. Den bisherigen
   pauschalen `ps_`-Typansatz ersetzen, keine getrennte Kurzinfo-Palette einführen.
2. Tatsächliche C-/Physim-Deklarationen und Scopes anbieten: sichtbare Variablen,
   Parameter und Quellstellen, Shadowing, Schleifen-/Block-/Funktionsgrenzen,
   Ausschluss späterer Deklarationen und der eigenen unvollständigen
   Initialisierung. Der Browser-Demo-Lexer ist kein nativer Scope-Backend-Ersatz.
3. Eine eigene Aufrufsignatur einschließlich verfügbarer Benutzerfunktionen,
   aktueller Argumentposition, verschachtelter Aufrufe und ehrlichem Überlauf.
   Strg+Umschalt+Leertaste sowie getrennte Escape-Zustände umsetzen; Übernahme
   eines Variablennamens lässt die Aufrufsignatur bestehen. Leere Argumente
   sollen ebenfalls sichtbare Namen anbieten. Kein Typinferenzversprechen.
4. Anfragen und spätere Scope-/Signaturdaten an tatsächliche Dokumentrevisionen
   binden. Echte IME-, Screenreader-, DPI-, Monitor- und Plattformprüfung sowie
   D-Vergleich mit der freigegebenen HTML-Referenz abschließen.

Keine dieser offenen Anforderungen wird durch den Eingabetest oder vorhandene
Browserbelege als fertig erklärt. Die übrigen Designfreigaben bleiben erhalten.
