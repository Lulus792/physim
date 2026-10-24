# Nutzerbefunde: noch offene sichtbare Designfehler

Die sieben vom Nutzer eingereichten Ausschnitte vom 21. Oktober zeigen einen
Zwischenstand der tatsächlichen Oberfläche. Sie ergänzen den vollständigen
[Referenzabgleich](NATIVE-FIDELITY.md); diese Liste begrenzt den Auftrag nicht
auf die sieben Aufnahmen. Die vollständige Oberfläche bleibt vor der weiteren
Projektumsetzung zu korrigieren und nativ abzunehmen.

Die Originaldateien liegen unverändert mit ihren SHA-256-Werten unter
[user-review-2026-10-21](user-review-2026-10-21/inputs.json). Die Ausschnitte allein
belegen keine vollständige Fenstergröße, Konfiguration oder genaue Quellrevision.

| Aufnahme | Konkreter Abgleich und Arbeit |
| --- | --- |
| [23:36:21: Zoomaktionen](user-review-2026-10-21/user-233621.png) | Kontur, Symbolform, Typografie, Hover/Fokus und Abstand direkt gegen die Auswertungsvorlage prüfen. Kein vollständiger Referenznachweis aus dem bisherigen Funktionslauf. |
| [23:36:03: Analysebericht](user-review-2026-10-21/user-233603.png) | Der Bericht verwendet noch die alte breite Export-/Größen-/Zoomanordnung. Den freigegebenen Grafikzugang und Dialog nach ADR 010 sichtbar anbinden; Daten- und Rasterbausteine allein beheben diesen Befund nicht. Auswahlpfeile gehören ebenfalls zum gemeinsamen Komponentenabgleich. |
| [23:34:20: Szenenbaum](user-review-2026-10-21/user-233420.png) | Gefüllte Aufklappbuttons, vollständige blaue Checkboxspalte und abgeschnittene Namen entsprechen nicht der Vorlage. Zeilen mit getrenntem Auswählen und Sichtbarkeit, schlanken Chevrons, begrenzter Einrückung und vollständigem Textumbruch umsetzen. Reale Gruppen/Koordinatenrahmen und alle Objekte erhalten. |
| [23:33:42: Inspektor](user-review-2026-10-21/user-233342.png) | Die Plus-/Minuszeichen gehören zu `inspector.css`. Dagegen fehlen die einpixeligen Abschnittsregeln; Zusammenfassungen und Textgrößen gegen die vorgegebenen 43-px-Zeilen prüfen. Inhalte, Scrollzugang und Editierbarkeit vollständig erhalten. |
| [23:32:47: Tempoauswahl](user-review-2026-10-21/user-233247.png) | Native Auswahlpfeile, deren Größe, Fläche und Innenabstände konsistent zur Vorlage zeichnen. Dass die Auswahl fachlich funktioniert, beweist ihr Design nicht. |
| [23:31:30: Workspace-Ordner](user-review-2026-10-21/user-233130.png) | Gefülltes Dreieck durch den schmalen Chevron der Vorlage ersetzen und Abstand zum Ordnerzeichen prüfen; tatsächliche Verzeichnisfunktion erhalten. |
| [23:31:08: Projektdatei/Pfad](user-review-2026-10-21/user-233108.png) | Dateisymbol, Pfadlinien, Auswahlfläche und Tooltip weiter vergleichen. Den vollständigen Pfad lesbar halten; doppelte Trenner und Breiten-/Bildschirmgrenzen gesondert prüfen. Noch keine erledigte Tooltip-Abnahme. |

Der [geprüfte native Korrekturstand](COMPONENTS-NATIVE.md) ersetzt gemeinsame
Auswahlpfeile, Workspace-Chevrons und Szenenzeilen, ergänzt Inspektorabschnitte
und behebt dessen blockierenden Modellfehler-Scrollbereich. Windows besteht
sechs ausgewählte native Gruppen, Linux sieben. Die vollständige Windows-
Messlaborgruppe besitzt weiterhin einen getrennten offenen Hostfokus-Nachweis.
Der folgende [native Berichts-Exportdialog](GRAPHIC-DIALOG-NATIVE.md) ersetzt
die alte PNG-/SVG-/Größenleiste der Aufnahme 23:36:03 durch **Grafik exportieren…**
neben kompakten Zoomaktionen und öffnet die freigegebene Vorschau mit Optionen.
Das ist keine vollständige Erledigung dieses Screenshots: verbleibende
Typografie-/Raster-/Komponentendetails, Mehrlaufansicht und Gesamtzustände sind
weiter aufgelistet. Die gesamte Tabelle ist keine Abnahme; Zoom-/Tooltipdetails
und die übrigen Oberflächenzustände bleiben ausdrücklich offen.

Der folgende [Materialabgleich](GRAPHIC-MATERIAL-NATIVE.md) ergänzt den bislang
fehlenden Hintergrund-Blur und die proportionale Feldzeichnung bei großer Schrift.
Der folgende [Schatten-/Maßabgleich](GRAPHIC-SHADOW-NATIVE.md) ergänzt beide
Schatten, normale helle Konturen, die Referenzgröße und weitere kleine Fensterzustände.

