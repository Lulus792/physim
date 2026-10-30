# UI: Was noch gestaltet und ausgearbeitet werden kann

Stand: **30. Oktober 2026.** Auf Nutzerauftrag erstellte Designliste für die
Fortsetzung. Grundlage sind der lokale Stand, die freigegebenen Entwürfe,
[Inhaltsinventar](CONTENT-INVENTORY.md), [Abdeckung](../design-options/COVERAGE.md),
[nativer Referenzabgleich](NATIVE-FIDELITY.md), [Bedienung](../../workspace.md),
[Einstellungen](../../settings.md), [Berichte](../../reports.md) und
[Projektplan](../../../Physim_Projektplan.md).

**P1:** zuerst gestalten, weil alltägliche Abläufe oder Daten betroffen sind.
**P2:** danach, für umfangreiche Projekte und wissenschaftliche Arbeit.
**P3:** zusätzliche Komfortideen. Die Priorisierung ist ein Arbeitsvorschlag.
„Detailentwurf offen“ heißt: Der betrachtete Designbestand legt diesen Ablauf
noch nicht vollständig fest. Es heißt nicht, dass seine native Funktion fehlt.
Laufende lokale Implementierungsänderungen sind keine abschließende UI-Abnahme.

## Bereits entschieden – umsetzen und abgleichen

Diese Teile brauchen **keine erneute Variantenentscheidung**:

| Teil | Gültige Entscheidung | Verbleibende Arbeit |
| --- | --- | --- |
| Gemeinsamer Rahmen / drei Bereiche / Trenner | Bestehende gemeinsame Referenz und Resizing-Vertrag | Native Gleichheit, kleine Fenster, große Schrift und herausgelöste Fenster abgleichen. |
| Ohne Projekt | Bestätigte Leeransicht | Alle drei Bereiche tatsächlich leer, passende deaktivierte Aktionen, Öffnen und Anlegen erreichbar. |
| Protokoll | C · Navigator, gemeinsame Werkbankfarben | Zusätzliche echte Zustände und umfassende native Aufgaben; das Layout C nicht neu entwerfen. |
| Szene & Werte / Auswertung / Läufe und Serien | A · Werkbank | Variable Inhalte, echte Daten und noch nicht vollständig abgeglichene Detailzustände. |
| Neues Projekt / Explorer | B · Dreischritt / A · Orte–Liste–Vorschau | Zusammenspiel und Fehler mit echten Daten; B ruft weiterhin Explorer A auf. |
| Grafikexport | Freigegebener Dialog mit Vorschau, PNG/SVG, Bildkopie | Reale Ausgabe-/Fehler-/Plattformabnahme; keine neue Exportrichtung. |
| Markierung / Rechtsklick | **A2 · Dezenter Schleier**, jetzt final freigegeben | Exakte Farbrollen und Auswahlverhalten nativ übernehmen, einschließlich Kamera-Konfliktprüfung. |
| Speichern / Dateikonflikte / Wiederherstellung | **B2 · gemeinsamer Git-Diff**, final freigegeben | Native Abläufe und echte Dateioperationen gemäß Spezifikation umsetzen; keine neue Variantenwahl. |
| Einstellungen | **A · Kategorien**, final freigegeben | Eigene Aufgabenfenster, echte Preferences/Einheiten und vollständige Zustandsabnahme gemäß Spezifikation. |
| Hinweise und Rückmeldungen | **A · Leiser Hinweis**, final freigegeben | Echte Ereignisse/Protokoll C, lokale Rückmeldungen und Verlauf mit Fokus-/Fenster-/Versionsprüfung nativ umsetzen. |
| Fortschritt und Abbruch | **B · Aufgabenleiste mit Pop-out**, gewählt | Echte OS-Fenster mit gemeinsamem Auftragszustand, Rückholen ohne Abbruch/Neustart, native Adapter-/Endzustandsabnahme. |
| Kontextmenüs | **A · Kompakt**, gewählt | Neun Zielarten, echte Befehlsverfügbarkeit/Zielbindung, Untermenüs, Fokus und rechte Kamerageste gemäß Spezifikation nativ umsetzen. |
| Editor-Suche / einzelne Ersetzungen | **Vorhandene kühle Suchinsel mit aufklappbarer Ersetzung** | Bestehendes Layout und native Funktion erhalten. Verbleibende Detail-/Plattformprüfungen brauchen keine erneute Anordnungswahl. |
| F1 / Hilfe | Bestehendes Design H und dessen Folgeverträge | Reale Inhalte, Tastatur, Screenreader und verbleibende Abweichungen weiter prüfen. |

