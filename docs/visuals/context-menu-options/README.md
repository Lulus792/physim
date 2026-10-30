# D07: Kontextmenüs · A ist gewählt

Stand: **30. Oktober 2026 · A · Kompakt verbindlich ausgewählt.** Die getrennte
Auswahlphase ist abgeschlossen; A nativ mit umsetzen. Die vorherige Übernahmesperre
ist aufgehoben. [Spezifikation](APPROVED.md) und
[ADR 021](../../decisions/021-selected-context-menu-a.md) sind verbindlich.
B/C bleiben Vergleichshistorie; die [UI-Liste](../unified-workspace/UI-DESIGN-BACKLOG.md)
führt D08 als nächste neue Entwurfsrunde.

[Freigegebene Vorschau öffnen](index.html?approved=1&context=text&theme=dark) ·
[historischer Vergleich A](index.html?approved=0&variant=a&context=text) ·
[historisch B · Mit Kontext](index.html?approved=0&variant=b&context=message) ·
[historisch C · Mit Erklärung](index.html?approved=0&variant=c&context=multi).
Lokaler Server: `http://127.0.0.1:8791/context-menu-options/`.
Die Seite funktioniert auch als lokale Datei, ohne externe Fonts/Netzwerk.

## Gewählte Richtung und Vergleichshistorie

| Variante | Gestaltung | Zweck |
| --- | --- | --- |
| **A · Kompakt · gewählt** | Ruhige Liste, Trennlinien, Tastaturhinweise | Wenig Fläche, vertraute Reihenfolge |
| **B · Mit Kontext** | Zielkopf, beschriftete Gruppen, kleine Symbole | Deutlich machen, worauf der Befehl wirkt |
| **C · Mit Erklärung** | Zielkopf und kurze Erklärung je Befehl | Wirkung vor dem Ausführen verstehen |

Standardaufrufe und `approved=1` verwenden fest A, selbst bei altem B/C-Parameter.
Nur `approved=0` zeigt den historischen Variantenvergleich.
Alle Richtungen verwenden identische Verfügbarkeit, Zielbindung und Aktionen.
Die Variante ändert nicht die Befehlsmenge. Wechsel erhält bearbeiteten Text,
Beispielpuffer und ausgewählte Elemente. Ziel-/Zustandswechsel in der Galerie
lädt dagegen eine neue Fixture-Ausgangslage.

**A2 bleibt verbindlich:** [genaue Markierung und Bedienung](../selection-options/APPROVED.md).
Keine neue Farbvariante für Auswahl; Dunkel `rgba(170,184,202,.15)`, Hell
`rgba(90,108,130,.11)`, Akzent `#abb9ca` / `#65788d`. Zeichen bleiben mit voller
Deckkraft und ursprünglicher Farbe über der Fläche. Fehler-/Warnfarben bleiben.
Beim offenen Textmenü zeichnet die Browserstudie nur den Auswahlhintergrund
über einen ausgeblendeten Textspiegel nach; der wirkliche Text und Zeichenbereich
bleiben im Editor. Spiegel ist `aria-hidden`, keine zweite lesbare Textquelle.
Nativ direkt den vorhandenen Text-/Auswahlrenderer verwenden, keine WebView-Vorgabe.

## Vollständige Befehlsmatrix dieser Studie

„Vollständig“ bezeichnet die neun beauftragten Ziele mit geprüfter
Verfügbarkeit, nicht jede denkbare Dateiverwaltungs- oder Editorfunktion.
Die Kontextzugänge sind neue Gestaltung; vorhandene native Funktionen erhalten.