Der folgende [sichtbare Messlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md)
verbindet jetzt bis zu acht Kurven mit Referenz, vollständiger Statistik und
demselben Exportzugang. Er belegt weitere echte Auswertungszustände; die
verbleibenden Schrift-/Raster-/Komponentenunterschiede der gesamten Oberfläche
sind damit weiterhin nicht vollständig abgenommen.

Der [gemeinsame Float-Zeichenpfad](FLOAT-GEOMETRY-NATIVE.md) erhält nun auch
gebrochene Widgetmaße, Symbolpositionen und Strichbreiten. Die erste native
Gegenprobe belegt deren vorherige Kürzung. Die scharfen Exportfeldkonturen
werden anschließend direkt gegen beide Originalreferenzen geprüft. Die
Zoom-/Tooltipdetails und die Gesamtabnahme dieser Befunde bleiben offen.

Der spätere [Abgleich von Diagrammwerkzeugen und Pfad-Tooltips](CHART-TOOLS-PATHS-NATIVE.md)
bearbeitet jetzt genau diese beiden Detailbereiche: gemeinsame 12-px-Kontrollen
mit Original-Export-SVG, äußeren Fokusringen und vollständigem Textumbruch sowie
vollständige, im Fenster begrenzte Workspace-Pfade. Für sehr lange Tooltip-Inhalte
sind Scrollzugriff, sichtbares Pfadende und unveränderter Editor getrennt geprüft.
Diese Fortschritte ersetzen weiterhin keine vollständige Abnahme der sieben
Nutzerbefunde oder der gesamten Oberfläche.

Der [Dateitab-/Dateizeichen-Abgleich](WORKBENCH-TABS-NATIVE.md) ergänzt natürliche
Tabbreiten, echte starke C-/P-Zeichen, Projektraute und Ordnerabstände. Die
Teilfensterprüfung findet und korrigiert außerdem eine verdrängte Messfläche
und eine trotz erreichbarer Zeile leere Protokollbeschriftung. Die negativen
Originalbilder bleiben erhalten. Die aktuelle große Linux-Schrift zeigt weiter
Fehler an Tempoauswahl und Laufsteuerung; diese und die übrige Oberfläche sind
noch nicht vollständig abgenommen.

Der folgende [Simulationsleisten-Abgleich](SIMULATION-TRANSPORT-NATIVE.md)
bearbeitet Tempoaufnahme und große Laufaktionen: Originalmaße, 8-px-Chevron,
einzeilige volle Beschriftungen und native Fokusführung. Windows und Linux
bestehen beide Schriftpfade mit sechs Schrift-/Themezuständen. Der dabei
gefundene Fehler der semantischen Popup-Verdeckung ist korrigiert. Ein
zusätzlicher alter F1-Lifecycle-Test setzt dagegen ein bereits sichtbares
Suchfeld voraus und bleibt getrennt offen; die Probe scheitert gleichartig
im aktuellen und im älteren Build. Die übrige Oberfläche ist weiter abzugleichen.

Der folgende [Zeitleistenabgleich](SIMULATION-TIMELINE-NATIVE.md) bearbeitet einen
weiteren sichtbaren Bereich der gemeinsamen Oberfläche: ursprüngliche Aufteilung,
kompakte Aktionen, große Zeitangabe und lesbare Skala. Zusätzliche Rahmen und
überdeckende Hinweise sind entfernt. Echte Zeitnavigation, Tastaturbedienung,
Abbruch und Projektwechsel werden separat geprüft. Die übrigen Nutzerbefunde
und die vollständige Gestaltung bleiben weiterhin offen.

Der [Inspektor-Teilabgleich](INSPECTOR-FIELDS-NATIVE.md) ersetzt danach die
generischen Abschnittsköpfe und Modellwerte durch die gewählte B-Gestaltung:
13-px-Köpfe, einzelne Trennlinien und Wertefelder mit großen Zahlen.
Er korrigiert den tatsächlich überdeckten Objekttitel sowie die halb
abgeschnittene Kartenkontur. Die neue Mindestbreitenprüfung ergänzt außerdem
vollständige Kopftexte bei großer Schrift. Das ist keine Abnahme des gesamten
Szenenbaums, aller Inspektorgruppen oder der gesamten Oberfläche.

Der folgende [Objektabgleich](OBJECT-PROPERTIES-NATIVE.md) übernimmt danach den
fehlenden Auswahlkopf sowie B-Eigenschaften, Koordinaten und Objektaktionen.
Eine echte Kugel, Organisationsgruppe und Beschriftung werden per Tastatur
ausgewählt; Ausblenden/Einblenden und der unveränderte pausierte Lauf sind
separat geprüft. Die Daten werden dem tatsächlichen Modell entnommen. Die
Gesamtgestaltung, der vollständige Szenenbaum und weitere Gruppen bleiben offen.