Verträge: [Bereichsauswahl](../design-options/APPROVED.md),
[Markierung A2](../selection-options/APPROVED.md), [Export](EXPORT-EXAMPLE.md),
[Gesamtauftrag](NEXT-CHAT.md). Eine offene Detailfrage hebt diese Freigaben nicht auf.

## Als Nächstes: konkrete Detailentwürfe

**D01–D03 sind gestalterisch entschieden:** Der Nutzer hat
**[B2 mit gemeinsamer Git-Diff-Ansicht](../file-safety-options/APPROVED.md)**
ausdrücklich ins fertige Design übernommen. Die folgenden Zustände bleiben
Umsetzungs-/Abnahmeanforderungen; keine erneute Entwurfswahl. Das bestätigt
keine bereits abgeschlossene native Funktion. **D04 ist jetzt ebenfalls
entschieden: [Einstellungen A · Kategorien](../settings-options/APPROVED.md).**
**D05 ist ebenfalls entschieden: [Rückmeldungen A · Leiser Hinweis](../feedback-options/APPROVED.md).**
**D06 ist entschieden: [B · Aufgabenleiste mit Pop-out](../progress-options/APPROVED.md).**
**D07 ist entschieden: [Kontextmenüs A · Kompakt](../context-menu-options/APPROVED.md).**
**D08 war als neue Layoutrunde doppelt:** Die vorhandene Suchinsel bleibt.
Die [ergänzende Studie](../search-options/README.md) ist nicht freigegeben und
keine Pflichtentscheidung. **D09: automatische Anzeige beauftragt, A/B/C verworfen;
neue [Optik D/E/F](../completion-typing-options/README.md) offen.**
Vorhandene Vervollständigung/API-Karte/F1 bleiben bestehen.

