# Vervollständigung D · nativer Umsetzungsstand

Maßgeblich bleiben [ADR 022](../../decisions/022-automatic-completion.md),
[ADR 023](../../decisions/023-selected-completion-d.md) und die
[vollständige Freigabe](../completion-typing-options/APPROVED.md).
**D ist noch nicht vollständig nativ umgesetzt.** Die Eingabegrundlage ist
ergänzt; der vorhandene Viererlisten-Renderer ist noch keine D-Designabnahme.

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

## Weiterhin verbindlich umzusetzen

1. Fünf sichtbare Einträge als Standard, alle weiteren Treffer scrollbar erhalten;
   verfügbare Höhe und große Schrift berücksichtigen. Eine echte schmale
   Scrollleiste, flache D-Zeilen, Rollenicons, Typen und Gesamtzähler ergänzen.
2. Automatische Vorschläge und sichtbare Trefferzahl in Einstellungen A / Editor
   mit echter Preferences-Speicherung, Migration, Übernehmen/Abbrechen/Defaults
   integrieren. Der bisherige PSPREF05-Bestand enthält diese Felder noch nicht.
3. Gemeinsame freigegebene Syntaxrollen und Palette für Editor, Liste, neutrale
   Kandidateninfo und Aufrufsignatur, insbesondere gelbe Funktionen. Den bisherigen
   pauschalen `ps_`-Typansatz ersetzen, keine getrennte Kurzinfo-Palette einführen.
4. Tatsächliche C-/Physim-Deklarationen und Scopes anbieten: sichtbare Variablen,
   Parameter und Quellstellen, Shadowing, Schleifen-/Block-/Funktionsgrenzen,
   Ausschluss späterer Deklarationen und der eigenen unvollständigen
   Initialisierung. Der Browser-Demo-Lexer ist kein nativer Scope-Backend-Ersatz.
5. Eine eigene Aufrufsignatur einschließlich verfügbarer Benutzerfunktionen,
   aktueller Argumentposition, verschachtelter Aufrufe und ehrlichem Überlauf.
   Strg+Umschalt+Leertaste sowie getrennte Escape-Zustände umsetzen; Übernahme
   eines Variablennamens lässt die Aufrufsignatur bestehen. Leere Argumente
   sollen ebenfalls sichtbare Namen anbieten. Kein Typinferenzversprechen.
6. Anfragen und spätere Scope-/Signaturdaten an tatsächliche Dokumentrevisionen
   binden. Echte IME-, Screenreader-, DPI-, Monitor- und Plattformprüfung sowie
   D-Vergleich mit der freigegebenen HTML-Referenz abschließen.

Keine dieser offenen Anforderungen wird durch den Eingabetest oder vorhandene
Browserbelege als fertig erklärt. Die übrigen Designfreigaben bleiben erhalten.
