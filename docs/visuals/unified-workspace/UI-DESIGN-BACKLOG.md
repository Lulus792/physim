# UI: Was noch gestaltet und ausgearbeitet werden kann

Stand: **29. Oktober 2026.** Auf Nutzerauftrag erstellte Designliste für die
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
| F1 / Hilfe | Bestehendes Design H und dessen Folgeverträge | Reale Inhalte, Tastatur, Screenreader und verbleibende Abweichungen weiter prüfen. |

Verträge: [Bereichsauswahl](../design-options/APPROVED.md),
[Markierung A2](../selection-options/APPROVED.md), [Export](EXPORT-EXAMPLE.md),
[Gesamtauftrag](NEXT-CHAT.md). Eine offene Detailfrage hebt diese Freigaben nicht auf.

## Als Nächstes: konkrete Detailentwürfe

| ID / Prio | Was gestalten? | Benötigte Zustände und Ergebnis |
| --- | --- | --- |
| D01 · P1 | **Speichern, Schließen und Projektwechsel** | Ungespeicherte einzelne/mehrere Dateien, Speichern/Verwerfen/Abbrechen, fehlgeschlagenes Speichern. Kompakter Dialog mit eindeutigem Dateibezug; Inhalt bei Abbruch erhalten. |
| D02 · P1 | **Externe Dateiänderung und Konflikt** | Datei außerhalb der App geändert, gelöscht, verschoben oder schreibgeschützt; lokaler Entwurf vorhanden. Warnung und Vergleich der Versionen, verständliche Wahl ohne stilles Überschreiben. Tatsächlich verfügbare Funktionen vor einem Diff-Editor-Entwurf prüfen. |
| D03 · P1 | **Autosave und Wiederherstellung** | Sicherung gefunden, mehrere betroffene Quellen, Original und Sicherung unterscheiden, Wiederherstellung/Abbruch/Fehler. Vorhandene [Autosave-Funktion](../../autosave.md) gestalten, kein neues Sicherungssystem voraussetzen. |
| D04 · P1 | **Einstellungen als vollständige Oberfläche** | Theme, UI-/Codeschrift, Autosave, Einheiten und Panelgrößen; Entwurf geändert, Übernehmen/Abbrechen/Standardwerte. Bestehende Einstellungsfunktionen und Tastaturpfade erhalten, Gruppen und Vorschau konsistent ausarbeiten. |
| D05 · P1 | **Hinweise und Rückmeldungen außerhalb des Protokolls** | Gespeichert, Kopie fehlgeschlagen, Pfad ungültig, Ergebnis veraltet, Hintergrundaktion beendet. Festlegen, was inline, dauerhaft in der Statuszeile, vorübergehend als Hinweis oder als Dialog erscheint; keine mehrfachen störenden Meldungen. |
| D06 · P1 | **Fortschritt und Abbruch über alle Bereiche** | Bauen, Projekt laden, Datei-/Laufvorschau, Analyse, Export; Fortschritt unbekannt, Abbrechen angefordert, fertig oder fehlgeschlagen. Vorhandene Läufe-A-Start-/Limitzustände erhalten und die übrigen Abläufe ergänzen. |
| D07 · P1 | **Vollständige Kontextmenüs je Ziel** | Text, Tab, Ordner, Datei, Meldung, Diagramm, Szenenobjekt, Mehrfachauswahl und leere Fläche. A2/Menu-Bedienung ist entschieden; offen ist die vollständige Befehlsmatrix mit Verfügbarkeit und sinnvollen Gruppen. Nur vorhandene/gezielt beauftragte Befehle zeigen. |
| D08 · P1 | **Editor-Suche und Ersetzen** | Keine Treffer, mehrere Dateien, aktive Fundstelle vs. A2-Auswahl, Ersetzen einzelner/aller Treffer, ungespeicherte Änderungen. Bestehende Suchfunktion einpassen; projektweite Suche separat als mögliche Erweiterung behandeln. |
| D09 · P2 | **Vervollständigung und Hilfe direkt am Code** | Vorschlagsliste, Signatur, Parameter, Dokumentation, abgeschnittener Name, keine Treffer und Ladezustand. Vorhandene API-Vervollständigung/F1 nicht neu erfinden; Popup, Kontext und Fokus gemeinsam abgleichen. |
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

Zuerst **D01–D03** gemeinsam entwerfen: Speichern, Konflikte und Wiederherstellung
bilden einen zusammenhängenden Ablauf. Danach **D04–D07**: Einstellungen,
Rückmeldungen, Fortschritt und die vollständige Kontextmenümatrix. Anschließend
Explorer-/Szenen- und Auswertungsdetails anhand realer Inhalte ausarbeiten.

Pro Thema liefern: Normalzustand, Leer-/Lade-/Fehler-/Abbruchzustand, Auswahl,
Tastatur-/Fokusweg und eine kleine/helle/dunkle Darstellung. Neue Gestaltungs-
richtungen zunächst getrennt vergleichen; bestehende Freigaben erhalten.
Nach Auswahl Referenz, genaue Werte, Datenannahmen und Abnahme im Gesamtauftrag
festhalten. Diese Liste erstellt selbst noch keine dieser zusätzlichen Vorschauen.
