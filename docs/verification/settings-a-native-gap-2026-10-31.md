# Offener nativer Befund: Einstellungen A fehlen

Stand: 31. Oktober 2026, Quellstand `68ab5f71`.

Der tatsächliche Windows-Release-Lauf `run-vt2p0lgi` besteht die zwölf
Schriftgrößen-/Wiederöffnen-/Abbruch-/Standardwert-Teilabläufe. Das bestätigt
Schriften und Preferences-Transaktionen, **nicht das freigegebene Design A**.
Die native 22-px-Einstellungsansicht wurde separat betrachtet.

[ADR 018](../decisions/018-selected-settings-a.md) und die
[verbindliche Spezifikation](../visuals/settings-options/APPROVED.md) verlangen
ein eigenes Aufgabenfenster nach ADR 015 mit Kategorien links, Optionen mittig,
Live-Vorschau rechts, Suche und Änderungsprüfung. Die Auswahl ist verbindlich;
keine neue Variantenentscheidung einholen.

Die native Ansicht zeigt weiterhin das bisherige Formular im mittleren
Workspace des Hauptfensters. Dessen normale Seitenleiste und Inspektor bleiben
daneben. `app/settings_ui.inc:134` schaltet in `open_settings` nur auf Tab 6;
`settings_view` ab Zeile 257 zeichnet die bisherigen vertikal gestapelten
Schrift-/Theme-/Autosave-/Sichtbarkeits-/Größenfelder. Die fünf Kategorien,
kategorienübergreifende Suche, separate Entwurfs-Vorschau und Änderungsprüfung
aus A sind dort nicht umgesetzt. Das ist eine Umsetzungslücke, keine belegte
technische Unmöglichkeit der nativen Oberfläche.

Ein einzelner nativer Bildbeleg bleibt geschützt unter
`build/validation/macos-menu-workflows-2026-10-30/ui-22-typography-settings.png`
(146.649 Bytes, SHA256
`5f6f617faeb3c532c8b3d2b4f6f5923e49ba2656619d6ad5dcde85da19e2085c`). Das Original stammt aus
`run-vt2p0lgi/ui_typography_workflow/files ä/workflow ä/UI 22/initial-size-captures/ui-size-settings.bmp`.
Keine weitere Vollbildserie ist zur Beschreibung dieses Befunds nötig.

Nach Abschluss der laufenden Menü-/Typografieprüfung ist Einstellungen A der
nächste native Umsetzungsblock. Bestehende Transaktionen und Daten erhalten:
UI-/Codeschrift, Autosave, Sichtbarkeitsflags, Übernehmen/Abbrechen/Standardwerte,
Schreib-/Fontfehler sowie unabhängige Arbeitsflächengeometrie und Einheiten.
Die Ergänzungen der gewählten Vervollständigung D gehören gemäß Spezifikation
in „Editor & Sicherung“. Die erforderliche Implementierung und neue native
Design-/Bedienungsabnahme sind noch offen.
