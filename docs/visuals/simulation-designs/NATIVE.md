# Messlabor · native Umsetzung

Die native Ansicht verwendet C, SDL, Nuklear und OpenGL. Die Browseransicht bleibt
eine Gestaltungsreferenz. Ausgewählt ist ausschließlich **B · Wertefelder**.

## Bedienung

Im Arbeitsbereich **Simulieren** stehen Laufsteuerung und Tempo über der Szene,
Messwerte und die optionalen zwei Messkurven darunter. **Szenenbaum**, **Inspektor**
und **Kurven anzeigen** sind unabhängig. Bei einer unveränderten Standardanordnung
beginnt die Sitzung mit geschlossenem Baum, offenem Inspektor und geschlossenen
Kurven. Ein Arbeitsbereichwechsel
erhält die innerhalb der Sitzung gewählten Zustände; beim Beenden wird die reguläre
Docking-Anordnung gespeichert, keine neue automatische Sichtbarkeitseinstellung.
Bewusstes Speichern in den Einstellungen und benannte Panelanordnungen behalten
ihren vorhandenen ausdrücklichen Speichervertrag. Einstellungen aus Simulieren
übernehmen die dort gewählte Anordnung; Abbrechen erhält den Sitzungsstand.

### Feste Plätze und echte Zusatzfenster

Die ergänzende Nutzerentscheidung vom 20. Oktober ersetzt das freie interne
Andocken: Die Plätze bleiben fest. Panelüberschriften lassen sich nicht mehr
verschieben, zu Tabs zusammenführen oder innerhalb des Hauptfensters schweben
lassen. Seitenbreiten und Bereichshöhen bleiben verstellbar.

Ein Bereich wird an seiner **vorhandenen Überschrift** aus dem Hauptfenster
herausgezogen und außerhalb losgelassen. Workspace/Szenenbaum, Editor/Szene,
Protokoll, Inspektor, Messbereich und Zeitleiste erhalten jeweils ein echtes
SDL-Betriebssystemfenster. Die vorhandenen Dokumenttabs dienen als Editorgriff;
für die Szene dient die bestehende Überschrift „SZENE / MESSLABOR“.
Ziehen und Loslassen innerhalb des Hauptfensters ordnet keine Panels um.
Es werden **keine Symbole, Leisten, Platzhalterkarten, zusätzlichen Abstände
oder Materialflächen** zum festgelegten Design hinzugefügt. Die ursprünglichen
Plätze bleiben frei, solange ihr Inhalt ausgelagert ist. Das native Fenster-X
setzt den Bereich an seinen festen Platz zurück. Dabei bleiben gewünschte Größen,
betrachtete Zeit, Kamera, Auswahl,
Editorinhalt und Undo sowie der laufende Runner erhalten.

Die vier allgemeinen Panels folgen dem gewählten Arbeitsbereich, auch wenn ihr
Fenster ausgelagert ist. Messbereich und Zeitleiste behalten ihre Simulationsfunktion.
So entsteht beim Wechsel von Entwickeln zu Simulieren keine zweite, widersprüchliche
Editor- oder Inspektorinstanz. Die Fenster haben eigene Eingaben, Accessibility-Modelle,
Nuklear- und GL-Kontexte. Sie teilen unveränderliche Schrifttexturen und den einen
Anwendungszustand. Vor dem Zurückholen wird noch anstehende Texteingabe verarbeitet.
Die Fensterposition wird nicht auf das Hauptfenster begrenzt; negative Koordinaten
und weitere Monitore sind möglich. Auslagerungen sind Sitzungszustand.
Die Tastaturalternative ist Ctrl+Shift+Enter auf einer fokussierten
Bereichsüberschrift beziehungsweise im Zusatzfenster (auf macOS Cmd statt Ctrl).
Die drei bisher reinen Simulationsüberschriften bieten dieselbe Aktion über
das native Accessibility-Modell, ohne ein zusätzliches sichtbares Bedienelement.

Alte Layoutdateien bleiben lesbar. Größen und Sichtbarkeit werden übernommen;
alte freie Topologien werden bei der Anzeige in feste Plätze übersetzt. Vor dem
automatischen Überschreiben einer solchen Einstellungsdatei wird deren Original
einmalig unter **preferences.bin.before-fixed-panels** gesichert. Benannte alte
Layoutkataloge werden beim Anwenden nicht umgeschrieben.

