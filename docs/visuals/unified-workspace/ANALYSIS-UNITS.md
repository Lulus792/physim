# Auswerten: kompakte Einheitenwahl und große Schrift

Stand: 21. Oktober 2026. Fortsetzung des [nativen Aufbaus](ANALYSIS-NATIVE.md)
und des [Feldabgleichs](ANALYSIS-CONTROLS.md). Der Gesamtauftrag für Entwickeln,
Simulieren, Auswerten und F1 bleibt offen.

## Umsetzung

Die Anzeigeeinheit ist jetzt eine kompakte Auswahl im Stil der Vorlage. Bei
Winkelkanälen bietet sie Radiant und Grad. Andere Kanäle verwenden passende
Vorgaben der vorhandenen Dimensionslogik. „Benutzerdefiniert …“ erhält den
bestehenden Dialog für eigene Symbole und lineare Faktoren. Die gespeicherte
Originaleinheit bleibt erreichbar. Unbekannte zusätzliche Größen werden nicht
aus dem Browserbeispiel in Messdateien ergänzt.

Die Auswahl verwendet die vorhandene dauerhafte Einheitendatei und prüft, ob
Messwerte und Statistik mit dem Faktor darstellbar bleiben. Erst ein erfolgreicher
atomarer Schreibvorgang übernimmt die neue Wahl. Bei einem Speicherfehler bleiben
die vorherige Einheit und Datei erhalten; die Oberfläche zeigt den Fehler. Bei
einer geschützten oder beschädigten Einheitendatei sind Vorgaben deaktiviert,
„Benutzerdefiniert …“ bleibt für den vorhandenen Wiederherstellungsweg erreichbar.
Messdateien, Zeitwerte, Auswahl, Zoom und vollständige Statistik werden durch
einen Darstellungswechsel nicht verändert. CSV behält seinen gespeicherten
Einheitenvertrag.

Das Feld ist ein benannter nativer Menübutton mit Tastaturaktivierung. Die
Einträge sind native fokussierbare Aktionen. Nach der Auswahl beziehungsweise
Rückkehr aus dem eigenen Einheitendialog wird Fokus zur Einheitenwahl angefordert.
Dies ersetzt keine manuelle Screenreader-Abnahme oder den Nachweis aller
plattformabhängigen Control-Patterns.

## Korrekturen bei großer Schrift

Der Suchplatzhalter verwendete zuvor die globale UI-Schrift statt der Schriftrolle
seines Feldes. Er verwendet nun dieselbe Rolle wie die Eingabe. Lange Dateinamen
erhalten ein sichtbares Auslassungszeichen und bleiben vollständig als zugänglicher
Name beziehungsweise Tooltip erhalten. Laufmetadaten umbrechen; die Kartenhöhe
wächst mit Text und Schrift. Die Quellen-/Metadatenwerte bleiben echte Laufdaten.

Die Kopfzeile benennt kompakt die Punkte des aktiven Laufs. Die Statistikerläuterung
bekommt den tatsächlichen verbleibenden Platz; reicht dieser nicht, steht sie
umgebrochen in einer eigenen Zeile. Achsenbeschriftungen runden einen relativ zur
Achsenweite sehr kleinen Wert auf die sichtbare Genauigkeit. Die eigentlichen
Tickkoordinaten, Messwerte und Statistiken bleiben unverändert. Ebenso kleine
Achsen behalten ihre kleinen, von null verschiedenen Werte.

## Native Popup-Anbindung

Die neue Tastaturaufgabe deckte mehrere konkrete Lücken auf: Die ursprünglichen
Nuklear-Einträge hatten keine native Registrierung, interne Popups keinen Namen,
und die abschließende Fensterprüfung erkannte ausschließlich Hauptfenster. Eine
Fokusanforderung auf den internen Popupnamen setzte zudem Nuklears aktives Fenster
auf null. Daraus wurde keine technische Unmöglichkeit der Vorlage abgeleitet.

