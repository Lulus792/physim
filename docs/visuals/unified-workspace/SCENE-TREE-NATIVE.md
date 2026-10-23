# Szenenbaum und linke Darstellung im Messlabor

Teilabgleich auf Grundlage von `163ed89f1b2e05c0c4db15f42f4eef167148aef7`.
Die gesamte Oberfläche bleibt im verbindlichen Referenzabgleich. Dieser Bericht
belegt keine vollständige Design-, Plattform- oder Accessibility-Abnahme.

Maßgeblich sind `simulation-designs/index.html`, die Baumregeln in
`simulation.css`, die gemeinsame Palette aus `bridge.js` und der jüngere
[Vertrag für die Trenner](RESIZING.md). Der separate Szenentext, die Objekttypen
und die tatsächlich vorhandene Hierarchie bleiben fachlich gebunden.

## Gestaltung und Verhalten

Die Hierarchie beginnt jetzt unmittelbar unter dem bestehenden Panelkopf.
Die alte vorgeschaltete Auswahl-/Koordinatenfläche und der zusätzliche
Abschnitt „Szeneneinträge“ entfallen in dieser Messlaboransicht; dieselben
Objekteigenschaften bleiben im rechten Inspektor zugänglich. Die Zeilen erhalten
8 px äußeren Seitenabstand, einen schlanken Chevron beziehungsweise Zweig,
13-px-Typzeichen und vollständige Beschriftungen. Wurzelgruppen verwenden
12-px-Schrift und mindestens 33,4 px Höhe, Kinder 11-px-Schrift und mindestens
33 px Höhe. Die Auswahlfläche hat 6 px Radius. Das Auge erscheint bei Auswahl,
Hover, Fokus oder ausgeblendeten Objekten und bleibt eine getrennte Aktion.

Tiefe Einrückung wird auf den verfügbaren Textplatz begrenzt. Passt neben den
Bedienelementen kein sinnvoller Textbereich, steht die vollständige Beschriftung
darunter. Schrift wird deshalb nicht verkleinert. Die Tastaturauswahl klappt
Vorfahren auf und bringt die gewählte Zeile einmalig vollständig in den
sichtbaren Scrollbereich. Auf- und Zuklappen verändert keine Szenengeometrie;
die getrennte Sichtbarkeit wirkt weiterhin auf Darstellung und Picking.

Darunter steht der ruhige Darstellungsbereich der Vorlage: 9-px-Überschrift mit
1,2 px Laufweite, drei 11-px-Filter mit 13-px-Checkboxen und ein 10-px-Hinweis
mit 16 px Zeilenhöhe. Er bleibt bei kurzer Hierarchie am unteren Panelrand.
Bei langen Bäumen sind Baum und Optionen gemeinsam scrollbar. Es gibt keinen
zusätzlichen inneren Scrollbereich und keine neue Auslagerungsleiste.

Die bestehende Überschrift „DARSTELLUNG“ öffnet die weiteren tatsächlichen
Anzeige-/Kameraoptionen. Diese bleiben links erreichbar, wie im Nutzerauftrag
gefordert. Der rechte Kameraabschnitt verwendet dieselben Werte. Die drei
Grundfilter werden links nicht zusätzlich im aufgeklappten Inhalt wiederholt.
Lange Hinweise und Aktionen umbrechen auch bei großer Schrift. Der sichtbare
Standardzustand erhält dafür keine zusätzliche Schaltfläche.

Die linke Messlaborspalte verwendet bei neuer Standardanordnung 190 px,
mindestens 150 px und höchstens 420 px entsprechend dem gemeinsamen
Resizevertrag. Bereits individuell gespeicherte Breiten werden erhalten.
Die übrigen Arbeitsbereiche behalten ihre getrennten Layoutzustände.

## Tatsächliche Daten und verbleibende Unterschiede

Gruppen und Beschriftungen zeigen ihren tatsächlichen Szenentext. Geometrische
Objekte ohne eigenen Anzeigenamen verwenden Typ und stabile ID. Der aktuelle
Objektvertrag enthält keinen allgemeinen Anzeigenamen für jede Geometrie;
die Demobezeichnung „Pendelkörper“ wird deshalb nicht einem beliebigen Objekt
zugeschrieben. Eine fachliche Erweiterung bleibt mögliche Folgearbeit.

Die aufgeklappten Kameraoptionen verwenden weiterhin vier vorhandene
Presetaktionen statt der Dropdown-Darstellung der Vorlage. Dieser Bereich ist
damit noch nicht abschließend angeglichen. Auch der rechte Inspektor besitzt
weiter seinen bisherigen Breitenbereich; die linke Resizekorrektur ist keine
Abnahme sämtlicher Trenner. Die übrigen Inspektorgruppen, Szene, Auswertung,
Werkbank und F1 bleiben Bestandteil des Gesamtauftrags.

## Prüfung und Fehlerverlauf

Die [eingefrorenen Belege](scene-tree-2026-10-23/README.md) unterscheiden die
tatsächlich ausgeführten Stände und bewahren fehlgeschlagene Läufe auf.
Der neue native Prüfweg verwendet einen tatsächlich kompilierten C-Pendelversuch
mit pausiertem Runner. Er prüft Auswahl und automatische Sichtbarmachung per
Tastatur, Zeigerauswahl, Sichtbarkeit einer übergeordneten Gruppe, Aufklappen,
Filter per Zeiger und Enter sowie den gemeinsamen linken/rechten Kamerazustand.
Quelle, Szenensnapshot, pausierte Zeit und aufgezeichnete Messdatei müssen dabei
unverändert bleiben. Die Bildprobe verlangt vollständige native Textbefehle
und sichtbare Glyphen innerhalb der tatsächlichen Zeilen.

