# Technische Übergabe: Messlabor mit vollständigem Inspektor

Die native Umsetzung und ihr enger Modellwert-Bindungsvertrag werden in
[NATIVE.md](NATIVE.md) dokumentiert. Die folgenden Abschnitte bleiben als
Anforderungs- und Abnahmegrundlage erhalten; sie sind selbst kein Testnachweis.

Stand 19. Oktober 2026. Der Nutzer hat **Simulieren nach Messlabor umbauen**
bestätigt und zusätzlich einen Entwurf mit allen vorhandenen Inspektorfunktionen
angefordert. Diese Datei und [NEXT-CHAT.md](NEXT-CHAT.md) bereiten die Umsetzung
vor. Die native Anwendung ist durch diese Übergabe nicht umgestellt.

## Maßgebliche Artefakte

Neue Anforderungen: [Datendarstellungen, Größenänderung und Modellbearbeitung](MODEL-EDITING.md).
Die Abschnittsstruktur bleibt erhalten; neun Gruppen einschließlich der neuen
quellen-/buildgebundenen **Modellwerte**. **B · Wertefelder ist ausgewählt**:
Label über größerem Wert, dezente Flächen, eindeutige Einheiten; Eigenschaften
bei geeigneter Breite zweispaltig. A/C sind nur Vergleich, keine zusätzlichen
nativen Darstellungsmodi. Referenzen: `inspector-fields-dark.png` und
`inspector-fields-light.png` sowie die aktuellen `labor-*.png`-Fensterbilder.

- [Auswahl ADR 008](../../decisions/008-simulation-messlabor.md), [Design](DESIGN.md).
- [Lokale Vorlage](index.html), ohne Server; Standard Messlabor.
- Start: [Dunkel](labor-dark.png), [Hell](labor-light.png).
- Kurven sichtbar: [Dunkel](labor-dark-curves-open.png), [Hell](labor-light-curves-open.png).
- Alle rechten Gruppen: [Dunkel](inspector-dark-overview.png), [Hell](inspector-light-overview.png).

Die HTML-Galerie und ihre Beispielaktionen sind keine Produktfunktionen. Die
separate [Auswertungsübergabe](../analysis-workspace/NEXT-CHAT.md) bleibt erhalten.
Alle folgenden Quellpfade beziehen sich auf die Repositorywurzel. Funktionsnamen
sind Suchanker; vor Umsetzung aktuellen Stand und lokale Änderungen erneut prüfen.

## Zuordnung zum tatsächlichen Bestand

