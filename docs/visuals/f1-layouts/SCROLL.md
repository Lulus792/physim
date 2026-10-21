# F1 H: gemeinsamer Artikel-Scrollbereich und volle Navigationsbreite

Die HTML-Vorlage enthält Herkunftszeile, Überschrift und Artikel in einer
gemeinsamen `.article-scroll`-Fläche. Nativ lag die Herkunftszeile bisher über
der scrollenden Gruppe. Zusätzlich verkleinerte Nuklear deren Inhalt um die
Scrollleistenbreite. Das betraf auch die linke Navigation.

Der native Artikel verwendet jetzt die gesamte mittlere Fläche als Viewport.
Sein Inhalt bleibt mittig auf höchstens 810 px mit 28 px horizontalem Abstand
begrenzt; bis einschließlich 900 px Fensterbreite gelten 20 px. Herkunft und
Artikel scrollen gemeinsam. Der seitliche Rand bleibt auch bei breitem Fenster
für das Mausrad nutzbar. Die vertikale Nuklear-Scrollleiste sitzt am äußeren
Rand der Fläche; eine zusätzliche leere horizontale Scrollleistenzeile entfällt.

Die linke Navigation erhält ebenfalls eine überlagerte Scrollleiste. Bei
250 px Seitenbreite und zweimal 15 px Innenabstand sind ihre Themenzeilen
220 px breit, bei 220 px Seitenbreite 190 px. Die bisher zusätzlich abgezogenen
7 px entfallen. Das ist eine Änderung der Inhaltsgeometrie; weitere
Navigationsabstände und die Kontextspalte brauchen ihren eigenen Abgleich.

Die obere Herkunft bleibt bei Schrift-/Seitenleistenwechseln am Artikelanfang
sichtbar. Tiefe Lesepositionen verwenden weiterhin die native Textposition.
Suche und Aktionsmenü überlagern den Artikel. Ein Mausradereignis über dem
Artikel erhält das aktive Suchfeld einschließlich Auswahl; Ereignisse über
dem Suchfenster laufen nicht zum Artikel durch. Abschnittssprünge beachten
die sichtbare Suchüberlagerung. „Ende“ wird nach dem aktuellen Umbruch aus den
tatsächlich eingereichten Zeilen ermittelt.

## Nachweisumfang

Der neue native SDL-Prüfweg `documentation_scroll` prüft sechs Kombinationen:
1080×780, 760×540 und 1440×900, jeweils Dunkel/16 px und Hell/32 px Leseschrift
bei 16 beziehungsweise 22 px allgemeiner UI-Größe. Er verwendet echte
Tastatur-/Mausradereignisse, tatsächliche Widget-Rechtecke und native Aufnahmen.
Er prüft gemeinsam scrollende Herkunft und Überschrift, feste Seitenflächen,
CSS-Inhaltsbreiten, Home/End, einen unmittelbar vorausgehenden Schriftumbruch,
den letzten Textblock, Schrift-/Navigationswechsel am Artikelanfang sowie
Mausrad und Abschnittssprung bei geöffnetem Suchfeld mit echter Textauswahl.

Der [maschinenlesbare Nachweis](scroll-2026-10-21/validation.json) enthält
LF-normalisierte Quellhashes, getrennte Fremdänderungen, ausgeführte Befehle,
Ergebnisse, unveränderte Referenzhashes und verlustfreie native Bilder. Windows
verwendet den MSVC-Debugbuild mit AddressSanitizer; Alpine-Dev den GCC-Debugbuild
mit AddressSanitizer/UndefinedBehaviorSanitizer, Xvfb und Openbox.

| Quellstand | Windows | Alpine-Dev / Linux |
| --- | --- | --- |
| 74 | Build scheitert an fehlender expliziter Rechteck-Konvertierung; keine UI-Prüfung | Derselbe Compilerfehler; keine UI-Prüfung |
| 75 | 4/4, danach 11/12; Paneelwiederholung 1/1 | 4/4 und 12/12 |
| 76 | 14/16; Paneele ohne Hostfokus, H mit 60-s-Abbruch bei Stage 21 | 16/16 |
| **77, endgültig** | Breiter Lauf 14/16, erste gezielte Wiederholung 0/2; **mit tatsächlichem Hostfokus 2/2** auf unverändertem Code | **16/16** |

Damit sind auf dem endgültigen Stand **16 verschiedene Gruppen je Plattform**
positiv belegt, unter Windows verteilt über breiten und gezielten Lauf. Der
breite Windows-Lauf wird dadurch nicht nachträglich grün. Der Nutzer hat für
die abschließende Wiederholung das Testfenster im Vordergrund gehalten.