Der neue kleine Adapter verwendet weiterhin Nuklears normalen nicht blockierenden
Combo-Lebenszyklus. Das Menü erhält eine stabile Kennung aus Besitzer und Feld.
Die Fensterprüfung ordnet aktuelle, offene Popup-Nachfahren ihrem Besitzer zu und
behält die Prüfung höherer verdeckender Fenster bei. Geschlossene, versteckte oder
veraltete Popups gelten nicht als gültige Besitzer. Bei Fokusgewinn wird das
tatsächliche Besitzerfenster aktiviert; der native Fokus bleibt auf dem benannten
Menüeintrag. Der Test wartet nach dem Schließen auf die tatsächliche Fokus-Rückgabe
über die getrennten Publikations- und Aktivierungsframes.

## Prüfung und offene Arbeit

Der [eingefrorene Nachweis](analysis-units-2026-10-21/validation.json) gehört zum
internen Quellstand 30, keiner Produktversion. Er erfasst 30 Quelldateien,
Programmhashes, Befehle, Rückgabewerte und zehn unverändert vorgefundene Dateien
anderer lokaler Accessibility-Arbeit. Die Builds enthalten diese Dateien;
ein sauberer Checkout wurde damit nicht geprüft.

| Tatsächliche Prüfung | Umfang pro Plattform |
| --- | --- |
| Windows und Alpine-Dev: Auswertungsmatrix | Fünf bestandene Varianten: Hell/Dunkel und Kontrast bei 1440 × 1000 sowie Hell/Dunkel bei 1080 × 740 mit UI-Größe 22. Seitenzustände, native Teilfenster mit Rückkehr, Einstellungen, Trenner und Scrollzugang bleiben geprüft. |
| Windows und Alpine-Dev: neue Einheitenaufgabe | Dunkel und Hell bei großer Schrift bestanden. Öffnen per Tastatur, Gradwahl, tatsächliche Fokus-Rückgabe, Abbruch des eigenen Dialogs, echter atomarer Speicherfehler, Wiederherstellung und Zugang zum eigenen Dialog im kontrollierten Nur-Lesen-Zustand. |
| Windows und Alpine-Dev: bestehende Einheitenfolge | Mit echten C- und Physim-Projekten bestanden: laufende Simulation, eigene Faktoren, Ablehnung ungültiger/nicht darstellbarer Faktoren, Persistenz, Wiederöffnen, tatsächlich beschädigte Datei und ausdrücklich ausgelöstes Zurücksetzen. Quellen und Roharchive werden per Dateihash auf Erhalt geprüft. |
| Windows und Alpine-Dev: weitere Regression | Große Laufdetails bei UI-Größe 22 und die bestehende native F1-H-Aufgabe bestanden. Diese F1-Funktionsaufgabe ist kein vollständiger neuer Bildabgleich mit Design H. |

Das sind je fünf Auswertungsvarianten und fünf Funktionsgruppen mit insgesamt
je 17 nativen Programmausführungen. Die neuen Einheitenaufgaben vergleichen
Roharchiv, Zeit- und Messwertpuffer, vollständige Statistik, Zoom und Auswahl
vor/nach den Aktionen. Sie prüfen die tatsächliche Umrechnung des ersten Werts
von 0,3 Radiant auf Grad sowie den Erhalt sehr kleiner Achsenwerte. Die beiden
301-Punkte-Archive enthalten deterministische synthetische Testsignale; sie
belegen keine physikalische Modellgüte und keine Mehrlaufdarstellung.

Der Nur-Lesen-Fall setzt gezielt den vorhandenen Modellzustand. Die bestehende
Einheitenfolge prüft zusätzlich eine wirklich beschädigte Einstellungsdatei.
Der Speicherfehler entsteht beim tatsächlichen atomaren Speichern auf einen
bereits vorhandenen Verzeichnispfad; er ist kein Windows-ACL-Nachweis. Neue native
UIA-/AT-SPI-Clientaufgaben oder manuelle Screenreader wurden nicht ausgeführt.

Frühere Fehler bleiben im Nachweis negativ: Stand 25 ohne registrierte Einträge,
26 ohne Popupkennung, 27 mit fehlender Besitzerzuordnung, 28 mit verlorener
Nuklear-Fensteraktivierung und 29 mit verfrühter Prüfung der Fokus-Rückgabe.
Das [sichtbare, aber nicht nativ fokussierbare Menü](analysis-units-2026-10-21/counterexample-visible-inaccessible-menu-v25.png)
bleibt als Gegenbild erhalten. Der finale Test wartet auf den beobachteten
Fokusgewinn und behält die 60-Sekunden-Gesamtgrenze.

