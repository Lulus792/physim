# Einstellungen A: Entwurf sicher zusammenführen

31. Oktober 2026. **Technische Grundlage, noch keine native Designabnahme.**
Der [offene visuelle Befund](settings-a-native-gap-2026-10-31.md) bleibt bestehen:
Einstellungen verwendet weiterhin Tab 6 im Hauptfenster. Das eigenständige Fenster,
Kategorienlayout, Suche, Vorschau und Änderungsvergleich sind noch umzusetzen.

## Änderung

`app/preferences.c` führt einen Öffnungsstand, den Entwurf und den aktuellen Stand
feldweise zusammen. Nur tatsächlich bearbeitete Einstellungen werden übernommen;
unveränderte Werte behalten spätere Änderungen. Sichtbarkeitsflags werden einzeln
verglichen. Die vierzehn Änderungskennungen können später den vollständigen
Änderungsvergleich speisen; es gibt noch keine neue Vergleichsoberfläche.

Fenstergröße/-zustand, Workspace, Inspektoraufklappung und Docktopologie gehören
nicht zum allgemeinen Einstellungsentwurf. Standardwerte ändern ausschließlich
die freigegebenen Einstellungswerte. Sie setzen weder diese Zustände noch die
initialisierten Sitzungsgrößen von Simulieren/Auswerten zurück.

Der bestehende App-Einstieg übernimmt die Größenwerte von Entwickeln nach dem
Verlassen des Quellkontexts. Übernehmen validiert vor Schriftvorbereitung und
Dateioperationen; fehlgeschlagene Übernahme erhält den Entwurf. Erfolgreiches
Speichern verwendet weiterhin den sicheren Ersatz einer Geschwisterdatei.
Das Format bleibt PSPREF05 mit 312 Bytes; kein neuer Produktdefault.

## Tatsächlich ausgeführte Prüfungen

[Kommandos, Quell-/Runtime-Manifeste, Hashes und Einzelergebnisse](settings-a-transactions-2026-10-31.json).
Isolierter eigener Quellstand auf Basis `36bc7247`, 136 erfasste Eingaben;
fremde AT-SPI-/Orca-Arbeit wurde nicht übernommen.

| Prüfung | Windows Release | Linux Debug ASan/UBSan |
| --- | --- | --- |
| Preferences: Zusammenführung, zurückgenommener Einzelentwurf, einzelne Sichtbarkeitsflags, Defaults, Output-Aliasing, ungültige Werte; bisherige Datei-/Migrationsfälle | 1/1, `run-m2aq5i3w` | 1/1, `run-hxbvq_ma`, Leakprüfung an |
| Tatsächliche App: nachträglich geänderte Größen/Sichtbarkeit, Defaults, real gescheiterter Dateiersatz, Wiederholung, beide Workspace-Sitzungen, ungültige Größe und Escape | bestanden, `run-ro2m7jiy` | bestanden, `run-ysv6o4i0` |
| Bestehende Settings-Tastaturgruppe | bestanden, gleicher Lauf | bestanden, gleicher Lauf |
| Historischer `settings_workflow` | fehlgeschlagen, Stufe 23 | fehlgeschlagen, Stufe 23 |

Die beiden Display-Aufrufe sind **insgesamt fehlgeschlagen, jeweils 2/3**.
Der historische Gesamttest benutzt für die Protokollhöhe den nicht mehr gültigen
`panel_bounds[1]`-Griff (0/0) und erwartet 212 statt der unverändert gebliebenen
340. Die hashgeprüfte archivierte Windows-Vorgänger-App aus dem bisherigen
Anschluss scheitert mit demselben Prüfer exakt dort: `font=20 flags=79 side=282
log=340`. Der neue Code verursacht diesen Befund nicht. Die alte Fixture bleibt
offen; die bereits abgenommene dauerhafte Protokoll-Trennlinie wird hierdurch
nicht neu zur Prüfung gestellt. Bei ihrer späteren Portierung echte aktuelle
Resize-Eingaben verwenden und ihre nachfolgenden alten Default-/Tab-Erwartungen
gegen den freigegebenen Vertrag prüfen.

Die App-Prüfung injiziert unabhängige Änderungen in den tatsächlichen App-Zustand.
Sie beweist noch keine parallele physische Bedienung eines eigenen
Einstellungsfensters. Die neuen Transaktionsfälle benötigen keine Bilder.
Keine neue macOS-Ausführung oder Nutzung des ausgeschalteten Nutzer-Mac;
manuelle Screenreader-, DPI- und vollständige Designabnahme bleiben offen.

Nach Ende sämtlicher Aufrufe wurden vier Rohbilder aus den bestandenen
Tastaturfällen und 212 ausschließlich zum Vorgängervergleich wiederhergestellte
Runtime-Dateien entfernt: **216 Dateien, 198.471.296 Bytes**. Alle Pfade, Größen
und Hashes wurden vor dem Löschen geprüft. Die Vorgänger-Runtime bleibt aus dem
bereits geprüften Archivindex rekonstruierbar; Vergleichslogs, fehlgeschlagene
Fallbilder, der geschützte einzelne Designbefund und Quell-/Runtime-Archive
bleiben erhalten. Plan/Ergebnis unter
`build/validation/settings-transactions-2026-10-31/cleanup-{plan,result}.json`.

## Nächste Umsetzung

Der nächste Schritt bleibt das native Settings-A-Fenster nach ADR 015/018.
Beim Ablösen von Tab 6 darf die Integration nicht voraussetzen, dass das
Hauptfenster seinen Workspace verlässt: Entwickeln-Ausgangswerte und aktuell
sichtbare Simulieren-/Auswerten-Sitzungsgrößen ausdrücklich getrennt zusammenführen.
Die geprüften Transaktionsfälle an den echten neuen Einstieg anpassen, ohne die
Sicherheitsassertionen zu entfernen. No-op-Sperre, Fehlerfokus, Suche/Vergleich,
unabhängige Anzeigeeinheitenaufgabe und Completion-D-Preferences bleiben offen.
