# D09: erste Popup-Runde · A/B/C verworfen

**Aktualisierung vom 30. Oktober 2026:** Der Nutzer hat alle drei Entwürfe
verworfen und automatische Anzeige beim Tippen beauftragt. Es gibt eine
[neue Runde D/E/F](../completion-typing-options/README.md). Diese Seite bleibt
Vergleichshistorie, keine erneute Wahl von A/B/C. [ADR 022](../../decisions/022-automatic-completion.md).

Historischer Ausgangspunkt: Auf Nutzerauftrag folgt nach dem
[Bestandsabgleich der Suche](../search-options/README.md) der Punkt
„Vervollständigung und Hilfe direkt am Code“. Die vorhandenen Funktionen werden
weiterverwendet. **Nur das Vorschlags-Popup wird verglichen**, keine neue
API-Karte, F1-Hilfe oder Suchinsel. A/B/C nicht ins fertige Design übernehmen.

[Vorschau öffnen](index.html?variant=a&scenario=normal&theme=dark) ·
[A · Kompakt](index.html?variant=a) · [B · Liste & Kurzinfo](index.html?variant=b) ·
[C · Info in der Zeile](index.html?variant=c).
Server: `http://127.0.0.1:8791/completion-options/`.
Auch als lokale Datei verwendbar; keine externen Fonts, Netzdienste oder Abhängigkeiten.

## Was schon besteht

| Bestand | Beleg | Konsequenz für diese Runde |
| --- | --- | --- |
| API-Vervollständigung für C/Physim | [completion.c](../../../app/completion.c), [completion.h](../../../app/completion.h), [completion_ui.inc](../../../app/completion_ui.inc) | Vorhandener Katalog, Präfixsuche, Sprach-/Memberregeln, Einfügen und Tastatur sind die Grundlage. |
| Signatur und Referenzbutton im Popup | [completion_ui.inc](../../../app/completion_ui.inc), `completion_view` | Keine neue Vervollständigungsengine oder neue API erfinden. |
| Umschalt+F1 am aktiven Editor | [main.c](../../../app/main.c), `SDLK_F1`/`api_reference_from_editor` | Bereits vorhandener direkter Referenzzugang; in der Studie ebenfalls erreichbar. |
| API-Karte und dokumentiertes Beispiel | [WORKBENCH-CONTEXT.md](../unified-workspace/WORKBENCH-CONTEXT.md), [workbench_api_ui.inc](../../../app/workbench_api_ui.inc) | Gestaltung behalten. Nur ausdrücklich markierte Beispiele sind dort einfügbar; Signatur ist kein Beispiel. |
| F1-Hilfe H | [verbindliche Spezifikation](../f1-layouts/DESIGN-H.md) | Bestehendes Design und echte Symbol-/Abschnittsbindung weiterverwenden. |
| Referenzladen und Fehlererhalt | [api_reference_ui.inc](../../../app/api_reference_ui.inc) | Bisheriges Referenzdokument bei Fehler erhalten; kein fiktiver Erfolg. |
| Unicode-/Undo-Abläufe | [completion_tests.inc](../../../app/completion_tests.inc) | Bestehende Dokumentziele, Byte-/Rune-Konvertierung, Undo/Redo und Dirty-Regeln erhalten. |

Die echte offene Designfrage ist die Informationsdichte **im Popup**: kompakte
Signatur, Kurzbeschreibung, vorhandene Parameterhinweise, lange Inhalte und
Popup-Randfälle. Die vorhandene API-Karte und das Handbuch nicht erneut auswählen.

## Drei Richtungen

| Variante | Aufbau | Bedienung |
| --- | --- | --- |
| A · Kompakt | Schmale Liste, gemeinsame Signatur darunter; Kurzinfo/Referenz aufklappbar | Hauptsächlich Namen ergänzen. Beschreibung nur bei Bedarf. |
| B · Liste & Kurzinfo | Liste links, Signatur/Beschreibung rechts; bei wenig Breite untereinander | Bedeutung beim Navigieren sehen. Beide Bereiche behalten eigenen Scrollzugang. |
| C · Info in der Zeile | Aktiver Eintrag zeigt Signatur und Kurztext direkt unter seinem Namen | Ein Lesefluss. Pfeile ändern die ausführliche Zeile; Maus-Hover klappt nichts um und verschiebt keine Klickziele. |

