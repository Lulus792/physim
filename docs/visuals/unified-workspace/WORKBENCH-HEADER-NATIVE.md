# Werkbank: gleicher Editorkopf im Haupt- und Zusatzfenster

Stand: 25. Oktober 2026, auf Grundlage von `e7e9022`.
Der direkte Abgleich verwendet die unveränderten Regeln aus
`develop-designs/workbench.css` und `workbench.js` sowie die beiden gemeinsamen
Entwickeln-PNGs. Die neue, noch auszuwählende Designgalerie wird nicht übernommen.

## Umsetzung

Die Such-/Speichergruppe hatte bisher zwei 32 px breite, über die ganze
Kopfhöhe reichende Ziele. Ihre linke Trennlinie fehlte. Jetzt zeichnet ein
gemeinsamer nativer Pfad die CSS-Aufteilung: eine 1-px-Kante, 6 px Innenabstand,
zwei 28-px-Aktionen mit 3 px Abstand und 6 px rechts. Bei Standardschrift sind
das insgesamt 72 statt 64 px. Eingeklappter Codekontext reserviert rechts
40 statt 6 px; der vorhandene Kontextschalter behält seine feste Position.

Der wieder eingeblendete linke Schalter verwendet die 24-px-Iconfläche mit
je 5 px Innenabstand und eigener 1-px-Abgrenzung. Hover und Tastaturfokus gehören
zu den kompakten Aktionsflächen. Speichern bleibt während nicht erlaubter
Verarbeitung deaktiviert. Gebrochene Positionen bei UI 22 werden für die beiden
schmalen Trennlinien gezielt auf das logische Pixelraster gelegt; die übrigen
Kontroll- und Tabmaße bleiben gebrochen.

Im herausgezogenen Editor fehlten Suche und Speichern ganz; seine Dateileiste
verwendete außerdem die allgemeinen Fensterinnenabstände. Er benutzt jetzt
denselben Kopf, dieselben Dateitabs und dieselbe untere Linie. Die vorhandenen
Such- und Speicherfunktionen arbeiten auf demselben Dokument. Es entsteht kein
zweiter Puffer und keine zusätzliche Auslagerungsleiste. Workspace- und
Kontextschalter steuern weiterhin ihre festen Plätze im Hauptfenster.

## Beim Bedienen gefundener Eingabefehler

Der Klick auf die Suchaktion öffnet die Insel außerhalb des Klickpunkts.
Die Insel interpretierte denselben Druck anschließend als Außenklick und
deaktivierte erst ihre logische Tastaturzuordnung, dann ihr Textfeld. Die
Debuggerprobe unterscheidet beides: Nach der ersten Teilkorrektur waren
`search_keyboard=1` und das richtige Fenster aktiv, aber `edit_active=0` und
das Suchfeld leer. Im lokalen `nk_do_edit` setzt ein gedrückter Außenklick den
Aktivzustand ausdrücklich zurück.

Der Öffnungsklick wird deshalb ausschließlich während des Zeichnens der neuen
Insel maskiert; danach wird der ursprüngliche Mauszustand wiederhergestellt.
Spätere Außenklicks behalten ihre vorhandene Eingaberoute. Ein zusätzliches
Anklicken des Suchfelds ist nicht nötig. Die native Probe schreibt tatsächlich
`α` in die Suche, erhält den Quelltext und prüft Escape samt Fokus auf der
Suchaktion. Der eigene Fensterfall prüft ebenfalls Öffnen, Escape, Fokus,
Speichern und Rückkehr. Vor jeder geprüften Speicheraktion wird der echte
Dirty-Zustand gesetzt; die Aktion muss ihn löschen. Die gespeicherten Bytes
bleiben identisch zum ursprünglichen Pendelbeispiel.

## Ausgeführte Prüfungen

| Plattform | Aktueller Lauf | Ergebnis |
| --- | --- | --- |
| Windows, MSVC Release | `run-a__niv7e` | 5/5 Gruppen bestanden |
| Alpine-Dev, GCC Debug mit ASan/UBSan, Xvfb/Openbox und Software-GL | `run-is4cqw8n` | 5/5 Gruppen bestanden |

Beide Läufe verwenden den abschließenden Quellstand 5 und dieselben Gruppen:
`file_tabs_design`, `workbench_workflow`, `docking_workflow`,
`editor_replace_workflow` und `analysis_editor`.

