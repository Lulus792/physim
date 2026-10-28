# Neues Projekt B: nativer Dreischritt

Geprüfter Stand vom 28. Oktober 2026, Ausgangscommit `2f9b318a`.
Maßgeblich sind [ADR 011](../../decisions/011-selected-workspace-additions.md),
[APPROVED.md](../design-options/APPROVED.md), `dialogs.js` und `options.css`
im selben Referenzordner. Die gespeicherte Referenz `review/project-b.png`
ist ein visueller Vergleich; die lokale HTML-Seite wurde nicht über einen
anderen Zugriffsweg geöffnet.

Der bisherige einseitige Projektmanager ist durch **Startpunkt → Projektangaben
→ Prüfen** ersetzt. Die bestehenden Erstellungsfunktionen erzeugen weiterhin echte
Projektdateien. Alle acht Modelle, unabhängig kombinierbare C-/Physim-Sprachen und
reine Analyseprojekte bleiben erhalten. Zurückgehen erhält die Eingaben;
Name, Elternordner und Kollisionen werden geprüft. Abbrechen erhält das geladene
Projekt. Die zugänglichen Namen bezeichnen die tatsächlichen Aktionen.

Die Umsetzung übernimmt die CSS-Maße für Dialog, zentrierten Inhaltsbereich,
Kartenraster, Rundungen, Symbolkonturen, zwölf Schriftrollen und inhaltsbreite
Fußaktionen. Die native Schriftmetrik wird in logische Pixel umgerechnet.
Große Schrift und die bestehende Mindestfenstergröße erhalten Scrollzugang.
Maus, Tab, Rückwärts-Tab und die nativen Accessibility-Aktionen benutzen dieselben
Kontrollen. Escape behandelt zuerst die aufgeklappte Sprachauswahl; F1 bleibt
im Dialog erreichbar.

Die Karten bilden eine native Einzelauswahlgruppe. Vollständig verdeckte Karten
bleiben mit stabiler Identität und leerer Bildschirmgeometrie erreichbar. Fokus
holt sie in den sichtbaren Bereich. Der Dialog veröffentlicht keine bedienbaren
Editfelder aus der darunterliegenden Auswertung. Diese Ergänzung wird gezielt
für den Dialog aktiviert; andere Ansichten behalten ihren bisherigen Vertrag.