Die vorhandenen Docking-Trenner ändern die Seitenbreiten. Die drei zusätzlichen
Trenner ändern Szene/Messbereich, Zeitleistenhöhe und Kurvenverhältnis. Ziehen
verwendet Pointer-Capture, Escape verwirft den laufenden Drag. Fokussierte innere
Trenner reagieren entlang ihrer Achse auf Pfeiltasten (10 Pixel, mit Shift 40),
Home/End gehen zu den Grenzen und Enter oder Doppelklick setzt zurück. Die
Kurventrennung rechnet Pixelbewegungen in das tatsächliche Verhältnis um. Die
neun Pixel breiten Trefferflächen zeigen bei Hover/Fokus eine durchgehende
Akzentlinie und den passenden Größenänderungs-Cursor. Fokusverlust oder Resize
bricht einen laufenden Drag ab. Die Größen werden gegen Fenster und verbleibende
Mitte begrenzt; bloßes Verkleinern erhält die gewünschten inneren Bereichsgrößen.

Der Inspektorkopf einschließlich Schließen bleibt fest. Die Fachgruppen scrollen
unabhängig. Modellwert-Hinweise, Build/Verwerfen und wichtige Runner-Warnungen
stehen vor diesem Scrollbereich. Lange Fehlermeldungen haben einen begrenzten
eigenen Scrollbereich. Objektpositionen stammen aus der tatsächlich betrachteten
Szene; Hierarchie, Sichtbarkeit und Kamera verwenden denselben Zustand wie der
linke Zugang. Nicht deklarierte Masse oder Integrator-Metadaten werden nicht erfunden.

Die Kurven wählen unabhängig tatsächliche Kanäle. Anzeigeeinheiten verändern
keine Rohdaten. Ungültige und ausgefallene Messungen erzeugen Lücken. Die Vorschau
ist begrenzt und gegebenenfalls ausgedünnt; vollständige Daten verbleiben in der
Messdatei. Szene, Werte und Zeitzeiger verwenden die betrachtete Zeit. Live kehrt
zur aktuellen beziehungsweise letzten Szene zurück. Offline bedeutet maximale
Runner-Geschwindigkeit. Historische Wiedergabe verändert den Runner nicht.

## Explizite Modellwert-Bindung v1

Die neue Bindung ist optional. Nur eine unmittelbar folgende, vollständige
numerische Deklaration wird freigegeben. Beispiel C:

```c
// @physim-model-value-v1 length_m m 1 0.1 10 Fadenlänge
static const double length_m = 1.5;
```

Entsprechend in Physim:

```text
// @physim-model-value-v1 lengthDefault m 1 0.1 10 Fadenlänge
let lengthDefault: Float64 = 1.5
```

Nach Versionskennung und Kennung folgen Anzeigeneinheit, positiver Faktor von der
Anzeigeeinheit zur Quelleneinheit, Minimum und Maximum in Quelleneinheiten sowie
die sichtbare Bezeichnung. Für Zentimeter wäre beispielsweise `cm 0.01 0.1 10`
zulässig. Die Quellendeklaration bleibt ein Float64-Wert in der Modellkonvention.
Der Faktor ist ausdrücklich deklariert; es gibt keine automatische Dimensions-
oder Massenerkennung aus beliebigem Quellcode.

Höchstens 32 Bindungen und 256 KiB UTF-8-Quelle werden indexiert. Kommentare,
Strings, lokale Definitionen, bedingte C-Präprozessorzweige, Makros und Ausdrücke
erteilen keine Schreibfreigabe. Unbekannte Versionen, ungültige aktive Bindungen
und doppelte Kennungen sperren die Bindung. Änderungen ersetzen ausschließlich
die zuvor validierten numerischen Token des exakten Quellensnapshots. Alle anderen
Bytes einschließlich Zeilenenden und unveränderter Literalschreibweisen bleiben
erhalten. Dies ist ein enger deklarativer Vertrag, kein allgemeiner C-/Physim-Parser.

Die Pendelbeispiele deklarieren Fadenlänge und Masse als Quellstandards. **Bereits
gewählte Laufparameter bleiben beim Build erhalten und können diese Standards
übersteuern.** Wer den neuen Quellstandard auch als Laufparameter verwenden möchte,
wählt dort ausdrücklich **Standard**. Bestehende Projekte ohne Bindungen erhalten
beim Öffnen keine eingefügten Annotationen. Ihr Laufparametervertrag bleibt erhalten.

Tippen merkt Werte nur vor. Bei laufender oder pausierter Simulation bietet die
Ansicht **Stoppen & neu kompilieren** an. Erst nach Ende des Runners wird die
gebundene Quelle konfliktgeprüft gespeichert und gebaut. Nach erfolgreichem Build
wird ein neuer Lauf angeboten. Frühere Läufe bleiben erhalten. Neue Eingaben
während eines Builds bleiben vorgemerkt; der Abschluss bestätigt nur die tatsächlich
eingereichten Quellwerte. Dirty-Editor, externe Dateiänderung, Buildfehler und
Abbruch erhalten die Eingaben und zeigen eine Erklärung. Verwerfen setzt auf den
zuletzt bestätigten Stand zurück, soweit der Quellensnapshot konfliktfrei ist.

