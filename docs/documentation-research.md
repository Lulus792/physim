# Grundlagen der Physim Dokumentation

Stand: 12. Oktober 2026. Diese Gestaltungsentscheidungen verbinden den aktuellen
SecondBrain-Leser mit einem verständlichen Physim-Handbuch. Die Inhalte und
Beispiele werden für Physim geschrieben; fremde Handbücher werden nicht kopiert.

## Untersuchte Dokumentationsstrukturen

| Projekt | Beobachteter Aufbau | Anwendung in Physim |
| --- | --- | --- |
| [Rust Book](https://github.com/rust-lang/book/blob/main/src/SUMMARY.md) | Einstieg und kleines vollständiges Projekt vor dem systematischen Ausbau | Erst ein messbares Ergebnis, danach Lebensphasen und APIs |
| [Godot](https://github.com/godotengine/godot-docs/blob/master/index.rst) | Wege nach Erfahrung und Ziel; schrittweise fertige Projekte | App benutzen, eigenes Experiment schreiben und Referenz getrennt erreichbar |
| [NumPy](https://github.com/numpy/numpy/blob/main/doc/source/user/index.rst) | Getting started, Grundlagen und fortgeschrittene Nutzung | Einstieg bleibt kurz; Detailwissen folgt über gezielte Links |
| [SciPy](https://github.com/scipy/scipy/blob/main/doc/source/tutorial/index.rst) | User Guide nach Fachaufgaben und getrennte API-Details | Beispiele nach physikalischer Frage und Auswertungsziel ordnen |
| [Matplotlib](https://github.com/matplotlib/matplotlib/blob/main/doc/users/index.rst) | Kurzer Einstieg, erklärte Abbildungen und thematische Vertiefung | Jede Grafik bekommt Beschriftungen, Bedeutung und einen nächsten Arbeitsschritt |
| [Matplotlib Examples](https://matplotlib.org/stable/gallery/index.html) | Visuelle Galerie mit gezieltem Zugang zu ausführbaren Beispielen | Example-Übersicht verknüpft Modell, erwartetes Ergebnis und vollständige Quellen |

Diese konkreten Entscheidungen orientieren sich außerdem an
[Diátaxis](https://diataxis.fr/): Tutorials führen beim Lernen, Anleitungen lösen
Aufgaben, Erklärung vertieft Verständnis und Referenz beantwortet präzise Fragen.
Die bisherige API-Referenz und belegte Testgeschichte bleiben erhalten.

## Lernen und Verständnis

Der [IES Praxisleitfaden zum Lernen](https://ies.ed.gov/ncee/wwc/PracticeGuide/1)
empfiehlt unter anderem den Wechsel zwischen ausgearbeiteten Beispielen und
eigenen Aufgaben, erklärte Grafiken, Verbindungen zwischen konkreten und
abstrakten Darstellungen, aktiven Abruf und erklärende Fragen. Der Leitfaden
unterscheidet die Evidenzstärken seiner Empfehlungen; er ist keine Prüfung
unserer konkreten Physim-Texte.

Daraus folgen für dieses Handbuch:

- Jeder Einstieg nennt eine Frage und ein sichtbares Ergebnis.
- Ein Schritt führt eine neue Entscheidung ein und zeigt, wie das Ergebnis geprüft wird.
- Gleichung, Geometrie, Messkanal und Diagramm werden am selben Modell verbunden.
- Vor einer Parameteränderung sagt der Leser eine Wirkung voraus und vergleicht danach den Lauf.
- Eine eigene Variante folgt auf das vollständige Beispiel; Hinweise stehen getrennt von der Lösung.
- Kurze Rückfragen lassen zentrale Begriffe aus dem Gedächtnis erklären.
- Vertiefungen sind verlinkt, damit der erste Versuch überschaubar bleibt.

## Medien und SecondBrain

SecondBrain 0.9.74 liest Markdown, Tabellen, Code und relative Quellenlinks.
Bilder erscheinen derzeit als Alternativtexte; HTML wird nicht ausgeführt.
Physims Offline-Hilfe ergänzt Markdown um native Vektordiagramme und eine
eingebettete 3D-Erklärung mit dem vorhandenen Renderer. Die gebündelten
Grafikziele wählen diese nativen Darstellungen; Dokumente führen keine Skripte aus.
Die SVG-Dateien stellen dieselben Begriffe im Repository dar. Sämtliche Bedienung
und Erklärung bleibt in der App. Die Textfassung enthält zusätzlich die
wesentliche Aussage jeder Grafik für SecondBrain und zum Nachschlagen.

Der [SecondBrain Datenvertrag](https://github.com/Lulus792/SecondBrain/blob/main/docs/DATENVERTRAG.md)
und die [lokale Anbindung](secondbrain.md) legen Metadaten und Grenzen fest.
