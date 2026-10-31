# Anzeigeeinheiten als eigene native Aufgabe

Stand 31. Oktober 2026. Umsetzung nach [ADR 015](../../decisions/015-independent-task-windows.md)
und [Einstellungen A](../settings-options/APPROVED.md).
**Geprüfter nativer Teilstand; keine gesamte UI-/Plattformabnahme.**
Windows Release und Linux Debug ASan/UBSan bestehen je fünf relevante Gruppen;
anschließende gezielte Fensterprüfungen decken Anfangshöhe und Aufrufkontext ab.
[Umfang, ursprüngliche Fehler und kompakte Nachweise](../../verification/units-window-2026-10-31.md).

Die Einheitenauswahl verwendet ein eigenes nichtmodales SDL-Fenster mit eigenem
GL-, Nuklear-, Eingabe- und Accessibility-Kontext. Die bisherige vollflächige
Überlagerung und ihre globalen Bedien-/Dock-/Explorer-/Kontextmenüsperren sind
entfernt. Das Hauptfenster und andere Aufgabenfenster bleiben bedienbar.

`app/channel_units_ui.inc` enthält weiterhin den bestehenden Einheitenbestand,
die Anzeigeumrechnung, vollständige Prüfung realer Daten und sichere Speicherung.
`app/channel_unit_window.inc` verwaltet Fenster, kopierte Kanalidentitäten,
Entwürfe und Ereignisse. `app/channel_unit_task_ui.inc` setzt die ausgewählte
Einheitenreferenz nativ um: Messkanal, passende Vorgabe, Symbol, Faktor,
Umrechnungsvorschau und getrennte Aktionen für SI, Schließen und Anwenden.

## Zustand und Besitz

Die Kanalliste enthält den tatsächlich angefragten Kanal sowie die vorhandenen
Live- und geladenen Messkanäle. Identität ist wie im Katalog Name plus SI-Dimension.
Keine festen Beispielkanäle werden in das Produkt eingebaut. Kanalbeschreibungen
werden kopiert; das Aufgabenfenster hält keine Pointer auf Worker-Daten oder
Quellfenster. Höchstens 64 Kanalentwürfe, entsprechend der bestehenden
Kataloggrenze, bleiben in einer geöffneten Aufgabe erhalten. Ein erneuter Aufruf
hebt dasselbe Fenster an und erhält die vorhandenen Eingaben.

Kanalwechsel bewahrt auch ungültige Texteingaben. Vorgaben und SI-Wiederherstellen
ändern ausschließlich den jeweiligen Entwurf. „Einheit anwenden“ übernimmt den
aktuellen Kanal; die Aufgabe und andere Kanalentwürfe bleiben wie in der
freigegebenen Einheitenreferenz geöffnet. „Schließen“, Escape oder OS-Schließen
verwerfen nur noch nicht angewendete Entwürfe. Ein allgemeiner Einstellungsabbruch
schließt diese Aufgabe nicht und setzt angewendete Einheiten nicht zurück.

Die Vorschau verwendet einen tatsächlich vorhandenen endlichen Messwert.
Ohne Messwerte zeigt sie einen Leerzustand. Sie ersetzt nicht die vollständige
Validierung: aktuelle Werte, Verlauf, geladene Daten und Statistiken werden vor
Übernahme auf Über-/Unterlauf geprüft. Faktor muss positiv und endlich sein;
Symbol 1–63 UTF-8-Bytes, gültig und ohne äußere ASCII-Leerzeichen oder
Steuerzeichen. Die Prüfung weiterer Unicode-Randleerzeichen bleibt als
gesonderter Auditpunkt offen.
Offseteinheiten werden nicht eingeführt. Rohdaten, Modell und CSV bleiben in SI;
Berichte behalten die im Analysecode festgelegten Einheiten.

Speichern arbeitet auf dem aktuellsten Katalog und ersetzt nur den betreffenden
Kanaleintrag. Erst ein erfolgreicher sicherer Dateiersatz aktiviert die Auswahl.
Dateifehler erhalten Katalog und Entwurf. Eine unlesbare Katalogdatei bleibt bis
zum ausdrücklich beschrifteten Zurücksetzen erhalten. Persönliche Preferences
und Workspace-Geometrie gehören nicht zur Einheitentransaktion.

## Bedienung und Darstellung

Die Rollen und Abstände folgen der Einheitenaufgabe in `settings.css`.
Die Anfangsgröße folgt der Referenz 850 × 473 bei Standardschrift; die Höhe
skaliert mit der persönlichen Schriftgröße. Breit stehen Felder und Vorschau
nebeneinander; schmal folgt die Vorschau unter den Feldern. Inhaltshöhe ergibt
sich aus tatsächlichem Textumbruch; die Fußaktionen
bleiben erreichbar und brechen bei Bedarf um. Große Schrift, Hell/Dunkel und
hoher Kontrast verwenden die aktiven persönlichen Darstellungswerte.
Fehler erhalten die Referenzfarben, bleiben lesbar und werden ins Sichtfeld
gescrollt. Ein gewöhnlicher passiver Hover sperrt keine fokussierte Texteingabe.

Tab/Umschalt+Tab folgt den sichtbaren Controls. Offene Auswahllisten erhalten
ihre Pfeil-/Aktivierungseingaben; Tab schließt sie und bewegt den Fokus weiter.
Escape schließt zunächst eine offene Liste, danach nur die eigene Aufgabe.
Der aufrufende Control benennt sein tatsächliches Quellfenster und seine Aktion;
ein alter Menü-Fokusrücklauf darf die Aufgabe nach dem Öffnen nicht übersteuern.
Fokus kehrt beim Schließen nur zu einem noch vorhandenen sichtbaren Auftraggeber
zurück, wenn das Aufgabenfenster selbst Tastaturfokus hatte. Ein geschlossener
Auftraggeber wird über seine Fenster-ID erkannt; fremde aktive Arbeit wird nicht
durch verzögerte Fokuskorrektur übersteuert. Eigene Ressourcen werden an einer
Frame-Grenze freigegeben; geliehene Fonts bleiben Eigentum des Hauptkontexts.
Erzeugen und Freigeben erhalten den tatsächlich aktuellen GL-Kontext des
Aufrufers, auch beim Öffnen per Maus innerhalb des Einstellungsrenderers.

Pointerfokus gilt nur innerhalb des tatsächlichen sichtbaren Ausschnitts.
Ein hinter der festen Fußleiste abgeschnittenes Eingabefeld darf deren freien
Klickbereich nicht übernehmen; die nächste Tabaktion bleibt dadurch erhalten.
[Gegenprobe, Windows-/Linux-Nachlauf und kompakte Belege](../../verification/unit-focus-2026-10-31.md).

Der neue native Fall verwendet eine tatsächlich geschriebene und geladene
Messdatei mit zwei Kanälen. Die C-/Physim-Lauffälle behalten numerische und
Dateischutzprüfungen; ihre inhaltlich ungeprüften Vollbilder sind durch Textnachweise
ersetzt. Analyse und Inspektor behalten ihre gezielten Rasterassertionen.
Die vollständige GUI-, manuelle Screenreader-, physische Eingabe-, Monitor-/DPI-
und Plattformabnahme ist durch diese Implementierung noch nicht bestätigt.
Der Nutzer-Mac bleibt ausgeschaltet; gehostete Tests getrennt nach Quellstand werten.
