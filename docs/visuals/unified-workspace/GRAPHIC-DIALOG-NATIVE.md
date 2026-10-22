# Grafikexport: sichtbarer nativer Berichtsdialog

Stand 22. Oktober 2026: Der neue Dialog ist an echte Berichtsdiagramme angebunden.
Die alte breite PNG-/SVG-/Größenleiste aus der Nutzeraufnahme 23:36:03 entfällt;
neben den kompakten Zoomaktionen steht **Grafik exportieren…**. CSV bleibt separat.
Dies ist ein weiterer Teilstand des gesamten UI-Auftrags, keine vollständige
Abnahme von Auswerten, F1 oder der übrigen Oberfläche.

## Umgesetzter Ablauf

Die native C-/SDL-/Nuklear-Oberfläche übernimmt den freigegebenen Aufbau aus
[ADR 010](../../decisions/010-analysis-graphic-export.md) und
[EXPORT-EXAMPLE](EXPORT-EXAMPLE.md): Vorschau links, Optionen rechts, Abschluss
unten; bei wenig Platz untereinander und vollständig scrollbar. PNG startet
mit 2400 × 1700 Pixeln, ganzem Diagramm und weißem Papier. Alle vier PNG-Größen,
SVG, der tatsächliche X/Y-Ausschnitt und die beim Öffnen festgehaltene Darstellung
sind angebunden. Der Dateiname bleibt vollständig umbrochen lesbar.

Der Dialog kopiert das tatsächlich gewählte Berichtsdiagramm mit Masken,
Einheiten, Titel und Achsengrenzen. Spätere Auswahl- und Themeänderungen ändern
diesen Stand nicht. Berichtskurven erhalten keine erfundene Laufreferenz.
Vorschau, PNG, SVG und Bildkopie verwenden die gemeinsame Figur; die Vorschau
zeigt die kanonischen RGB-Pixel als Textur. Hoher Kontrast besitzt eine eigene
schwarze Papierpalette bei **Wie in der Auswertung**; Standard bleibt Weiß.

Speichern öffnet den SDL-Dateidialog. Der Callback hält nur einen eigenen
CPU-Auftrag; ein einzelner Schreibthread schreibt eine exklusiv angelegte
Geschwisterdatei und ersetzt erst nach erfolgreichem Schreiben das Ziel.
Abbruch, Fehler und Erfolg werden angezeigt. Ein später Callback besitzt keine
Zeiger auf die bereits geschlossene Oberfläche. Dies verspricht keine
Stromausfallbeständigkeit eines Dateisystems.

Bild kopieren veröffentlicht eigene PNG-/BMP-Bytes. Ein unabhängiger Prozess
übernimmt das Bild erst nach Zerstören von Dialog und Raster und vergleicht
sämtliche RGB-Pixel: Windows über echtes CF_DIB, Linux über X11 und einen
zweiten SDL-Client. Dies ist eine tatsächliche OS-Bildübergabe.

Tab erreicht auch die zunächst außerhalb des Ausschnitts liegenden Aktionen.
SVG überspringt die deaktivierte Pixelgrößenwahl. Escape schließt zuerst eine
offene Auswahl, dann den Dialog; der Exportbutton erhält den Fokus zurück.
Hintergrundklicks und App-Tastaturaktionen werden währenddessen abgefangen.

## Nachweise und eingegrenzte Fehlerkorrekturen

[Prüfmanifest](graphic-dialog-2026-10-22/validation.json) und zugehörige native
Aufnahmen enthalten die tatsächlichen Ergebnisse, Quellenhashes und Grenzen.
Der finale Quellstand ist **export-dialog-source17**. Die getrennten vorigen
Regressions- und Kernläufe gelten nur für ihre jeweils unveränderten Bausteine;
sie sind kein erfundener gemeinsamer Gesamtlauf.

| Tatsächlich ausgeführt | Ergebnis |
| --- | --- |
| Windows Debug / MSVC / ASan | `graphic_dialog` 1/1, 30,656 s |
| Alpine-Dev / GCC / ASan+UBSan / X11+Mesa | `settings_keyboard_workflow`, `graphic_dialog` 2/2; Dialog 68,211 s |
| Windows Release | `window_smoke`, `graphic_dialog` 2/2; Dialog 7,906 s |
| Testkatalog auf Windows und Linux | Beide Selbstprüfungen bestanden, einschließlich bewusst fehlschlagender Katalogfälle |

Die Settings-Prüfung besteht auch unter Windows im separat erhaltenen Stand 16.
Hierarchie/Sichtbarkeit bestehen in den getrennten zwei Gruppen pro Plattform;
die unveränderte HC-Figur besteht jeweils im eigenen Kernlauf.