Der anschließende [Baumabgleich](SCENE-TREE-NATIVE.md) bearbeitet die linke
Szenenleiste direkt: Hierarchie unmittelbar unter dem Kopf, schlanke Zeichen,
vollständige Beschriftungen, getrennte Sichtbarkeit und ruhige Grundfilter.
Die tatsächlichen Objekte und die Kameraoptionen bleiben erhalten. Gefundene
native Fokus-/Startgrößenfehler und fehlgeschlagene Prüferstände werden separat
belegt. Dieser Schritt erklärt die verbleibenden Kamera-/Metadatenunterschiede
und ersetzt weiterhin keine vollständige Abnahme aller Nutzerbefunde.

Der anschließende [Kamera-Abgleich](CAMERA-CONTROLS-NATIVE.md) ersetzt die alten
Preset-Knöpfe und verschachtelten Kameraabschnitte durch die flachen B-Felder.
Native Werte, vollständige Beschriftungen, Pos1/Ende, Spurklick und fortgesetztes
Ziehen sind getrennt geprüft. Die große Schrift erhält bei schmaler Spalte
die nötige konstante Feldhöhe. Aktuelle Kameraaufnahmen und frühere Gegenbelege
sind gemeinsam archiviert. Die verbleibenden Bereiche und die praktische
Windows-/Assistive-/DPI-Abnahme bleiben offen.

Der folgende [Szenenrahmen-Abgleich](SCENE-FRAME-NATIVE.md) übernimmt Kameraüberzug
und untere Ansichtsleiste aus der Vorlage. Echte Kamerasteuerung, Tastaturfolge
und Modellbeschriftungen bleiben nutzbar. Die Szenengestaltung wechselt beim
Herausziehen bei gleicher Inhaltsbreite nicht mehr auf andere Abstände/Hinweise.
Die vollständige Oberfläche ist damit weiterhin nicht abgenommen.

Der folgende [Messbereich-Abgleich](MEASUREMENTS-NATIVE.md) korrigiert große
Einheiten neben den Zahlen, Kanaltypografie, Abstände, Kurvenschalter und
Diagrammflächen. Echte Daten, Kanalwahl, Einheiten und Zeigerzeit bleiben
gebunden. Bild- und Bedienbelege einschließlich verworfener Prüferstände sind
gesondert archiviert; die vollständige Liste sichtbarer Befunde bleibt offen.

Der folgende [Kanalkopf-/Kurvenabgleich](CURVES-NATIVE.md) korrigiert die
Textverteilung und Kurvenfarben weiter. Lange Metadaten, Offsethinweise,
leere/ungültige Vorschauen und vorhandene Datenlücken werden nativ gerendert
und geprüft. Zeichenbefehle behalten ihre gebrochenen Strichstärken. Eine
vollständige Abnahme aller Nutzerbefunde wird damit weiterhin nicht behauptet.

Der folgende [Abgleich ruhiger Diagrammköpfe](chart-captions-2026-10-24/README.md)
entfernt den zusätzlichen Auswahlpfeil und die Hoverfüllung aus den Kurvenköpfen.
Der native Kanalwechsel und sichtbarer Tastaturfokus bleiben erhalten; das
Inspektorfeld behält seine Auswahlgestaltung. Die Gesamtoberfläche und praktische
Plattform-/Assistive-Nachweise bleiben weiter offen.

Der folgende [Abgleich gemeinsamer Rundungen und vollständiger Titel](ROUNDED-SECTIONS-NATIVE.md)
korrigiert nichtkonvexe Kapseln im zentralen Zeichenpfad und Wortteilungen in
den Inspektorköpfen. Tatsächliche Mindestbreiten erhalten die gewählte Schrift
und gespeicherte Wünsche. Die breiten nativen Nachläufe und der getrennte
F1-Zeitüberschreitungsbefund bleiben in den zugeordneten Belegen nachvollziehbar.

Der folgende [Abgleich nativer B-Wertefelder](RUN-FIELDS-NATIVE.md) korrigiert
Laufparameter und Laufeinstellungen einschließlich Standardknöpfen, Einheiten,
Zahlendarstellung und Scrollfluss. Native Bedien- und Datenprüfungen sowie die
zusätzliche Windows-Kopfprüfung bestehen. Weitere sichtbare Altbedienelemente
in Laufgrenzen, Ereignissen, Aufzeichnung und anderen Bereichen bleiben offen.

Der anschließende [Abgleich von Laufgrenzen, Ereignissen und Aufzeichnung](INSPECTOR-TAIL-NATIVE.md)
korrigiert diese drei Inspektorbereiche einschließlich ihrer Leerzustände,
Warnungserkennbarkeit, Grenzvalidierung und Tastaturbedienung. Ursprüngliche
Gegenbilder und 128 geprüfte native Aufnahmen bleiben nachvollziehbar erhalten.
Die übrigen Nutzerbefunde und der Gesamtauftrag sind damit nicht abgeschlossen.