| Native Aufnahme | Windows | Linux |
| --- | --- | --- |
| Dunkel | [Bild](analysis-units-2026-10-21/windows-dark-v30.png) | [Bild](analysis-units-2026-10-21/linux-dark-v30.png) |
| Hell | [Bild](analysis-units-2026-10-21/windows-light-v30.png) | [Bild](analysis-units-2026-10-21/linux-light-v30.png) |
| Hoher Kontrast | [Bild](analysis-units-2026-10-21/windows-contrast-v30.png) | [Bild](analysis-units-2026-10-21/linux-contrast-v30.png) |
| Große Schrift, dunkel | [Bild](analysis-units-2026-10-21/windows-dark-large-narrow-v30.png) | [Bild](analysis-units-2026-10-21/linux-dark-large-narrow-v30.png) |
| Große Schrift, hell | [Bild](analysis-units-2026-10-21/windows-light-large-narrow-v30.png) | [Bild](analysis-units-2026-10-21/linux-light-large-narrow-v30.png) |
| Einheitenmenü, dunkel | [Bild](analysis-units-2026-10-21/windows-units-dark-menu-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-dark-menu-v30.png) |
| Einheitenmenü, große Schrift | [Bild](analysis-units-2026-10-21/windows-units-light_large_narrow-menu-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-light_large_narrow-menu-v30.png) |
| Umgerechnet auf Grad | [Bild](analysis-units-2026-10-21/windows-units-dark-degrees-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-dark-degrees-v30.png) |
| Grad und Fokus-Rückgabe, große Schrift | [Bild](analysis-units-2026-10-21/windows-units-light_large_narrow-degrees-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-light_large_narrow-degrees-v30.png) |
| Bewusster Speicherfehler | [Bild](analysis-units-2026-10-21/windows-units-dark-save-error-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-dark-save-error-v30.png) |
| Kontrollierter Nur-Lesen-Zustand | [Bild](analysis-units-2026-10-21/windows-units-dark-readonly-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-dark-readonly-v30.png) |
| Originaleinheit wiederhergestellt | [Bild](analysis-units-2026-10-21/windows-units-dark-restored-v30.png) | [Bild](analysis-units-2026-10-21/linux-units-dark-restored-v30.png) |

Die 24 aktuellen Bilder und das negative Bild wurden verlustfrei aus echten
nativen BMPs konvertiert; die dekodierten RGB-Pixel sind jeweils identisch.
Vier Farbstichproben bestätigen die Hintergrundfläche der Einheitenwahl in
Hell/Dunkel direkt gegen die unveränderten Referenzbilder. Das ist kein
vollständiger Geometrie-, Typografie- oder Zustandsvergleich.

Der Fokusrahmen des neuen Menübuttons verwendet in der hellen Aufnahme noch die
allgemeine Vordergrundfarbe statt des blauen Akzents. Ursache ist die bestehende
generische Fokuszeichnung; ihre Token-Zuordnung ist weiter anzugleichen. Das ist
keine technische Grenze. Bei großer Linux-Schrift wird die Laufliste länger und
scrollt innerhalb ihres festen Platzes. Lange Namen sind bewusst gekürzt und
vollständig benannt, Metadaten umbrechen. Eine vollständige visuelle und
Tastaturabnahme langer Listen bleibt erforderlich.

Mehrlaufkurven und -statistik, der freigegebene Grafikexport, weitere Metadaten,
der gemeinsame Analyseeditor und der vollständige Abgleich aller Arbeitsbereiche
bleiben umzusetzen. Physische Windows-Mausbewegung, manuelle Screenreader,
gemischte Monitor-DPI und macOS sind durch diesen Schritt nicht nachgewiesen.

Die geprüften lokalen Programme liegen unter
`build/native/Design-analysis/bin/physim.exe` und
`build/native/Design-analysis-linux/bin/physim`. Andere vorhandene Buildordner
wurden dadurch nicht aktualisiert.

Nach dem Einfrieren wurden zwölf eigene redundante PNG-Vorschaubilder aus den
ignorierten Prüfordnern entfernt (1.686.352 Byte). Vorher wurden ihre RGB-Pixel
gegen die weiter erhaltenen nativen BMPs verglichen. Finale PNGs, native Belege
der negativen Fälle und alle Originalreferenzen bleiben erhalten.