| ID / Prio | Was gestalten? | Benötigte Zustände und Ergebnis |
| --- | --- | --- |
| D01 · P1 | **Speichern, Schließen und Projektwechsel · B2 gewählt** | Ungespeicherte einzelne/mehrere Dateien, Speichern/Verwerfen/Abbrechen, fehlgeschlagenes Speichern. B2-Aufgabenfenster mit eindeutigem Dateibezug; Inhalt bei Abbruch erhalten. |
| D02 · P1 | **Externe Dateiänderung und Konflikt · B2 gewählt** | Datei außerhalb der App geändert, gelöscht, verschoben oder schreibgeschützt; lokaler Entwurf vorhanden. Gemeinsamer B2-Diff, verständliche Wahl ohne stilles Überschreiben. Lesbare Vergleichsansicht, kein automatischer Merge-Editor. |
| D03 · P1 | **Autosave und Wiederherstellung · B2 gewählt** | Sicherung gefunden, mehrere betroffene Quellen, Original und Sicherung unterscheiden, Wiederherstellung/Abbruch/Fehler. B2 mit vorhandener [Autosave-Funktion](../../autosave.md) verbinden, kein neues Sicherungssystem voraussetzen. |
| D04 · P1 | **Einstellungen · A gewählt** | Kategorien links, Optionen mittig, Live-Vorschau rechts; Suche, Vergleich, Übernehmen/Abbrechen/Standardwerte und Fehler. Theme, Schrift, Autosave, Sichtbarkeit, Größen und eigene Einheitenaufgabe nativ umsetzen; unabhängige Transaktionen und ADR-015-Fenster erhalten. Keine neue Variantenwahl. |
| D05 · P1 | **Hinweise/Rückmeldungen · A gewählt** | Höchstens zwei kurze Karten, erreichbarer Verlauf, lokale Kopier-/Pfadfehler und dauerhafte Veraltet-Kennzeichnung; Gruppierung, Fokus und optionales Ausblenden. Mit echten Ereignissen aus Protokoll C nativ umsetzen, keine neue Variantenwahl. |
| D06 · P1 | **Fortschritt/Abbruch · B mit Pop-out gewählt** | Kompakte B-Leiste mit aufklappbaren Details und eigenem Fenster für dieselbe Aufgabe; Rückholen/Schließen ohne Abbruch/Neustart. Echte Zähler/Status, Abbruchbestätigung und Teilresultate nativ umsetzen, keine neue Variantenwahl. |
| D07 · P1 | **Kontextmenüs · A gewählt** | Kompakte Liste mit Trennlinien und Tastaturhinweisen für neun Ziele. A2, Rechtsklick/Tastatur, Mehrfachauswahl, Untermenüs, gesperrte Befehle mit Grund und Ziel-/Versionsbindung nativ umsetzen; keine neue Variantenwahl. |
| D08 · P1 | **Editor-Suche und Ersetzen · vorhandene Suchinsel erhalten** | Grundlayout und native Einzelersetzung bestehen. Detailprüfungen für Treffer/A2, Unicode, Nur lesen und Dateibezug offen halten; keine neue Layoutwahl. Zusatzstudie nicht freigegeben. Alle ersetzen und projektweite Suche sind kein stiller Implementierungsauftrag. |
| D09 · P2 | **Automatische Vervollständigung · D/E/F offen** | Nutzer beauftragt Auto-Anzeige beim Tippen; A/B/C verworfen. Neue flache Editor-/Neovim-/IDE-Listen mit separater Kurzinfo. Debounce, bewusste Übernahme, Escape-/IME-/Timer-/Zielregeln in Vorschau; Optik noch nicht übernehmen. Bestehende API-Karte/F1 erhalten. |
| D10 · P1 | **Explorer-Inhalte und Dateitypen** | Langer Dateiname/Pfad, viele Einträge, leer, Laden, verschwunden, Rechtefehler; Text, Bild und Messlauf als verschiedene Vorschauen. Explorer-A-Grundlayout steht; API-Kontext befindet sich in laufender nativer Arbeit und darf nicht pauschal als fehlend gelten. |
| D11 · P2 | **Kamera und räumliche Orientierung** | 2D/3D, Projektion, Standardblick, Zentrieren auf Auswahl, verdeckte/außerhalb liegende Objekte. Bestehende Kamerasteuerungen in eine klare, unaufdringliche Werkzeuggruppe einpassen; rechter Kameradrag vs. Rechtsklick-Menü ist Teil der Prüfung. |
| D12 · P1 | **Kräfte, Beschriftungen und überlagerte Objekte** | Mehrere Pfeile, Nullvektor, kollidierende/mehrzeilige Labels, ausgeblendete Eltern und verschwundene Auswahl. Szene-A-/A2-Grundlage erhalten; Maßstäbe getrennt und Werte nur bei expliziter Kanal-/Metadatenzuordnung. Kein Newtonwert aus beliebigem Pfeil. |
| D13 · P2 | **Lange Zeitleisten und Unterbrechungen** | Live vs. gespeicherter Zeitpunkt, große Dauer, Lücken, wiederhergestellter Teil, nicht verfügbare Szene. Auswahl, erreichbare Position und wirkliche Datenabdeckung klar unterscheiden. Grundgestaltung der Läufe A bleibt. |
| D14 · P2 | **Vergleich ungleicher Messläufe** | Fehlender Kanal, inkompatible Einheit, andere Zeitbasis, Referenzlauf, Datenlücke und reduzierte Vorschau. Vorhandene Auswertung-A-Zustände vertiefen; vorhandene Vergleichsfunktionen erhalten. |
| D15 · P2 | **Tabellen und lange Analyseberichte** | Lange Zellen, wissenschaftliche Zahlen, Auswahl/Kopie von Zeilen oder Zellen, mehrere Diagramme, leere/fehlerhafte Ausgabe. Auswahlgeometrie für Tabellen konkretisieren; A2-Farben sind bereits festgelegt. |
| D16 · P2 | **Herkunft, Stand und Reproduzierbarkeit** | Aktueller Code unterscheidet sich vom gespeicherten Lauf, alter Bericht, Modell-/Analyseversion, archivierte Quelle. Zusammenhängende Detailansicht aus vorhandenen Herkunftsdaten; Neuberechnen und archivierten Stand eindeutig trennen. |
| D17 · P2 | **Workspace- und Fensterverwaltung** | Benannte Workspaces/Layouts, umbenennen, ersetzen, fehlende Pfade, zweites Fenster, Rückkehr nach Schließen. Vorhandene Funktionen gestalten; feste Panelplätze und vorhandenes Herausziehen erhalten, keine zusätzliche Popout-Leiste. |
| D18 · P1 | **Zugängliche und kleine Varianten aller Dialoge** | Hell/Dunkel/hoher Kontrast, große Schrift, kleine Fenster, Tastaturfokus, disabled/selected/error. Als gemeinsame Zustandstafel ausarbeiten, keine isolierte zweite Designsprache. |

