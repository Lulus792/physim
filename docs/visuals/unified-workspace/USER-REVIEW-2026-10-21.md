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