| Bereich | Vorhandene Stellen | Übergabe |
| --- | --- | --- |
| Simulationsfläche und Steuerung | `app/design_ui.inc`: `workspace_panel`, Zweige `a->tab == 1`; `app/main.c`: `start_run`, `reset_run`, `command`, `select_simulation_speed` | Kopf, Szene, Messwerte, optionale Kurven und Zeitleiste anordnen; Runner-Zustandsmaschine erhalten. |
| Rendern und betrachteter Zustand | `app/main.c`: `viewport`, `display_scene`, `display_time`, `display_values` | Reale OpenGL-Fläche; Live und historischen Snapshot durchgängig unterscheiden. |
| Rechte Seite | `app/design_ui.inc`: `inspector_panel`, `camera_inspector` | Bestehende Funktionen in unabhängig scrollbare Gruppen ordnen; API-Referenzkontext anderer Arbeitsbereiche erhalten. |
| Modellparameter | `parameter_form_view`, `app/parameter_catalog.c/.h` | Modellabhängige Definitionen, Einheiten, Bereiche, Standardaktionen, Dirty- und Laufzeitsperren erhalten. |
| Buildgebundene Modellwerte (neu) | Bestehende Editor-/Dokument-, Projekt- und Buildpfade; bislang keine universelle schreibbare Bindung im Parameterkatalog | Typisierte Bindung ergänzen, Änderungen vormerken, Quellenkonflikte erkennen, kontrolliert bauen und erst im neuen Lauf anwenden; MODEL-EDITING.md ist verbindlich für diesen Zusatz. |
| Live-Daten und Meldungen | `live_measurements_view`, `live_events_view`, `live_event_text`; `history_t`, `history_v`, `history_count` in `app/main.c` | Aktuelle Einzelkurve vom Messwertzugang trennen. Zwei optionale Kurven mit echten, begrenzten Daten versorgen. Warnungen/Protokollzugang behalten. |
| Laufeinstellungen | `camera_inspector`; `simulation_settings_valid`, `save_project_settings` in `app/main.c` | dt, Seed, Adaptivität und Schritt-Haltepunkte weiter validieren/speichern. Keine neue Parallelvalidierung mit abweichenden Grenzen. |
| Laufgrenzen | `inspector_panel`: `runner_memory_mib`, `runner_wall_seconds` | 0 = unbegrenzt; Speicher bis 16384 MiB, Zeit bis 86400 s; vorhandene Sperre bei nicht untätiger App erhalten. |
| Szenenbaum/Objekt/Kamera | `scene_controls_view`, `app/scene_tree_ui.inc`; Szene/Kamera in `app/main.c` | Baum links zunächst zu; optionaler vollständiger Zugang rechts verwendet dieselben Auswahl-/Sichtbarkeits-/Kamerawerte. |
| Zeitleiste | `app/timeline_ui.inc`, `app/timeline.c/.h`; `timeline_select`, `timeline_live`, `timeline_tick` | Reale Historie, Live-Rückkehr und Wiedergabe erhalten. |
| Plots/Einheiten | `app/plot_ui.inc`, `app/plot_view.c/.h`, `app/channel_units_ui.inc` | Vorhandene Darstellung/Konversion verwenden, Statusmasken und Grenzen der Vorschau erhalten. |
| Anordnung und Themes | `app/docking_ui.inc`, `app/docking.c/.h`, `app/layout_ui.inc`, `app/preferences.h`, `app/design_tokens.h` | Kontextzustände vom gemeinsamen Docking unterscheiden; gespeicherte Anordnungen und Skalierung erhalten. |
| Assistive Bedienung | `app/accessibility.h`, plattformspezifische Accessibility-Dateien in `app/` | Native Namen, Rollen, Zustände, Grenzen und Fokuspfade der neuen Kontrollen ergänzen. |

## Vollständiger Inspektorvertrag

Der bestehende Inspector verwendet in **Simulieren** Parameter, Live-Messwerte,
Ereignisse, Laufeinstellungen und Laufgrenzen. Die Kamera-/Darstellungsfunktionen
liegen hier im Bestand überwiegend in `scene_controls_view` links; in anderen
Kontexten kann `camera_inspector` sie rechts zeigen. Die neue gemeinsame rechte
Gruppe ist deshalb eine Zusammenführung von Zugängen, keine Behauptung, dass
bereits alle Gruppen in Simulieren rechts existieren.

| Gruppe im Entwurf | Inhalt und zu erhaltendes Verhalten |
| --- | --- |
| Objekt | Reale Auswahl, Typ/ID, vorhandene Geometrie, lokale/Weltkoordinaten, bei Frames Skalierung; Auswahl ausblenden, alle Objekte einblenden. Keine erfundene Masse für beliebige Objekte. |
| Modellparameter | Nur nach erfolgreicher Beschreibung deklarierte Parameter; Name/Beschreibung/Einheit/Bereich/Standard, Wertfeld, Standardaktion, Validierungsfehler und Dirty-Zustand. Gesperrt während Runner/Reset. |
| Laufeinstellungen | Positives endliches dt bis 1 s; adaptive Schritte mit `0 < Minimum ≤ Start ≤ Maximum ≤ 1 s` und Modellfähigkeit; Seed als uint64; bis zu 32 verschiedene Schrittzahlen 0…1000000000 oder leer. Bestehende Laufzeitsperren erhalten. |
| Laufgrenzen | Speicher/Walltime mit bestehenden Eingabegrenzen; Nullbedeutung, Pausenzeit und eigener Serienvertrag bleiben verständlich. |
| Messkanal & Einheit | Verfügbare reale Kanäle, kompatible Anzeigeeinheiten, betrachteter Messwert und Gültigkeitsstatus. Kurvenfläche separat über Kurven anzeigen. Keine Rohdatenänderung durch Anzeigeeinheit. |
| Ereignisse | Level/Zeit/Meldung, Warnungen, ausgelassene ältere Einträge und echtes Protokoll. Fehlende Runner-Antwort weiter erkennbar und Stop erreichbar. |
| Kamera & Darstellung | Standard/Vorne/Seite/Oben, 2D-Modus/Ebene, orthografisch, Orbit/Neigung/Zoom, Maus/Tastatur; Vektoren/Pfade/Punkte/Labels/Raster und Achsen; echte Projektvorgabe speichern. |
| Aufzeichnung | Tatsächlicher Laufpfad und verfügbare Inhalte. Kein fixer Dateiname oder vermeintlicher Aufzeichnungsstatus ohne echte Daten. |

