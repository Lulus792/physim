# Gemeinsame Kopfzeile und Fokusfarbe

Stand: 21. Oktober 2026. Die native Kopfzeile zeigt die fünf Referenzmenüs
Datei, Bearbeiten, Ansicht, Build und Hilfe. Einstellungen sind über
Bearbeiten → Einstellungen … erreichbar. Die kompakte Menüfläche verwendet
die Kontur, Rundung und Texteinzüge der gemeinsamen Vorlage.

Die spätere [Werkbank-Geometrie](WORKBENCH-GEOMETRY.md) korrigiert die unten
noch als offen dokumentierten Standardmaße und die überlappende API-Hilfszeile.
Die Stand-33-Aufnahmen und ihre abgegrenzten Aussagen bleiben unverändert.

Der direkte Hell-/Dunkel-Schalter liegt rechts neben der Projektbeschriftung.
Sein Bereich löst keinen Fensterzug aus. Maus und native Tastaturaktivierung
funktionieren auch dann, wenn Editor oder Szene den Widgetfokus besitzen.
Der Wechsel erfolgt an der nächsten Framegrenze und ändert ausschließlich
Darstellung, Farben und die davon abhängige F1-Layoutinvalidierung. Er wendet
keinen veralteten vollständigen Einstellungsentwurf auf die Arbeitsumgebung an.

Die nativen Fokusrahmen verwenden jetzt die semantische Akzentfarbe. Das gilt
auch für den Einheitenbutton und Dokumentationslinks. Der vorher schwarze
Rahmen der hellen Einheitenwahl ist damit korrigiert.

## Tatsächliche Prüfung

Der [eingefrorene Nachweis](shell-native-2026-10-21/validation.json) gehört zum
internen Quellstand 33 auf Basis von `fad76ef`, keiner Produktversion. Er erfasst
33 Quellhashes, Programmhashes, Befehle, Rückgabewerte und die zehn unverändert
vorgefundenen Dateien anderer lokaler Accessibility-Arbeit. Diese Dateien sind
in den Builds enthalten; ein sauberer Checkout ist damit nicht geprüft.

Windows und Alpine-Dev bestehen jeweils neun Gruppen mit neun tatsächlichen
nativen Programmausführungen:

- Theme-Wechsel aus Dunkel, Hell mit UI-Größe 22 bei 1080 × 740 und Kontrast;
  jeder Ablauf wechselt in Entwickeln, Simulieren und Auswerten, zuletzt bei
  geöffnetem F1. Quellen, Editorauswahl, Layoutwünsche, Messwerte, vollständige
  Statistik, Ansicht und Simulationszeit bleiben erhalten.
- Bestehende Toolbar- und Tastaturmenüaufgaben jeweils in kleiner und großer
  Fenstergröße, einschließlich des geänderten Einstellungszugangs.
- Einheitenwahl mit großer Schrift und der bestehende F1-H-Funktionsablauf.

Die Theme-Aufgabe verwendet zwei deterministische synthetische 301-Punkte-
Archive. Sie prüft weder Modellgüte noch Mehrlaufdarstellung. Tastaturereignisse
laufen durch SDL und das native Accessibility-Modell. Neue externe UIA-/AT-SPI-
Clients, manuelle Screenreader, physische Zeigerbedienung, gemischte Monitor-DPI
und macOS wurden in diesem Schritt nicht geprüft.

| Native Aufnahme | Windows | Linux |
| --- | --- | --- |
| Entwickeln, Dunkel | [Bild](shell-native-2026-10-21/windows-develop-dark-v33.png) | [Bild](shell-native-2026-10-21/linux-develop-dark-v33.png) |
| Entwickeln, Hell | [Bild](shell-native-2026-10-21/windows-develop-light-v33.png) | [Bild](shell-native-2026-10-21/linux-develop-light-v33.png) |
| Entwickeln, große Schrift | [Bild](shell-native-2026-10-21/windows-develop-light-large-narrow-v33.png) | [Bild](shell-native-2026-10-21/linux-develop-light-large-narrow-v33.png) |
| Simulieren | [Bild](shell-native-2026-10-21/windows-simulate-dark-v33.png) | [Bild](shell-native-2026-10-21/linux-simulate-dark-v33.png) |
| Auswerten | [Bild](shell-native-2026-10-21/windows-analyze-light-v33.png) | [Bild](shell-native-2026-10-21/linux-analyze-light-v33.png) |
| Akzentfarbener Einheitenfokus | [Bild](shell-native-2026-10-21/windows-units-light-large-focus-v33.png) | [Bild](shell-native-2026-10-21/linux-units-light-large-focus-v33.png) |
| Dateimenü | [Bild](shell-native-2026-10-21/windows-file-menu-v33.png) | [Bild](shell-native-2026-10-21/linux-file-menu-v33.png) |

Alle PNGs wurden verlustfrei aus nativen BMPs konvertiert; die dekodierten
RGB-Pixel stimmen vollständig überein. Referenzen und Aufnahmen sind unverändert.

Die negativen Versuche bleiben negativ: Stand 31 bestand den Mauswechsel,
scheiterte aber am nativen Fokuszugang des Schalters in Simulieren
([Gegenbild](shell-native-2026-10-21/counterexample-theme-focus-v31.png)). Stand 32
scheiterte beim Windows-Linken an einem nicht vorhandenen Diagnosehelfer. Ein
Linux-Build von Stand 32 wird nicht behauptet. Stand 33 korrigiert diesen Helfer
und besteht die vollständige oben genannte Folge auf beiden Plattformen.

## Weitere Abweichungen

Die nativen Minimieren-/Maximieren-/Schließen-Aktionen benötigen zusätzlich
138 logische Pixel rechts in der Clientkopfzeile. Die HTML-Seite läuft innerhalb
eines Browsers mit eigener Fenstersteuerung und zeigt diese Aktionen nicht im
Seiteninhalt. Dies erklärt den zusätzlichen Platzbedarf, aber keine übrigen
Abweichungen der Gestaltung.

Werkbank-Standardmaße, Baum, Dateitabs und Kontextgliederung bleiben weiter
anzugleichen. Die Aufnahme mit großer Schrift zeigt zudem eine überlappende
API-Hilfszeile und Aktion im kurzen Kontextpanel. Das ist ein offener
Implementierungsfehler, keine technische Unmöglichkeit. Vollständige Menü-
Screenreader-Semantik, alle Zustände und der erneute direkte F1-H-Bildabgleich
bleiben offen. Die [Gesamtliste](NATIVE-FIDELITY.md) gilt unverändert; diese
Teilprüfung ist keine 100-%-Designabnahme.