In C liegt der Referenzbutton im gemeinsamen Fuß **außerhalb der Listbox-Optionen**.
Lesen/Klicken in der Kurzinfo fügt keinen Namen ein. Klick auf einen Namenseintrag
fügt wie im vorhandenen Ablauf unmittelbar ein. A/B können per Hover die aktive
Kurzinfo wechseln, ohne den Text zu verändern.

Alle Varianten verwenden dieselbe Logik, echten Katalognamen und dieselben
Verfügbarkeiten. Wechsel von Variante/Theme erhält Text und Auswahlzustand.
Szenariowechsel/Zurücksetzen lädt ausdrücklich neue Beispiele.

## Katalog und Referenzdaten

[catalog.js](catalog.js) enthält **1.264 Einträge** aus dem lokalen
[completion_symbols.inc](../../../app/completion_symbols.inc): Namen, Module,
Signaturen, Funktions-/Typ-/Wertrolle und Sprach-/Instanzflag bleiben unverändert.
Der deterministische [Generator](../../../tools/build_completion_preview_data.py)
übernimmt zusätzlich den ersten geeigneten Absatz und bis zu zwei vorhandene
Argumenthinweise aus den lokalen Modulreferenzen. Quellhashes stehen im Asset.

```powershell
python tools/build_completion_preview_data.py --check
```

Die eingefrorenen Kurztexte sind echte Dokumentauszüge, kein generiertes
API-Wissen. Die Extraktion ist eine **begrenzte Fixture-Aufbereitung**, kein Ersatz
für den nativen Markdownparser oder dessen Typ-/Sprach-Fallbacks. Fehlende
Fixture-Kurzbeschreibung ist kein Grund, echte native Funktionen abzuschalten.
Die vorhandene API-Karte verwendet weiterhin ihr geladenes Originaldokument.

Die Studie bildet die gelesenen lexikalischen Regeln nach: ASCII-Bezeichner,
Präfix bis zum Cursor, gesamter Bezeichner als Ersetzungsbereich, Sprache,
Kommentare/Zeichenketten, 127-Byte-Präfixgrenze und Physim-Member. Ein bekannter
statischer Empfänger begrenzt auf passende statische Einträge; ein unbekannter
Instanzname ist **keine semantisch ermittelte Variable mit bewiesenem Typ**.
Keine LSP-, KI-, Projektindex-, Überladungs- oder AST-Typanalyse behauptet.

## Ausprobieren

1. Die Galerie startet mit einem ausdrücklich geöffneten Beispiel-Popup. Nativ
   bleibt Strg+Leertaste bzw. die vorhandene Aktion der Öffnungsweg. Linksauswahl
   öffnet weder dieses Popup noch eine Aktionsleiste automatisch.
2. Code fokussieren: Auf/Ab wählen, Pos1/Ende springen in der Studie zum ersten/
   letzten Vorschlag. Enter/Tab oder Namensklick ersetzt genau den Bezeichner.
   Beispiel: `ps_vlOLD` am Cursor nach `ps_vl` wird `ps_vlength`, ohne `OLD`.
3. Bei `direction.normOLD` wird nur der Member ergänzt; `direction.` bleibt.
   `Mat4.transOLD` wird `Mat4.translation`. Keine Klammern/Argumente oder
   vollständigen Signaturen als Code einfügen. No-op-Einfügen erzeugt kein Undo.
4. Eine Änderung markiert ausschließlich die richtige Beispieldatei als geändert.
   Rückgängig/Strg/⌘+Z stellt Text wieder her. Kein Speichern, Kompilieren oder
   Ausführen. Andere Dokumententwürfe bleiben beim Tabwechsel erhalten.
5. Escape schließt mit Fokus zum Code. Cursor-/Textauswahl-/Dokumentwechsel,
   tatsächlicher Codescroll, Außenklick oder Fenster-Fokusverlust verwerfen das
   Popup. Alte Aktionen dürfen weder alten Text noch eine inzwischen andere Datei ändern.
6. „Referenz öffnen“ oder Umschalt+F1 erzeugt eine **Demo-Rückmeldung** mit exakt
   gewähltem Symbol und Referenzpfad. Nativ dieselbe vorhandene API-Karte öffnen;
   deren Handbuchaktion führt nach F1. Der separate Link hier zeigt nur die
   vorhandene F1-Designreferenz mit deren eigenen Daten, keinen echten Symbolsprung.
7. „Signatur & Parameterhinweise“ zeigt `ps_channel_add` mit tatsächlich
   dokumentierten Hinweisen für `context` und `unit`. Keine Signatur als Beispiel
   einfügen und keinen aktuellen Aufrufparameter oder Stack aus der Syntax erraten.

