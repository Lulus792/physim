# Einstellungen: drei getrennte Designvorschauen

Stand: **29. Oktober 2026 · Auswahl offen.** Auf Nutzerauftrag sind A/B/C
isolierte Entwürfe. Keine Variante ist ins fertige Design übernommen.
Die native Umsetzung und sämtliche bisherigen Freigaben bleiben erhalten.

[Vorschau öffnen](index.html?variant=a&group=appearance&theme=dark).
Der lokale Vorschau-Server verwendet `http://127.0.0.1:8791/settings-options/`.
Ohne Server funktioniert `index.html` auch als lokale Datei.

| Richtung | Gestaltung | Schwerpunkt |
| --- | --- | --- |
| **A · Kategorien** | Links fünf Kategorien, mittig die Optionen, rechts die Vorschau | Vertraute, übersichtliche Navigation |
| **B · Übersicht** | Zusammengehörige Karten auf einer scrollbaren Seite, daneben Vorschau | Einstellungen im Zusammenhang sehen |
| **C · Fokus** | Links einzelne Einstellungen, mittig eine Aufgabe mit aktuellem und geplantem Wert | Gezielt suchen und einzeln bearbeiten |

Alle Richtungen haben dieselben Funktionen. Wechsel zwischen A/B/C erhält den
Entwurf. Suchfeld, Hell/Dunkel/Hoher Kontrast, Fehlerbeispiele und „Nur
Fensterinhalt“ sind direkt bedienbar. Letzteres zeigt einen Browserrahmen und
beweist kein eigenständiges Betriebssystemfenster.

## Enthaltene Inhalte

15 Einträge in fünf Gruppen, aus dem vorhandenen Funktionsbestand abgeleitet:

- Darstellung: Theme und UI-Schriftgröße 16/18/20/22 px.
- Editor & Sicherung: Codeschrift 12/14/16/18/20/22 px und Autosave
  10/30/60/120 Sekunden. Autosave sichert Entwürfe; bewusstes Speichern und B2
  bleiben eigene Abläufe.
- Simulation: Vektoren, Bahnen, Punkte, Beschriftungen, Raster und
  orthografische Projektion. Änderungen betreffen Darstellung, keine Physik.
- Arbeitsfläche: Protokoll geöffnet und gespeicherte Ausgangsgrößen für
  Entwickeln (Seite, Inspektor, Protokoll). Grenzen 140–480 / 180–480 /
  126–500 px. Dies sind gespeicherte Größen, nicht gemeinsame Sollmaße aller
  Arbeitsbereiche. [Resizing-Vertrag](../unified-workspace/RESIZING.md) erhalten.
- Anzeigeeinheiten: Einstieg in eine **separate Aufgabe**, mit Position und
  Geschwindigkeit als Beispieldaten, SI-Vorgaben und eigener Einheit.

Standardwerte aus `PS_PREFERENCES_DEFAULT`: UI 16, Code 12, Autosave 30 s,
Seite 218, Inspektor 255, Protokoll 340 px, dunkles Theme. Sichtbarkeitsmaske
31: Vektoren/Bahnen/Punkte/Beschriftungen/Raster an, Ortho/Protokoll aus.
Der sichtbare normale Demo-Ausgangsstand ist absichtlich davon verschieden,
damit Zurücksetzen und Abbrechen vergleichbar sind.

Quellen: [native Einstellungen](../../../app/settings_ui.inc),
[Preferences](../../../app/preferences.h), [Defaults](../../../app/preferences.c),
[Einstellungsverträge](../../settings.md). Laufende lokale native Änderungen
sind keine abgeschlossene Abnahme. Ältere Aussagen über frei verschiebbare
interne Panels werden durch [ADR 009](../../decisions/009-fixed-workspace-windows.md)
und den gemeinsamen Resizing-Vertrag begrenzt.

## Entwurf, Übernehmen und Fehler

- Die Miniatur zeigt den **Entwurf**. Aktuelle Beispielwerte ändern sich erst
  nach Übernehmen; „Änderungen prüfen“ vergleicht aktive und geplante Werte.
