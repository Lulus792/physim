# D05: Hinweise und Rückmeldungen · getrennte Entwürfe

Stand: **30. Oktober 2026 · Auswahl offen.** Der Nutzer hat den nächsten Punkt
der UI-Liste beauftragt. Diese Runde gestaltet D05; keine Variante ist bereits
für das fertige Design ausgewählt. Alle bisherigen Freigaben bleiben gültig,
insbesondere Protokoll C, Export, A2, B2 und Einstellungen A.

[Vorschau öffnen](index.html?variant=a&scenario=burst&theme=dark) ·
[A · kurze Karten](index.html?variant=a&scenario=saved) ·
[B · Statusband](index.html?variant=b&scenario=saved) ·
[C · Aktivitäten](index.html?variant=c&scenario=burst).
Lokal unter `http://127.0.0.1:8791/feedback-options/`, auch direkt als Datei
ohne Server/Netzwerk nutzbar. Keine externen Fonts oder Abhängigkeiten der Seite.

## Drei Gestaltungsrichtungen

| Variante | Rückmeldung | Verlauf |
| --- | --- | --- |
| **A · Leiser Hinweis** | Maximal zwei kleine Erfolgskarten rechts unten, einzeln ausblendbar; lokale Fehler bleiben an der Aktion. | „Rückmeldungen“ öffnet eine kleine Liste am Rand. Während sie offen ist, werden die Karten verborgen. |
| **B · Statusband** | Gemeinsames, hervorgehobenes Band unten. Der Inhalt bleibt frei von Erfolgskarten. | Band/Schaltfläche klappt die Liste darüber auf. |
| **C · Aktivitäten** | Von Beginn an sichtbarer Verlauf rechts. Lokale Fehler besitzen nur einen kurzen Verweis im Verlauf. | Verkleinert die Arbeitsfläche statt sie zu überlagern; ein-/ausblendbar. Schmal unter dem Inhalt. |

Gleiche Inhalte, andere Gewichtung und Platzierung. Wechsel erhält Rückmeldungen,
Wiederholungszähler und bearbeiteten Beispielcode. Die gezeigte Werkbank ist nur
Kontext für D05 und ersetzt keine ausgewählten Bereichslayouts. Galerie,
Variantenwahl, Szenarien und Fehlerfreigaben sind Prüfhülle, keine Produktseiten.

## Beispiele und Platzierungsregeln

| Beispiel | Ort und Verhalten | Warum |
| --- | --- | --- |
| Datei gespeichert | A: kurze Karte; B: Statusband; C: Verlauf. Kein Bestätigungsdialog. | Eine erfolgreiche Aktion braucht keine Unterbrechung. |
| Bildkopie fehlgeschlagen | Inline im Export bei der Aktion; Grund, Wiederholen und Link zur vorhandenen Speicheralternative. Keine zusätzliche Fehlerkarte. | Kontext und Eingaben bleiben sichtbar; der Export wird nicht neu entworfen. |
| Projektpfad ungültig | Direkt unter dem Pfad, `aria-invalid` und verständliche Erklärung. Eingabe bleibt erhalten. | Zur betroffenen Stelle zurückführen, keine zweite Fehlerbox. |
| Ergebnis veraltet | Dauerhaft oberhalb des betreffenden Diagramms. Schließen des Verlaufs entfernt diese Kennzeichnung nicht. | Historische Daten nicht als aktuell ausgeben. |
| Hintergrundaktion beendet | Erfolg mit Lauf-/Ergebnisbezug und „Ergebnis ansehen“. | Abschluss melden und Zugriff auf das Ergebnis geben. |
| Mehrere Rückmeldungen | Drei Ursachen, vier Ereignisse: wiederholtes Speichern wird als 2× zusammengefasst. Warnung bleibt lokal. | Keine Meldungsflut, mehrere Ursachen bleiben unterscheidbar. |
| Keine Rückmeldungen | Ruhiger Status und verständliche leere Liste. | Ohne Inhalt keinen Fehler-/Aufmerksamkeitszustand erfinden. |

Status/Verlauf zeigen bei lokalen Fehlern nur Zusammenfassung und Navigation,
nicht eine zweite vollständige Erklärung. Ein zentraler höflicher Live-Bereich
kündigt neue Ereignisse an; kein Fokusdiebstahl und keine native OS-Mitteilung
in dieser Runde. Farbe wird durch Symbol, Text und Zustand ergänzt.
Entscheidungen über Verwerfen/Überschreiben bleiben im freigegebenen B2-Ablauf.
D05 führt dafür keine zusätzlichen Bestätigungsdialoge ein. Ein reiner
Informationshinweis braucht ebenfalls kein Aufgabenfenster.

## Interaktiv ausprobieren

1. Richtung und Beispiel wählen; „Beispiel erneut auslösen“ zeigt Gruppierung.
2. Im Code ein Wort ändern, dann „Im Beispiel speichern“. Nur der Seitenspeicher
   wird aktualisiert. Richtungs-/Bereichswechsel erhalten diesen Text.
3. Bei Bildkopie bleibt Wiederholen erfolglos, solange die Beispielursache
   besteht. Erst die Checkbox „Zwischenablage verfügbar“ erlaubt simulierten
   Erfolg. **Es wird kein echtes Bild kopiert.** Die Speicheralternative öffnet
   die bestehende Exportreferenz mit deren eigenen Beispieldaten, nicht einen
   Speichervorgang für den hier dargestellten Screenshot.
4. Bei Pfaden ist ausschließlich `D:\Physim\experiments\pendel` ein verfügbarer
   Fixture-Ordner. Einsetzen allein behebt nichts; erst Prüfen validiert.
   Kein Dateisystemzugriff, keine Projektanlage. Andere Pfade werden im Beispiel
   zurückgewiesen, unabhängig von ihrer tatsächlichen Existenz.