## Zustände und Grenzen

| Zustand | Darstellung und Aktion |
| --- | --- |
| Normal / langer Name und Signatur / Typ | Reale Vorschläge; Namen umbrechen, Signaturen vollständig scrollbar, Rolle durch Symbol und Text. |
| Instanz-/statischer Member | Sprache und Empfänger sichtbar; nur Blattname wird eingesetzt. |
| Keine Vorschläge / Kommentar oder Zeichenkette | Popup geschlossen, ruhiger lokaler Hinweis, Text unverändert. Kein leerer Kasten oder erfundener Vorschlag. |
| Nur lesen | Einfügen/Öffnen gesperrt, Erklärung am Code. Native Berechtigungen und tatsächliche Ziele prüfen. |
| Referenz fehlt | Signatur aus Katalog und Namenseinfügen bleiben. Referenzaktion gesperrt, bestehende native Karte bei tatsächlichem Fehler erhalten. |
| Referenz lädt | **Nur ein gekennzeichneter Zustandsentwurf** für Zusatzinformationen. Katalog und Einfügen bleiben sofort verfügbar. |
| Cursor am unteren Rand | Popup klappt oberhalb auf; bei kleinem Raum begrenzen und intern scrollen. |
| Ziel-/Versionsverlust | Schließen; aufbewahrte Einfüge-/Referenzaktionen sind wirkungslos. |

Der vorhandene Katalog ist lokal und synchron; auch `api_reference_open` lädt im
gelesenen Bestand synchron. Diese Runde beauftragt **keinen neuen asynchronen
Provider oder Worker**. Der Ladefall ist eine visuelle Zustandsstudie, die über
einen Galerie-Prüfknopf beendet wird. Kein automatisch laufender Hintergrundjob.

Die beiden Dokumente sind Browser-Fixtures. Kein Dateisystem-/Systemclipboard-
Zugriff, kein Local Storage, keine echte Physim-Ausführung. Neuladen setzt zurück.
Bytepositionen nativ explizit in Rune-/Glyphpositionen umrechnen; JavaScript-
Textarea-Bereiche sind UTF-16-Positionen. Gültigkeit vor Einfügen erneut prüfen.

## Darstellung, Fokus und Übergabe

Gemeinsame Werkbankrollen und [A2](../selection-options/APPROVED.md) erhalten.
Aktive Listenzeile mit dezenter Fläche/Kante; keine Textauswahl am Präfix
vortäuschen. Zeichen behalten ihre volle Deckkraft. Namen, Rollen, aktive Option,
Status und Referenzzugang semantisch publizieren. Echtes Screenreaderverhalten
und native Popup-Verdeckung nach späterer Integration prüfen.

Breiten: A 410, B 730, C 510 logische Einheiten, jeweils auf Editorbreite minus
16 begrenzt; Radius 9, Rahmen 1. Höhe an Cursorplatz und Editor anpassen, oberhalb
oder unterhalb; kein fremdes Panel überdecken. Details/Listen bleiben scrollbar.
Schmale Fenster, große Schrift, Hell/Dunkel, hoher Kontrast und reduzierte
Bewegung sind Teil der Prüfung. Vollständige Werte in [completion.css](completion.css),
gemeinsame Rollen aus [menus.css](../context-menu-options/menus.css).

**A/B/C sind verworfen.** Die neue Runde führt automatische Anzeige und
Optik D/E/F fort. Nach deren Nutzerentscheidung genaue Popup-Geometrie/Bedienung,
native Daten-/Ziel-/Versionsbindung und Referenzen im
[Gesamtauftrag](../unified-workspace/NEXT-CHAT.md) festschreiben. Bestehende
Suche, API-Karte, F1 H, A2, Kontextmenüs A und alle anderen Freigaben erhalten.
Kein neuer aktiver-Parameter-Renderer oder anderer Editorauftrag aus der Studie ableiten.

D10 ist der nächste Listenpunkt. Vor einer neuen Explorer-Runde dessen bereits
umgesetzte Inhalte, Vorschauen und API-Kontext abgleichen; keinen bestehenden
Explorer-A- oder F1-Entwurf erneut zur Wahl stellen.

[Prüfbericht](review/VALIDATION.md) · [Maschinenbericht](review/validation.json).
Browserbelege bestätigen keine native Windows-/Linux-/macOS- oder reale
Screenreaderabnahme. Die native Integration bleibt bis zur Auswahl getrennt.
