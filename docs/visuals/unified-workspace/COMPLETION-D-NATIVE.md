# Vervollständigung D · nativer Umsetzungsstand

Maßgeblich bleiben [ADR 022](../../decisions/022-automatic-completion.md),
[ADR 023](../../decisions/023-selected-completion-d.md) und die
[vollständige Freigabe](../completion-typing-options/APPROVED.md).
**D ist noch nicht vollständig nativ umgesetzt.** Die Eingabegrundlage ist
ergänzt. Fünfer-Viewport und wirksame Editor-Preferences sind als weiterer
Teilstand ergänzt. Gemeinsame Syntaxrollen und Funktionsfarben sind inzwischen
ebenfalls verbunden; eine vollständige D-Designabnahme steht weiterhin aus.
Eigener Dokumentbesitz, echte Kurzbeschreibung und vorhandene Argumenttexte sind
nun ergänzt. Anzeigeverzögerung, sofortige Umschaltung und Tastatur-Scrollen sind
als weiterer geprüfter Teilstand verbunden; Assistive-Abnahme bleibt offen.

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
und den bisherigen Referenzzugang. Echte Kurzbeschreibung und vorhandene
Argumenttabellen sind inzwischen verbunden; keine Aufrufhilfe daraus ableiten. Bei Platzmangel
kann die Karte seitlich schmaler werden.

Einstellungen A / Editor enthält „Vorschläge beim Tippen“ und „Sichtbare
Vervollständigungen“ (1–20, Standard 5). Ausschalten deaktiviert Vormerkung und
automatische Anzeige; Strg+Leertaste bleibt wirksam. PSPREF06 speichert die
Werte, liest 01–05 weiterhin mit den neuen Defaults. Feldweises Zusammenführen,
Änderungsprüfung, Übernehmen, Abbrechen und Standardwerte verwenden denselben
Vertrag wie die bisherigen Einstellungen. Geometrie und Nutzdaten bleiben
unabhängig. [Ausführungsstand, Gegenfall und Grenzen](../../verification/completion-d-2026-11-04.md).

## Gemeinsame Syntaxrollen: Anschluss vom 4. November 2026

Editor, Vorschlagsnamen und Rollenkennzeichen, neutraler Kartentitel und Signatur
sowie Codeblöcke der API-Karte verwenden dieselben zehn aktiven Rollen. Funktionen
sind dunkel `#dcdcaa`, hell `#795e26`; Typen und Variablen werden getrennt. Der
pauschale `ps_`-Typansatz ist entfernt. Kennzeichen sind `ƒ`, `T` und `#`; das
Präfix nutzt echte vorhandene Semibold-Glyphen. Zeilenhöhen berücksichtigen auch
die Codeschrift. Die neutrale Karte und Referenzzugang bleiben erhalten.

Exakte Katalogidentitäten, echter Physim-Lexer und gemeinsame Compiler-Typregistry
bilden die lexikalische Grundlage. Der vorhandene Dokumentumriss ergänzt
quellstandsgebundene Deklarationsrollen, ohne lokale Scope-Vollständigkeit oder
Typinferenz zu behaupten. C-Standardtypnamen sind ein lexikalischer Rückfall,
keine Include-Auflösung. F1-Codeblöcke und weitere Beispielkontexte sind noch
nicht allgemein migriert. Der vorhandene ASCII-Semiboldfont deckt die aktuellen
Katalogpräfixe; Unicode-Scopevorschläge brauchen später ihre eigene Abdeckung.
[Native Ausführung, reale Gegenfälle und genaue Grenzen](../../verification/syntax-roles-2026-11-04.md).

## Kandidateninhalt und Dokumentbesitz: Anschluss vom 4. November 2026

