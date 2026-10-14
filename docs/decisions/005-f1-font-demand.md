# F1-Schriften nach Bedarf und Besitz der Atlanten

Architekturentscheidung vom 15. Oktober 2026. Der zusätzliche Atlasbesitz im
Backend ist implementiert; F1s Größenwahl verwendet ihn noch nicht.
Die vollständige Struktur ist deshalb noch nicht abgenommen. Sie folgt dem zentralen
`software-architect`-Skill unter `D:/Skills/skills` und den tatsächlich
ausgeführten Messungen in [004](004-f1-handbook.md).

## Problem und unveränderte Anforderungen

F1 erzeugt beim Öffnen 38 primäre Schriftvarianten samt ergänzenden Quellen:
neun normale, neun Titel-, elf fette und neun Codegrößen. In der anfänglichen
Ansicht wird nur ein Teil dieser Größen gebraucht. Im gemessenen ASan-Lauf
braucht allein die CPU-Berechnung des F1-Atlas 6,325 Sekunden; Prüfung und
Upload benötigen zusammen 0,013 Sekunden. Der R8-Atlas spart Speicher, reicht
aber für verlässlich kurze Bereitschaft nicht aus.

Alle bisherigen Codepunkte, echten Schriftquellen, Rollen, Größen bis 32 Pixel,
H2-Größen bis 36 Pixel, Oversampling und Dichten müssen erhalten bleiben.
Ein fehlgeschlagener Aufbau darf weder alte Handles noch lesbaren Inhalt,
Zoom, Auswahl, native Inhaltsversion oder Scrollposition beschädigen.
Die vorhandenen Bereitschafts- und Clientgrenzen bleiben bestehen.

## Verglichene Strukturen

| Struktur | Vorteil | Konkretes Risiko |
| --- | --- | --- |
| Ein Atlas nur für die gerade benötigten Rollen und Größen, bei jedem Zoom vollständig ersetzen | Weniger Eigentümer und weitgehende Wiederverwendung des vorhandenen Ersatzpfads | Jeder Zoom wiederholt bereits geleistete Arbeit; ein Austausch während eines bereits begonnenen Frames kann dessen alte Textbefehle ungültig machen. Erfordert zusätzlich eine sauber verzögerte Größenübernahme. |
| Fehlende Varianten in zusätzlichen Atlanten berechnen und bereits veröffentlichte Atlanten bis zum Dichtewechsel behalten | Einmal berechnete Größen bleiben nutzbar; bestehende Textbefehle und Handles bleiben während Zoom gültig | Mehrere GPU-Ressourcen müssen begrenzt, eindeutig besessen und gemeinsam beim Dichtewechsel oder Schließen freigegeben werden. |

Für die Umsetzung wird die zweite Struktur gewählt. Der Renderer kann bereits
pro Textbefehl verschiedene Fonttexturen verwenden; `nk_sdl` besitzt aktuell
den Atlas, die Fonttextur und das weiße Nulltexel. Das zusätzliche Eigentum
gehört daher ebenfalls in `nk_sdl`, nicht in den Dokumentparser oder die
nativen Accessibility-Brücken. Die Anwendung entscheidet über benötigte Rollen
und Größen; das Backend verantwortet Bake, Validierung, Upload und Freigabe.

## Vertrag und Umsetzungsstand

- Beim Öffnen werden nur die benötigten normalen, Titel-, Code- und fetten
  Varianten einschließlich der größeren H2-Rolle vorbereitet. Jede Variante
  erhält weiterhin den vollständigen bisherigen Zeichenvorrat.
- Ein Ergänzungsauftrag baut ausschließlich fehlende Varianten. Alte Atlanten
  und bereits aufgezeichnete Befehle bleiben gültig. Erst nach vollständigem
  Erfolg werden neue Handles veröffentlicht; ein Fehler verwirft nur den
  Kandidaten. Das Backend nimmt nur unberechnete Kandidaten an; nach einem
  fehlgeschlagenen Upload verbleiben berechnete Daten im Besitz des Aufrufers.
  Dieser muss sie freigeben, bevor er einen neuen Kandidaten vorbereitet.
- Die Zahl der ergänzenden Atlanten ist durch die Zahl der möglichen Varianten
  begrenzt. Es gibt kein unbegrenztes Wachstum pro Zoomereignis oder Fontgröße.
- Bei neuer Rasterdichte wird der aktuell benötigte Satz vollständig vorbereitet.
  Erst anschließend werden Stil und Handles übernommen und alle Atlanten der
  alten Dichte gemeinsam freigegeben. Eine gescheiterte Vorbereitung erhält
  den vollständigen bisherigen Satz.
- Zoom und Anwendungseinstellungen dürfen erst nach erfolgreicher Vorbereitung
  auf Handles einer neuen Größe verweisen. Der GL-Kontext muss dabei explizit
  der Dokumentationsansicht gehören; Einstellungen werden teilweise aus dem
  Hauptfenster heraus angewendet.
- Die Zeicheninventur muss weiterhin sämtliche 38 Varianten tatsächlich
  aufbauen und prüfen können. Sie darf fehlende Größen weder überspringen noch
  durch anders große Handles ersetzen.