## Fachoberflächen für den weiteren Projektumfang

Diese Themen folgen aus dem **bereits vereinbarten Gesamtumfang**, nicht aus
einer neuen Funktionsfreigabe dieser Liste. Der Projektplan zählt einschließlich
CFD/FEM/Mehrphysik zur 1.0; „als Nächstes / später gestalten“ verschiebt diese
Ziele nicht automatisch in eine spätere Version. Der jeweilige Backend- und
Datenstand muss vor einer interaktiven Funktionszusage geprüft werden.

| ID / Prio | Künftiger Entwurf | Was er zeigen sollte |
| --- | --- | --- |
| F01 · P2 | **Materialien und Medien** | Eigenschaften, Einheiten, Datenquelle, Temperatur-/Druckabhängigkeit und Gültigkeitsbereich. Vorhandene [Materialdaten](../../properties.md) als Basis; eine komfortable Bibliotheksoberfläche gesondert entwerfen. |
| F02 · P2 | **Unsicherheit und Monte Carlo** | Verteilungen, Seeds, Stichprobenzahl, Bandbreiten, Konfidenzintervalle und fehlgeschlagene Stichproben. Bestehende Fachfunktionen prüfen; keine statistische Sicherheit durch Dekoration vortäuschen. |
| F03 · P2 | **Erweiterte Parameterstudien und Optimierung** | Bereiche, Kombinationen, Zielgrößen, Warteschlange, Teilresultate und Vergleich. Die freigegebene Läufe-/Serienansicht A bleibt Ausgangspunkt; neue Optimierungssteuerung braucht ihren eigenen Vertrag. |
| F04 · P2 | **Skalare und vektorielle Felder** | Farbleiste mit Einheit/Bereich, Vektordichte, Schnittebenen, Zeitbezug und fehlende Werte für Wärme-, Strömungs- oder andere Feldmodelle. Kontinuierliche Daten nicht wie Objektattribute behandeln. |
| F05 · P2 | **CFD-Aufbau und Solverbeobachtung** | Gebiet, Rand-/Anfangsbedingungen, Netzqualität, Solverparameter, Residuen, Konvergenz und Abbruch. Fachumfang aus dem [CFD-Plan](../../cfd-project-plan.md), keine fertige Oberfläche behauptet. |
| F06 · P2 | **FEM-Aufbau und Ergebnisdarstellung** | Geometrie/Netz, Materialzuweisung, Lager, Lasten, Einheiten, Verformung und Spannungsdarstellung. Fachumfang aus dem [FEM-Plan](../../fem-project-plan.md), physikalische und Darstellungsvergrößerung trennen. |
| F07 · P2 | **Mehrphysikkopplung** | Beteiligte Modelle, ausgetauschte Größen, Richtung, Einheiten, Zeitschritte und Kopplungsfehler. Als fachlichen Ablauf entwerfen, ohne einen vorhandenen visuellen Node-Editor vorauszusetzen. |
| F08 · P2 | **Fachgebiete und Lernwege** | Einstieg nach Modellfamilie, benötigtes Vorwissen, C/Physim-Beispiel und Übergang zum eigenen Experiment. Vorhandenes F1-Design H und Beispiele weiterverwenden; keine zweite getrennte Hilfe schaffen. |