API-Referenz, Entwickeln, Auswerten, gespeicherte Layouts und Dateinavigation
behalten ihre bestehenden Zugänge. Diese Übergabe ersetzt keine Fachfunktionen
durch die wenigen Werte des Beispielpendels. Insbesondere ist der Integrator
vom jeweiligen Modell bestimmt; das RK4-Label der Demo erzeugt keinen universellen
Integratorwahlschalter.

## Daten, Zustände und Geometrie

Alle Inhaltsbereiche skalierbar gestalten: Seitenbreiten, Szene/Messbereich,
Zeitleiste und Kurvenverhältnis. Die neue Browservorlage demonstriert sechs
Trenner einschließlich ihrer freien Gesamthöhe. Native Größen aus Dock-Rechtecken
und Schrift ableiten; verfügbare Mitte begrenzen, Pointer-Capture sauber lösen,
Tastatur und Rücksetzen anbieten. Bereichsänderungen erhalten Feldwerte,
Staging-/Buildzustand, Zeit, Kamera und Objektauswahl. Zusätzliche Tests für
Extremgrößen, abgebrochenen Drag, Fensterverkleinerung und gespeicherte Layouts.

Anfang: Baum zu, Inspektor auf, Kurven zu. Drei getrennte Sichtbarkeitszustände
führen; nicht für die neue Ansicht die globalen Docking-Voreinstellungen anderer
Bereiche überschreiben. Themewechsel und Wiedereintritt erhalten Benutzerauswahl.
Bestehende persönliche Layouts beim Einführen des Anfangszustands berücksichtigen.

Die Vorschau zeigt beispielhaft zwei Kanäle; nativ alle tatsächlichen Kanäle
erreichbar halten und höchstens zwei gleichzeitig ausgewählte Verläufe in dieser
kompakten Fläche zeigen. Weniger vorhandene Kanäle ergeben weniger Kurven bzw.
eine Erklärung, keine erfundenen Werte. Kanalwahl darf nicht unbemerkt den
Auswertungsbereich oder den zweiten Kurvenkanal verändern.

Kurven verwenden echte Zeitvektoren, Einheiten und Gültigkeitsmasken. Die
vorhandene begrenzte Historie wird bei Bedarf ausgedünnt. Daraus weder komplette
Messdateien noch unveränderte Abtastraten vortäuschen. Beim Rückblick stammen
Szene/Einzelwerte aus dem betrachteten Snapshot; der Zeitzeiger der Kurven stimmt
damit überein. Ein ggf. begrenzter Historienbereich muss erkennbar sein. Kein
ungeprüftes Nachladen der ganzen Datei auf dem UI-Thread, keine sampleindexbasierte
Paarung unterschiedlicher Reihen und keine erfundene Interpolation.

Die native Breite/Höhe stammt aus dem aktuellen Fenster/Dock-Rechteck, nicht aus
den festen Browserreferenzen. Freie Fläche nach Schließen einer Seite oder der
Kurven nutzen; keine Nullbreite der Mitte, Überlagerung von Plot/Viewport oder
unzugängliche Aktionen. Kopf und Schließzugang bleiben außerhalb des scrollenden
Inspektorinhalts. Große Schrift darf Gruppenhöhe und Scrollbedarf ändern.

## Schritte und Abnahme

1. Aktuellen Stand und native Ausgangsansicht mit echten Daten erfassen.
2. Kontextzustände, Seitenzugänge, Szenenfläche und Messwert-/Kurvenbereich
   implementieren; acht Sichtbarkeitskombinationen prüfen.
3. Vollständige Inspektorgruppen mit vorhandenen Fachaktionen verbinden.
4. Zwei reale Verläufe, Kanalauswahl, Einheiten, Masken und historische Zeit
   integrieren; Sonderfälle und Ressourcenverbrauch prüfen.