Die Diagnose hält auch fehlgeschlagene Stände fest. Ein Flow-Buttonadapter
ersetzte innerhalb des absoluten Dialoglayouts die ganze Zeilenhöhe durch eine
Buttonhöhe; ein eigener fester Buttonpfad erhält Layout und Accessibility.
Parenthesieren half nicht, weil die Umleitung ein objektartiges C-Makro ist.
Zwei weitere Ursachen waren ein nicht zurückgesetzter Scrolloffset nach
Vergrößern des Fensters und eine Fokusabfrage, die am gleichnamigen Textlabel
statt am tatsächlichen Feld stoppte. GDB-Beobachtungen sind getrennt von normalen
Sanitizerläufen erhalten; absichtlich gestoppte Diagnoseprozesse sind keine
bestandenen App-Tests.

Die frühen Stände 14/15 setzten irrtümlich UI-Schriftgröße 32 außerhalb der
vorhandenen vier Hauptfontbänke. Linux meldete den Zugriff mit UBSan; nominell
erfolgreiche Windows-Läufe dieser Stände belegen daher keine gültige große
Schrift. Die Einstellung wird jetzt vor einem Fontzugriff validiert. Der finale
Test verwendet gültige 22 px und zusätzlich eine explizite doppelte
Dialogskalierung. Diese Komponentengeometrie ist kein Nachweis einer globalen
32-px-Einstellung oder eines physischen 200-%-DPI-/Monitorwechsels.

Stand 16 erreichte unter Linux nach erfolgreicher nativer Bildkopie beim
zusätzlichen Skalierungsschritt das alte 60-s-Limit des inzwischen erheblich
erweiterten Tests. Stand 17 gibt ausschließlich dem erweiterten Dialogfall
120 s und protokolliert Stage und Laufzeit bei Timeout. Andere Plotfristen
bleiben erhalten. Ein Timeout wird ausdrücklich als fehlgeschlagen erfasst.

## Verbleibende Abweichungen und Abnahme

- Der folgende [sichtbare Messlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md)
  integriert den gemeinsamen Cache und denselben Exportzugang für bis zu acht
  ausgewählte Läufe. Seine eigenen Nachweise bleiben von dieser früheren
  Berichtsdialog-Abnahme getrennt.
- Der folgende [Materialabgleich](GRAPHIC-MATERIAL-NATIVE.md) ergänzt den reinen
  Sigma-3-Hintergrundfilter und skalierte Auswahlfelder. Der folgende
  [Schatten-/Maßabgleich](GRAPHIC-SHADOW-NATIVE.md) ergänzt die Schatten,
  Referenzgröße und kleine Umbrüche. Weitere Schrift-/Raster-/Komponentendetails bleiben abzugleichen.
  Es besteht keine behauptete Pixelidentität.
- Die echten Dateidialog-APIs sind angeschlossen; automatisierte Speichertests
  injizieren deren Antwort. Die Bedienung des OS-Auswahldialogs, dessen
  Überschreiben-/Abbruchfokus und reale Monitorwechsel sind damit nicht belegt.
- Dialogrolle, vollständige Assistive-Modalsperre und externe Screenreader,
  ausgehängter Quellbereich, physische DPI und macOS benötigen eigene Abnahme.
  Der neue Clipboard-Prüfer ist ausdrücklich auf Windows und Linux begrenzt.
- Die globale UI-Einstellung bietet weiterhin 16/18/20/22 px. Eine globale
  200-%-Option verlangt zusätzliche Font-, Layout- und Persistenzarbeit.

Lokale Builds enthalten separat erfasste Accessibility-Arbeiten. Diese werden
erhalten und nicht diesem Patch zugeschrieben. Kein sauberer isolierter
Checkout und keine vollständige Plattformgleichheit werden behauptet.

## Speicherpflege

Die geplante Entfernung von 1.058 eigenen ersetzten Rohbildern und lokalen
Vorschauen (4.637.014.302 Bytes) wurde von der automatischen Freigabeprüfung mit
**blocked by policy** abgelehnt. Ein genauerer Grund wurde nicht geliefert.
Es wurde nichts gelöscht und kein Speichergewinn behauptet; kein Umgehungsweg
wurde versucht. [Plan und Status](graphic-dialog-2026-10-22/cleanup.json) bleiben
für eine spätere zulässige Bereinigung erhalten. Nutzdaten, Referenzen,
Programme, Symbole und Abhängigkeiten waren nicht als Löschziele vorgesehen.