## Umsetzung und erforderliche Nachweise

Das Backend besitzt jetzt bis zu 38 zusätzliche Atlanten. Die gemeinsame
Upload-/Validierungsroutine wird auch vom bisherigen Dichteersatz verwendet.
Danach wird F1s Größenwahl mit diesem Vertrag verbunden;
Zoom- und Einstellungsfehler benötigen echte Rücknahme der Größenänderung.
Die bestehende Dichteersetzung und der vollständige Inventurpfad werden an
denselben Besitzvertrag angeschlossen.

Vor einer Abnahme sind erforderlich: tatsächliche Kaltstart-/F1-Zeiten mit
derselben Schriftabdeckung, alle 38 Varianten mit System- und mitgelieferten
Schriften, wiederholter Zoom einschließlich Zurücksetzen, unabhängige UI-/Code-
Einstellungen, 1/1,25/1,5/2-Dichterundlauf, vorher erzeugte gültige Textbefehle,
gezielte Allokations-/Uploadfehler und vollständige Freigabe. GPU-Bilder und
native Lese-/Feld-/Linkabläufe müssen auf den jeweils ausgeführten Plattformen
weiterhin bestehen. Die bisherigen Backend-Nachweise ersetzen keine Abnahme
des noch fehlenden Aufbaupfads nach Bedarf.

## Bisheriger Backend-Nachweis

Der echte GPU-Fall `font_bank_ui` zeichnet Text, ergänzt einen Atlas bei noch
aufgezeichneten Textbefehlen und verlangt anschließend bytegleiche BMP-Ausgabe.
Er zählt tatsächlich helle Textpixel und zeichnet auch mit dem ergänzten Font.
Er prüft die Kapazitätsgrenze, abgewiesene andere Rasterdichte, fehlgeschlagene
Allokation, erhaltenen Bestand bei fehlgeschlagenem Dichteersatz und einen
erfolgreichen Dichtewechsel. Zählende Allokatoren und echte `glIsTexture`-
Abfragen prüfen vollständige Freigabe beim Ersatz sowie beim Schließen.

Der erste Stand besteht unter MSVC ASan zusammen mit dem bisherigen
F1-Dichterundlauf in `run-lx6hmyx9` **2/2**, unter GCC 15.2 ASan/UBSan mit
X11/Openbox zusammen mit dem GPU-Renderer in `run-tanthpk0` **2/2**.
Die Windows-Bilder vor/nach Ergänzung und mit ergänzter Schrift wurden geprüft.
Der Plattform-Katalog samt absichtlichen Fehlergegenproben besteht in
`build/native/font-bank-runner-proof`.

Eine zusätzliche kontrollierte GL-Fehlergegenprobe findet anschließend eine
Retry-Lücke (`run-o8ix6moi`): Ein Kandidat mit bereits berechneten Puffern wurde
erneut angenommen. Da Nuklears Bake diese Puffer nicht vor einem neuen Bake
freigibt, könnte dies den alten Speicherbesitz überschreiben. Die gemeinsame
Vorprüfung weist solche Kandidaten jetzt ohne weiteren Aufbau zurück.
Die Gegenprobe kontrolliert außerdem, dass die beim Grafikfehler neu erzeugte
Textur gelöscht wird und der zuvor installierte Font gültig bleibt. Dies ist
eine kontrollierte GL-Fehlerweitergabe, keine vorgetäuschte echte GPU-OOM-Prüfung.

Die erste Korrektur macht eine zweite Freigabelücke sichtbar (`run-u_k1g7uw`):
`nk_font_atlas_clear` gibt den Pixelpuffer zwischen erfolgreichem Bake und
fehlendem `atlas_end` bisher nicht frei. Der vendorte Clear-Pfad gibt jetzt auch
diesen temporären Puffer frei. Der native Grenztest bricht beide Atlasformate
gezielt vor `atlas_end` ab und verlangt jeweils null verbleibende Allokationen.

Der finale Windows-Stand besteht mit MSVC ASan in `run-w5tcouw4` **3/3**
native Schriftfälle und in `run-fph31f8x` **3/3** GUI-Fälle: Atlasbesitz samt
kontrolliertem Grafikfehler, vollständige F1-Zeicheninventur mit System- und
mitgelieferten Fonts sowie zehnfacher unabhängiger Link-Stresstest.
Die finale Katalog-/Fehlergegenprobe besteht in
`build/native/font-bank-final-runner-proof`.
Der finale GCC-15.2-ASan/UBSan-Stand unter Alpine 3.24.2 besteht native
Schriftfälle in `run-f9mjpamq` **3/3** und unter X11/Openbox Atlasbesitz,
GPU-Renderer sowie unabhängige AT-SPI-Links in `run-8p39b1tp` **3/3**.
Der zentrale `code-reviewer`-Skill wurde auf Eigentumsübergabe, Lebenszeiten,
Abbruch und Ersetzen angewendet. Die beiden dabei konkretisierten Fehlerfälle
sind durch die genannten Gegenproben und finalen Erfolge eingegrenzt.
Eine macOS-Ausführung des neuen Atlasbesitzes steht noch aus; die angepasste
Workflow-Konfiguration ist allein kein Nachweis dafür.