Der endgültige Scrolltest verarbeitet Schriftwechsel und End-Taste vor
demselben neuen Zeilenaufbau. Er prüft den tatsächlich sichtbaren letzten
Textblock und erhält eine per Tastatur erzeugte Auswahl im Suchwort „Pendel“.
Seine Aufnahmen werden vor dem folgenden Zustandswechsel gespeichert.

Zwölf Rasterprüfungen an der äußeren Scrollleistenspalte belegen pro Plattform
und Fenster-/Schriftkombination die Lage des nativen Griffes bei Anfang,
PageDown und Ende. Sie beweisen keine Gleichheit mit einer Browser-Scrollleiste,
deren Betriebssystemdarstellung die CSS-Datei nicht festlegt. 112 vollständige
native H-Bildpaare zwischen Stand 75 und 77 sind pixelgleich. Das begrenzt die
Auswirkung der End-/Prüfweg-Verfeinerung auf diese Zustände; es ist kein Vergleich
der gesamten nativen Oberfläche mit der HTML-Vorlage.

Die unveränderten begrenzten Flächenprüfungen gegen H bestehen auf beiden
Plattformen. Die Titelkantenprüfung besteht unter Windows bei weiterhin 1 px
Toleranz; unter Linux bleibt sie wegen anderer Glyphen und Zeilenumbrüche rot.
Der gespeicherte 73er-Gegenfall hat einen 580 px breiten Artikelplatz, aber nur
556 px Körper-Viewport. Er verletzt den neuen Breitenvertrag; dies ist eine
erneute Auswertung alter Laufzeitgeometrie, kein neuer Lauf des alten Programms.

Nach der endgültigen Prüfung wurden 364 ersetzte Rohbilder erfolgreicher
75er-/76er-Gruppen entfernt: 781.221.432 Dateibytes, etwa 745 MiB.
Die [Einzelliste mit Hashes](scroll-2026-10-21/cleanup.json) dokumentiert die
abgeschlossene Bereinigung. Aktuelle 77er-Aufnahmen, fehlgeschlagene Gruppen,
H-Bildpaare, Scroll-Vorläufe und Formel-/Inline-/Typeset-Belege bleiben erhalten.

| Native Aufnahme | Windows | Linux |
| --- | --- | --- |
| Artikelanfang, Dunkel / 16 px | [Bild](scroll-2026-10-21/windows-scroll-0-top.png) | [Bild](scroll-2026-10-21/linux-scroll-0-top.png) |
| PageDown, Herkunft mitgescrollt | [Bild](scroll-2026-10-21/windows-scroll-0-page.png) | [Bild](scroll-2026-10-21/linux-scroll-0-page.png) |
| Ende nach Schriftwechsel auf 18 px | [Bild](scroll-2026-10-21/windows-scroll-0-end.png) | [Bild](scroll-2026-10-21/linux-scroll-0-end.png) |
| Abschnitt unter geöffneter Suche | [Bild](scroll-2026-10-21/windows-scroll-0-find.png) | [Bild](scroll-2026-10-21/linux-scroll-0-find.png) |
| Mindestfenster, Hell / 32 px | [Bild](scroll-2026-10-21/windows-scroll-3-top.png) | [Bild](scroll-2026-10-21/linux-scroll-3-top.png) |
| Breites Fenster mit begrenzter Artikelspalte | [Bild](scroll-2026-10-21/windows-scroll-4-top.png) | [Bild](scroll-2026-10-21/linux-scroll-4-top.png) |

## Grenzen

Dies ist keine vollständige H- oder Gesamtoberflächenabnahme. Die hier
geprüfte Scrollgeometrie ersetzt keinen vollständigen Bildvergleich. Native
Artikel enthalten die echten Dokumente und zusätzlichen Quellcode; die kurze
Browserdemo ersetzt diese Inhalte nicht. Linux verwendet weiterhin eine
andere Schrift mit abweichenden Glyphenbreiten. Die CSS-Referenz nennt Segoe UI;
die gemeinsame Schriftgrundlage bleibt zu klären. Kontextdetails und weitere
Navigationsmetriken sind weiterhin offen.

Die lokale HTML-Datei war über das Browserwerkzeug gesperrt. Dieser Schritt
stützt sich auf die lesbaren HTML-/CSS-Quellen, unveränderte Referenz-PNGs und
native Aufnahmen. Er beansprucht keine neue interaktive Browserabnahme und
keinen macOS-Lauf. Bereits vorhandene lokale Accessibility-Änderungen bleiben
separat erfasst; die lokalen Builds sind kein sauberer Checkout nur dieses
Änderungspakets.
