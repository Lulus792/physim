# Feste Bereiche mit nativen Zusatzfenstern

Status: durch Nutzerauftrag festgelegt, 20. Oktober 2026.

## Vertrag

Die Bereichsanordnung bleibt fest; vorhandene Trenner ändern weiterhin Größen.
Freies internes Verschieben, neue Panel-Tabs, Docking-Zielkreuze und interne
schwebende Fenster entfallen. Die spätere Präzisierung verlangt ausdrücklich
das unveränderte festgelegte Aussehen: keine zusätzlichen Herauslösen-Symbole,
Zurückholen-Leisten, Platzhalterkarten oder dafür eingefügten Abstände.

An vorhandenen Bereichsüberschriften beziehungsweise Dokumenttabs ziehen und
außerhalb des Hauptfensters loslassen öffnet ein echtes zusätzliches Fenster.
Das gilt auch einzeln für Messbereich und Zeitleiste. Internes Ziehen verändert
keine Panelplätze. Das native Fenster-X setzt den Inhalt an seinen festen Platz
zurück; die gewünschten Größen bleiben erhalten. Auslagerungen gelten für die
Sitzung. Die vier allgemeinen Bereiche folgen dem gewählten Arbeitsbereich;
Messbereich und Zeitleiste bleiben Simulationsansichten.

Das Herauslösen erzeugt keinen zweiten Runner und kopiert keinen Anwendungszustand.
Kamera, Auswahl, betrachtete Zeit, Editoren, Undo und Modelltransaktionen bleiben
gemeinsam. Noch anstehende Texteingabe wird vor dem Zurückholen verarbeitet.
Für Tastaturbedienung steht Ctrl+Shift+Enter am fokussierten Bereich bereit;
im Zusatzfenster holt derselbe Befehl zurück. macOS verwendet dafür Cmd.

## Technik und Kompatibilität

Wie F1 haben Zusatzfenster eigene SDL-Fenster, GL- und Nuklear-Kontexte sowie
native Accessibility-Modelle. Die unveränderlichen Schrifttexturen stammen aus
dem gemeinsamen Hauptkontext. Alle Zusatzkontexte werden vor dem Besitzer ihrer
Schriften abgebaut. Kontextwechsel und Rendering erfolgen auf dem UI-Thread.
Die SDL-Grundlage ist [Fenstererzeugung](https://wiki.libsdl.org/SDL3/SDL_CreateWindow)
und [GL-Kontextteilung](https://wiki.libsdl.org/SDL3/SDL_GLAttr).

Die Datei- und Katalogformate bleiben lesbar. Alte Topologien werden für die
Anzeige auf feste Plätze abgebildet; Größen und Sichtbarkeit bleiben nutzbar.
Eine betroffene Einstellungsdatei erhält vor automatischem Überschreiben eine
einmalige Originalkopie mit dem Suffix `.before-fixed-panels`. Alte benannte
Kataloge werden durch bloßes Anwenden nicht umgeschrieben.

Diese Entscheidung ersetzt die freie Docking-Topologie aus früheren Verträgen,
auch die entsprechende Ausnahme in Entscheidung 008. Messlabor-Fachfunktionen,
Sitzungstrennung und Modellwertbindung aus 008 bleiben bestehen.

## Nachweise

Siehe [native Beschreibung](../visuals/simulation-designs/NATIVE.md) und den dort
verlinkten Prüfbericht. Prüfungen unterscheiden tatsächliche Windows-/Linux-Läufe
von nicht ausgeführter macOS-, Screenreader- und gemischter DPI-Abnahme.