Die erweiterte Dateitabgruppe prüft System- und gebündelte Schriften, Dunkel/Hell,
UI 22 im 1080×740-Client, hohen Kontrast, Hover, Fokus, Suche, Speichern,
eingeklappte Seiten, Wiederöffnen und ein echtes separates SDL-Fenster mit
anschließender Rückkehr. Die vorhandenen Tab-/Schließen-/Overflow-Prüfungen
bleiben erhalten. Die übrigen Gruppen prüfen unter anderem die drei Trenner,
alle sechs echten Teilfenster, C-/Physim-Dokumente, Ersetzen/Speichern und die
Analysewerkbank mit erhaltenem Bericht und Datenstand.

188 aktuelle Tab-/Kopfbilder und weitere 204 Regressionsbilder sind mit ihren
Hashwerten erfasst. Die [eingefrorenen Belege](workbench-header-2026-10-25/README.md)
enthalten Ergebnisse, Befehle, Programmhashes, fünf Quellkarten, 18 unveränderte
Fontressourcen, Gegenfälle und die Debuggerdiagnose. Die zwölf bereits zuvor
vorgefundenen fremden Dateien bleiben unverändert. Parallel entstandene
Designauswahl-Unterlagen sind kein Teil der nativen Änderung. Die Builds sind
damit ausdrücklich kein Nachweis aus einem sauberen Checkout.

## Verbleibende Unterschiede

Der zusätzliche strenge RGB-Vergleich betrachtet jeweils dieselben 967×40 px
des nativen Kopfs im 1440×1000-Hauptfenster und im 967×740-Zusatzfenster.
Layout, Flächen und Grenzen stimmen überein. Vollständige Pixelgleichheit
besteht trotzdem nicht:

| Plattform / Schriften | Abweichende Pixel | Größte RGB-Abweichung je Kanal |
| --- | --- | --- |
| Windows / System | 15 von 38.680 | 1 |
| Windows / gebündelt | 21 von 38.680 | 1 |
| Linux / System | 29 von 38.680 | 1 |
| Linux / gebündelt | 31 von 38.680 | 1 |

Die Unterschiede liegen an geglätteten Glyphen-/Iconpixeln. Die unterschiedlichen
Framebuffermaße und ihre Raster-/Texturinterpolation sind eine mögliche
Erklärung; die Ursache ist nicht abschließend isoliert. Das wird nicht als
technische Unmöglichkeit oder als bestandene exakte Pixelgleichheit ausgegeben.
Die Vergleichsbilder werden weder skaliert noch umgefärbt. Nur für die Messung
werden gleich große Rechtecke im Speicher gelesen; die gespeicherten Bilder
bleiben vollständig und unverändert.

Für Modellnotizen fehlt weiterhin ein freigegebener nativer Datenvertrag.
Die ursprüngliche Werkbank-Übergabe verlangt vorhandene Inhalte beziehungsweise
einen ehrlichen Leerzustand. Die illustrative Pendelformel wird daher nicht
beliebigen Quellen zugeschrieben. Neue Varianten warten auf die getrennte
Nutzerentscheidung.

Der Gesamtauftrag einschließlich F1, vollständiger Zustands-/Schriftabnahme,
macOS, physischer Monitor-/DPI-Aufgaben und externer Screenreader bleibt offen.
Die SDL-Testeingaben und semantischen Fokusanforderungen sind keine manuelle
Desktop- oder Screenreader-Abnahme. Leakprüfung ist in der Linux-Grafikumgebung
deaktiviert; die Sanitizerläufe beweisen keine eigenständige Leakfreiheit.

Der anschließende Computer-Use-Versuch wurde vom Werkzeug gestoppt, weil es
URL-Zugriffsregeln im vorhandenen Firefox nicht durchsetzen kann. Daraus entsteht
keine interaktive Browserabnahme der lokalen Vorlage. Die Referenzquellen und
PNG-Vergleiche bleiben davon getrennt nutzbar.

Fehlerbilder, Rohaufnahmen und aktuelle SDKs werden für die noch offene Abnahme
weiter benötigt und bleiben erhalten. Es wurden keine weiteren SDK-Kopien
angelegt und keine fremden oder aktiven Bestände bereinigt.