Die Karte besitzt eine eigene geparste Referenzseite, unabhängig von der geöffneten
Inspektor-Karte. Gleiche Seite wiederverwenden, neue Auswahl neu auflösen; bei
Fehlern keine Beschreibung des alten Eintrags zeigen. Listenschließen gibt den
Cache frei. Echte Zusammenfassung, Art, Katalogsignatur und Quellenpfad sind
verbunden. Sprachhandbuch-Fallbacks bleiben als solche erkennbar.

Alle vorhandenen Argumenttabellenzeilen bleiben erhalten, einschließlich ihrer
tatsächlichen Spaltenüberschrift. Beispielbezogene Texte werden nicht als volle
formale Parameterdokumentation ausgegeben. Die auf 260 Einheiten begrenzte Karte
scrollt vertikal mit 6-Einheiten-Leiste; kurze Inhalte erhalten keine unnötige
Leiste. Schriftgröße, Umbruch und unterer Abstand erhalten den erreichbaren
Referenzknopf. Fehlende Referenz wird lokal erklärt, der Knopf gesperrt; nach
Listenschließen ist ein neuer Ladeversuch möglich. Die Signatur bleibt sichtbar.

Mausinput der Karte erreicht keine darunterliegenden Dockgruppen. Nach der
Übernahme bleibt der Editor Tastaturbesitzer bis zum bewussten Fokuswechsel;
Quelle und Undo bleiben erhalten. [Native Ausführung, echte Fehler, dritte
Renderer-Prüfzeile und Gegenprobe](../../verification/completion-help-2026-11-04.md).

## Kurzinfo-Bedienung: Anschluss vom 4. November 2026

Erstes manuelles Öffnen zeigt die Liste mit Übernahmeauswahl, aber ohne Kurzinfo;
automatisches Öffnen bleibt ohne Übernahmeauswahl. Pfeile oder eine Mausbewegung
über einen anderen Eintrag starten 280 ms bis zur Kurzinfo. Hover allein erlaubt
keine Enter-/Tab-Übernahme. Mausrad und Griff verschieben nur den Listenausschnitt.
`i` und Strg+Leertaste im offenen Popup schalten die Info sofort, ohne die Auswahl
oder Übernahmebereitschaft zurückzusetzen. Ausblenden verwirft die alte Vormerkung.

Der Hinweis besitzt eine vollständige begrenzte Quellkopie und bindet sich an
Editor, Katalogidentität, Cursor und Editor-Scrollposition. Jede Änderung verwirft
die Vormerkung bzw. sichtbare Karte. Alt+BildAuf/BildAb und Alt+Pos1/Ende scrollen
die sichtbare Karte ohne Änderung am Code oder an der Listenauswahl. Tooltip und
zugänglicher Name nennen den Zugang. Umschalt+Tab verlässt das Codefeld ohne
Einrückung oder Übernahme. Kein zusätzlicher Worker oder Dienst.

Windows Release und Linux Debug ASan/UBSan jeweils Unit 1/1 und UI 4/4;
neue Bedienfolge in C/Physim und drei Editor-/Theme-Kontexten tatsächlich geprüft,
einschließlich gezeichneter Texte, versteckter alter Kartenfläche, schneller
Neuauswahl, Quelländerung außerhalb des Präfixes und Cursorabbruch. Der neue
Ablauf erzeugt keine Bilder. [Originalbefunde, Korrekturen und Grenzen](../../verification/completion-info-2026-11-04.md).
Dies ersetzt keine vollständige Screenreader-/physische Eingabe-/DPI-Abnahme;
echte lokale Scopes und separate Parameterhilfe bleiben offen.
Dies schließt die nachstehenden Interaktions-/Scope-/Aufrufhilfe-Gates nicht ab.

## Weiterhin verbindlich umzusetzen

1. Kurzinfo-Anzeige nach Auswahl verzögern, versteckte Trefferflächen sowie
   Tastatur-Scrollen und Assistive-Bedienung prüfen/ergänzen. Die gemeinsamen Rollen
   auch in die noch offene separate Aufrufhilfe übernehmen; keine getrennte Palette.
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
