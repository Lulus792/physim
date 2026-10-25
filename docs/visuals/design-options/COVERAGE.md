# Abdeckung der fehlenden Inhalte

25. Oktober 2026 · [konkrete Auswahl freigegeben](APPROVED.md), native Abnahme offen.
Grundlage ist das [Inhaltsinventar vom 24. Oktober](../unified-workspace/CONTENT-INVENTORY.md).
Die Varianten stellen Inhalt und Interaktion dar; sie implementieren keine
fehlenden nativen Fachfunktionen neu.

| Inventarpunkt | Wo vergleichen? | Gezeigte Inhalte / verbleibende Grenze |
| --- | --- | --- |
| Gewicht, Stangenkraft, Widerstand und Geschwindigkeit | Szene & Werte · A/B/C | Unterschiedliche Pfeile, Symbole, Maßstäbe, explizite Beispielbeträge, Auswahl; kein Ableiten physikalischer Werte aus beliebigen Pfeilen |
| Szenentexte und lange Beschriftungen | Szene & Werte · Zustand „Lange Beschriftung“ | Textkontur, Sichtbarkeit, Umbruch, Beschriftung als eigener auswählbarer Typ |
| Zehn Objekttypen | Szene & Werte · Szenenbaum | Kugel, Linie, Box, Vektor, Punkt, Ebene, Pfad, Beschriftung, Gruppe, Koordinatenrahmen; gemeinsame/typspezifische Beispiel-Eigenschaften, kein vollständiger Geometrieeditor |
| Hierarchie und wechselnde Auswahl | Szene & Werte · Baum und Zustand „Auswahl nicht mehr vorhanden“ | Verschachtelte Gruppen, IDs, Weltbezug und lokaler Rahmen; verständliche Wiederherstellung einer Auswahl |
| Beliebige 2D-/3D-Szenen | Szene & Werte | Flächenbedarf, Überlagerung und räumlicher Kontext am SVG-Pendel; Kamera-/Picking-/Transparenzrenderer bleibt die bestehende native Aufgabe |
| Weitere Modellparameter | Szene & Werte → Modellwerte | Acht Parameter aus dem echten Pendelbeispiel, Grenzen und SI-Einheiten; Vorlage ist `examples/pendulum/main.c` |
| Adaptive Schritte und Integratorabhängigkeit | Szene & Werte → Modellwerte | Minimum/Maximum, RK45, ungültiges Verlet mit Luftwiderstand; UI-Prüfung statt Numeriklauf |
| Weitere Messkanäle | Szene & Werte → Messkanäle | Elf unterschiedliche Namen, Einheiten, kleine Werte und fehlender Wert, Listen und Karten |
| Modellmeldungen / Warnungen / Abbruch | Protokoll · A/B/C | Schweregrad, Bereich, Zeit, fehlende Quellstelle, Folgen, Rohtext, Flutunterdrückung; Fehler-Level allein führt zu keinem simulierten Abbruch |
| Start, Runner-Antwort, Limits, Aufzeichnungsfehler | Läufe & Serien · Zustandswahl | Startet, keine Antwort, Speichergrenze und validierter Teil einer Aufzeichnung; keine native Prozesssteuerung |
| Gespeicherte Zeitleisten / unvollständige Daten | Läufe & Serien sowie Auswertung | Lange Aufzeichnung und erreichbarer Zeitpunkt, Wiederherstellungskennzeichnung; keine erfundenen Messungen oder Zwischenbilder |
| Serien / Parameterstudien | Läufe & Serien · A/B/C | Sechs Läufe, Parametervariation, Seeds, Endkanal, Teilfehler und Abbruch mit erhaltenen Ergebnissen |
| Mehrere Compilerdiagnosen | Protokoll · A/B/C | Mehrere Dateien/Kontexte, Warnung/Fehler, Originaltext und Quellbeispiel; reale GCC-/MSVC-Verarbeitung bleibt nativ |
| Projektdateien und echte Dateifehler | Datei-Explorer · A/B/C | Projektordner, Code, Manifest, Assets, Messläufe, langer Dateiname, versteckte Einträge; Suche, Vorschau, Konflikt-/Fehlerzustände |
| Sprachen und reale Buildprozesse | Projekt-Popup, Datei-Explorer, Protokoll | Separate C-/Physim-Wahl, entsprechende Beispieldateien und Diagnosen; kein Compiler-/Linkerlauf in der Galerie |
| Große API-/Dokumentationsinhalte | Explorer-Vorschau und Protokoll-Quellkontext; bestehende F1-Referenz | Lange Code-/Originaltexte erhalten. Der schon laufende umfangreiche F1-/API-Abgleich wird nicht durch einen neuen vollständigen Handbuchentwurf ersetzt; vorhandene [F1-Referenz](../f1-layouts/index.html) weiterverwenden |
| Laufbibliothek, Herkunft und Dateifehler | Auswertung sowie Explorer | Langer Laufname, Metadaten, leere Bibliothek, wiederhergestellter Lauf; Öffnen im Beispiel ist kein realer Import |
| Unterschiedliche Kanalschemata | Auswertung · „Kanal fehlt in einem Lauf“ | Betroffener Lauf und fehlender Kanal benannt, übrige Daten erhalten |
| Variable Analyseausgaben | Auswertung · A/B/C | Text, zwei Diagramme, Tabelle, Gliederung und Herkunft; festgehaltener Bericht gegen geänderte Laufauswahl |
| Große Datenmengen / Grenzwerte | Läufe & Serien, Messkanäle, Protokollflut | Lange Dauer, wissenschaftliche Schreibweise, fehlende Werte, viele Meldungen; keine behauptete Millionen-Punkte-Performanceprüfung |
| Echte Export-/Dialogfehler | Bestehende freigegebene Exportreferenz | Der freigegebene Export bleibt unverändert. Keine neue Exportvariante oder nachträgliche Aufhebung seiner Freigabe |

