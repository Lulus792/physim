# Explorer A im Arbeitsbereich

Stand: 29. Oktober 2026. Die freie Dateinavigation aus der
[freigegebenen A-Referenz](../design-options/APPROVED.md) liegt jetzt innerhalb
des nativen Arbeitsbereichs. „Datei → Dateien …“ öffnet sie über die volle
Inhaltsbreite unter dem gemeinsamen Kopf. Die Hauptnavigation bleibt erreichbar;
„Zur Werkbank →“ verlässt die Ansicht. Es gibt weder Dialogabdunklung noch eine
zusätzliche Titel-/Abbrechenleiste. Orte, Liste und feste Vorschau verwenden die
gemeinsame A-Komponente; die vorhandene Protokollfläche begrenzt ihre Unterkante.

## Zustand und Gestaltung

Die freie Ansicht und der Auswahlpicker besitzen ausdrücklich getrennte Kontexte,
wie in [ADR 014](../../decisions/014-inline-explorer-contexts.md) beschrieben.
Ordnerwahl, Messlaufauswahl und der Unteraufruf aus Projektdialog B bleiben modal.
Sie verändern weder Suche, Ort, Auswahl und Vorschau des freien Explorers noch
Inhalt, Cursor, Auswahl und Undo eines offenen Dokuments. Bereichswechsel erhalten
die freie Navigation innerhalb der Sitzung. Die Orte-Verknüpfungen werden beim
erneuten Öffnen an das aktuelle Projekt angepasst. Veraltete Bestätigungen einer
verlassenen Ansicht dürfen keinen anderen Arbeitsbereich ersetzen.

Die Liste erhält die 440 px hohe Referenzfläche, die Vorschau eine Breite von
270 px und die Orte-Spalte 170 px bei normaler Textgröße. Unter der wirksamen
1150-px-Grenze liegt die Vorschau unter der Liste. Große Schrift skaliert diese
Maße und erhält einen äußeren Scrollweg. Die Vorschauaktionen verwenden
inhaltsbreite Buttons, die kleine Referenzschrift und skalierende Fokuslinien.
Eine gespeicherte ausgeblendete Workspace-Fläche bleibt ausgeblendet; die freie
Ansicht schreibt die bestehende Panelanordnung nicht um.

## Tatsächliche Prüfungen

Der [kompakte Nachweis](../../verification/explorer-inline-2026-10-29.json)
enthält Quell-/Laufzeitarchive, Kommandos, Ergebnisberichte und gezielte
Diagnosen. Der abschließende Windows-Release- und Linux-Debug-Lauf mit ASan/UBSan
prüft je fünf Gruppen: eingebettete Ansicht, bestehender Auswahlpicker,
Projektdialog B sowie unabhängige native Clients für beide Explorer-Kontexte.
Beide abschließenden Läufe bestehen jeweils 5/5 Gruppen. Die endgültigen
Ergebnisse stehen im Bericht. Die Linux-GUI-Prüfung deaktiviert die Leak-Erkennung;
die früheren separaten Modell-/Arbeitertests mit Leak-Prüfung werden nicht als
erneut ausgeführt ausgegeben.

Die eingebettete Fixture prüft sieben Zustände: dunkel, hell, hoher Kontrast,
große Schrift, schmales Fenster, große Schrift im schmalen Fenster und eine
gültige ausgeblendete Workspace-Fläche. Sie navigiert tatsächlich zu Auswerten,
kehrt zurück, öffnet B samt Picker, kehrt über den Workspace-Manager zurück und
öffnet dieselbe reale Datei erneut. Ein absichtlich ungespeicherter Editor muss
seinen Inhalt, Cursor, Auswahl und vollständigen Undo-Zustand behalten. Die
Fixture setzt diesen Ausgangsinhalt direkt; ihre folgenden SDL-Ereignisse sind
injiziert und kein physischer Tastaturnachweis.

Die unabhängigen UIA-/AT-SPI-Clients lesen und ändern in normaler und großer
Schrift echte Pfadfeldwerte bis 4095 UTF-8-Byte, prüfen die unveränderte Ablehnung
von 4096 Byte und verlassen die freie Ansicht über die normale Headeraktion.
AT-SPI prüft zusätzlich Einfügen/Löschen in 3000-Byte-Texten. Das belegt native
Schnittstellen, keine Screenreader-Sprachausgabe oder real existierenden
4095-Byte-Dateipfade. Der Katalogtest deckt auch das Darwin-Auswahlmodell ab;
ein macOS-Build wurde hierfür nicht ausgeführt.

## Folgender Abgleich und Restarbeit

Die bei dieser Einbindung beobachtete feste Textvorschauhöhe und der im großen
Linux-Bild abgeschnittene Name „Geöffnetes Projekt“ sind im folgenden
[Vorschau- und Fokusabgleich](EXPLORER-PREVIEW-NATIVE.md) korrigiert. Natürliche
Höhe, weicher Umbruch, unveränderte Textbytes und tatsächliche Orte-Glyphengrenzen
werden nun zusätzlich geprüft. Im folgenden dunklen Windows-Bild ist die
Öffnen-Aktion mit derselben langen Ortsangabe direkt im Vorschauausschnitt
sichtbar. Das ist keine Zusage, dass beliebig große Metadaten ohne Scrollen
in die feste Vorschaufläche passen.

Weitere offene Punkte aus dem [Explorer-Gesamtnachweis](EXPLORER-A-NATIVE.md)
bleiben: echte Bild-/Messlaufvorschau, C-API-Kontext, große native Collections,
vollständige Tastatur-/Screenreader-Aufgaben, deutsche Sortierung, reale
Berechtigungs-/SMB-Fälle, durchgehende lange Importpfade, 200 % Text sowie
zusätzliche DPI-/Monitorzustände und macOS. Auch die Hauptbereichs-Navigation
braucht eine eigene vollständige native Accessibility-Abnahme. Der frühere
UIA-Timeout im vorherigen Einbindungsnachweis hat weiterhin keine nachgewiesene
Ursache; spätere erfolgreiche Läufe erklären ihn nicht rückwirkend.