5. Native Funktions-, Accessibility- und visuelle Abnahme ausführen; ausgeführte
   Kommandos, Exitcodes, Umgebung, Fensterbilder und offene Grenzen dokumentieren.

Vorhandene Einstiegstests: `app/inspector_tests.inc`, `app/parameter_form_tests.inc`,
`app/channel_units_tests.inc`, `app/reset_tests.inc`, `app/speed_tests.inc`,
`app/timeline_tests.inc`, `app/hierarchy_tests.inc`, `app/scene_visibility_tests.inc`,
`app/docking_tests.inc`, `app/layout_tests.inc`, `app/series_mask_tests.inc` und
passende Accessibility-Tests. Vor Nutzung alle Dateinamen am aktuellen Stand prüfen.
Fachliche Assertions nicht bloß an die neue Optik anpassen und damit abschwächen.

Das vorhandene CLI unterstützt unter anderem folgende **geplante** Prüfläufe:

```text
python tools/build.py --config Debug --test
python tools/build.py --config Debug --test-display --test-filter inspector_workflow --test-filter timeline_workflow --test-filter reset_workflow --test-filter speed_workflow --test-filter parameter_form_workflow --test-filter live_inspector_22_workflow
```

Weitere berührte Fach-/Darstellungsfälle gezielt ergänzen. Diese Kommandos wurden
für die Dokumentationsvorbereitung nicht als native Tests ausgeführt.

| Abnahmefall | Erforderlicher nativer Nachweis |
| --- | --- |
| Anfang und Wiederöffnung | Baum zu/Inspektor auf/Kurven zu beim neuen Kontext; eigene Zustände und gespeicherte Layouts bleiben respektiert. |
| Acht Kombinationen | Tatsächliche Sichtbarkeit und nutzbare Szenenrechtecke; andere Zustände, Kamera und Zeit unverändert. |
| Echte Modelle | Mindestens Pendel und anderes Modell, C/Physim, keine festen Demo-Kanäle/Objekte/Parameter. |
| Inspektor | Jede obige Funktion erreichbar, korrekte Sperren/Validierung, keine Funktionsverluste durch Scrollen oder Schließen. |
| Eingabegrenzen | Gültige/ungültige Parameter, adaptive Grenzen, Seed 0/uint64-Maximum/Überlauf, doppelte/zu viele Haltepunkte, Limits. |
| Modell bearbeiten und bauen | Typisierte Quellen-/Buildbindung, Einheiten, gültige/ungültige Änderung, Verwerfen, externe/Dirty-Konflikte, Buildfehler/Abbruch, neuere Eingaben während Build; Übernahme erst im neuen Lauf. |
| Größen ändern | Alle Inhaltsbereiche per Pointer/Tastatur, Mindest-/Maximalgrößen, sichtbare Fokusmarke, Capture-Ende, Rechtecke von Szene/Plots und Erhalt aller Fachzustände. |
| Runner | Build/Initialisierung, Run/Pause/Schritt/Stop/Reset, alle Tempi/Offline, Timeout, Fehler, Abbruch, neue Messdatei ohne Verlust bisheriger Läufe. |
| Historie | Rückblick und Wiedergabe während Live-Lauf, Live-Rückkehr, fehlende Szene, ungültige Messwerte, begrenzte Vorschau. |
| Darstellung | Beide Themes/Hochkontrast, Mindestfenster, Größenwechsel, große Schrift und tatsächliche DPI-Konstellationen soweit verfügbar. |
| Tastatur/Assistive | Fokus, Enter/Leertaste, Gruppen, Schalterzustand, Ein-/Ausblenden mit Fokuskehr, vorhandene Szenen-/Kamerashortcuts. |
| Nebenbereiche | Entwickeln, Auswerten, F1, Bibliothek, Analyseprojekt mit importierter Szenenwiedergabe und gespeicherte Anordnungen. |

Browserprüfung: `node tools/check_simulation_designs.cjs` prüft die Vorlage;
`--export` erneuert bewusst ihre Referenzbilder. Sie ersetzt weder echte native
Fensterbilder noch ausgeführte Linux-/macOS-Builds/Tests. Die Browseraktion
„Als Projektvorgabe speichern“ schreibt beispielsweise ausdrücklich keine Datei.