## Gestaltungsabweichungen und Prüfgrenzen

Die Szene zeigt die vorhandenen echten Modellgeometrien und Projektionen.
Illustrative Browserkugel, Schatten, künstliche Winkelbögen und Demomesswerte
werden nicht übernommen. Farben der Szenenfläche folgen dem semantischen Theme.
Native Schrift und Rasterung unterscheiden sich zwischen Segoe UI und der
Linux-Ersatzschrift. Größere Werte verwenden dauerhafte Font-Handles und passen
sich bei knapper Kartenbreite an, ohne ihre vollständige Accessibility-Beschriftung
zu verlieren. Große Schrift und schmale Seiten ordnen Eigenschaften einspaltig an.

Die Messkarten zeigen die zwei ausgewählten echten Kanäle; Namen, Einheiten,
Anzahl und Zahlenformat folgen deren Metadaten. Die Vorschau zeigt dagegen
zusätzliche illustrative Pendelwerte. Die vorhandene Timeline bietet außerdem
Einzelschritte zwischen gespeicherten Zuständen. Native Mindestbreiten bleiben
größer als die 190-px-Demoseitenleiste, damit die vorhandenen Fachkontrollen
bedienbar bleiben. Bei gleichzeitig offenen Seiten und Kurven am Mindestfenster
wird die Szene entsprechend klein; die Aufteilung lässt sich verändern.

Das direkte Öffnen der lokalen HTML-Datei wurde vom verfügbaren Browserwerkzeug
wegen seiner Beschränkung auf HTTP/HTTPS abgelehnt. Die gelieferten PNG-Referenzen
und der lokale Quelltext wurden als Referenz verwendet. Native Fensterbilder und
ausgeführte Testresultate werden separat festgehalten. SDL-injizierte Bedienabläufe
belegen keine manuelle Screenreader-Abnahme. Ein vorhandener CI-Job belegt keinen
ausgeführten macOS-Test; diese Plattform bleibt ohne tatsächlichen Lauf offen.

## Native Fensterbilder

Die PNGs sind unveränderte Formatkonvertierungen der echten OpenGL-Aufnahmen,
keine nachgebauten Browserbilder. Maße, Herkunft und Hashes stehen im
Prüfbericht `native-validation.json`.

| Zustand | Nachweis |
| --- | --- |
| Windows, Anfangszustand | [Dunkel](native/windows-dark.png) |
| Beide Seiten und Kurven | [Windows dunkel](native/windows-dark-curves.png), [Windows hell](native/windows-light-curves.png), [Linux hell](native/linux-light-curves.png) |
| Große Schrift am Mindestfenster | [Windows](native/windows-large-minimum.png), [Linux, Kurvenverhältnis an der Grenze](native/linux-large-curve-limit.png) |
| Hoher Kontrast und Historie | [Hochkontrast](native/windows-high-contrast.png), [Rückblick](native/windows-history.png) |
| Vorgemerkte Modellwerte | [Pausierter Lauf](native/windows-model-staged.png), [neuere Eingabe nach älterem Build](native/windows-model-newer-edit.png) |
| Fehler und erfolgreicher neuer Build | [Buildfehler](native/windows-model-build-error.png), [bereit](native/windows-model-ready.png) |
| Herausgezogene Bereiche ohne zusätzliche Leisten | [Szene](native/windows-detached-scene.png), [Inspektor](native/windows-detached-inspector.png), [Zeitleiste](native/windows-detached-timeline.png) |
| Ausgelagerte echte Messwerte und Kurven | [Windows](native/windows-detached-values.png), [Linux](native/linux-detached-values.png) |

Die abschließenden gezielten Windows- und Linux-Prüfungen sind bestanden.
Die normale Simulationsansicht wurde zusätzlich bei 1440 × 1000 Pixeln mit dem
vorherigen nativen Stand verglichen: **auf beiden Plattformen null abweichende
Pixel**. Das ist ein Nachweis für diesen Zustand und diese Dichte, keine Behauptung
über sämtliche Zustände oder gemischte Monitor-Skalierungen. Die breite frühere
Testsuite hatte 744/749 erfolgreiche Fälle; ihre fünf offenen Befunde und der
separat erfolgreiche Optionalwerte-Nachlauf stehen im Prüfbericht.

Die [anfänglich falsche Startfarbe](native/counterexample-startup-theme.png) und
die [verletzte minimale Kurvenbreite](native/counterexample-curve-limit.png)
bleiben als ausdrücklich fehlgeschlagene Gegenbeispiele erhalten. Die positiven
Prüfungen verlangen anschließend korrekte Canvas-Pixel beziehungsweise mindestens
179 tatsächlich zugewiesene Pixel pro Kurvenfeld einschließlich Rundung.