5. Bei veraltetem Ergebnis erst „Im Beispiel neu auswerten“ wählen; die Warnung
   wird erledigt und ein Abschluss erscheint. Keine echte Analyse ausgeführt.
6. Karten ausblenden oder Verlauf schließen erhält die Ereignisse. Wiederöffnen
   über „Rückmeldungen“/Statusband. Escape schließt den Verlauf und gibt Fokus
   an den Rückmeldungsbutton zurück. „Zur betroffenen Stelle“ navigiert/fokussiert.

Erfolgshinweise können optional nach sechs aktiven Sekunden ausgeblendet werden;
die Checkbox ist zum Vergleichen zunächst aus. Fokus/Mouseover auf den Karten
und eine verborgene Seite pausieren das Ausblenden. Der Verlauf bleibt erhalten,
lokale Fehler und Veraltet-Warnungen laufen nie automatisch ab. Bei B kehrt das
Band zum ruhigen Zustand zurück; C behält den Verlauf sichtbar.

## Quellen und Grenzen

Ausgangspunkt ist der aktuelle lokale native Stand, nicht die Annahme, dass
diese Funktionen fehlen. Gelesene Fundstellen:

- [Dokumentaktionen](../../../app/document_actions.inc): `document_error`,
  vorhandene Statusrückmeldungen zu gespeichertem/fehlerhaftem Dokument.
- [Grafikexport](../../../app/graphic_export_ui.inc): echte Bildkopie über
  `ps_graphic_image_clipboard`, vorhandene `d->message`-Erfolgs-/Fehlermeldung.
- [Explorer](../../../app/file_browser_ui.inc): Rechte-/Lese-/Vorschaufehler.
- [Analyse](../../../app/analysis_ui.inc), [Workspace](../../workspace.md),
  [gemeinsamer Protokollvertrag](../unified-workspace/PROTOCOL-DESIGN.md).

Die Studie erprobt Platzierung und Meldungstexte mit synthetischen Daten.
Editorcode ist dekorativer Beispieltext, kein geprüftes Physim-Programm.
Diagramm, Szenenwerte und gemeldete Tabellenanzahl sind Fixtures; keine
Simulation, Worker, Projekt-/Datei-/Clipboard-Operationen. Neu laden oder
Szenario wechseln setzt die Beispielereignisse zurück; kein Local Storage.
Die Export-/Projektteile sind reduzierte Kontextausschnitte, keine alternativen
Dialogentwürfe. [Exportvertrag](../unified-workspace/EXPORT-EXAMPLE.md),
[Bereichsauswahl](../design-options/APPROVED.md) und
[ADR 015](../../decisions/015-independent-task-windows.md) bleiben erhalten.

## Übergabe nach späterer Auswahl

**Vor Auswahl A/B/C nicht nativ als finales Design übernehmen.** Nach Entscheidung
Variante, genaue Farben/Abstände aus `feedback.css`, Zustände und Referenzen
festschreiben und in den [Gesamtauftrag](../unified-workspace/NEXT-CHAT.md)
aufnehmen. Folgende Anforderungen unabhängig von der Wahl weiter prüfen:

- Echte Ereignisse mit Dokument-/Projekt-/Lauf-/Fenster- und Versionsbezug
  verbinden. Eine alte erfolgreiche Aktion darf keinen späteren Fehler oder
  ungespeicherten Zustand verdecken.
- Gruppierung nach echter Ursache, Ziel, Operation und Revision statt nur
  Meldungstext. Die Demo hat je Typ nur ein Fixture-Ziel und gruppiert danach;
  das ist kein hinreichender nativer Schlüssel.
- Mit dem vorhandenen Protokoll-C-Ereignisbestand verbinden. Der kurze Verlauf
  ist eine Darstellung derselben Ereignisse, keine zweite unabhängige technische
  Wahrheit und kein Ersatz für vollständige Originalmeldungen im Protokoll.
- Aktion an echte Quelle binden; bei geschlossenem/fehlendem Ziel verständlich
  reagieren statt ein anderes Projekt zu öffnen. Keine automatische Neuberechnung
  oder Dateiänderung beim bloßen Anklicken einer Meldung.
- Zwischenablage-/Schreibfehler aus realem Ergebnis ableiten; Wiederholen nur
  mit aktueller Gültigkeitsprüfung. Lokale Meldung im eigenen Export-/Projekt-
  fenster anzeigen, Hauptfenster nach ADR 015 weiter bedienbar halten.
- Veraltet-Markierung an echte Quell-/Analyse-/Ergebnisrevision knüpfen.
  Erst erfolgreiche passende Neuberechnung erledigt sie; fehlgeschlagene Arbeit
  lässt die Kennzeichnung bestehen.
- Fokus, Lesereihenfolge, Screenreaderankündigung, kleine Fenster, große Schrift,
  hoher Kontrast und reduzierte Effekte mit echten nativen Fenstern prüfen.
  Keine Animation ist für Information oder Navigation erforderlich.

D06 (Fortschritt/Abbruch) und D07 (vollständige Kontextmenüs) sind noch eigene
offene Entwurfsrunden. Diese Studie führt dafür weder einen neuen Fortschritts-
dialog noch neue Kontextmenüs ein.

## Prüfung

[Prüfbericht](review/VALIDATION.md), [Maschinenbericht](review/validation.json).
`node tools/check_feedback_options.cjs` benötigt Playwright und einen Browser;
auf Windows Edge, alternativ `PHYSIM_BROWSER`. Ausgaben ausschließlich unter
`build/feedback-options-review`. Browserbelege sind keine native Plattformabnahme.