## Neu hinzugekommen

- **Projekt-Popup:** A mit Vorschau/Formular wie beim Export, B als Dreischritt,
  C mit Vorlagennavigation. Inhalt basiert auf `app/project_manager_ui.inc`,
  `app/design_ui.inc` und `create_managed_project` in `app/main.c`. Die acht
  Vorlagennamen entsprechen dem vorhandenen nativen Katalog.
- **Custom Explorer:** drei eigene Darstellungen, auch als verschachtelte
  Ordnerauswahl und als Messlauf-Auswahl. Der sichere Dateizugriff, reale Rechte,
  Verknüpfungen, Laufwerksnavigation und OS-Integration müssen später mit den
  vorhandenen nativen Dateipfad-/Dialogadaptern verbunden werden.
- **Vollständige Leeransicht:** aktuelle gemeinsame Menü-/Bereichs-/Kontextsprache
  ohne geladenes Projekt; eigener Standalone-Zugang, kein automatisch erzeugtes
  Demoexperiment. Ein leerer Start bleibt auch nach dem Schließen der Kandidaten leer.

## Freigegebene Auswahl

| Thema | Freigegebene neue Variante |
| --- | --- |
| Protokoll | C · Navigator |
| Szenen-/Wertedarstellung | A · Werkbank |
| Variable Auswertungsinhalte | A · Werkbank |
| Läufe und Serien | A · Werkbank |
| Projekt-Popup | B · Fokus / drei Schritte |
| Custom Explorer | A · Werkbank |
| Vollständige Leeransicht | Vom Nutzer bestätigt; native Abnahme offen |

Diese direkte Nutzerentscheidung ist die Grundlage für die weitere Umsetzung.
Die getrennten Varianten bleiben Vergleichsquellen; die ausgewählte Kombination
ist [gemeinsam aufrufbar](index.html?approved=1&topic=empty).