- Übernehmen validiert und schließt bei Erfolg. Schreibfehler oder fehlende
  Schriftressourcen lassen aktive Werte und Entwurf erhalten. Erneuter Versuch
  ist nach Freigabe der simulierten Fehlerbedingung möglich.
- Abbrechen/Schließen/Escape verwerfen den allgemeinen Entwurf. Standardwerte
  ändern ebenfalls zunächst nur den Entwurf. Fenstergröße und feste Topologie
  werden nicht zurückgesetzt.
- Unlesbare Einstellungen starten mit Defaults und Warnung. Die gedachte
  Originaldatei bleibt bis zum ausdrücklichen Übernehmen erhalten.
- Unabhängiges Verbreitern des Inspektors wird durch Abbrechen oder Übernehmen
  eines reinen Theme-Entwurfs nicht zurückgesetzt. Explizit bearbeitete Größen
  sind dagegen Teil dieses Entwurfs. Sitzungsgrößen pro Workspace erhalten.
- Einheiten besitzen eine eigene Transaktion. Separat angewendete Einheiten
  und offene Einheitenentwürfe überleben Abbrechen der allgemeinen Einstellungen.
  SI-Wiederherstellen wird hier erst als Einheitenentwurf angezeigt.

Einheitenbeispiele prüfen Symbol (höchstens 63 UTF-8-Bytes, keine Steuerzeichen
oder äußeren Leerzeichen), positiven endlichen Faktor und Darstellbarkeit des
Beispielwertes. Keine Offseteinheiten; Rohwerte und CSV bleiben unverändert.
Die vollständige native Prüfung aller Daten sowie eigene Berichts-Einheiten
müssen erhalten bleiben. Die zwei Fixture-Werte ersetzen sie nicht.

## Abgrenzung für den umsetzenden Chat

**Erst Nutzerwahl abwarten.** Suche, Kategorien/Karten/Fokus, Miniatur,
Änderungsprüfung, direkte Zahlenfelder für gespeicherte Panelgrößen sowie der
Einheiten-Einstieg aus Einstellungen sind Gestaltungsvorschläge; die bestehende
native Oberfläche besitzt diese Anordnung noch nicht. Die SI-Aktion als
bestätigbarer Einheitenentwurf ist ebenfalls vorgeschlagenes Bedienverhalten.

Persönliche Preferences, unabhängige Workspace-/Fenstergeometrie, benannte
Layouts und Einheitenkatalog nicht zu einer gemeinsamen Speichereinheit machen.
Die momentane native Einstellungsansicht im Arbeitsbereich ist kein Beleg für
die künftige Fensterform. [ADR 015](../../decisions/015-independent-task-windows.md)
verlangt eigenständige Aufgabenfenster mit weiter bedienbarem Hauptfenster.
Die Browserstudie realisiert keine nativen Fenster, Schriftressourcen,
Dateischreibvorgänge, Autosave-Timer oder laufende Simulation.

Alle Änderungen dieser Vorschau leben nur im JavaScript-Speicher der Seite;
Neuladen setzt sie zurück. Kein Zugriff auf persönliche Einstellungen oder
Projektdateien, keine Speicherung in Local Storage.

Nach einer Auswahl genaue Variante und Referenz festschreiben, dann in den
[Gesamtauftrag](../unified-workspace/NEXT-CHAT.md) aufnehmen. A2, B2,
Grafikexport, Projekt B und Explorer A bleiben verbindlich. Diese Runde darf
keine erneute Auswahl dieser bereits freigegebenen Teile auslösen.

## Prüfung

[Prüfbericht](review/VALIDATION.md), [Maschinenbericht](review/validation.json).
Browserprüfung: `node tools/check_settings_options.cjs` mit installiertem
Playwright und Edge; Ausgaben ausschließlich unter `build/settings-options-review`.
Dies ist eine Browserprüfung unter Windows, keine native Plattformabnahme.
