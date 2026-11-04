# Rechnen & Geräte · A · Geräteliste gewählt

4. November 2026. Der Nutzer hat das Vorhaben als **späteren Projektplan** aufgenommen. Die laufende UI-/Oberflächenarbeit hat Vorrang; kein vorgezogener Auftrag zur Implementierung des Rechendiensts. Diese eigenständige Entwurfsrunde liefert tatsächliche interaktive Vorschläge. **A · Geräteliste ist für die spätere Umsetzung gewählt.** B/C bleiben Vergleichshistorie. [Verbindliche Spezifikation](APPROVED.md) · [ADR 025](../../decisions/025-selected-compute-devices-a.md). Einstellungen A und Fortschritt B mit Pop-out bleiben verbindlich.

[Galerie öffnen](index.html) · [A · Geräteliste](index.html?variant=a) · [B · Gerätekarten](index.html?variant=b) · [C · Leistungsvergleich](index.html?variant=c) · [Leerzustand](index.html?scenario=empty).

Beim bestehenden lokalen Vorschau-Server: `http://127.0.0.1:8791/compute-device-options/`.
Die HTML-Datei funktioniert auch direkt ohne Server, Installation, externe Fonts oder Netzwerkabhängigkeiten.

| Richtung | Unterschied innerhalb der gewählten Einstellungen A |
| --- | --- |
| A · Geräteliste | Kompakte Zeilen; ausgewähltes Gerät mit Details und Messwerten rechts |
| B · Gerätekarten | Eigenständige Flächen mit Status und Kurzmetriken; Details rechts |
| C · Leistungsvergleich | Direkt vergleichbare Spalten für Einzellauf und Serie; Details rechts |

Gemeinsam: Kategorie Rechnen & Geräte, Gerätekopplung als eigene Aufgabe mit Adresse und bewusstem Codevergleich, auswählbare Testgeräte, simulierter Testfortschritt/Abbruch/Fehler und Auswahl beim Serienstart erst nach Verknüpfung. Nicht erreichbare oder inkompatible Geräte sind erklärt und gesperrt. Test- und Serienabläufe laufen ausschließlich im Speicher der Seite. Keine echten Codes, Hardwaredaten, Messungen, Prozesse, Netzverbindungen oder Speicherung. Neuladen/Beispielwechsel setzt zurück. Kopplung und Auftragslebenszeiten im nativen Produkt sind im [Konzept](../../remote-computing-concept.md) weiter zu klären.

Die Startkarte verdeutlicht das Freischalten nach Kopplung; sie ist keine erneute Layoutwahl für Läufe/Serien A. Die Fortschrittszeile ist eine reduzierte Simulation; das echte OS-Pop-out bleibt gemäß ADR 020 später erforderlich. Die Kategorien außerhalb Rechnen & Geräte sind hier Kontext, keine funktionierenden Einstellungsseiten. Gerätekopplung hat in der Studie sofortige Wirkung und ist kein allgemeiner Einstellungsentwurf. Ein Entkoppeln während echter aktiver Aufträge benötigt später eine eigene Folgenklärung.

Hell/Dunkel/Hoher Kontrast; kleine Fenster ordnen Kategorien und Details um. Dialoge unterstützen Escape mit Fokusrückkehr. Beispielzustände umfassen keine Geräte, verbunden, offline, inkompatibel und Testfehler. Beispielwerte 42/28/35 Sekunden und 24/60/36 Läufe pro Minute sind illustrativ. Einzel- und Serienfaktoren verwenden denselben jeweiligen lokalen Referenzwert.

## Prüfung und Grenzen

`node tools/check_compute_device_options.cjs` nutzt Playwright und Edge. Lokale Werkzeugabhängigkeit unter `build/compute-device-review-tools`, Ausgaben unter `build/compute-device-review`; erforderliche Referenzbilder und kompakter Bericht unter [review](review/validation.json). Browserprüfung und visuelle Sichtung sind Entwurfsnachweise, keine native Windows-/Linux-/macOS- oder Screenreader-Abnahme. Die Quellen sind HTML/CSS/JS und brauchen kein Produkt-Build.