Der Linux-Client prüft dabei genau eine gesetzte `CHECKED`-Karte und acht
`CHECKABLE`-Radiobuttons, entsprechend der
[AT-SPI-Radiobutton-Semantik](https://gnome.pages.gitlab.gnome.org/gtk/atspi2/enum.Role.html).
Der erste neue Client hatte fälschlich zusätzlich die Selection-Schnittstelle
einer auswählbaren Liste verlangt. Der Windows-Client prüft getrennt die dortige
UIA-SelectionItem-/Selection-Schnittstelle. Rollen, Einzelauswahl und tatsächliche
Auswirkungen bleiben auf beiden Plattformen unabhängige Prüfbedingungen.

## Bewusste Unterschiede und offene Umsetzung

| Punkt | Stand und Grund |
| --- | --- |
| Variantenknöpfe A/B/C und Demo-Hinweise | Entfallen gemäß freigegebenem Produktauftrag. Sie dienen nur der Auswahlgalerie. |
| Dateivorschau | Zeigt die wirklich erzeugten Dateien. Die Browserdemo zeigt zusätzliche `assets/` und ein abweichendes Analyseprojekt. Solche Dateien werden vom bestehenden Ersteller nicht angelegt; eine erfundene Vorschau wäre falsch. |
| Ordnerauswahl | Verwendet weiterhin den vorhandenen Systemdialog. **Explorer A ist noch umzusetzen.** Das ist eine offene Implementierungsarbeit, keine behauptete technische Unmöglichkeit. |
| Material | Native Hintergrundabdunklung, Unschärfe und Schatten mit bestehendem Fallback für hohen Kontrast/reduzierte Transparenz. Die Betriebssysteme und GPU-Treiber müssen getrennt bewertet werden. |
| Rasterung | Browser und native Schrift-/GPU-Rasterung können an Konturen abweichen. Die Maße werden gegen CSS geprüft; das allein beweist keine vollständige Bildgleichheit. |
| Kleine Fenster | Bisheriger Prüfbereich ab 1080 × 740 und UI-Schrift 16/22. Kein Nachweis für 560 px, 200 % oder beliebige DPI. |
| Manuelle Windows-Taste | Auf Nutzerwunsch bis zu seiner erneuten Rückmeldung vertagt. Automatisierte logische Fokus-/UIA-Prüfungen ersetzen diesen physischen Nachweis nicht. |
| macOS und Screenreader | Eine Testregistrierung allein belegt weder native AppKit-Läufe noch einen tatsächlichen VoiceOver-/NVDA-Leseablauf. |

## Nachweisführung

Testprogramme und kompakte Berichte bleiben versioniert; Rohprojekte, Compiler-
und Testausgaben liegen ausschließlich unter `D:/Physim/build`. Vor dem Ersetzen
der Binärdateien werden die Compilerquellen und Laufzeitdateien archiviert und
alle referenzierten Archivbytes mit SHA-256 zurückgelesen. Fremde lokale Änderungen
sind aus dem isolierten Build ausgeschlossen. Die Gegenprobe erhält das alte
Kartenverhalten und muss dessen fehlende Radio-Semantik tatsächlich ablehnen.

Der [kompakte Nachweis](../../verification/project-wizard-b-2026-10-28.json)
enthält Kommandos, Quell-/Programmhashes, Rohbericht-Verweise, alte Gegenproben,
Reparaturbefunde und Grenzen. Die abschließenden Ergebnisse:

| Prüfung | Windows | Linux |
| --- | --- | --- |
| Modell/Fokus/Edit/Textlayout | 7/7 | 7/7, ASan/UBSan mit Leak-Prüfung |
| CSS-Maße und Schriftrollen | 6 Zustände | 6 Zustände |
| Unabhängige native Kartenauswahl | UIA, UI 16/22 | AT-SPI, UI 16/22 |
| Popup-Escape, F1 öffnen/schließen, Dialog-Escape | UI 16/22 | UI 16/22 |
| Ungültiger Name/Ordner, Kollision, Abbruch und erneutes Öffnen | UI 16/22 | UI 16/22 |
| Alle Vorlagen/Sprachkombinationen und Analyseprojekte | 34 × 2 = 68 | 34 × 2 = 68 |

Die 136 realen Projektabläufe prüfen Eingabeerhalt, richtige Quelldateien,
Build, Aufzeichnung beziehungsweise Import und die tatsächlich erzeugte
Auswertung. Zeitmessungen und Grenzen pro Bedien-/Verarbeitungsschritt verhindern,
dass ein echter Stillstand hinter dem längeren Gesamtlauf verschwindet.
Die Testregistrierung ist separat für Windows/Linux/Darwin geprüft; eine solche
Katalogprüfung ist kein macOS-Laufnachweis.

Die versionierten Testprogramme sind über den normalen Builder ausführbar,
auf diesem Windows-Rechner beispielsweise:

```powershell
python tools/with-artifacts.py -- python tools/build.py --config Release --sdl third_party/SDL3-3.2.30 --test-display --test-filter "project_wizard*" --test-filter "project_manager_keyboard*"
```

Unter Linux dieselben Filter mit `--config Debug --sanitizers` und dem lokalen
SDL-Präfix verwenden. Die GUI-Prüfungen benötigen einen laufenden Displayserver
und Fenstermanager; der dokumentierte Nachweis verwendet eigenes Xvfb/Openbox.
Die im JSON verlinkten Kommandos nennen die tatsächlich verwendeten Pfade.

Der Linux-Debug-Lauf deckte außerdem das Schließen des aktuell gezeichneten
Nuklear-Fensters durch eine native Abbrechen-Aktion auf. Das Fenster wird jetzt
zwischen zwei Zeichenframes entfernt. Der nächste ausdrückliche Tastatur- oder
Mauseingriff verwirft eine noch ausstehende Fokusrückgabe; sofortiges erneutes
Öffnen über das Menü bleibt damit möglich. Beide Regressionen sind geschlossen.

Nur zwei unterschiedliche Bildzustände sind übernommen:
[Windows dunkel](../../verification/project-wizard-b-2026-10-28/windows-dark.png)
und [Linux mit großer Schrift](../../verification/project-wizard-b-2026-10-28/linux-large.png),
zusammen 326.415 Bytes. Die übrigen Zustände sind über die kompakten Berichte
und gezielt erzeugten lokalen Testdaten nachvollziehbar.

Die vollständige Abnahme des Projektdialogs einschließlich Explorer A und der
gesamten Oberfläche bleibt offen. Auch die sieben bereits vor diesem Schritt
festgestellten, in F1 nicht erreichbaren Dokumentationslinks und deren
macOS-Abnahme sind weiter zu bearbeiten.