## Zusätzliche Designideen – noch keine Produktentscheidung

| ID / Prio | Idee | Vor einem Entwurf klären |
| --- | --- | --- |
| I01 · P3 | Befehlssuche für Aktionen | Welche vorhandenen Befehle erreichbar sein sollen und wie Fokus/Shortcut mit der Editorsuche zusammenpassen. |
| I02 · P3 | Eigene Tastenkürzel | Konflikte, Plattformtasten, Zurücksetzen und Export der Belegung; aktueller Einstellungsumfang nicht als bereits erweiterbar voraussetzen. |
| I03 · P3 | Projektweite Suche / mehrere Dateien vergleichen | Suchumfang, Änderungsrechte, große Ergebnisse und native Datenbasis. Ein visueller Diff-/Merge-Editor ist ein eigener Umfang. |
| I04 · P3 | Bericht als zusammenhängendes Dokument ausgeben | Welche Formate und Inhalte nötig sind. Zusätzlicher PDF-/Gesamtberichtsexport ist nicht durch den freigegebenen PNG/SVG-Grafikexport abgedeckt. |
| I05 · P3 | Wiederverwendbare Diagrammgestaltung | Benannte Achsen-/Farben-/Legendenprofile, Vorschau und Herkunft; wissenschaftliche Einheiten und bestehende Exporte erhalten. |
| I06 · P3 | Geführter erster eigener Versuch | Kurzer optionaler Lernablauf innerhalb des bestätigten leeren Starts. Kein automatisch geladenes Demoexperiment und kein blockierendes Onboarding. |

Cloud, Konten, Teamchat oder verteilte Simulation sind kein stillschweigender
Bestandteil dieser Liste; der aktuelle Projektplan schließt einen Cloud-Dienst
und verteilte Simulation für die erste Version aus.

## Empfohlene nächste Entwurfsrunde

**D01–D03 sind mit B2 freigegeben** und gehören jetzt in die native Umsetzung.
**D04 ist mit A freigegeben** und gehört ebenfalls in die native Umsetzung.
**D05 ist mit A freigegeben** und gehört ebenfalls in die native Umsetzung.
**D06 ist mit B plus eigenem Pop-out-Fenster gewählt** und gehört ebenfalls in die native Umsetzung.
**D07 · Kontextmenüs A ist gewählt** und gehört zur nativen Umsetzung.
**D08 ist keine neue Layoutrunde:** vorhandene Suchinsel und Einzelersetzung erhalten.
Für **D09** sind automatische Vorschläge beauftragt; neue Optik D/E/F ist offen.
A/B/C sind verworfen und brauchen keine erneute Auswahl.
**D10 ist der nächste Listenpunkt, vor Entwurfsstart am nativen Bestand prüfen:**
Explorer A, Dateivorschauen und C-API-Kontext bestehen bereits. Nur tatsächlich
offene Detailzustände gestalten, keine neue Grundlayoutwahl. Danach Szenen-/
Auswertungsdetails ebenfalls anhand realer Inhalte abgleichen.

Pro Thema liefern: Normalzustand, Leer-/Lade-/Fehler-/Abbruchzustand, Auswahl,
Tastatur-/Fokusweg und eine kleine/helle/dunkle Darstellung. Neue Gestaltungs-
richtungen zunächst getrennt vergleichen; bestehende Freigaben erhalten.
Nach Auswahl Referenz, genaue Werte, Datenannahmen und Abnahme im Gesamtauftrag
festhalten. Weitere Vorschauen entstehen erst durch einen eigenen Entwurfsauftrag.