| Ziel | Gezeigte Befehle | Verfügbarkeit / Bezug |
| --- | --- | --- |
| Text / Code | Rückgängig, Ausschneiden, Kopieren, Einfügen, Alles auswählen | Ohne Änderung kein Undo; ohne Auswahl kein Cut/Copy; Nur lesen sperrt Änderungen; leerer Puffer sperrt Paste. |
| Datei-Tab | Datei speichern, Als Text kopieren → Namen/Pfade, Tab schließen | Gilt für rechts angeklickten Tab. Nur lesen sperrt Speichern. Ungespeicherter Tab führt in B2, kein stilles Verwerfen. |
| Datei | Datei öffnen, Vorschau, Namen/Pfade kopieren, Alles auswählen, Auswahl aufheben | Bei mehreren Zielen nur gemeinsame Vorschau-/Text-/Auswahlbefehle; kein unbelegtes „alle öffnen“. |
| Ordner | Ordner öffnen, Vorschau, Namen/Pfade kopieren, Alles auswählen, Auswahl aufheben | Ordnerpfad festhalten; kein stilles Projektwechseln. |
| Protokollmeldung | Zur Quelle, Meldung kopieren, Details, Alles auswählen, Auswahl aufheben | Quellenbefehl nur bei einer eindeutigen vorhandenen Quellstelle. Originaltext einschließlich Zeilenende unverändert kopieren. |
| Diagramm | Grafik exportieren…, Zoom zurücksetzen, Herkunft anzeigen | Bestehenden Export mit demselben Diagrammstand verwenden. Kein neuer Exportdialog. Reset nach vollem Bereich deaktiviert. |
| Szenenobjekt | Eigenschaften, Objektnamen kopieren, Alles auswählen, Auswahl aufheben; Szenenraster anzeigen | Raster ist ausdrücklich globale Szenendarstellung, kein Objektattribut; Checkbox zeigt Zustand. Keine erfundenen Transformationswerkzeuge. |
| Mehrfachauswahl | Auswahldetails, Namen/Pfade kopieren, Alles auswählen, Auswahl aufheben | Gemischte Datei-/Ordner-Auswahl; gemeinsamer Schnitt der unterstützten Aktionen. |
| Leere Fläche | Im Ordner: Neues Projekt…, Ordner öffnen…, Dateibaum aktualisieren. In der Szene nur globales Raster, im Protokoll Auswahlbefehle. | Kein heimlicher Befehl auf eine zuvor gewählte Datei/Meldung. Vorhandene Auswahl bleibt, das Menü benennt den freien Bereich. |

Untermenü „Als Text kopieren“ kopiert **Text**, keine Dateien. Es hat einen
Maus-Zurückzugang und Tastaturrückkehr. Bekannte, situationsabhängig gesperrte
Befehle bleiben als solche erkennbar, mit Grund. Nicht vorhandene Produkt-
funktionen werden nicht allein als scheinbar bedienbarer Eintrag erfunden.

## Interaktiv ausprobieren

1. Ziel wählen, auf Text/Element klicken oder markieren. Es öffnet sich **kein**
   automatisches Menü oder eine schwebende Leiste. Rechtsklick, Menütaste oder
   Umschalt+F10 öffnen die Aktionen. Keine zusätzliche Linksklick-App-Aktion.
2. Strg/⌘ ergänzt Listenelemente, Umschalt wählt einen Bereich. Rechtsklick auf
   ausgewählte Elemente erhält die Menge; auf ein anderes Element wird zuerst
   dieses Ziel ausgewählt. A2-Markierung bleibt sichtbar.
3. Pfeile auf/ab, Pos1/Ende, Enter, Tab und Escape. Deaktivierte Punkte werden
   übersprungen. Rechts/Enter öffnet Untermenü, Links/Escape schließt zunächst
   dieses. Zweites Escape schließt das Hauptmenü mit Fokus zur Quelle.
4. Klick außerhalb, tatsächliches Scrollen des Inhalts, Resize oder Fokusverlust
   schließen ohne Aktion. Das eigene Menü darf scrollen. Ein bereits vor dem
   Öffnen ausgelöstes, verspätet zugestelltes Scrollereignis ohne Positionsänderung
   ist kein nachträgliches Scrollen und verwirft das Menü nicht.
5. In der Szene eine rechte Ziehbewegung ausführen: Menü wird unterdrückt.
   Die Studie erkennt nur die Geste; sie implementiert keine echte 3D-Kamera.
6. Beispielzustände **Nur lesen**, **Puffer leer**, **Keine Quellstelle** und
   **Ungespeicherter Tab** zeigen die Grenzen der Aktionen. Andere Tabaktionen
   dürfen einen ungespeicherten `pendel.c`-Tab nicht versehentlich speichern.
7. „Ziel entfernen / Revision ändern“ verwirft das offene Menü. Ein alter Befehl
   schreibt danach nichts. Zurücksetzen stellt die Fixture-Ausgangslage her.

Die Menübefehle verwenden ausschließlich den sichtbaren **Beispielpuffer**.
Cut/Paste/Undo ändern nur den Beispieltext der Seite; keine Betriebssystem-
Zwischenablageabfrage oder Erlaubnis nötig. Die dargestellten Shortcuts stehen
für die native Belegung. Browser-eigene Tastenkürzel außerhalb des Menüs bleiben
Browserfunktionen und sind nicht an den Demo-Puffer gebunden.

## Quellen und erforderliche native Bindung

Der lokale Projektstand einschließlich paralleler Änderungen ist die Grundlage.
Gelesene Fundstellen (sie bestätigen die Funktion, nicht diese Menüanordnung):

- [Tab-UI](../../../app/file_tabs_ui.inc): stabile Tab-Schlüssel,
  `file_tab_select`/`file_tab_close`, Dokument-Schließen und vorhandene Pending-Regeln.