Stand 1 besteht fünf von zehn Linux-Gruppen. Der neue Fokusprüfer verwechselt
den semantischen Gruppennamen mit dem internen Nuklear-Fensternamen. Weitere
Alttests setzen den alten Abschnittszustand oder den äußeren statt des
tatsächlichen Inhaltsclips voraus. Diese Fehler werden im Prüfer korrigiert;
die ursprünglichen Verhaltensanforderungen und Zeitgrenzen bleiben erhalten.

Daneben reproduziert Stand 1 einen realen Größenfehler: Mit zunächst kleinem
Testfenster wird vor dem Maximieren 1080 × 740 statt der angeforderten
1200 × 800 gespeichert. Ein älterer lokaler Build besteht denselben Gegenlauf.
Stand 2 synchronisiert deshalb die angeforderte Startgröße vor dem Maximieren.
Der gezielte Gegenlauf und die vollständige Einstellungsgruppe bestehen danach
mit unveränderten Sollmaßen. Der verwendete
[SDL-Vertrag für `SDL_SyncWindow`](https://wiki.libsdl.org/SDL3/SDL_SyncWindow)
beschreibt das Abwarten anstehender Fensterzustandsänderungen; daraus wird keine
allgemeine Fehlerursache aller Plattformen abgeleitet.

Stand 2 deckt anschließend eine zu strenge obere Sicherheitsmarge im
Scroll-/Klickhelfer auf: Der erste Baum-Eintrag liegt direkt am Inhaltsclip,
die Probe verlangt dort aber acht zusätzliche Pixel. Der Lauf wartet deshalb
beim ersten Aufklappen beziehungsweise beim Sichtbarkeitsauge. Die Aufnahme
zeigt den erreichbaren unverdeckten Eintrag. Der genaue Folgeprüfstand und
seine Ergebnisse stehen im Belegmanifest.

Die zusätzliche native Resizeprobe von Stand 2 findet außerdem einen echten
Fokusfehler. Der Debugger zeigt beide vertikalen Trenner als deaktiviert, obwohl
der Host Tastaturfokus hat: Die eigenständigen horizontalen Messlabor-Trenner
überschneiden ihre neun Pixel breiten Eingabeflächen an den Enden. Die allgemeine
Verdeckungsprüfung interpretiert das als Sperre. Stand 3 nimmt diese anderen
Trennerfenster wie bereits die Dock-Trenner ausschließlich für die Separatorrolle
von der gegenseitigen Verdeckung aus. Gewöhnliche Fenster und Popups sperren
weiterhin die darunterliegenden Trenner. Der neue Prüfer verlangt deshalb
zusätzlich Home/End/Enter für 150/420/190 px und echte Menüöffnung mit gesperrtem
Trenner, gefolgt von Escape und wiederhergestelltem Tastaturzugriff.

Stand 3 besteht neun vorhandene Gruppen sowie die nativen Größenassertionen
150/420/190 px. Die neue Probe scheitert erst beim anschließenden F10: Eine
unbedingte Nachbehandlung des bereits fokussierten nativen Controls schließt
das gerade aktivierte Menü. Stand 4 übergibt F10 deshalb ausdrücklich an die
bereits vorhandene gemeinsame Funktion für eigene Tastaturnavigation. Zusätzlich
zum Baumprüfer werden beide vorhandenen Tastaturmenügrößen und die vorhandene
Eingabeisolierung geprüft. Der Baumprüfer fokussiert den Trenner nach Escape
erneut ausdrücklich; eine allgemeine automatische Rückkehr zu jedem früheren
nativen Control wird damit nicht behauptet.

Stand 4 besteht unter Alpine-Dev mit GCC, ASan/UBSan und Xvfb/Openbox **4/4**:
Eingabeisolierung, Tastaturmenü klein/groß und der neue Baumprüfer. Letzterer
besteht beide Schriftquellen in 133,459 Sekunden mit 72 Bildzustandsdatensätzen
und zwei zusätzlichen Menüaufnahmen. Die vorherigen neun Regressionen bestehen
auf Stand 3; zwischen Stand 3 und 4 ändert sich ausschließlich die beschriebene
F10-Übergabe. Sie werden im Manifest ausdrücklich ihrem tatsächlichen Stand
zugeordnet. Der MSVC-Release-Build von Stand 4 ist ebenfalls erfolgreich.

Die Bildkontrolle betrachtet zusätzlich den vollständig geöffneten Baum,
die automatisch sichtbare tiefe Auswahl bei Schrift 22 sowie den unteren
Darstellungsbereich bei zugeklappter Hierarchie. Die eingefrorene Gegenprobe
vergleicht die tatsächlichen Glyphen für den ersten Eintrag und die untere
Überschrift mit dem alten nativen Bild ohne diese Inhalte. Das belegt diese
konkreten Korrekturen, keinen universellen Bildähnlichkeitswert.

System- und mitgelieferte Schriften, Hell/Dunkel, Schriftgrößen 16 bis 22 und
Hochkontrast gehören zum neuen Prüfweg. Diese Größen sind kein Nachweis von
physischer 200-%-DPI oder eines Monitorwechsels. Ein aktueller Windows-Build
liegt vor; ein Windows-GUI-Nachweis dieser Baumänderungen, macOS und praktische
Screenreader-Aufgaben sind hier noch offen. Der getrennte Windows-Abschluss
von Inspektor-Stand 5 ist inzwischen bestanden und gilt nur für dessen Quellen.
