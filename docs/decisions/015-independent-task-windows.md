# Eigenständige Fenster für Dialogaufgaben

Status: vom Nutzer am 29. Oktober 2026 beauftragt; Umsetzung und Abnahme offen.

## Auftrag

Dialoge wie **Neues Projekt** und **Grafik exportieren** sollen tatsächliche,
eigenständig verschiebbare Fenster werden. Während sie geöffnet sind, muss der
Nutzer im Hauptfenster weiterarbeiten können. Der Auftrag ergänzt die festen
Arbeitsbereiche mit herausziehbaren Fenstern aus [ADR 009](009-fixed-workspace-windows.md).
Die freigegebenen Inhalte und Gestaltungen aus [ADR 010](010-analysis-graphic-export.md)
und [ADR 011](011-selected-workspace-additions.md) bleiben erhalten.

## Vertrag

1. Eigene OS-Fenster mit eigener Eingabe, Nuklear-/Accessibility-Oberfläche und
   Grafikressourcen. Ein gezeichnetes Overlay oder das bloße Entfernen der
   Hintergrundabdunklung erfüllt den Auftrag nicht.
   Die Betriebssystem-Titelleiste kommt als Fensterrahmen hinzu; die bestehende
   Vorschau und die Dialoginhalte bleiben innerhalb der Clientfläche erhalten.
2. Das Hauptfenster und herausgelöste Arbeitsbereiche bleiben bedienbar. Eingaben
   an ein anderes Fenster dürfen nicht vom offenen Dialog verbraucht werden.
   Dialoge dürfen dessen Accessibility-Baum nicht pauschal deaktivieren.
3. Fensterzustand und Domänenzustand trennen: pro Dialogtyp eigene Eingaben,
   Vorschau, Auswahl und laufende Operationen. Ein erneuter Aufruf aktiviert das
   bestehende Fenster und verwirft keine Eingaben.
4. Grafikexport behält die beim Öffnen kopierten Daten, Einheiten, Kurven,
   Ausschnitte und Darstellung. Projekt-/Laufwechsel im Hauptfenster verändert
   diesen Export nicht. Quellfenster können währenddessen geschlossen werden.
5. Der Projektdialog behält Namen, Zielordner, Vorlagen und Sprachen über
   Fensterwechsel und Unteraufrufe. Ein Projektwechsel im Hauptfenster darf
   diese Eingaben nicht überschreiben. Anlage und anschließendes Öffnen müssen
   aktuelle ungespeicherte Inhalte weiterhin korrekt behandeln.
6. Unterdialoge gehören zu ihrem tatsächlichen Auftraggeber. Eine Ordnerwahl aus
   dem Projektdialog liefert ihr Ergebnis dorthin; ein Speicherdialog gehört zum
   Exportfenster. Eine notwendige Sperre des Auftraggebers blockiert nicht die
   gesamte Anwendung.
7. Escape und die OS-Schließenaktion schließen das betreffende Aufgabenfenster.
   Fokus kehrt nur bei sinnvoll vorhandenem Auftraggeber dorthin zurück;
   verspätete Fokuskorrekturen dürfen keine andere aktive Arbeit übersteuern.
8. Ressourcen erst bei abgeschlossenem Zeichnen freigeben. Shutdown, laufendes
   Speichern und verspätete Dateidialogantworten behalten ihre Ownership-Verträge.
   Unbekannte oder inzwischen geänderte Quellen vor einer Änderung neu prüfen.

## Umfang und Ausgangslage

Zunächst Grafikexport und Projektdialog einschließlich Datei-/Ordnerwahl;
anschließend die weiteren Aufgabenoberflächen für Workspace, Layouts,
Kanaleinheiten und Laufdetails sowie die vorhandenen Bestätigungs-/Fehlerdialoge
inventarisieren und entsprechend umstellen. Gewöhnliche aufklappbare Menüs und
Comboboxen bleiben an ihr eigenes Aufgabenfenster gebunden.

Im untersuchten Ausgangsstand besitzen F1 und herausgelöste Bereiche bereits
eigene SDL-/GL-/Nuklear-Fenster. Der Grafikexport besitzt einen unveränderlichen
Datensnapshot, verwendet jedoch die Fensterressourcen des Auftraggebers und
verbraucht auch dessen andere Eingaben. Der Projektdialog hängt zusätzlich an
globalen Layout-/Bediensperren; mehrere Toolbaraktionen setzen seinen Offenstatus
direkt zurück. Diese Stellen gehören zur Umsetzung, nicht nur die Fensteranlage.

## Abnahme

Die erste Umsetzung führt einen gemeinsamen App-/Simulationszustand weiter und
übergibt dem Exportrenderer seinen eigenen Dialogkontext ausdrücklich. Eine
zweite App-Instanz würde Projekt-, Runner- und Workerzustände verdoppeln und ist
deshalb ungeeignet. Ein weiter schwebendes Overlay erfüllt den geforderten
OS-Fenstervertrag ebenfalls nicht. Gemeinsame unveränderliche Schriftressourcen
werden wie bei herausgelösten Bereichen über geteilte GL-Kontexte genutzt;
deren Lebenszeit endet nach den Aufgabenfenstern.

Nachweise auf tatsächlich ausgeführten Windows-/Linux-Builds: eigene Fenster-ID
und nativer Fenster-/Accessibility-Baum, Wechsel zwischen Haupt- und Dialogfenster,
Weiterbearbeiten/Navigation bei geöffnetem Dialog, unveränderte Dialogeingaben
und Exportdaten, Resize/Theme/Textgröße, Tastatur und Fokus, Unterdialogzuordnung,
OS-Schließen und App-Shutdown. Bestehende Exportdateien und Projektabläufe bleiben
vollständig zu prüfen. Eigene macOS-Laufnachweise bleiben erforderlich; ein
gemeinsamer Codepfad oder eine CI-Datei ersetzt sie nicht.

Es liegen noch keine Abnahmeergebnisse für diesen neuen Fenstervertrag vor.
