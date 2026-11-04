# Gewähltes Design A · Geräteliste

**Am 4. November 2026 vom Nutzer für die spätere Umsetzung festgehalten.**
Die aktuelle UI-/Oberflächenarbeit hat Vorrang. Keine sofortige Backend- oder
native Implementierung beauftragt. [ADR 025](../../decisions/025-selected-compute-devices-a.md).

## Referenz und Umfang

[A öffnen](index.html?variant=a&scenario=ready&theme=dark) ·
[Leerzustand](index.html?variant=a&scenario=empty) ·
[nicht erreichbar](index.html?variant=a&scenario=offline) ·
[inkompatibel](index.html?variant=a&scenario=incompatible) ·
[Testfehler](index.html?variant=a&scenario=error) ·
[A-Referenzbild](review/a-dark.png) · [Kopplungsdialog](review/pair-dark.png).

- Bestehende Einstellungen A behalten ihre Kategorienstruktur. Neue Kategorie
  Rechnen & Geräte; kompakte Geräteliste mittig, Details zum ausgewählten Gerät
  rechts. Auswahlfläche/Fokus nach bestehendem A2-Vertrag. B/C nicht übernehmen.
- Geräteliste mit Hauptrechner und bewusst verknüpften Geräten: Name, Kurzinfo,
  verfügbar/verbunden/nicht erreichbar/inkompatibel. Details zeigen Status,
  Einzelzeit, Seriendurchsatz und getrennte Vergleichsfaktoren zum Hauptrechner.
- Gerät verknüpfen führt über Name/Adresse zur bewussten Identitätsbestätigung
  mit Codevergleich. Kopplung ist eine eigene wirksame Verwaltungsaktion;
  allgemeines Abbrechen der Einstellungen darf sie nicht still zurückrollen.
- Leistungstest startet bewusst für ausgewählte verfügbare Geräte einschließlich
  Hauptrechner. Identische versionierte Last und Testoptionen; Einzellauf in
  Sekunden und gültige Serienläufe pro Minute getrennt. Keine tatsächlichen
  Leistungswerte aus den illustrativen Fixture-Zahlen ableiten.
- Ohne Kopplung bleibt die bisherige lokale Ausführung. Erst danach zusätzliche
  Ziele im vorhandenen Lauf-/Serienstart zeigen. Einzellauf: genau ein Ziel;
  Serie: mehrere geeignete Geräte und optional der Hauptrechner. Gesperrte Ziele
  mit Grund zeigen. Startkarte ist eine Illustration, kein Ersatz für Läufe A.
- Bestehende Aufgabenleiste B mit echtem OS-Pop-out später verwenden: gemeinsamer
  Auftragszustand, ehrliche Phasen/Zähler, bestätigter Abbruch, eindeutige Resultate.
  Die reduzierte Browser-Fortschrittszeile ist kein neuer Fenstervertrag.

## Gestaltung und Zustände

`devices.css` und A in `devices.js` liefern die Referenz. Kategorien / Liste /
Details bei normaler Breite; bei wenig Platz Details unter dem Inhalt,
Kategorien bei schmaler Ansicht oben. Logische Maße mit UI-Schrift/DPI skalieren.
Bestehende semantische Physim-Farben, Hell/Dunkel/Hoher Kontrast, Fokus und
Tastaturbedienung erhalten. Dialog-Escape führt zum Auslöser zurück.

Leerzustand mit lokalem Test und Kopplungszugang; verbundene, offline und
inkompatible Geräte; Codebestätigung/Abbruch; Testauswahl, laufender Test,
Abschluss, Abbruch und Fehler sind Teil des gewählten Ablaufs. Gültige alte
Messwerte bei abgebrochenem/fehlgeschlagenem Test erhalten und kennzeichnen.

## Spätere Umsetzung und Grenzen

[Konzept](../../remote-computing-concept.md) enthält offene technische Fragen
zu Auftragsdaten, Plattform-/Modulkompatibilität, Vertrauen, Isolation,
Wiederverbindung und Aufbewahrung. Diese Freigabe beantwortet sie nicht durch
Browsercode. Native Aufgabenfenster, tatsächliche Runner, echte Testlast,
validierte Ergebnisse, Tastatur/Screenreader/DPI und Plattformen später prüfen.
[Vorhandene Browsernachweise](review/VALIDATION.md) sind unverändert historische
Entwurfsnachweise; durch die Auswahl wurde kein neuer Produkt-Test ausgeführt.