- [Toolbar](../../../app/toolbar_ui.inc): gemeinsame Befehlsverfügbarkeit,
  Speichern, Ordnerzugang, Projektanlage, Aktualisieren, Suchen und bestehende
  Menüs. Für inaktive Tabs nicht pauschal die aktive globale Save-Aktion auslösen.
- [Explorer](../../../app/file_browser_ui.inc): Datei-/Ordneröffnen, feste
  Vorschau, Lese-/Rechtefehler und vorhandene Ordneraktionen.
- [Protokoll C](../../../app/protocol_ui.inc): Originaltext, Kopieren,
  Quellpfad/-zeile und `protocol_jump` sowie zugängliche Details.
- [Berichtsdetails](../../../app/report_details.inc),
  [Export](../unified-workspace/EXPORT-EXAMPLE.md),
  [Markierung A2](../selection-options/APPROVED.md),
  [gewählte Bereiche](../design-options/APPROVED.md) und
  [Einstellungen A](../settings-options/APPROVED.md) für das vorhandene Rasterflag.

Die native Neuverknüpfung der Menübefehle, Mehrfachauswahl und Objektmetadaten
ist gemäß Auswahl A zu prüfen/umzusetzen. Namen-/Pfadkopie verwendet vorhandene Daten
und Clipboard-Adapter, ist kein unbelegter Dateikopier-/Verschiebeauftrag.
Umbenennen, Dateilöschen, Objekttransformation, Split-Editor und zusätzliche
Exportformate sind nicht durch diese Studie eingeführt.

## Verbindliche native Übergabe

**A ist Teil des fertigen Designs.** Genaue Werte aus `menus.css`, Befehlsmatrix
und Referenzen sind in [APPROVED.md](APPROVED.md) und im
[Gesamtauftrag](../unified-workspace/NEXT-CHAT.md) festgeschrieben. Native Arbeit
und alle bestehenden Freigaben fortsetzen; keine erneute Wahl von A2, Export,
B2, Rückmeldungen A, Fortschritt B mit Icon-only-Pop-out oder Kontextmenüs A.

- Ziel-/Projekt-/Dokument-/Lauf-/Fenster-/Versionsidentitäten beim Öffnen
  festhalten. Menü nach Zielwechsel/Entfernen verwerfen; Ausführung erneut gegen
  aktuelle Gültigkeit/Berechtigung prüfen. Nicht auf eine inzwischen andere
  aktive Datei oder Auswahl umbiegen.
- Gleiche Befehlsverfügbarkeit für Maus, Tastatur und Accessibility. Nur vorhandene
  bzw. gemäß Auswahl gezielt angebundene Befehle anbieten; keine Menüpunkte, die
  nur Erfolg behaupten. Fehler aus realem Ergebnis über Rückmeldungen A/Protokoll C.
- Kopieren/UTF-8/Originalzeilen und bestehende Undo-/Clipboard-Verträge erhalten.
  Editoränderungen dürfen keine unsichtbare neu formatierte Kopie verwenden.
- B2 für ungespeicherte Inhalte, bestehende Projekt-B-/Explorer-A- und Export-
  Aufgabenfenster nach ADR 015 verwenden; keine neuen Ersatzdialoge erfinden.
- Eigene native Menü-/Untermenü-Ownership und Fokus publizieren. Echte Popup-
  Verdeckung, Screenreader, kleinen Raum, große Schrift, DPI/Monitorwechsel
  und rechte Kamera-Ziehbewegung auf jeder verfügbaren Plattform prüfen.

Browserflächen, Galerie, Prüfschalter, Beispielpuffer und kleine Ergebnis-/
Referenzbereiche sind Prüfhülle. Die Links öffnen bestehende Referenzen mit
deren eigenen Beispieldaten; nativ muss dieselbe echte Quelle weitergereicht
werden. Keine realen Dateien geöffnet/gespeichert, keine Physik oder Kamera
berechnet. Neuladen setzt zurück; keine Speicherung in Local Storage.

D08 „Editor-Suche und Ersetzen“ bleibt die nächste neue Entwurfsrunde.

## Prüfung

[Freigabeprüfung](review/APPROVAL-VALIDATION.md),
[historischer Prüfbericht](review/VALIDATION.md), [historischer Maschinenbericht](review/validation.json).
`node tools/check_context_menu_options.cjs` mit Playwright und Edge; alternativ
`PHYSIM_BROWSER`. Ausgaben unter `build/context-menu-options-review`.
Keine native Windows-/Linux-/macOS- oder reale Screenreaderabnahme behauptet.
