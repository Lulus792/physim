# Einstellungen A: natives Kategorienfenster

Stand 31. Oktober 2026. Umsetzung nach [ADR 018](../../decisions/018-selected-settings-a.md)
und [freigegebener Spezifikation](../settings-options/APPROVED.md).
**Geprüfter Teilstand; keine vollständige Design-/Plattformabnahme.**
Der frühere Einstellungs-Tab im Hauptfenster wurde durch ein eigenes, nichtmodales
SDL-Fenster mit eigenem GL-, Eingabe-, Nuklear- und Accessibility-Kontext ersetzt.
Hauptfenster und Aufgabenfenster gehören weiterhin zu derselben Anwendung.

`app/settings_window.inc` verwaltet Lebenszeit, Ereignisse, Entwurf, Suche,
Tastaturnavigation und Übernahme. `app/settings_a_ui.inc` zeichnet das freigegebene
Kategorienlayout mit fünf Kategorien, Optionen, separater Entwurfsvorschau,
Änderungsstatus und vollständiger dreispaltiger Änderungsprüfung.
Die Fenstergröße folgt der tatsächlich verfügbaren Fläche; Kategorien und
Vorschau ordnen sich bei schmalen Fenstern beziehungsweise großer Schrift um.
Schriftrollen, Abstände, Konturen und transparente Auswahlfarben stammen aus
der Referenz. Der äußere OS-Fensterrahmen bleibt Sache des Fenstersystems.

Wiederaufruf hebt dasselbe Fenster mit erhaltenem Entwurf und Suchtext an.
Suchfeld und Größenfelder nehmen tatsächliche Texteingaben entgegen; die Suche
arbeitet ohne Unterscheidung der Groß-/Kleinschreibung über Kategorien hinweg.
Tab/Umschalt+Tab, Auswahlpfeile und Escape verwenden die sichtbaren Felder.
Escape verlässt zuerst die Änderungsprüfung, danach verwirft es den Entwurf.
Schließen über das Betriebssystem hat dieselbe Abbruchwirkung.

Die vierzehn vorhandenen persönlichen Einstellungen bleiben wirksam.
Theme-/Schriftentwürfe ändern zunächst nur die Miniatur. Übernehmen validiert
Ganzzahlen und Wertebereiche, bereitet Fonts vor und speichert sicher, bevor
der aktive Stand geändert wird. Fehler erhalten Eingabe, Entwurf und bisherigen
Stand. Standardwerte bleiben Entwurf. Ohne wirksame Änderung sind Prüfen und
Übernehmen gesperrt; bewusstes Speichern einer unlesbaren Konfiguration bleibt
als gesonderter Wiederherstellungsweg möglich.

Die feldweise Zusammenführung erhält unabhängige Fenster-/Workspace-Zustände.
Explizite Entwickeln-Ausgangsgrößen werden gespeichert, während die aktuelle
Simulieren-/Auswerten-Sitzungsgeometrie erhalten bleibt. Die gemeinsame aktuelle
Protokollgröße/-sichtbarkeit bleibt beim parallelen Übernehmen bestehen;
explizite Ausgangswerte werden beim nächsten Entwickeln-Aufruf wirksam.
Eine spätere bewusste Größen-, Sichtbarkeits- oder Layoutänderung hat Vorrang.
Auch beim Beenden wird der gespeicherte Ausgangswert nicht versehentlich durch
den noch sichtbaren anderen Workspace überschrieben.

Fonts werden unveränderlich vom Hauptkontext geliehen. Das Aufgabenfenster gibt
nur seine eigenen Ressourcen frei. Übernahme, Schließen und Ressourcenwechsel
erfolgen an Frame-Grenzen. Ein Fensterfokuswechsel allein beendet keinen Entwurf.
Bei eigenem Tastaturfokus wird genau der zugehörige Nuklear-Bereich aktiviert;
ein passiver Hover auf einem anderen Teilfenster darf Texteingaben nicht sperren.

Die tatsächlich ausgeführten Prüfungen, Zwischenfehler und Grenzen stehen im
[kompakten Nachweis](../../verification/settings-a-window-2026-10-31.md).

Die Suchaktionen im Kopf und im leeren Ergebnis haben eigene Namen, Fokus-IDs
und Besitzer. Beide geben Texteingabe und Fokus ans Suchfeld zurück. Abgescrollte
Controls können keinen Klick auf den festen Suchkopf übernehmen; Pointerfokus
beachtet den sichtbaren Ausschnitt. Das erste Suchzeichen verwendet für das
Löschsymbol eine passende Breitenreservierung. [Gezielte Suchprüfung und
Mac-Inspektorbefund](../../verification/settings-search-2026-10-31.md) halten den
späteren Nachlauf und ursprüngliche Fehlschritte getrennt fest.

## Noch umzusetzen

- Anzeigeeinheiten ist inzwischen eine [eigene native Aufgabe](UNIT-TASK-NATIVE.md)
  mit erhaltenen Kanalentwürfen und unabhängigem Anwenden/Abbrechen. Fünf relevante
  Gruppen bestehen unter Windows/Linux; der neue macOS-Lauf und die gesamte
  assistive/physische Abnahme bleiben gesondert offen.
- Completion D: „Vorschläge beim Tippen“ und sichtbare Trefferzahl (Standard fünf)
  benötigen die tatsächliche Backend-/Preferences-Integration. Keine wirkungslosen
  Ersatzfelder in die Kategorienansicht einbauen.
- Historische Tab-6-/Radio-/globale-Fokus-Fixtures an das neue Fenster anpassen,
  insbesondere den alten `settings_workflow` einschließlich Dateiwiederherstellung
  und Geometrie/Maximieren. Die neue Prüfung ersetzt nicht diese gesamte Abdeckung.
- Screenreader, physische Eingaben, Monitorwechsel, DPI und macOS-Vollbild bleiben
  gesonderte Abnahmen. Der ausgeschaltete Nutzer-Mac wird nicht verwendet.
