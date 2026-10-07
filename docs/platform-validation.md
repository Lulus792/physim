# Plattformprüfung

Stand: 7. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

## Projekterstellung per Tastatur am 7. Oktober 2026

Die Projektmaske besitzt einen zyklischen Tab-/Shift+Tab-Pfad über Zielordner,
Ordnerwahl, Namen, Projekttyp, Vorlage, Experiment-/Auswertungssprache und
Anlegen/Zurück. Reine Analyseprojekte überspringen die Experimentfelder.
Pfeile wechseln Auswahlen; Enter/Leertaste bedienen Aktionen, Escape verlässt
das Formular. Text und Clipboard funktionieren in den Textfeldern, bereits
angekommene Eingaben werden vor Tab abgeschlossen. Der sichtbare Fokus scrollt
mit. Validierungsfehler erscheinen im Formular und werden sichtbar gescrollt.
[Bedienung](workspace.md).

Die erste vollständige Matrix reproduziert auf beiden Plattformen einen
bestehenden Vorlagenfehler: C-Kugelstoß kombiniert mit Physim-Auswertung erhält
die allgemeine Analyse für `position.x`, obwohl das Experiment `a.position` und
`b.position` liefert. Mac-Nachweis unter
`build/contact-world-language-release-mac/test-results/run-26r8ucg0`, Linux unter
`build/contact-world-language-release-linux/test-results/run-8h520cxp`.
Der zusätzliche direkte Runneraufruf endet mit Exit 5 und einer strukturierten
Laufzeitdiagnose an analysis.phys:7:34 (`build/project-keyboard-collision-reproduction.json`).
C-Kugelstoß, Sensorwurf und Boxstoß erhalten nun die passenden Physim-Analysen
wie ihre Physim-Experimentvorlagen. Die unveränderten Vorlagenquellen werden
unabhängig byteweise mit den erzeugten Projekten verglichen.

Die anschließenden Läufe bauen, zeichnen auf und analysieren alle 34 Projekte
bei 16 Pixeln, scheitern aber im nachfolgenden Python-Dateivergleich unter
`build/contact-world-language-release-mac/test-results/run-u0cc1fjb` und
`build/contact-world-language-release-linux/test-results/run-uttwyo8_`.
Der Prüfer ordnet dem Wurf mit Luftwiderstand fälschlich projectile.phys zu;
projectile_drag.phys ist die beabsichtigte und tatsächlich erzeugte Vorlage.
Ein unabhängiger erneuter Vergleich bestätigt alle 34 Mac-Projektquellen.
Die fehlgeschlagenen Gesamtfälle werden nicht nachträglich als Erfolg gewertet.

Die zusätzliche Tastaturprüfung für ungültige Namen, einen Zielpfad, der eine
Datei ist, und vorhandene Projektquellen besteht unter macOS unter
`build/contact-world-language-release-mac/test-results/run-_hs6tipl` und Linux
unter `build/contact-world-language-release-linux/test-results/run-db2y9afz`.
Die vorhandene UTF-8-Quelldatei bleibt bytegleich, Escape und Zurück funktionieren.
Die Aufnahme `build/project-keyboard-existing-error.png` wurde visuell geprüft;
Fokus und vollständige Fehlermeldung sind sichtbar.

Neun geänderte Code-/Test-/Werkzeugdateien stimmen zwischen den Plattformen
per SHA-256 überein (`build/project-manager-keyboard-source-freeze.json`).

Die endgültige Linux-Matrix besteht unter
`build/contact-world-language-release-linux/test-results/run-t3l3l__0`.
Sie erzeugt und prüft mit 16 und 22 Pixeln jeweils die 32 Vorlagen-/Sprachpaare
und zwei unabhängige Analyseprojekte: insgesamt 68 Projekte. Sämtliche Projekte
werden gebaut und ausgewertet; die Experimente werden tatsächlich aufgezeichnet,
Analyseprojekte importieren einen zuvor aufgezeichneten Lauf. Experiment- und
Auswertungsquellen werden unabhängig byteweise mit den beabsichtigten SDK-Vorlagen
verglichen. Verborgene Experimentfelder werden übersprungen; Zurück funktioniert.

Die Prüfkiterstellung besteht direkt mit dem aktualisierten Prüfer auf beiden
Plattformen (`build/project-keyboard-kit-check-{mac,linux}.log`): 58 exakt
manifestierte Dateien, SHA-256, Grenzen und Fehlererhaltung bleiben geprüft.
Der neue SDK-Gate verlangt dieselbe vollständige Matrix, einschließlich Fehlereingaben,
und liest die konkreten Erfolgsmarker. Nur dieser längere Aufruf erhält ein
910-Sekunden-Limit; die bisherigen 300-Sekunden-Limits bleiben erhalten.

Dieselbe endgültige Matrix mit 68 Projekten besteht auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-s8h2bpyy`.
Die Schriftgrößenläufe sind getrennte App-Prozesse mit eigenen Projektordnern.
Die ersten 34 Projekte werden nicht als Nachweis der späteren 22-Pixel-Runde
verwendet: beide Runden erreichen eigenständig den vollständigen Erfolgsmarker.

Die macOS-Aufnahme `build/project-keyboard-focus-22.png` zeigt den sichtbaren
Tastaturfokus beim Anlegen im kleinen Fenster mit 22 Pixeln. Die lange
Zurück-Beschriftung kann in dieser Ansicht teilweise außerhalb des Ausschnitts
liegen; eine vollständige responsive Abnahme sämtlicher Texte bleibt offen.
Die Prüfkiterstellung im Release-Katalog besteht außerdem unter
`build/contact-world-language-release-mac/test-results/run-v_wo3pds`.

Beide verschobenen SDK-Apps bestehen dieselbe 68-Projekt-Matrix und vier weitere
gezielte Abläufe unter `build/project-manager-keyboard-sdk-proof-mac` und
`build/project-manager-keyboard-sdk-proof-linux`: Fehlereingaben, beide
Tastaturmenügrößen und die vorhandene Mausprüfung für unabhängige Analyseprojekte.
`PASSED.json` enthält App-Prüfsumme, sämtliche Befehle und Exitcodes. Die Apps
sind bytegleich zu den geprüften Release-Binaries; alle übrigen SDK-Produkte
bleiben bytegleich zum vorherigen SDK. Dies ist eine gezielte App-/Projektabnahme,
keine neue vollständige numerische SDK-Suite.

Die gesammelte Plattformseite überschreitet durch den neuen Nachweis die
256-KiB-Grenze des Handbuchs. Ältere Abschnitte ab „Lokale Szenenkoordinaten“
wurden deshalb vollständig in die vorhandene Historienseite verschoben:
130730 erhaltene Bytes, SHA-256
`19300ad2b3b388a451347d401c4f91a7b648626b4801d9b7150e84359633c7de`.
Beide Seiten liegen unter der Grenze. Die abschließende verschobene SDK-App
prüft nach Dokumentations-/Manifestaktualisierung nochmals sämtliche Themen
und beide Lernwege (`DOCS-PASSED.json`). Die Codeprodukte bleiben dabei unverändert.

Der Katalog enthält nun 75 Fensterfälle. Neue vollständige Windows-/Apple-Silicon-
und Remote-Abnahmen bleiben separat erforderlich. Vollständige Tastaturführung
weiterer Arbeitsbereiche und Screenreader-Zugang bleiben offen; der Projektplan
ist nicht vollständig abgenommen.

## Handbuch per Tastatur am 7. Oktober 2026

Das Handbuch besitzt einen aus dem aktuellen Inhalt erzeugten Tab-/Shift+Tab-Pfad.
Er umfasst beide Lernwege, Themen/Inhalt, globale Suche und Kategorie, Themen
oder Überschriften, Zurück/Start/Arbeitsbereich, Dokumentensuche und Treffer,
Lesebereich sowie sämtliche Links und Codekopieren-Schaltflächen. Der Fokus
scrollt sichtbar mit. Pfeile und Bild auf/ab scrollen im Lesebereich; Home/End
springen zum Anfang/Ende. Kategorien, Suche und Navigation bleiben per Tastatur
bedienbar; Mausbedienung und Escape bleiben erhalten. [Bedienung](workspace.md).

Die neuen SDL-Prüfungen öffnen beide Lernwege ohne Maus, wählen das C-Tutorial,
springen im Inhaltsverzeichnis, vergleichen kopierten Code vollständig mit dem
Quellblock, folgen einem internen Link und gehen zurück. Sie prüfen Lesetasten,
Dokumentensuche und globale Suche samt Rücksetzen sowie Schließen per Escape.
Dies geschieht im 760×540-Fenster mit 16 und 22 Pixeln UI-Schrift.

Der zunächst fehlschlagende Eingabeordnungstest unter
`build/contact-world-language-release-mac/test-results/run-xcarrtzj` reproduziert
Zeichenverlust bei Text und unmittelbar folgendem Tab im selben SDL-Paket.
Vor dem semantischen Fokuswechsel wird jetzt die eingegangene Textfeld-Eingabe
mit Nuklears Editorlogik abgeschlossen. Eine zusätzliche Prüfung ohne Fenster
kontrolliert UTF-8, Auswahlersetzung, Rücktaste und eine volle Puffergrenze mit
unverändertem Schutzbyte. Die bisherige Text-/Clipboard-Prüfung bleibt erhalten.

Die Eingabe- und Pufferprüfungen bestehen auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-righmsct` und Debian
unter `build/contact-world-language-release-linux/test-results/run-ewj3gvqv`.
Die Mac-Prüfkiterstellung besteht unter `.../run-pmzxh96n`.
Elf geänderte Code-/Test-/Werkzeugdateien wurden zwischen den Plattformen per
SHA-256 verglichen (`build/documentation-keyboard-source-freeze.json`).

Die Sichtprüfung bei 22 Pixeln zeigte schmale Trefferknöpfe und abgeschnittenen
Statustext. Größere Schriften verwenden deshalb getrennte Such-/Trefferzeilen,
weniger Navigationsspalten und umbrochene Hinweise; der Lesebereich verwendet
den tatsächlich verbleibenden Platz.

Ein erster verschobener macOS-SDK-Lauf unter
`build/documentation-keyboard-final-sdk-proof-mac` besteht die neuen
Tastaturfälle, scheitert aber in der bisherigen Mausprüfung beim Wechsel zum
Simulationsbereich (Stufe 14, weiterhin tab=0). Dieser fehlgeschlagene Lauf wird
nicht als SDK-Erfolg gewertet. Der Test filtert nun fremde Zeiger-/Fokusereignisse
wie die bisherigen Plot-/Toolbar-/Einstellungstests. Ein zusätzlicher Ablauf
injiziert echte SDL-Zeiger-, Loslass-, Scroll- und Fokusereignisse fremder Geräte;
die bisherigen Mausaktionen, Dokumentensuche und Lernwegnavigation bleiben
unverändert geprüft. Das allein belegt nicht die Ursache des ursprünglichen
sporadischen Fehlschlags.

Die endgültigen vier Handbuchabläufe bestehen auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-a_l1vl91` und Debian
unter `build/contact-world-language-release-linux/test-results/run-ssekxyqu`.
Sie umfassen die bisherige Mausprüfung, beide Tastatur-/Schriftgrößenprüfungen
und den neuen Ablauf mit fremden Zeigerereignissen. Die Aufnahme
`build/documentation-keyboard-final-focus-22.png` wurde visuell geprüft:
Trefferknöpfe und vollständiger Statustext sind bei 22 Pixeln lesbar.

Die aktuelle verschobene macOS-SDK-App besteht sieben gezielte Abläufe unter
`build/documentation-keyboard-reviewed-sdk-proof-mac`: die vier Handbuchabläufe,
Einstellungen per Tastatur und kleine/große Tastaturmenüs. `PASSED.json` hält die
App-Prüfsumme und sämtliche Befehle/Exitcodes fest. Alle übrigen SDK-Produkte
sind bytegleich zum vorherigen SDK. Dies ist eine gezielte App-Abnahme und keine
neue vollständige numerische SDK-Suite. Der Katalog enthält jetzt 73 Fensterfälle.
Dieselben sieben Abläufe bestehen mit der aktuellen Linux-SDK-App unter
`build/documentation-keyboard-reviewed-sdk-proof-linux`. Ihre protokollierte
App-Prüfsumme stimmt ebenfalls mit dem aktuellen Release-Binary überein.

Vollständige Tastaturführung in weiteren Arbeitsbereichen und Screenreader-Zugang
bleiben offene Ziele von PP-0710; der Gesamtplan ist nicht abgenommen.

## Persönliche Einstellungen per Tastatur am 7. Oktober 2026

Die persönlichen Einstellungen besitzen einen durchgehenden Tab-/Shift+Tab-Pfad
über 14 Gruppen. Pfeile wechseln Schriftgrößen, Theme und Sicherungsintervall;
Enter/Leertaste bedienen Aktionen und Ansichtsoptionen. Der Fokus erhält einen
sichtbaren Rahmen und scrollt im kleinen Fenster bei 22 Pixeln automatisch mit.
F10 bleibt für die Hauptmenüs verfügbar. Escape und Abbrechen verwerfen den
Entwurf; Standardwerte verändern erst den Entwurf. Tastatur und Maus verwenden
identische Speichervorgänge. [Bedienung](settings.md).

Der neue SDL-Fenstertest öffnet die Einstellungen ausschließlich über F10,
Shift+Tab und Enter, durchläuft alle Gruppen, ändert sämtliche sieben Flags,
speichert und startet erneut. Ein unabhängiger Python-Leser prüft Theme,
Code-/UI-Größe und Intervall sowie sämtliche Flags der gespeicherten Datei.
Der erneute Start prüft Fokuswechsel zum letzten Eintrag und zurück bei
22 Pixeln samt tatsächlich sichtbarer Geometrie. Abbrechen, Escape und
Standardwerte lassen die gespeicherten Werte unverändert. Die macOS-Aufnahme
`build/settings-keyboard-scroll-mac.png` wurde zusätzlich visuell geprüft.

Die ersten vier betroffenen macOS-Fensterprüfungen bestehen unter
`build/contact-world-language-release-mac/test-results/run-m5euel_g`.
Der anschließende Stand mit Fokus-/Scroll-Geometrie besteht in drei Abläufen
unter `build/contact-world-language-release-mac/test-results/run-07ejir_3`;
die letzte Erweiterung für Abbrechen/Standardwerte besteht unter
`build/contact-world-language-release-mac/test-results/run-gx2shw38`.
UI-Geometrie, Einstellungen und Prüfkiterstellung bestehen unter
`build/contact-world-language-release-mac/test-results/run-4kxekxdi`,
die ergänzte SDK-Prüfkiterstellung unter `.../run-hqq9ig8w`.

Der erste Linux-Versuch unter `.../run-j5kkwso4` scheitert bereits beim
SDL-Start, weil nach dem VM-Neustart kein grafischer Desktop läuft.
Nach tatsächlichem Start und Bereitschaftsnachweis von Xvfb/Openbox bestehen
alle sechs betroffenen Fensterabläufe unter
`build/contact-world-language-release-linux/test-results/run-ga41ooqn`.
Dieser Lauf umfasst Maus-Einstellungen, Themes, beide Tastaturmenügrößen,
UI-Schriftgrößen und Einstellungen per Tastatur, vor der abschließenden
Testergänzung für Abbrechen und Standardwerte.

Die abschließende Linux-Tastaturprüfung inklusive Abbrechen und Standardwerte
besteht unter
`build/contact-world-language-release-linux/test-results/run-d_lqaxa7`.
Sechs geänderte Code-/Test-/Werkzeugdateien wurden zwischen beiden Plattformen
per SHA-256 verglichen (`build/settings-keyboard-source-freeze.json`).

Die verschobene SDK-App besteht unter
`build/settings-keyboard-sdk-proof-mac` vier gezielte Abläufe:
Einstellungen per Tastatur und Maus sowie kleine/große Tastaturmenüs.
Der SHA-256 der gestarteten App stimmt mit dem aktuellen Release-Binary überein;
alle übrigen SDK-Produkte bleiben bytegleich zum vorherigen SDK.
Dies ist eine gezielte App-Abnahme, keine neue vollständige numerische SDK-Suite.
`PASSED.json` enthält App-Prüfsumme, konkrete Befehle und Rückgabecodes.

Die endgültigen drei Prüfungen ohne Fenster bestehen unter Linux unter
`build/contact-world-language-release-linux/test-results/run-xmnydtc9`.
Die Linux-SDK-App besteht dieselben vier gezielten Abläufe unter
`build/settings-keyboard-sdk-proof-linux`, ebenfalls mit protokollierter
App-Prüfsumme und unveränderten übrigen SDK-Produkten.

Die [C17-CI zu `48db576`](https://github.com/PhysicSimulator/physim/actions/runs/37588645984)
und die [frischen Linux-Pakete desselben Stands](https://github.com/PhysicSimulator/physim/actions/runs/37588645973)
sind abgeschlossen und erfolgreich. Die CI zur UI-Schriftgröße `d20ca00` läuft
noch; die neue Einstellungsführung benötigt eine eigene Remote-Abnahme.
Der Katalog enthält jetzt 70 Fensterfälle. Vollständige Tastaturführung und
Screenreader-Zugang bleiben offene Ziele von PP-0710.

## Unabhängige UI-Schriftgröße am 7. Oktober 2026

Die persönliche UI-Schriftgröße ist unabhängig von Codeschrift in 16, 18, 20
und 22 Pixeln einstellbar. Vorausgeladene Systemschriften gelten für Navigation,
Menüs, Beschriftungen, Diagramme und Dokumentation; ein bereits geöffnetes
Handbuch wechselt mit und verwirft seine gecachte Textgeometrie. Codeschrift
bleibt unabhängig. Kurze Zeilen erhalten ausreichende Höhe, mehrzeilige Texte
und lange Schaltflächen wachsen; größere Einstellungsoptionen und Plotcontrols
verwenden angepasste Spalten. Umfangreiche Ansichten bleiben scrollbar.
[Bedienung und Dateiformat](settings.md).

Das persönliche Format PSPREF05 ergänzt eine CRC-geschützte UI-Größe. Formate
1–4 bleiben lesbar und laden 16 Pixel zusätzlich zu ihren erhaltenen Werten.
Ungültige Größen, CRC-Fehler, abgeschnittene Dateien und Schreib-/Renamefehler
bewahren den vorherigen Stand. Die erweiterte C-Prüfung besteht unter macOS
unter `build/contact-world-language-release-mac/test-results/run-46tw4btu`
und Linux unter
`build/contact-world-language-release-linux/test-results/run-3t_fh838`.
Die App-Prüfung wählt alle vier Größen, startet erneut, prüft Abbrechen und
Standardwerte, unabhängige Code-/UI-Schriften, bereits geöffnete Dokumentation,
Menüführung, Menü/Editor/Einstellungen/Diagramm-Aufnahmen sowie die gespeicherten
312 Bytes, Größenfeld und CRC mit einem unabhängigen Python-Leser.

Die ersten 22-Pixel-Aufnahmen zeigten abgeschnittene Beschriftungen und fehlende
zweite Textzeilen. Deshalb werden längere Schaltflächen/Textzeilen passend
umgebrochen; schmale Einstellungs- und Plotcontrols erhalten zusätzliche Breite.
Die vorherige Einstellungsprüfung wurde nicht gestrichen: Sie scrollt nun die
unteren Controls tatsächlich an und prüft weiter dieselben gespeicherten Werte.

Die vollständigen 69 Fensterprüfungen des übernommenen Schriftgrößenstands
bestehen auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-zzkrvc73` und Debian
unter `build/contact-world-language-release-linux/test-results/run-87y8dn1z`.
Diese Läufe wurden vor den abschließenden Anpassungen an Sicherungs-/Plothinweisen
und der Breitenkorrektur für umgebrochene Schaltflächen ausgeführt.
Ein zusätzlicher Regressionstest prüft nun die tatsächlichen Zeichenbefehle:
alle UTF-8-Bytes und die volle Breite einschließlich des letzten Zeichens bleiben
erhalten. Die Nuklear-Hilfsfunktion meldet sonst die Breite vor diesem Zeichen.

Die abschließende Sichtprüfung zeigte zusätzlich verschwindende Schließen-
Symbole in schmalen Paneltiteln bei 22 Pixeln. Ein separater Geometrietest
reproduzierte unzureichende Zeichenbreiten und -höhen; einzelne Symbole verwenden
nun die verfügbare Schaltflächenfläche ohne den Innenabstand langer Beschriftungen.
Das umgebende Buttondesign und seine Eingabebehandlung bleiben erhalten.

Die abschließenden fünf Prüfungen ohne Fenster (`ui_geometry`,
`editor_clipboard`, `font_shape`, `preferences`, `verification_kit`) bestehen
mit der Symbolkorrektur auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-cvaex07p` und Debian
unter `build/contact-world-language-release-linux/test-results/run-sezob4vk`.
Die vorherige fehlschlagende Symbolprobe ist unter
`build/contact-world-language-release-mac/test-results/run-gcbzpw5a` erhalten.

Die vier betroffenen Fensterabläufe für Diagramme, Einstellungen, Themes und
Schriftgrößen bestehen vor der zusätzlichen Symbolkorrektur auf macOS unter
`build/contact-world-language-release-mac/test-results/run-u_2yo8bf` und Linux
unter `build/contact-world-language-release-linux/test-results/run-_62gj2i7`.
Der zusätzliche macOS-Schriftgrößenlauf mit vollständiger Zeichenbreitenprüfung
steht unter `build/contact-world-language-release-mac/test-results/run-tp3d9bvg`.

Die vollständige isolierte SDK-Prüfung vor der letzten Symbolkorrektur besteht
auf macOS unter `build/ui-size-sdk-proof-mac/Native SDK ä mm75zwvw` mit dem
58-Dateien-Prüfkit. Sie umfasst numerische/Sprach-/Projektprüfungen, alle neun
grafischen Beispielabläufe, beide Dokumentationswege, Tastaturmenüs in zwei
Fenstergrößen und alle vier UI-Schriftgrößen mit Neustart.
Im endgültigen SDK `build/UI typography final SDK ä mac` sind 428 von 429
Manifestdateien bytegleich; ausschließlich `bin/physim` enthält die Symbolkorrektur.
Der Abgleich steht unter `build/ui-size-final-sdk-mac-difference.json`.
Die endgültige App besteht anschließend sieben erneut ausgeführte Abläufe in
einer weiteren verschobenen SDK-Kopie unter `build/ui-size-final-app-proof-mac`:
Schriftgrößen, Einstellungen, Themes, Diagramme, kleine/große Tastaturmenüs und
Docking. `PASSED.json` hält den SHA-256 der tatsächlich gestarteten App sowie
sämtliche sieben Befehle und Rückgabecodes fest. Die 22-Pixel-Aufnahme
`build/ui-size-verified-settings-mac.png` bestätigt sichtbar erhaltene Schließen-
Symbole. Die ursprünglichen Größenaufnahmen werden vor Neustart-/Standardwert-
Prüfungen separat aufgehoben, damit spätere Schritte diese Belege nicht ersetzen.

470 Code-, Test- und Werkzeugdateien stimmen zwischen den geprüften lokalen
macOS-/Linux-Quellständen per SHA-256 überein
(`build/ui-size-cross-platform-source-hashes.json`).

Das endgültige Linux-SDK einschließlich Symbolkorrektur besteht die vollständige
isolierte Prüfung unter `build/ui-size-sdk-proof-linux/Native SDK ä 91etu6ft`.
Alle numerischen/Sprach-/Projektgates, neun grafischen Abläufe, beide Lernwege,
Tastaturmenüs und vier Schriftgrößen mit Neustart bestehen. Die Plattformen sind
macOS 14.6.1 auf Intel mit Apple Clang 16 und Debian 12 mit GCC 12.2, Kernel
6.1.0-53-cloud-amd64 und Mesa/Xvfb/Openbox. Die genauen Compiler-/Systemangaben
stehen unter `build/ui-size-tested-environments.json`. Dies ist keine neue
Windows-/Apple-Silicon-CI-Abnahme und keine vollständige Abnahme des Projektplans.

Die Linux-VM wurde für die Fortsetzung wieder gestartet und ihr Testdatenträger
von 32 auf 48 GiB vergrößert. Vorhandene Prüfarbeitsstände bleiben erhalten.
Vollständige Tastaturführung, Screenreader-Zugang und die übrigen Ziele von
PP-0710 bleiben offen. Sehr schmale Panels können weiterhin einzeilige Inhalte
abschneiden; die Schriftoption ersetzt keine vollständige responsive Abnahme.

## Reihenfolge der Menüaktionen und Editorfokus am 7. Oktober 2026

Ein isolierter App-Build mit dem vorherigen Handler aus `03ba81d` reproduziert
einen Fehler bei Enter und einer folgenden Pfeiltaste im selben SDL-Paket:
Die Auswahl verschiebt sich vor der vorgemerkten Ausführung, und die verlangte
Protokollaktion wird nicht ausgeführt. Die Ausgabe meldet Stage 13 mit Menü 2,
Eintrag 4 und weiter aktivierter Tastaturführung. Der Nachweis steht unter
`build/menu-previous-handler-build-v2.log` und
`build/menu-previous-handler-output.log`. Die erste Hilfsbuild-Konfiguration
verwendete versehentlich den aktuellen indirekt eingebundenen Handler; erst
die isolierte Kopie von `design_ui.inc` und `toolbar_ui.inc` prüft tatsächlich
den vorherigen Stand. Der frühere erfolglose Reproduktionsversuch ist kein
Nachweis gegen den Fehler.

Enter und Leertaste prüfen nun unmittelbar die aktuelle gemeinsame Aktionsliste
und führen die gewählte freigegebene Aktion aus. Der vorgemerkte Bool-Zustand
entfällt. Nachfolgende Eingaben können diese Aktion nicht mehr umdeuten.
Der neue Eingabepakettest verlangt die richtige Protokollaktion; die Editor-
prüfung kehrt außerdem nach Escape ohne einen Mausklick zur Texteingabe zurück.

Alle fünf Maus-/Tastaturmenüprüfungen bestehen im Release-Build auf Intel
macOS/Apple Clang 16 unter
`build/contact-world-language-release-mac/test-results/run-4mrvw_fu` und
Debian/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-80iadmah`.
Beide Fenstergrößen prüfen weiterhin deaktivierte Aktionen, Fokusmarkierung,
Textisolation, Schreiben/Speichern, Mausübergang und Fokusverlust.
Die vollständigen SDK-Prüfungen des vorherigen Menü-Meilensteins bleiben eigene
unveränderte Nachweise. Diese anschließende Korrektur besitzt die genannten
gezielten App-Prüfungen; eine erneute Remote-Abnahme bleibt erforderlich.
PP-0710 und die vollständige Produktabnahme bleiben offen.

## Hauptmenüs per Tastatur am 7. Oktober 2026

Die App bietet F10 für die Hauptmenüleiste, Pfeile und Tab/Shift+Tab für den
Menüwechsel, Home/End und Pfeile für verfügbare Einträge sowie Enter/Leertaste
zum Ausführen. Escape/F10, ein Klick außerhalb und Fensterfokusverlust beenden
die Menüführung. Ein sichtbarer Rahmen markiert Menü und Eintrag. Maus und
Tastatur verwenden dieselbe Aktionsliste und dieselben Freigabebedingungen;
deaktivierte Aktionen werden übersprungen. Menütasten und Texteingaben gelangen
während der Menüführung nicht in einen zuvor aktiven Editor.
[Bedienung](workspace.md).

Die elf betroffenen Release-Fensterprüfungen bestehen auf Intel macOS 14.6.1/
Apple Clang 16 unter
`build/contact-world-language-release-mac/test-results/run-eas2s_sy`.
Sie umfassen bisherige Mausmenüs, Inputisolation, die beiden neuen Tastaturabläufe,
Editorfenster, Einstellungen, Themes, Dokumentation und Docking.
Die Tastaturabläufe bei 1080×740 und 1440×940 prüfen Menüwechsel, übersprungene
inaktive Aktionen, zyklische Auswahl, Home/End, Protokoll, Hilfe, Einstellungen,
Projektmanager, unveränderten UTF-8-Editorinhalt, anschließendes Schreiben und
Speichern sowie Rückkehr zur Maus und Fokusverlust. Die tatsächliche kleine
Fokusaufnahme ist unter `build/keyboard-menu-focus-small.png` visuell geprüft.

Der Katalog enthält weiterhin 608 Fälle ohne Fenster und 589 ohne SDL, jetzt
68 Fensterfälle. Die vollständigen 608/608-Gesamtläufe des Builder-Meilensteins
bleiben eigene frühere Nachweise; diese UI-Änderung besitzt die genannten
betroffenen Fensterprüfungen und neue SDK-Prüfungen. PP-0710 bleibt unvollständig:
weitere Bedienelemente benötigen Tastaturführung, UI-weite Schriftvergrößerung
und grundlegender Screenreader-Zugang bleiben offen.

Der vorherige vollständig erfolgreiche Linux-SDK-Prüfordner ist vor dem
Freigeben von VM-Speicher auf dem Mac gesichert. Alle 14.417 regulären Dateien
stimmen vor und nach dem Archivieren per SHA-256 überein.
Archiv `build/native-domain-final-v2-sdk-proof-linux-evidence.tar.gz`, SHA-256
`ca8b22820d16c4042c7f36b8104c6ac0c1ccb306bb2959c67e1d398724f95654`.

Unter Debian 12/GCC 12.2 bestehen dieselben elf Release-Fensterprüfungen unter
`build/contact-world-language-release-linux/test-results/run-3kup514s`.

Die neue isolierte 58-Dateien-Kitprüfung besteht auf macOS vollständig unter
`build/keyboard-menu-final-sdk-proof-mac/Native SDK ä 7j2jyves`.
Sie prüft zusätzlich zu allen bisherigen numerischen/Sprach-/Projektgates die
neun grafischen Projektabläufe, Dokumentationsnavigation und die installierte
App mit Tastaturmenüs in beiden Fenstergrößen. Die Erfolgsmarker beider
Menüläufe werden ausdrücklich gelesen; erst danach entsteht PASSED.txt.

Unter Linux besteht dieselbe vollständige isolierte Prüfung unter
`build/keyboard-menu-final-sdk-proof-linux/Native SDK ä yckrcleo`, ebenfalls
mit allen neun grafischen Abläufen, Dokumentationsnavigation und beiden
Tastaturmenüläufen der installierten App.

Die endgültigen Pakete `build/Keyboard menu clean SDK ä mac` und
`build/Keyboard menu clean SDK ä linux` enthalten 429 Manifestdateien,
308 geprüfte Code-/Beispieldateien und 75 kompilierte Sprachprodukte.
Alle Code-/Beispieldateien stimmen byteweise mit den vollständig geprüften
SDK-Kopien überein; die Apps mit den geprüften Release-Binaries. Zum Abschluss
werden nur Dokumentation und Manifest aktualisiert. Eine neue Remote-Abnahme
bleibt gesondert erforderlich.

## Live-Geschwindigkeitswechsel und gepufferte Snapshots am 7. Oktober 2026

Das SHA-256-geprüfte Apple-Silicon-Artefakt von `d3120ad` zeigt, dass Stage 20
den früheren 4×-Fehler passiert. Stage 25 scheitert nach dem Live-Wechsel 4×→1×
an der kurzen oberen GUI-Taktschranke. Bereits gepufferte Snapshots können noch
unter 4× entstanden sein; ihr Empfang ist keine frische Wandtaktratenmessung.
Die GUI-Prüfung verlangt daher korrekte Auswahl, Fortschritt, unverändertes dt,
Pause, Einzelschritt, Reset und Persistenz. Die verzögerten C-/Physim-Durchläufe
stauen zusätzlich alte 4×-Snapshots vor dem Live-Wechsel auf 1× auf.

Alle acht App-Aufrufe bestehen unter Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-c5zrravs` und
Debian/GCC unter
`build/contact-world-language-release-linux/test-results/run-_rx_hr2t`.
Die direkten Zeitkonto-/Runner-Prüfungen bestehen mit je 3/3 unter
`run-rdyt5q75` auf macOS und `run-n0li8d7x` unter Linux in denselben Release-
Testordnern. Ein neuer direkter Runner-Test führt den Live-Wechsel ohne
Rendering aus, verwirft die Übergangsphase, verlangt danach die gemessene
1×-Rate und vergleicht weiterhin alle 201 Referenzmessungen jedes Kanals.
Auf dem Mac messen C und Physim jeweils 1,020 Simulationssekunden in 1,019–1,020
Wandsekunden. Eine ignorierte 1×-Anforderung würde die neue Rateprüfung verletzen.

Scheduling, Physikschritte, Daten-/Wire-/Modulformate wurden nicht verändert.
Die vollständigen 608/608-Gesamtläufe und SDK-Prüfungen des Builder-Meilensteins
`11b709f` bleiben eigene vorherige Nachweise. Diese anschließende Korrektur
besitzt die genannten gezielten GUI-/Runner-Prüfungen. Eine erneute tatsächliche
Apple-Silicon-CI-Abnahme bleibt erforderlich.

## Gemeinsamer Core im nativen Projektbuilder am 7. Oktober 2026

Der native Projektbuilder enthielt noch 21 Core-Module; der Repository-Build
und die installierten SDK-Quellen enthielten bereits 26. Ein frisches C-Projekt
mit den veröffentlichten Thermodynamikquellen reproduziert unter Intel macOS
den Linkfehler bei `ps_heat_flow`, `ps_ideal_gas_energy`, `ps_ideal_gas_pressure`
und `ps_thermal_pair_step`. Der Nachweis steht in
`build/native-domains-baseline.log`; vorhandene SDK-Prüfungen hatten diese
Domänenmodule direkt gegen den vollständigen Core gebaut und konnten diese
Lücke im tatsächlichen Projektbuilder daher nicht erkennen.

`app/build_main.c` baut nun zusätzlich Thermodynamik, Elektromagnetismus,
Wellen, Optik und Strömung. Die Größe des Objekt-/Integritätskatalogs wird direkt
aus seiner Modulliste abgeleitet. `core_catalog` vergleicht beide geordneten
Kataloge und alle Quelldateien. Ältere unvollständige Build-Caches werden
weiterhin automatisch neu gebaut; Fehler erhalten veröffentlichte Module.

Sechs gezielte Release-Prüfungen bestehen auf Intel macOS/Apple Clang 16 unter
`build/contact-world-language-release-mac/test-results/run-h034svj9` und
Debian/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-o4eix2_s`.
Die bestehende native Projektbuild-Prüfung bleibt erhalten, einschließlich
Cachekorruption, Compiler-/Linkerfehlern, Quellenintegrität und C-/Physim-
Analyseprojekten. Vier neue Domänenprüfungen bauen acht frische dokumentierte
C-/Physim-Projekte mit dem tatsächlichen `physim-build`, prüfen unveränderte
Cachewiederverwendung und führen anschließend die vollständigen unabhängigen
Decimal-, Gitter-/Fourier- und hydraulischen Lernorakel mit allen gemischten
Analysen aus. Die isolierte SDK-Prüfung enthält dieselben acht nativen Builds.

Für VM-Speicher sind die früheren generierten Linux-Nachweise vollständig auf
dem Mac gesichert. Alle Dateien wurden vor und nach dem Archivieren einzeln
per SHA-256 verglichen; anschließend wurden ausschließlich die verifizierten
generierten Prüfordner entfernt:

- `build/fluid-final-sdk-proof-linux-evidence.tar.gz`: 13.278 Dateien,
  SHA-256 `45cc21fc62d5d1f7f2172002a8fcb47151043085f03fdbbb90e5d59e389b1d91`.
- `build/native-release-linux-results-evidence.tar.gz`: 64.809 Dateien,
  SHA-256 `d0b3e370a7a7ca6da2f49d39491800a8a8c0215db126fb10e5ef492d38ad46b9`.

Die vollständige aktuelle Release-Suite besteht auf Intel macOS mit **608/608**
unter `build/contact-world-language-release-mac/test-results/run-namds9jz`.
Der Katalog enthält außerdem 589 Fälle ohne SDL und 66 Fensterfälle; die
Katalogzahl ersetzt keinen tatsächlichen Gesamtlauf der jeweiligen Auswahl.

Die [Remote-CI des vorherigen Commits `d3120ad`](https://github.com/PhysicSimulator/physim/actions/runs/37579137856)
besteht tatsächlich in allen vier Windows-v143-/ClangCL-Debug-/Release-Jobs.
Unter Apple Silicon bestehen 603/603 Tests ohne Fenster und 65/66 Fensterfälle.
Die fehlgeschlagene `speed_workflow`-Prüfung liegt nun in Stage 25 nach dem
Live-Wechsel 4×→1×: Die kurze GUI-Messung überschreitet ihre obere Taktschranke.
Der frühere Fehler in Stage 20 wird passiert. Dies bleibt ein eigener
Korrekturpunkt; die lokale Prüfung beweist keine neue Apple-Silicon-Abnahme.
Das Artefakt `build/speed-apple-silicon-ci.zip` ist gegen den API-SHA-256
`64443cfb551131a019bf2855aaec7878c83c0afd4b8f2df7aa9167cee923955c` geprüft.
Die absichtlich fehlschlagenden Test-Runner-Selbstprüfungen im Artefakt sind
Testdaten und keine zusätzlichen Produktfehler.

Die korrigierte isolierte 58-Dateien-Kitprüfung besteht unter macOS vollständig
unter `build/native-domain-final-v2-sdk-proof-mac/Native SDK ä igb3kqh1`,
einschließlich aller neun grafischen Projektabläufe und Dokumentationsnavigation.
Die beiden ersten SDK-Läufe scheiterten an einer Variablenüberschattung in der
neuen Prüfer-Erweiterung: Der Domänenloop ersetzte versehentlich den allgemeinen
Berichtsprüfer. `domain_probe` hält beide Rollen getrennt; die erneute Prüfung
läuft vom Anfang. Fehlgeschlagene Prüfläufe werden nicht als Abnahme gezählt.

Die vollständige Linux-Release-Suite besteht ebenfalls mit **608/608** unter
`build/contact-world-language-release-linux/test-results/run-n_5zynq9`.
Beide Ergebnislisten stimmen mit sämtlichen aktuellen Katalognamen überein.
Die korrigierte isolierte 58-Dateien-Kitprüfung besteht unter Linux vollständig
unter `build/native-domain-final-v2-sdk-proof-linux/Native SDK ä 0l0vty1y`,
ebenfalls mit allen neun grafischen Abläufen und Dokumentationsnavigation.

Die endgültigen Pakete `build/Native domain clean SDK ä mac` und
`build/Native domain clean SDK ä linux` enthalten weiterhin 429 Manifestdateien,
308 geprüfte Code-/Beispieldateien und 75 kompilierte Sprachprodukte.
Alle Code-/Beispieldateien sind bytegleich mit den vollständig geprüften
SDK-Kopien; die Apps mit den geprüften Release-Binaries. Zum Abschluss werden
nur Dokumentation und Manifest aktualisiert. Spätere Änderungen behalten
eigene Nachweise; diese Gesamtläufe werden nicht nachträglich umgedeutet.

## GUI-Geschwindigkeit bei Rückstau am 7. Oktober 2026

Das SHA-256-geprüfte Apple-Silicon-Artefakt des vorherigen Wellen-Commits
zeigt einen einzelnen Fehler in `speed_workflow`: Die GUI verlangt über eine
halbe Sekunde mindestens 2,8 Simulationssekunden pro Wandsekunde bei 4×.
Der Runner begrenzt bei Render-/Pipe-Rückstau jedoch ausdrücklich sein Zeitkonto
auf 0,25 Wandsekunden. Die Fenstermessung kann deshalb den Fortschritt korrekt
anzeigen und diese Mindesttaktrate trotzdem verfehlen.

Die Fensterprüfung prüft nun ausgewählte Geschwindigkeit, unverändertes dt,
Fortschritt, obere Taktschranke, Pause, Einzelschritt, Reset und Persistenz.
Zusätzliche C-/Physim-Durchläufe unterbrechen den GUI-Leser bei jeder laufenden
Geschwindigkeit absichtlich für eine Sekunde. Alle acht App-Aufrufe bestehen
auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-a68a9u8u` und unter
Debian/GCC unter
`build/contact-world-language-release-linux/test-results/run-n538qarn`.

Die unveränderten direkten Zeitkonto-/Runner-Prüfungen bestehen mit jeweils
3/3 unter `run-rv7jar5p` auf macOS und `run-jyawo_9t` unter Linux in denselben
Release-Testordnern. Sie messen weiterhin die tatsächlichen Geschwindigkeits-
verhältnisse und vergleichen 201 Messungen aller Kanäle mit Referenzdaten,
einschließlich verzögertem Leser und Live-Wechsel. Scheduling, Wire-/Modul-ABI
und Physikdaten wurden nicht verändert. Eine erneute Apple-Silicon-CI-Abnahme
bleibt erforderlich. Die zuvor geprüften Strömungs-SDKs behalten ihre eigenen
unveränderten Binaries und Manifestnachweise.

## Strömungs-Lehrmodelle und Physim 0.182.0 am 7. Oktober 2026

Das neue SI-Modul liefert laminare Rohrleitwerte, signierte Flüsse, Verlustleistung,
Reynolds/Hydrostatik, passive stationäre Netze bis 16 Knoten/32 Kanten und
periodischen Tracertransport bis 4096 Zellen. Sieben Sprachbindungen verwenden
dieselben Corefunktionen; Ergebnisarrays besitzen unabhängigen Speicher.
Netze und Tracer bleiben ausdrücklich getrennte Lehrmodelle. CFD/FEM, Pumpen,
Drucktransienten und offene Tracerränder sind nicht abgenommen.
[Vertrag, vollständige Quellen und Modellgrenzen](fluid.md).

Die endgültigen zehn ausgewählten Release-Prüfungen bestehen auf Intel macOS
14.6.1/Apple Clang 16 unter
`build/contact-world-language-release-mac/test-results/run-vhfqigsq` und Debian
12/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-zvy9dg0x`.
Fünf Linux-Clang-14-ASan/UBSan-Prüfungen bestehen unter
`build/fluid-asan-linux/test-results/run-rglx8fma`.
Die Coreprüfungen lösen auch das maximale 16-Knoten-/32-Kanten-Netz und führen
4096-Zellen-Schritte tatsächlich aus. Sie prüfen Erhaltung, Aliasing, atomare
Fehlerausgaben, Zahlenextreme und sämtliche Ergebnis-Allokationsfehler.
Komponentenweise Druckskalierung erhält getrennte Druckgrößen; verschwindende
positive Leitwerte oder Fixdrücke brechen mit PS_NUMERIC ab.

Der unabhängige Lernprüfer kontrolliert 2.412 Messungen in sechs Profilen,
24 gemischte C-/Physim-Analysen, hydraulische Knotenbilanzen, diskrete
Fourierverstärkung, Kontinuumsreferenz, Masse, Positivität, Flussvorzeichen,
SI-Metadaten, CSV, CRC/Footer und alle 65 aufgezeichneten Tracerpunkte.
Instabile Transportschritte und die im Tutorial gesetzte Re-Grenze werden
abgewiesen. Das grafische Beispiel verwendet 64 Zellen; der Core unterstützt
4096. Ein stationäres Rohrnetz liefert eine vorgegebene Geschwindigkeit für
den separaten periodischen Tracer, keine gekoppelte offene Netzströmung.

Neun bestehende Sprach-/Experiment-/Analyse-/Sensor-/Paritätsprüfungen bestehen
auf macOS unter `run-0bt2ijur`, das Hilfefenster unter `run-mmdawy4i`.
Lexer, Parser und Checker bestehen unter `run-xtkamgx2` in demselben
Release-Testordner. Unter Linux bestehen dieselben neun Prüfungen zusammen
mit Lexer/Parser/Checker (12/12) unter `run-a68x3we_`; das Hilfefenster
besteht unter `run-pxrp5p77`. Alle 29 generierten Referenzen sind geprüft.
Der aktuelle Katalog umfasst 603 Fälle ohne Fenster, 588 ohne SDL und 66
Fensterfälle. Diese Zahl ersetzt keinen aktuellen vollständigen Gesamtlauf.

Beide endgültigen SDKs bestehen die vollständige isolierte Prüfung aus dem
57-Dateien-Kit, einschließlich aller neun grafischen Projektabläufe und der
Dokumentationsnavigation:

- macOS: `build/fluid-final-sdk-proof-mac/Native SDK ä 8kpwgdwe`.
- Linux: `build/fluid-final-sdk-proof-linux/Native SDK ä pdm7hwol`.

Sie bauen alle öffentlichen Header, 23 Standalone-Programme und 52 Sprachmodule
nach. Die Strömungsprüfungen laufen jeweils gegen installierten und neu gebauten
Core mit 4.824 Tracermessungen und 48 gemischten Analysen je SDK.
Die endgültigen Pakete `build/Fluid clean SDK ä mac` und
`build/Fluid clean SDK ä linux` enthalten 429 Manifestdateien und 75 kompilierte
Sprachprodukte. Alle 308 Code-/Beispieldateien stimmen byteweise mit den
geprüften SDK-Kopien überein; die Apps mit den geprüften Release-Binaries.
Zum Abschluss werden nur Dokumentation und Manifest aktualisiert.

Die [Remote-CI des vorherigen Wellen-Commits `da3a3d2`](https://github.com/PhysicSimulator/physim/actions/runs/37575688333)
zeigt tatsächliche erfolgreiche Windows-v143- und ClangCL-Debug-/Release-Jobs.
Unter Apple Silicon bestehen 596/596 Prüfungen ohne Fenster und 65/66
Fensterprüfungen. `speed_workflow` scheitert bei der gemessenen 4x-Geschwindigkeit;
Build, SDK-Prüfung und LaunchServices-Start sind dort erfolgreich. Dieser
Geschwindigkeitsfehler bleibt ein eigener Korrekturpunkt. Das vollständige
Artefakt `build/waves-apple-silicon-ci.zip` stimmt mit dem API-SHA-256
`4dc047f0b084c4c24a61b39a3b2fee8b38563d8934009b2617e08893c5b95ffa` überein.
Diese früheren Nachweise ersetzen keine Remote-Abnahme der neuen Strömungsfunktionen.

Vor der Freigabe von Linux-VM-Speicher ist der frühere generierte Prüfordner
`build/waves-sdk-proof-linux` vollständig auf dem Mac gesichert. Alle 13.032
regulären Dateien stimmen per SHA-256 mit ihren ursprünglichen Werten überein.
Lokal erhaltenes Archiv `build/waves-sdk-proof-linux-evidence.tar.gz`, SHA-256
`01f8e9df26125f0040c850e2133cccbca096b5185a4eee181657cc379a499510`.

## Wellen/Optik und Physim 0.181.0 am 7. Oktober 2026

Die neuen SI-Module liefern exakten undämpften Oszillator, harmonische Laufwelle,
ideale Saitengeschwindigkeit, tatsächliche 1D-Gitterausbreitung, Reflexion,
Snell/Totalreflexion und paraxiale dünne Linsen. Der Saitenschritt prüft feste
Nullränder, endliche Arrays und c dt/dx≤1. Ein begrenzter Stackpuffer erhält
Ausgaben bei jedem Fehler und unterstützt Input-/Output-Aliasing.
Physim bindet neue besitzende Arrays; `simulationTimeStep()` liest das positive
Experimentintervall zur konsistenten Initialisierung. [Vertrag und Quellen](waves-optics.md).

Die 13 ausgewählten Release-Prüfungen bestehen auf Intel macOS/Apple Clang 16
unter `build/contact-world-language-release-mac/test-results/run-376butxe`
und Debian 12/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-zt7k3szw`.
Fünf Linux-Clang-14-ASan/UBSan-Fälle bestehen unter
`build/waves-asan-linux/test-results/run-tdeorpv8`.
Die Coreprüfungen kontrollieren Oszillatorenergie, Laufphase, diskrete Eigenmode,
CFL/Aliasing, Zahlenextreme, Snell/Totalreflexion und Linsenvorzeichen.
Sowohl C als auch Physim führen den maximalen 4096-Knoten-Schritt tatsächlich aus.
Ein unabhängiger Allocator prüft jeden Ergebnis-Allokationsfehler, unabhängige
Besitzer, verworfene CFL-Ergebnisse und Grenzprüfungen vor Allokation.

Der vollständige Saitenprüfer kontrolliert 2.412 Gittermessungen in sechs Profilen,
24 gemischte C-/Physim-Analysen, CRC/Footer, SI-Metadaten, sämtliche aufgezeichneten
Knoten, diskrete Energie und zweite Ordnung bei gemeinsamer Raum-/Zeitverfeinerung.
Das grafische Beispiel begrenzt sich ausdrücklich auf 65 Knoten, damit jeder
Knoten in den bestehenden 96-Punkte-Szenenpool passt. Der Core unterstützt
weiterhin 4096 Rechenknoten; größere Darstellungen sind damit nicht behauptet.

Neun bestehende Sprach-/Experiment-/Analyse-/Sensor-/Paritätsprüfungen bestehen
auf macOS unter `run-408xrijj` und Linux unter `run-sjqtbz6d` in den genannten
Release-Testordnern. Das Hilfefenster besteht unter `run-ki17aa0h` auf macOS und
`run-_uqacd5p` auf Linux; alle 28 generierten Referenzen sind geprüft.
Der Katalog umfasst nun 596 Fälle ohne Fenster, 581 ohne SDL und 66 Fensterfälle.
Die Katalogzahl ersetzt keinen vollständigen aktuellen Gesamtlauf.

Beide endgültigen SDKs bestehen die vollständigen isolierten 53-Dateien-Kitprüfungen,
einschließlich aller neun grafischen Projektabläufe und der Dokumentationsnavigation:

- macOS: `build/waves-sdk-proof-mac/Native SDK ä d8e6xesj`.
- Linux: `build/waves-sdk-proof-linux/Native SDK ä eob21v02`.

Sie bauen alle öffentlichen Header, 22 Standalone-Programme und 50 Sprachmodule
nach und prüfen Wellen/Optik gegen installierten und neu aufgebauten Core:
4.824 Gittermessungen und 48 gemischte Analysen je SDK, zusätzlich 4096-Knoten-
und Allokationsfehlerprüfungen. Die endgültigen Pakete
`build/Waves optics clean SDK ä mac` und `build/Waves optics clean SDK ä linux`
enthalten 416 Manifestdateien und 72 kompilierte Sprachprodukte. Alle 297
Code-/Beispieldateien stimmen byteweise mit den geprüften SDK-Kopien überein;
die Apps stimmen mit den geprüften Release-Binaries überein. Zum Abschluss
werden nur Dokumentation und Manifest aktualisiert. Die neue Remote-Matrix
bleibt ein eigenständiger Nachweis.

Für den [vorherigen Elektromagnetismus-Commit `8755399`](https://github.com/PhysicSimulator/physim/actions/runs/37573088919)
sind tatsächliche Windows-v143-Debug- und ClangCL-Debug-/Release-Jobs erfolgreich.
Dies ist keine Abnahme der neuen Wellen-/Optikfunktionen; die aktuelle
Remote-Matrix bleibt gesondert erforderlich.

Vor der Freigabe von Linux-VM-Speicher ist der frühere generierte Prüfordner
`build/em-sdk-proof-linux` vollständig auf dem Mac gesichert. Alle 12.765
regulären Dateien stimmen per SHA-256 mit ihren ursprünglichen Werten überein.
Lokal erhaltenes Archiv `build/em-sdk-proof-linux-evidence.tar.gz`, SHA-256
`bb0792203d3dcd07e5fd5db9e51df80cd23003166a11635ff50251ee45fdabf4`.

## Elektromagnetismus und Physim 0.180.0 am 7. Oktober 2026

Das neue SI-Modul liefert homogene Punktladungsfelder/Potentiale, Lorentzkraft,
ideale Widerstands- und Kondensatorgrößen sowie exakte konstante RC-Schritte.
Elf reine Sprachbindungen verwenden dieselben C-Funktionen. `expm1` ergänzt
stabile Exponentialdifferenzen in Physim. Normalisierte Produkte erhalten
repräsentierbare Ergebnisse bei großen Abständen, Feldern oder kleinen RC-
Relaxationsfaktoren. [Vertrag, Grenzen und vollständige Quellen](electromagnetism.md).

Die zwölf ausgewählten Release-Prüfungen bestehen auf Intel macOS/Apple Clang 16
unter `build/contact-world-language-release-mac/test-results/run-b8ekfw5j`
und Debian 12/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-de4x_2fu`.
Vier Linux-Clang-14-ASan/UBSan-Prüfungen bestehen unter
`build/em-asan-linux/test-results/run-fu_nqcv_`.
Die Coreprüfung vergleicht das Feld mit dem Potentialgradienten und kontrolliert
Lorentzvorzeichen, magnetische Arbeit, Schaltungsgesetze, Zeitkomposition,
Singularitäten, extreme Zahlen und unveränderte Ausgaben bei Fehlern.

Der RC-Prüfer kontrolliert 2.814 Messungen in sieben Szenarien mit 65-stelligen
Decimal-Lösungen. Widerstandswärme wird unabhängig als Integral von I²R geprüft,
Quellenarbeit mit ihrem Vorzeichen und Kondensatorenergie getrennt kontrolliert.
Langsame Relaxation verwendet stabile Exponentialdifferenzen; das Subtrahieren
großer Energien darf kleine Verluste nicht zerstören. Die 28 gemischten C-/
Physim-Analysen werden gegen Rohdaten, SI-Metadaten, sämtliche Szenengeometrien,
CSV und erneut geladene Berichte geprüft. Ein exakter RC-Schritt hat keine
Stabilitätsgrenze; dies ist kein Modell beliebiger Netzwerke oder Maxwell-Felder.

Neun bestehende Sprach-/Experiment-/Analyse-/Sensor-/Paritätsprüfungen bestehen
auf macOS unter `run-5zzq3n_v` und Linux unter `run-r5c6rjpu` in den genannten
Release-Testordnern. Das Hilfefenster besteht unter `run-julak5io` auf macOS und
`run-kkzbzxfh` auf Linux. Alle 26 generierten Referenzen sind geprüft.
Der Katalog umfasst nun 589 Fälle ohne Fenster, 574 ohne SDL und 66 Fensterfälle;
diese Katalogzahl ist kein vollständiger neuer Gesamtlauf.

Beide endgültigen SDKs bestehen die vollständige isolierte 49-Dateien-Kitprüfung,
einschließlich aller neun grafischen Projektabläufe und der Dokumentationsnavigation:

- macOS: `build/em-sdk-proof-mac/Native SDK ä 0yk8gx2j`.
- Linux: `build/em-sdk-proof-linux/Native SDK ä 64mfcmrk`.

Alle öffentlichen Header, 21 Standalone-Programme und 48 Sprachmodule werden
nachgebaut. Elektromagnetismus wird gegen installierten und neu aufgebauten
Core geprüft: 5.628 RC-Messungen und 56 gemischte Analysen je SDK, zusätzlich
Core-Extremwerte. Die endgültigen Pakete `build/Electromagnetism clean SDK ä mac`
und `build/Electromagnetism clean SDK ä linux` enthalten 400 Manifestdateien und
69 kompilierte Sprachprodukte. Alle 284 Code-/Beispieldateien stimmen byteweise
mit den geprüften SDK-Kopien überein; die Apps stimmen mit den geprüften
Release-Binaries überein. Zum Abschluss werden nur Dokumentation und Manifest
aktualisiert. Die neue Remote-Matrix bleibt ein gesonderter Nachweis.

Für den [vorherigen Commit `d4d3d1e`](https://github.com/PhysicSimulator/physim/actions/runs/37571221323)
sind tatsächliche Windows-v143-Debug sowie ClangCL Debug/Release erfolgreich; die übrige Matrix
wird separat beobachtet. Das ist ein Nachweis des damaligen Thermodynamikstands,
keine Abnahme der neuen Elektromagnetismusfunktionen.

Der frühere generierte Linux-Prüfordner `build/thermal-final-sdk-proof-linux`
wurde vollständig auf den Mac übertragen, bevor sein VM-Speicher freigegeben
wurde. Alle 12.498 regulären Dateien stimmen mit ihren ursprünglichen SHA-256-
Werten überein. Lokal erhaltenes Archiv:
`build/thermal-final-sdk-proof-linux-evidence.tar.gz`, SHA-256
`5d3b1d61b7214dab0c6a55ff3e17858046c6ecf8c17f4983f75cd079483ac854`.

## Thermodynamik und Physim 0.179.0 am 7. Oktober 2026

Das neue allokationsfreie SI-Modul berechnet ideale Gaszustände, Energie und
Entropiedifferenzen, konstante Wärmekapazitäten, signierten Wärmefluss sowie
exakte Reservoir- und isolierte Zweikörperrelaxation. Zehn reine Sprachbindungen
verwenden dieselben C-Funktionen und erhalten Quelldiagnosen bei Fehlern.
Ausgaben bleiben bei Fehlern unverändert; normalisierte Produkte vermeiden
Zwischenüberlauf und bewahren darstellbaren Wärmeaustausch auch bei unterlaufendem
Relaxationsfaktor. [Vertrag und vollständige Quellen](thermodynamics.md).

Die endgültigen neun gezielten Release-Prüfungen bestehen auf Intel macOS
(Apple Clang 16) unter
`build/contact-world-language-release-mac/test-results/run-mz7mtoms`
und Debian 12/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-ravfy132`.
Vier endgültige Linux-Clang-14-ASan/UBSan-Fälle bestehen unter
`build/thermal-asan-linux/test-results/run-i0vcdiw1`.
Eine versuchte Mac-Sanitizerprüfung wurde vor dem Build wegen des fehlenden
`ld64.lld` abgewiesen und wird nicht als ausgeführt gewertet.

Neun bestehende Sprach-/Experiment-/Analyse-/Sensor-/Paritätsprüfungen bestehen
mit der neuen Provenienz auf macOS unter `run-_nybq1u1` und Linux unter
`run-ro1dxr9n` in den jeweiligen Release-Testordnern. Die 25 generierten C-/
Sprachreferenzen sind geprüft. Der grafische Dokumentationsablauf besteht
auf macOS unter `run-p8tf_ue3` und Linux unter `run-zgj6zqre`.
Der aktuelle Katalog umfasst 583 Fälle ohne Fenster, 568 ohne SDL und 66
Fensterfälle. Dies ist kein neuer vollständiger 583-Fälle-Gesamtnachweis.

Der unabhängige Tutorialprüfer kontrolliert 2.412 Messungen mit 65-stelligen
Decimal-Exponentiallösungen in sechs Szenarien: Wärmefluss in beiden Richtungen,
Nullleitwert, thermisches Gleichgewicht sowie stark verschiedene Kapazitäten
und Leitwerte. Er kontrolliert alle SI-Kanäle, CRC/Footer, vollständige
Szenengeometrie, Zeitkomposition und 24 C-/Physim-Analysekonfigurationen.
Berichtsprüfer laden sechs Kurven und den Bilanzfehler neu; CSV-Werte müssen
mit den Rohdaten übereinstimmen. Ungültige absolute Temperaturen werden abgewiesen.
Reale Gase und weitere Stoffmodelle bleiben ausdrücklich offen.

Beide endgültigen SDKs bestehen die ausschließlich aus dem 46-Dateien-Kit
gestarteten vollständigen Prüfungen, einschließlich aller neun grafischen
Projektabläufe und der Dokumentationsnavigation:

- macOS: `build/thermal-final-sdk-proof-mac/Native SDK ä wtao5wwo`.
- Linux: `build/thermal-final-sdk-proof-linux/Native SDK ä tmja35_b`.

Sie bauen alle öffentlichen Header, 20 Standalone-Programme und 46 Sprachmodule
und prüfen Thermodynamik gegen installierten und neu aufgebauten Core:
4.824 unabhängig geprüfte Messungen und 48 gemischte Analysen je SDK.
Die endgültigen Pakete `build/Thermodynamics clean SDK ä mac` und
`build/Thermodynamics clean SDK ä linux` enthalten jeweils 387 Manifestdateien
und 66 kompilierte Sprachprodukte. Alle 273 Code-/Beispieldateien stimmen
byteweise mit den verifizierten SDK-Kopien überein; die Apps stimmen mit den
geprüften Release-Binaries überein. Abschließend werden nur Dokumentation
und Manifest aktualisiert.

Die tatsächliche Windows-CI von `3f14f3b` besteht bei v143 Debug und ClangCL
Debug/Release jeweils 576/577 Fälle. Die drei per offiziellem Artefakt-Digest
geprüften ZIPs zeigen denselben Fehler: `test_documentation_tracks.py` liest
UTF-8-Modellseiten mit der Standardcodierung CP1252. Alle Textleser dieses
Prüfers verwenden nun ausdrücklich UTF-8. Die Korrektur besteht lokal auch
mit simuliertem CP1252-Standard und ist als `49389c5` in beiden Repositories.
Die [erneute tatsächliche Matrix](https://github.com/PhysicSimulator/physim/actions/runs/37570296575)
bleibt ein eigener Nachweis; die Simulation ersetzt keinen Windows-Lauf.

Zwei frühere generierte Linux-SDK-Prüfordner wurden vor der Freigabe von
VM-Speicher vollständig auf den Mac übertragen und jede reguläre Datei gegen
ihren ursprünglichen SHA-256-Wert geprüft. Die Archive bleiben lokal erhalten:

- `build/batch-language-final-sdk-proof-linux-evidence.tar.gz`: 12.233 Dateien,
  SHA-256 `61f54526f0493eb993a35a4a1c8f4f8ae6b0de7f3d8c64f39b144925cbce0eca`.
- `build/documentation-tracks-sdk-proof-linux-evidence.tar.gz`: 12.259 Dateien,
  SHA-256 `77db4dbb3d26902e050f0a74ebc1831714834661a408cddff7d5c9e4a82ba250`.

## Getrennte C-/Physim-Dokumentation am 7. Oktober 2026

[Teil I: C](c-guide.md) und [Teil II: Physim](physim-guide.md) enthalten jeweils
sieben geordnete Kapitel, eigene ausführbare Spracheinstiege, vollständige
Experiment-/Analyseabläufe, Referenzen und dieselben acht Modelllernwege.
Das Hilfefenster wählt beide Teile separat aus und erhält den Startpunkt beim
Besuch gemeinsamer Themen. Die Prüfung lädt alle registrierten Themen,
kontrolliert Quelltext und Links und bedient die Auswahl und Startknöpfe über
SDL-Mausereignisse, auch im verkleinerten Fenster.

Die sechs gezielten Prüfungen bestehen auf Intel macOS (Apple Clang 16) unter
`build/contact-world-language-release-mac/test-results/run-c_azenr8` und
`run-8qrrmi9x` (je drei Fälle), unter Debian 12/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-hbhstge9` (sechs Fälle).
Die endgültige grafische Navigation besteht auf macOS unter `run-p4g3hwpq`
und Linux unter `run-mltm2ijg` in den jeweiligen Release-Testordnern.

Beide ausschließlich aus dem 43-Dateien-Prüfkit gestarteten SDK-Prüfungen
bestehen vollständig, einschließlich aller neun grafischen Projektabläufe und
der neuen Hilfefenster-Navigation:

- macOS: `build/documentation-tracks-sdk-proof-mac/Native SDK ä 00qmysrj`.
- Linux: `build/documentation-tracks-sdk-proof-linux/Native SDK ä 2s2tu214`.

Beide Prüfungen bauen die Spracheinstiege aus den verschobenen SDK-Quellen,
prüfen die veröffentlichten Modellquellen und führen die bisherigen fachlichen
Prüfungen gegen installierten und neu aufgebauten Core aus. Die endgültigen
Pakete `build/Documentation routes clean SDK ä mac` und
`build/Documentation routes clean SDK ä linux` enthalten 374 Manifestdateien
und weiterhin 63 kompilierte Sprachprodukte. Alle Code-/Beispieldateien werden
byteweise gegen die geprüften SDK-Kopien verglichen, die Apps gegen die
geprüften Release-Binaries. Zum Abschluss werden nur Dokumentation und Manifest
aktualisiert. Der Katalog umfasst jetzt 577 Fälle ohne Fenster und 562 ohne SDL;
die vorherige vollständige 572-Fälle-Abnahme wird dadurch nicht umgedeutet.
Die aktuelle Windows-/ARM-/Clang-Matrix bleibt ein eigener Abnahmenachweis.

## Batch-Hostdienste und Physim 0.178.0 am 7. Oktober 2026

Analysen erhalten einen optionalen, größen- und versionsgeprüften `run_host`-Tail
innerhalb ABI 3. Ein ausdrücklicher Host liefert den vorhandenen Controller;
der Core bleibt unabhängig von Prozess- und GUI-Code. Besitzende `Batch`-Werte
kopieren ihre Konfiguration und Ergebnisse, einschließlich Arrays, optionaler
Werte, Strukturfelder und Closures. Sie starten feste, zeitgesteuerte, adaptive
und Parameterstudien und können nach journalierter Teilerledigung pausieren.
Die Fortsetzung validiert die alte Serie erneut und schreibt ausschließlich in
einen neuen Ordner. [Benutzung und vollständige Beispiele](batch-language.md).

Die gemischte Prüfung führt 32 C-/Physim-Analysekonfigurationen und acht CLI-
Referenzen aus. Sie vergleicht alle gespeicherten Zeitpunkte und Werte,
Controllerzähler, Messstatus, Quellen, Histogramme, exklusiven Ausgabezugriff
und unveränderte frühere Archive. Die vollständigen C-/Physim-Monte-Carlo-
Beispiele werden in allen vier Kombinationen geprüft: 1024 rohe Trajektorien,
unabhängig berechnete Mittelwerte, Streuungen, Type-7-Quantile und Histogramme.
Die gezielte macOS-Prüfung besteht mit 10/10 unter
`build/contact-world-language-release-mac/test-results/run-hkaxnwf8`.
Die sieben gezielten Linux-Clang-14-ASan/UBSan-Prüfungen bestehen unter
`build/batch-language-clang-asan/test-results/run-uzbcfl58`.
Fehlgeschlagene Allokationen erzeugen keine Serien; verkürzte oder unbekannte
Hostdeskriptoren werden vor dem Callback abgewiesen. Ein gespeichertes Unit-
Symbol bleibt nach der Freigabe aller Batch-Kopien gültig.

Der [vorherige CI-Stand](https://github.com/PhysicSimulator/physim/actions/runs/37554714527)
bestätigt Linux/GCC, Windows/v143 Debug und Release sowie Windows/ClangCL Debug.
Die per veröffentlichtem SHA-256-Digest geprüften ClangCL-Release-, Linux-Clang-,
ARM- und Intel-Artefakte enthalten weitere Testfehler: erneute Floating-Point-
Berechnung statt Prüfung der geschriebenen Snapshotbits, ein abweichender
Rückgabetyp des alten `ps_get_analysis`-Fixturesymbols, Timing vor bestätigter
Fortsetzung beziehungsweise unabhängig von tatsächlicher verstrichener Zeit
und bitweise C-/Physim-Gleichheit adaptiver Messwerte. Die Tests bewahren jetzt
die geschriebenen Bytes, verwenden die korrekte Entry-Signatur, messen nach
Rückmeldung und prüfen numerische Parität mit einer 100-mal strengeren relativen
Grenze als der konfigurierte RK45-Vertrag. Anzahl, Endzeit und Struktur bleiben
gesondert geprüft. Clang 14 reproduziert den alten Entry-Ausfall nicht; die
erneute tatsächliche aktuellen Clang-/Windows-/ARM-CI-Abnahme bleibt erforderlich.

Auf dem Intel-Mac blockierten neue unsignierte Testprogramme sowie der
Homebrew-Git-Client im Loader. Eine lokal ad hoc signierte, aus denselben
Quellen gebaute Probe bestand. Der Builder signiert deshalb ausschließlich
frisch gelinkte temporäre Mac-Artefakte vor Veröffentlichung und Cacheprüfung;
Code-Signierfehler erhalten die vorherige Ausgabe. Der systemeigene Apple-Git-
Client ist verwendbar. Bereits blockierte Prozessinstanzen bleiben als
Betriebssystemproblem gesondert zu behandeln; ihre Läufe sind keine Testnachweise.

Ältere Linux-Prüfverzeichnisse unter `build/project-audit` sind vor der Freigabe
des VM-Speichers vollständig auf dem Mac unter
`build/project-audit-linux-evidence.tar.gz` gesichert. Alle 19115 regulären
Dateien wurden gegen ihre ursprünglichen SHA-256-Werte geprüft. Archiv-SHA-256:
`e09978e09f6aa49459134acb0eeb73b444b901f91ef3d1da820f8440f7650f14`.

Die vollständigen erneuten Release-Suiten bestehen mit **572/572** auf Intel
macOS unter `build/contact-world-language-release-mac/test-results/run-vliklgjg`
und Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-nr1abvnk`.
Die letzte Korrektur der Timingmessung und die eigenständige Einbindung des neuen
Host-Headers erhalten zusätzliche Nachweise: vier Mac-App-Prüfungen unter
`run-9o8ahm9d`, vier Linux-App-Prüfungen unter `run-ehaq83ko` und drei Linux-
Batch-Prüfungen unter `run-2z5ei6pe`, jeweils in den genannten Release-Testordnern.

Beide endgültigen, ausschließlich aus dem 41-Dateien-Prüfkit gestarteten SDK-
Prüfungen bestehen unter `build/batch-language-sdk-proof-mac/Native SDK ä kx911ojx`
und `build/batch-language-final-sdk-proof-linux/Native SDK ä pji_aiq0`.
Sie bauen alle öffentlichen Header separat nach stdio-/Locale-Headern,
prüfen die neuen Dienste sowie 2048 neue Batch-Beispielarchive je Plattform
gegen installierten und neu aufgebauten Core und führen die bisherigen
Lernpfade weiter aus. Linux besteht zusätzlich alle neun grafischen SDK-Abläufe.
Die endgültigen Pakete `build/Batch language clean SDK ä mac` und
`build/Batch language clean SDK ä linux` enthalten 368 Manifestdateien und
63 kompilierte Sprachprodukte. Code und Beispiele werden byteweise gegen die
verifizierten Kopien geprüft; die jeweiligen Apps müssen ebenfalls mit den
geprüften Release-Binaries übereinstimmen. Nur Dokumentation und ihr Manifest
werden zum Abschluss aktualisiert. Die neue Remote-Matrix bleibt gesondert offen.

## Elternprozess-Abbruch und Paketierungsfehler am 7. Oktober 2026

Ein unabhängiger Versuch mit dem unveränderten SDK von `222d4bb` beendet nur
die PID des Seriencontrollers. Der getrennte Runner schreibt danach weiter in
seine Heartbeat-Datei; die Fixture wurde anschließend gezielt beendet.
Die neue Offline-Pipeüberwachung endet auch innerhalb eines hängenden Create-
oder Step-Callbacks mit Code 125. Ein- und Vierworker-Serien prüfen den harten
Controllerabbruch, unveränderte abgeschlossene Archive, erhaltene Journalzeilen
und das Fehlen eines erfundenen Gesamtberichts.

Die sechs gezielten Runner-/Serienprüfungen bestehen auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-w2zuyyw1` und Debian
unter `build/contact-world-language-release-linux/test-results/run-nhql7sdy`.
Die neue Abbruchfixture besteht zusätzlich unter Linux mit ASan/UBSan unter
`build/spring-tutorial-asan-linux/test-results/run-um7q1_rf`.
Der grafische Endzeit-Ablauf besteht nach einer Korrektur der Scroll-/Klickfolge
im Test auf macOS unter `run-fj81o4wf` und Linux unter `run-2fkhvsx1` in den
jeweiligen Release-Testordnern. Die Prüfung ersetzt die Texteingabe nicht durch
direktes Setzen des App-Zustands.

Der [CI-Lauf von `222d4bb`](https://github.com/PhysicSimulator/physim/actions/runs/37550644122)
zeigt weitere Fehler im getrennten SDK-Schritt und in Windows-Tests.
Die heruntergeladenen GCC-/ARM-Artefakte scheitern am installierten
`language-contact_stack`-Modul: `--install` hatte ohne `--examples` keine
kompilierten Sprachbeispiele erzeugt. Installation baut jetzt den vollständigen
Katalog automatisch; der SDK-Prüfer verlangt alle Produkte und Manifesteinträge.
Die Windows-Testartefakte zeigen einen CRLF/LF-Vergleichsfehler in der
Extremwertprüfung und `select()` auf einer Pipe im Logging-Test. Der Textleser
verwendet den CRT-Textmodus; ein begrenztes Queue-/Thread-Leseverfahren erhält
die Fristprüfung auch für Windows-Pipes. Aktuelle Windows-Abnahme benötigt
einen erneut tatsächlich ausgeführten Workflow.

Die drei ausgewerteten Archive (`linux-ui-gcc`, `macos-macos-15` und
`sdk-windows-v143-Debug`) stimmen byteweise per SHA-256 mit den über die
GitHub-Artefakt-API veröffentlichten Digests überein. Ihre Hauptsuiten bestehen
mit 564/564 auf GCC und ARM sowie 561/564 unter Windows/v143 Debug. Die drei
Windows-Ausfälle sind ausschließlich `analysis_extremes`, `runner_logging_c`
und `runner_logging_phys`; dieser alte Stand bestätigt noch keine Korrektur.

Diese Arbeiten sichern bestehende Serien und Release-Prüfungen. Die im
Anforderungsabgleich fehlende Physim-Sprachbindung für Batch ist weiterhin offen.

Die vollständigen Release-Suiten mit der Abbruchprüfung und den CI-Testkorrekturen
bestehen mit **565/565** auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-n9fesl5k` und Debian/GCC
unter `build/contact-world-language-release-linux/test-results/run-vl2t4muf`.
Beide SDKs wurden mit `--install` ohne `--examples` erzeugt und enthalten den
vollständigen Katalog mit 61 kompilierten Sprachprodukten.

Beide ausschließlich aus dem unabhängigen Kit gestarteten SDK-Prüfungen bestehen:
macOS unter `build/parent-watch-sdk-proof-mac/Native SDK ä cb0fysqc`, Linux unter
`build/parent-watch-sdk-proof-linux/Native SDK ä m7a3cycd`. Linux führt zusätzlich
alle neun grafischen SDK-Projektabläufe erfolgreich aus. Die endgültigen Pakete
unter `build/Parent watch clean SDK ä mac` beziehungsweise `build/Parent watch clean SDK ä linux`
erhalten die aktuelle Dokumentation mit erneuertem SHA-256-Manifest. Alle Code-
und Beispieldateien werden gegen die verifizierten Kopien geprüft; die jeweilige
App muss zudem bytegleich zum geprüften Release-Binary bleiben.

## Gesamtabgleich und CI-Prüfpfade am 7. Oktober 2026

Der [Abgleich mit dem vollständigen Plan](project-audit.md) bewahrt 531 Quellblöcke
mit Originaltext und Zeilenbereichen, darunter alle 475 Aufzählungspunkte.
Ein SHA-256-Wert bindet die Erfassung an die unveränderte Plandatei.
Acht Lernpfade besitzen begrenzte Implementierungsnachweise; zehn konkrete
Lücken sind erfasst. 513 weitere Blöcke bleiben ohne Einzelabnahme. Diese Zahlen
sind kein Fertigstellungsprozentsatz: Entwürfe, Nicht-Ziele und spätere Domänen
bleiben mit ihrem ursprünglichen Kontext enthalten. Das Gesamtziel ist offen.

Am Ausgangsstand `4841ee2` bestehen die vollständigen Release-Suiten auf
Intel macOS unter `build/contact-world-language-release-mac/test-results/run-7kin6e77`
und Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-dbd70ucn`
mit jeweils 563/563 Fällen. Die zugehörigen CI-Artefakte für Linux/GCC und
Apple Silicon bestätigen ebenfalls 563 bestandene Fälle; ihre Archivbytes
stimmen mit den auf GitHub veröffentlichten SHA-256-Digests überein. Das sind
standbezogene Nachweise und keine Abnahme nachträglicher Änderungen.

Die getrennten CI-Schritte enthielten drei reproduzierbare Fehler: Ein mit nur
sechs Dateien gepacktes Linux-Prüfkit fehlte unter anderem `sdk_series_probe.c`;
der separate Sprachbeispielprüfer hatte die Dateien für `run_index_values`
nicht erzeugt; Linux/Clang wies den Logger-Test wegen einer nicht deklarierten
GNU-Funktion `strtod_l` nach vorangegangenen libc-Headern zurück.

Das Prüfkit wird nun aus den expliziten Repository-Dateiabhängigkeiten des
SDK-Prüfers und lokalen Testheadern erzeugt. Es enthält 33 unabhängige Harness-
Dateien plus SHA-256-Manifest, keine Core-Implementierung oder SDK-Header.
Fehlende, dynamische oder unerlaubte Pfade verhindern die Veröffentlichung.
Bestehende Archive bleiben erhalten; ein Schreibfehler entfernt nur die neu
erzeugte Ausgabe. Der separate Beispielprüfer baut zuerst die vorhandene
öffentliche Indexfixture und erzeugt damit finalisierte, alte und rekonstruierbare
Eingaben. Anschließend bestehen die 18 Programme, alle 43 vorhandenen Module
und die Tests für inkrementelle Übersetzung sowie Fehlererhaltung auf macOS
und Linux/GCC. Test-Runner- und Bootstrap-Selbstprüfungen bestehen separat auf
macOS; ihre absichtlich fehlschlagenden Fälle sind Teil der Prüfung.

Die Linux-Sprachlaufzeit verwendet im Dezimalkomma-Fallback `strtod` mit einer
vorübergehenden POSIX-Thread-Locale und stellt danach die aufrufende Locale
wieder her. Die Windows- und macOS-Implementierungen bleiben erhalten.
Ein strenger Clang-C17-Compile mit `-Werror=implicit-function-declaration`
reproduziert den vorherigen Fehler und besteht mit der Korrektur.
Linux/Clang 14 Debug besteht Logger, Locale, Sprachchecker und Kitprüfung unter
`build/project-audit-clang-linux/test-results/run-daitiwoc`; die Localeprüfung
lief tatsächlich mit installiertem `de_DE.utf8` und kontrolliert auch den
Erhalt eines eigenen aufrufenden Thread-Locale-Handles. Die drei Laufzeit-
Prüfungen bestehen mit AddressSanitizer/UndefinedBehaviorSanitizer unter
`build/spring-tutorial-asan-linux/test-results/run-cg8grlij`.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.

Das nur aus dem Kit entpackte SDK-Prüfprogramm besteht auf macOS unter
`build/project-audit/kit-sdk-proof-mac/Native SDK ä 4gzy3db9` und Linux unter
`build/project-audit/kit-sdk-proof-linux/Native SDK ä 0iuzgm9e`. Linux führt dabei
zusätzlich alle neun grafischen C-/Physim-Projektabläufe aus. Die geprüften SDKs
sind die unveränderten Installationen des vorherigen Lernpfads; aktuelle Pakete
mit der Localekorrektur erhalten gesonderte Nachweise.

Die erneuten vollständigen Release-Suiten mit allen aktuellen Änderungen bestehen
mit 564/564 auf dem Intel-Mac unter
`build/contact-world-language-release-mac/test-results/run-ty_ic4b4` und Debian/GCC
unter `build/contact-world-language-release-linux/test-results/run-1_8_4_wp`.
Dieser Nachweis ersetzt keine noch fehlenden aktuellen Windows-/Apple-Silicon-
CI-Ergebnisse und keine vollständige Produktabnahme.

Die neue Offline-Abnahmeseite und die gesamte Dokumentnavigation bestehen
unter macOS mit `build/contact-world-language-release-mac/test-results/run-p4cryhht`
und unter Linux mit `build/contact-world-language-release-linux/test-results/run-9gtpbm_o`.
Das neu installierte macOS-SDK mit der Localekorrektur besteht den vollständigen,
ausschließlich aus dem Kit gestarteten Prüfer unter
`build/project-audit/final-kit-sdk-proof-mac/Native SDK ä 9pkl2d81`.
Alle öffentlichen Header werden dabei nach bereits eingebundenen stdio-/Locale-
Headern kompiliert, um die problematische Einbindungsreihenfolge ausdrücklich zu prüfen.

Auch das neue Linux-SDK mit der Localekorrektur besteht den vollständigen
isolierten Prüfer und alle neun grafischen Projektabläufe unter
`build/project-audit/final-kit-sdk-proof-linux/Native SDK ä x_rvzwxt`.
Die endgültigen Pakete erhalten die aktuellen Nachweise mit erneuerten
Manifestwerten. Je Paket bleiben alle 141 Code-/Binärdateien und 110 Beispieldateien
bytegleich zur verifizierten Installation; die App bleibt bytegleich zur
geprüften Release-App. Alle 357 Manifestdateien werden per SHA-256 geprüft.
Die tatsächlichen Umgebungen sind Intel macOS 14.6.1/Apple Clang 16 und Debian 12/
GCC 12.2 beziehungsweise die genannten gezielten Clang-14-Prüfungen, jeweils
SDL 3.2.30. Linux-Grafik lief unter X11/Xvfb/Openbox/Mesa. Die Behebung der
remote CI bleibt durch den nächsten tatsächlichen Workflowlauf zu bestätigen.

## Gespeicherten Lauf unabhängig auswerten am 7. Oktober 2026

Der neue Lernpfad verwendet die bestehenden gleichförmigen C-/Physim-Experimente
und zwei vollständige neue Archivanalysen. Ein eigenständiges Analyseprojekt
importiert einen gespeicherten Lauf, leitet `position.x` über zentrale Sekanten
ab und rekonstruiert die Position mit Trapezen ab dem archivierten Anfangswert.
Drei Plots zeigen Position/Rekonstruktion, Geschwindigkeit und Residuum; zwei
Tabellen speichern Laufumfang, Wiederherstellungsstatus, Anfangs-/Endposition,
Verschiebung, Sekantengeschwindigkeit und maximale Rekonstruktionsabweichung.
Der CSV-Export enthält sämtliche fünf Reihen. Das Verfahren benötigt genau
einen Lauf, mindestens zwei Messpunkte, eine streng steigende Zeitachse und
Längenwerte. Ein Nulloffset prüft Dimensionskompatibilität; die gemeinsame
Positionsachse verwendet die kanonischen Meter der rekonstruierten Reihe,
auch wenn das Archiv ein anderes Anzeigeeinheitensymbol enthält.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.
[Lernziel, Modell, Gleichungen und vollständige Quellen](saved-run-tutorial.md).

Der unabhängige Pythonprüfer erzeugt neben beiden echten Modellarchiven sechs
weitere Archive direkt mit eigener PSRUN-Kodierung und CRCs: unregelmäßige
quadratische Positionen, eine spätere Startzeit, zwei rückwärts gerichtete
Messpunkte, ein lesbares Archiv ohne Abschlussfooter, ein abweichendes
Anzeigeeinheitensymbol und 2.049 Messpunkte. Beide Analysesprachen verarbeiten
alle acht Fälle. Ein separater C-Prüfer liest die Originalarchive, berechnet
Sekanten und Trapeze selbst und kontrolliert alle Berichtskurven, Vorschau-
Endpunkte, SI-Achsendimensionen/-maßstäbe/-symbole sowie beide Tabellen.
Python vergleicht jeden Wert der vollständigen CSV und die beiden Analysen
untereinander. Alle 16 gemischten Berichte bestehen; bei der großen Reihe
bleiben sämtliche 2.049 CSV-Zeilen erhalten, während die Vorschau auf höchstens
2.048 Punkte begrenzt ist. Datei-SHA-256-Werte bleiben nach jeder Analyse gleich.

Leere, zu kurze, mehrfach ausgewählte oder fehlende Eingänge, doppelte Zeiten,
fehlende Positionskanäle und falsche Dimensionen werden ohne fertigen Bericht
zurückgewiesen. Wiederhergestellte gültige Präfixe tragen ausdrücklich den
Tabellenwert `Recovered prefix=1`; fehlende spätere Messpunkte werden nicht
hinzugefügt. Die dokumentierten Grenzen unterscheiden Datenverarbeitung von
physikalischer Validierung und erklären Rauschen sowie grobe/unregelmäßige
Abtastung. Vier vollständige Dokumentationsblöcke entsprechen exakt den gebauten
Experiment-/Analysequellen.

macOS Release besteht drei ausgewählte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-fnegvx10`, Linux GCC
Release unter `build/contact-world-language-release-linux/test-results/run-l1y8qzpf`:
neuer Lernpfad, Quellcodegleichheit und bisheriges C-Dokumentationsbeispiel.
Linux Debug besteht den neuen Lernpfad mit AddressSanitizer und
UndefinedBehaviorSanitizer unter
`build/spring-tutorial-asan-linux/test-results/run-0vhqs921`.

Beide Plattformen bestehen drei ausgewählte Fensterabläufe: macOS unter
`build/contact-world-language-release-mac/test-results/run-l9s6h_uq`, Linux unter
`build/contact-world-language-release-linux/test-results/run-6ho99fgb`.
Der neue Ablauf erzeugt je ein echtes Archiv mit 32 angehaltenen Schritten
zu 0,0625 s und importiert es in ein Analyseprojekt der jeweils anderen Sprache.
Die Projekte enthalten keine Experimentquelle und kein Experimentmodul;
Simulation wird abgewiesen. Der Import erfolgt über die tatsächliche
Schaltfläche mit injiziertem Testpfad, nicht über eine automatisierte native
Dateiauswahl. Der Test kontrolliert 33 Messpunkte, drei Plots, zwei Tabellen,
Position 3 m und Geschwindigkeit 1,5 m/s und öffnet das Projekt samt Bericht
anschließend ohne Build erneut. Python vergleicht Original und importierte
Datei per SHA-256. Zusätzlich bestehen die bisherigen eigenständigen
Analyseprojekte und die Offline-Dokumentnavigation einschließlich des neuen
Lernpfads. Die geprüften Intel-macOS-/Debian-12-Umgebungen und Compiler-/SDL-
Versionen entsprechen den folgenden Nachweisen. Position, Kennzahlentabelle
und wiedergeöffnete Geschwindigkeit wurden auf macOS bei 1280 × 900 logischen
Pixeln visuell geprüft, Position und Geschwindigkeit unter Linux bei 1280 × 900.

Beide neu installierten SDKs bestehen den vollständigen Relokationsprüfer:
macOS unter `build/saved-run-tutorial-sdk-proof-mac/Native SDK ä tt9nk6ee`,
Linux unter `build/saved-run-tutorial-sdk-proof-linux/Native SDK ä eo571jes`.
Der Prüfer kontrolliert alle Manifestdateien und öffentlichen Header, baut Core
und alle 43 Sprachmodule aus der mitgelieferten Quelldistribution neu und führt
die bisherigen Index-, Kontakt-, Diagnose-, Serien- und Beispielprüfungen aus.
Der neue Lernpfad läuft mit installiertem und neu gebautem Core sowie über die
dokumentierten nativen Builds von Erzeuger und eigenständigem Analyseprojekt.
Letzteres darf kein Experimentmodul erstellen. Alle acht Archivszenarien und
16 gemischten Berichte bestehen in jedem Durchgang. Die endgültigen Pakete
enthalten die aktuellen Nachweise mit erneuertem Manifest: jeweils 355 Dateien
mit SHA-256-Prüfung. Alle 141 Code-/Binärdateien und 110 Beispieldateien bleiben
bytegleich zur verifizierten Installation; die App bleibt bytegleich zur
geprüften Release-App.

Der aktuelle Katalog umfasst 563 Prüfungen ohne Fenster (548 ohne SDL),
66 Fensterfälle, 18 eigenständige Sprachprogramme und 43 Experiment-/Analysemodule.
Ein Gesamtlauf aller aktuellen Fälle wird hier nicht behauptet; der frühere
Gesamtlauf aller 555 Fälle bleibt weiter unten belegt. Der Lernpfad schließt
keine anderen noch offenen Anforderungen des Projektplans ab.

## Monte Carlo mit unsicheren Anfangswerten am 7. Oktober 2026

Vier vollständige Quellen liefern in C und Physim denselben Vakuumwurf mit
normalverteilten Anfangsgeschwindigkeiten und eine Auswertung seiner archivierten
Serie. Die vier typisierten Geschwindigkeitsparameter gehören zur Modellinstanz.
Die Bewegung ist analytisch, ohne Sensorrauschen oder Integrationsfehler.
Neun Kanäle speichern Ist-/Sollposition, Geschwindigkeit, Populationsstreuung und
Energie; die Szene zeigt beide Bahnen und ein Kreuz von einer Standardabweichung.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.
[Lernziel, Modell, statistische Annahmen und vollständige Quellen](monte-carlo-tutorial.md).

Der unabhängige Pythonprüfer berechnet PCG32-Ziehungen und Ballistik selbst.
Er kontrolliert 1.536 archivierte Läufe mit jeweils 33 Messzeilen: vier Serien
mit 256 unterschiedlichen Seeds in beiden Sprachen und mit einem beziehungsweise
vier Workern sowie zwei konstante Serien mit beiden Standardabweichungen null.
Innerhalb derselben Sprache sind die Endwert-CSV-Dateien bei anderer Parallelität
bytegleich; beide Sprachen stimmen innerhalb der numerischen Vergleichsschranken
überein. Der Prüfer liest PSRUN-Magic, CRCs, Footer, sämtliche neun Kanäle,
SI-Dimensionen und Seedprovenienz unabhängig. Szenen für Seeds 0, 42, einen Wert
oberhalb 2^63 und UINT64_MAX stimmen in sämtlichen Objekt- und Punktfeldern
überein. Fehlende beziehungsweise gekürzte Mitglieder, verschiedene Endzeiten
und geänderte Populationsparameter werden von beiden Analysen abgewiesen.
Die Instanzprobe prüft unabhängige Parameter,
Seed-Wiederholung, Seed-Wechsel, Nullstreuung, Energie und unveränderte Messwerte
nach abgewiesenen Schritten.

Beide Analysesprachen lesen beide Laufsprachen. Vier Plots zeigen alle Endwerte
und die beiden Histogramme; zwei Tabellen vergleichen Mittelwert,
Stichprobenstreuung und Typ-7-Quantile mit der Modellreferenz und zeigen ein
95%-Intervall des Mittelwerts bei bekannter Normalstreuung. Ein separater
C-Prüfer kontrolliert sämtliche Kurven, Klassenhäufigkeiten und Tabellenwerte
gegen die Originalarchive. Acht gemischte Analysen bestehen, einschließlich
konstanter Populationen mit einer Histogrammklasse und Intervallbreite null.
Der CSV-Export enthält alle 256 Endwertpaare. Diese Beispielanalyse erwartet
explizit 256 archivierte Dateien; die allgemeine Serienauswertung bleibt für
andere Laufzahlen verfügbar. Vier dokumentierte Codeblöcke entsprechen exakt
den tatsächlich gebauten Quellen.

macOS Release besteht die drei ausgewählten Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-bnxl1mtn`, Linux GCC
Release unter `build/contact-world-language-release-linux/test-results/run-8exxinth`:
neuer Lernpfad, Quellcodegleichheit und bisherige Monte-Carlo-Serienreferenz.
Linux Debug besteht den neuen Lernpfad mit AddressSanitizer und
UndefinedBehaviorSanitizer einschließlich beider Experimente, Analysen und
Instanzprobe unter `build/spring-tutorial-asan-linux/test-results/run-n7_ec93l`.

Die neue Schaltfläche **Ersten Serienlauf auswerten** startet die gebaute Analyse
mit dem ursprünglichen ersten Archivpfad. Sie ist ohne abgeschlossene erfolgreiche
Serie beziehungsweise bei ungebauter oder ungespeicherter Analyse gesperrt.
Die Monte-Carlo-Beispielanalyse liest dadurch die 256 Nachbardateien, ohne den
Einzellaufimport oder dessen Kopiervertrag zu ändern. Gewöhnliche Analysen
verarbeiten über dieselbe Schaltfläche nur den ersten Lauf.

macOS besteht drei ausgewählte Fensterabläufe unter
`build/contact-world-language-release-mac/test-results/run-qzxygk3g`: der neue
Lernpfad, die bestehende Serienbedienung und die Offline-Dokumentnavigation.
Der neue Ablauf baut beide Sprachprojekte, führt 32 angehaltene Schritte aus,
öffnet 33 archivierte Messzeilen mit Szene, startet je 256 Läufe mit vier
Workern und betätigt die neue Schaltfläche tatsächlich per Mausereignis.
Er prüft anschließend den Originalpfad, vier Plots, zwei Tabellen und die
Modell-/Stichprobenwerte. Linux besteht dieselben drei Fensterabläufe unter
`build/contact-world-language-release-linux/test-results/run-8upn0u0o`.
macOS lief auf dem nativen Intel-Desktop; Linux auf Debian 12/GCC 12.2 unter
X11/Xvfb/Openbox/Mesa, jeweils mit SDL 3.2.30. Szene, Histogramm, Statistik und
Mittelwertintervall wurden auf macOS bei 1440 × 960 logischen Pixeln visuell geprüft.
Auch Szene, Histogramm und beide Tabellen wurden unter Linux bei 1440 × 960 geprüft. Breite
Statistiktabellen benötigen horizontales Scrollen; dies ist im Ablauf beschrieben.

Beide SDKs bestehen den vollständigen Relokationsprüfer: macOS unter
`build/monte-carlo-tutorial-sdk-proof-mac/Native SDK ä t7lngpyy`, Linux unter
`build/monte-carlo-tutorial-sdk-proof-linux/Native SDK ä 9_kllbwq`. Der Prüfer
kontrolliert alle Manifestdateien und öffentlichen Header, baut Core und alle
42 Sprachmodule aus der mitgelieferten Quelldistribution neu und führt die
bisherigen Index-, Kontakt-, Diagnose-, Serien- und Beispielprüfungen aus.
Der Monte-Carlo-Lernpfad läuft mit installiertem und neu gebautem Core sowie
über den dokumentierten nativen C-Projektbuild, jeweils mit den reproduzierbaren
Serien und allen acht gemischten Analysen. Die endgültigen Pakete enthalten
die aktuellen Nachweise mit erneuertem Manifest. Alle 140 Code-/Binärdateien
und 108 Beispieldateien bleiben bytegleich zur jeweils verifizierten Installation;
die App bleibt bytegleich zur geprüften Release-App. Insgesamt enthält jedes
Paket 351 per SHA-256 geprüfte Dateien.

Der aktuelle Katalog umfasst 561 Prüfungen ohne Fenster (546 ohne SDL),
65 Fensterfälle, 18 eigenständige Sprachprogramme und 42 Experiment-/Analysemodule.
Ein Gesamtlauf aller aktuellen Fälle wird hier nicht behauptet; der frühere Lauf
aller 555 Fälle ist weiter unten belegt. Die neue Analyse setzt unabhängige
Normalziehungen und bekannte Modellstreuung voraus und beweist keine allgemeine
Unabhängigkeit aufeinanderfolgender Seeds. Andere offene Anforderungen des
Projektplans bleiben offen.

## Elastischer und inelastischer Stoß am 7. Oktober 2026

Vier neue vollständige Quellen liefern ein zentrales Stoßexperiment im Vakuum
und dessen Auswertung in C und Physim. Massen, Anfangsgeschwindigkeiten und
Restitution sind typisierte Parameter je Modellinstanz. Die kontinuierliche
Kugelabfrage bestimmt den Kontakt innerhalb eines Zeitschritts; Impulsantwort
und Integration der Restzeit erhalten die Nachstoßbewegung. Das Experiment
modelliert genau diesen einen zentralen Stoß ohne äußere Kräfte, Reibung oder
Rotation. Bei vollkommen inelastischem Stoß bleiben zwei co-bewegte Kugeln
bestehen; ein gemeinsamer Körper oder eine Klebeverbindung wird nicht erzeugt.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.

Ein unabhängiger Pythonprüfer kontrolliert 7.250 primäre Messzeilen aus elf
Szenarien in beiden Sprachen: elastisch, teilweise und vollkommen inelastisch,
ungleiche Massen, einseitig bewegte, auseinanderlaufende, ruhende und co-bewegte
Kugeln sowie grobe Aufzeichnung und Kontakt auf einer Schrittgrenze. Er berechnet
Stoßzeit, Impuls, Nachstoßgeschwindigkeiten, dissipierte Energie und sämtliche
Positionen aus geschlossenen eindimensionalen Formeln. Er prüft alle elf
Kanalnamen mit SI-Dimensionen, Parameterprovenienz und sämtliche Felder der
Szenen mit acht Einträgen vor beziehungsweise zehn nach dem Kontakt. Ein
zusätzlicher einzelner großer Schritt muss den Stoß und die restliche Bewegung
vollständig erfassen. Ungültige Massen/Restitution werden zurückgewiesen.
Die Instanzprobe führt verschiedene Massen und Restitutionen gleichzeitig aus,
prüft Einheiten, Reset und unveränderte Messwerte bei abgewiesenen Schritten.

Die beiden Analysesprachen lesen C- und Physim-Läufe. Vier Plots zeigen
Positionen, Geschwindigkeiten, Gesamtimpuls und K/D/K+D; zwei Tabellen liefern
Erhaltungskontrollen und tatsächliche Ereignisse. Ausbleibende Kontakte erzeugen
keine erfundene Ereigniszeile. Ein separater C-Prüfer vergleicht acht vollständige
Berichtskurven, SI-Plotachsen und alle Tabellenwerte mit den Originaldaten.
Der unabhängige Parser kontrolliert jedes Feld des vollständigen CSV-Exports.
Alle 44 Kombinationen aus elf Modellen und beiden Experiment-/Analysesprachen
bestehen. Leere oder mehrfache Auswahl wird kontrolliert abgewiesen; diese
Beispielanalyse verarbeitet ausdrücklich einen Lauf. Vier dokumentierte
Codeblöcke entsprechen exakt den gebauten Quellen.
[Lernziel, Modell, Gleichungen und Ablauf](collision-tutorial.md).

macOS Release besteht fünf ausgewählte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-4tqyn295`, Linux GCC
Release dieselben fünf unter
`build/contact-world-language-release-linux/test-results/run-i_fspv_j`:
neuer Lernpfad, Quellcode- und Referenzdokumentation sowie bisherige Kugelstoß-
Referenz und C-/Physim-Kugelparität. Linux Debug mit AddressSanitizer und
UndefinedBehaviorSanitizer besteht den gesamten neuen Lernpfad einschließlich
beider Module, Analysen und Instanzprobe unter
`build/spring-tutorial-asan-linux/test-results/run-bwnqmvya`.

macOS besteht die drei ausgewählten Fensterabläufe unter
`build/contact-world-language-release-mac/test-results/run-s5t1jz8z`, Linux unter
`build/contact-world-language-release-linux/test-results/run-a8u4tf5i`.
Die neue Prüfung übernimmt die getesteten Quellen in beide App-Projektsprachen,
baut, wählt mA=2 kg/e=0, führt 100 angehaltene Schritte zu 0,02 s aus und öffnet
101 gespeicherte Messzeilen mit Szene und Analyse erneut. Die gemeinsame
Nachstoßgeschwindigkeit beträgt 0,2 m/s; K=0,06 J, D=0,48 J und K+D=0,54 J.
Impuls 0,6 kg m/s, Stoßzeit 4/3 s und Impuls auf A −0,8 kg m/s werden geprüft.
Zusätzlich bestehen der bestehende Physim-Kugelablauf und die komplette
Offline-Dokumentnavigation einschließlich des neuen Lernpfads. macOS lief im
nativen Desktop, Linux unter X11/Xvfb/Openbox/Mesa. Compiler-, SDL- und OS-
Versionen entsprechen den folgenden Intel-/Debian-12-Nachweisen. Screenshots
von Szene und Energieabrechnung wurden visuell geprüft. Auf kleiner Fläche
bleiben untere Diagrammteile über Scrollen erreichbar; die Darstellung verbindet
Messpunkte und berechnet keinen endlichen Stoßkraftverlauf.

Beide neu installierten SDKs wurden an einen anderen absoluten Pfad mit
Leerzeichen/Umlauten kopiert und vollständig verifiziert: macOS unter
`build/collision-tutorial-sdk-proof-mac/Native SDK ä ol9_d8s1`, Linux unter
`build/collision-tutorial-sdk-proof-linux/Native SDK ä 4sht9ghs`. Der Gesamtprüfer
kontrolliert Manifest und öffentliche Header, baut Core und alle Sprachmodule
aus der mitgelieferten Quelldistribution neu und führt die bisherigen Archiv-,
Kontakt-, Diagnose-, Serien- und Beispielprüfungen aus. Der neue Stoß-Lernpfad
läuft jeweils mit installiertem und neu gebautem Core sowie ein drittes Mal
über genau den dokumentierten nativen C-Projektbuild. Elf analytische Szenarien
und alle 44 Experiment-/Analysepfade werden bei jedem Durchgang geprüft.
Die endgültigen SDKs übernehmen die aktuellen Nachweise und erneuern deren
Manifestwerte; Code-, Beispiel- und App-Binärdateien bleiben bytegleich zur
verifizierten Installation und zur geprüften Release-App.

Der Katalog umfasst 559 Prüfungen ohne Fenster (544 ohne SDL), 64 Fensterfälle
und 18 eigenständige Sprachprogramme plus 40 Experiment-/Analysemodule. Beide
Release-Builds mit `--examples` erstellen sämtliche Programme und Module.
Ein Gesamtlauf aller aktuellen 559/64 Fälle wird für diese Sitzung nicht
behauptet; der frühere Lauf aller 555 Fälle ist weiter unten belegt.
Dieser Lernpfad erfüllt keine anderen offenen Anforderungen des Projektplans
und wird nicht als vollständige Produktabnahme ausgegeben.

## Pendel-Lernpfad in beiden Sprachen am 7. Oktober 2026

Vier neue vollständige Quellen liefern dasselbe konservative nichtlineare
Pendel und eine gemeinsame Mehrlaufanalyse in C und Physim. Die typisierten
Parameter `length`, `initialAngle` und `integrator` gehören zur Modellinstanz.
Fünf Verfahren stehen zur Wahl: explizites Euler, symplektisches Euler, RK4,
Velocity Verlet und Dormand–Prince 5(4). Tatsächlich adaptive Aufzeichnung
verlangt ausdrücklich Verfahren 4. Modell, SI-Kanäle, Grenzen und Auswahl werden
im Laufmanifest gespeichert. Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate
bleiben unverändert.

Der unabhängige Prüfer kontrolliert 40.010 primäre Messzeilen der fünf Verfahren
in beiden Sprachen samt allen sechs Kanalnamen und SI-Dimensionen sowie sämtliche
Felder der Szenen mit sechs geometrischen Einträgen und zwei Gruppen. Die
nichtlineare Periode wird durch Simpsonquadratur des elliptischen Integrals
berechnet. Eine rekursive Taylorentwicklung der Bewegung bei t=0,5 s prüft die
Ordnungen 1, 1, 4 und 2 beim Halbieren der Schrittweite; sie verwendet keine
RK-Stufen aus dem Produkt. Veränderte Längen, positive/negative Auslenkung,
Ruhezustand und zwei adaptive Läufe werden ebenfalls geprüft. Der
Instanzprüfer kontrolliert unabhängige Parameter/Verfahren, Einheiten, Reset,
fehlgeschlagene Schritte und dass eine fehlerhafte Sprachinstanz andere Instanzen
nicht beeinflusst. Beim Sprachadapter beendet ein Laufzeitfehler die betroffene
Instanz bis Reset; der Host beendet diesen Lauf. Messwerte werden vorher geprüft
und erst bei erfolgreichem Abschluss veröffentlicht.

Die Analysen verbinden einen bis acht gespeicherte Läufe auf deren eigenen
Zeitachsen. Sie erzeugen zwei Plots mit Winkel und Energieabweichung, zwei
Tabellen mit Laufkennzahlen und tatsächlich gemessenen Perioden sowie
vollständiges CSV pro Eingabe. Perioden beruhen auf linear interpolierten
positiven Nulldurchgängen; kurze oder ruhende Läufe erhalten keine erfundenen
Periodenwerte. Ein separater C-Prüfer vergleicht acht gemischte Mehrlaufberichte
und zwei Einzellaufberichte mit den Originaldaten, einschließlich Plotmetadaten,
Quellenzahl, vollständigen Kurven unterhalb der Vorschaugrenze und aller
Tabellenwerte. Der Pythonprüfer prüft sämtliche CSV-Zeilen, auch bei reduziertem
Berichtsplot. Die vier dokumentierten Codeblöcke entsprechen exakt den gebauten
Quellen. [Lernziel, Modell und vollständiger Ablauf](pendulum-tutorial.md).

macOS Release besteht sechs ausgewählte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-f_cdfqro`, Linux GCC
Release dieselben sechs unter
`build/contact-world-language-release-linux/test-results/run-5jvxw5yz`:
neuer Lernpfad, Quellcode-/Referenzdokumentation sowie die bisherigen drei
Pendelreferenzen für RK4, RK45 und Verlet. Linux Debug mit AddressSanitizer und
UndefinedBehaviorSanitizer besteht den gesamten neuen Lernpfad einschließlich
beider Module und Instanzprobe unter
`build/spring-tutorial-asan-linux/test-results/run-0h7jbom1`.

Die zusätzlichen Grenzprüfungen mit genau acht ausgewählten Läufen und einer
abgewiesenen leeren Auswahl bestehen auf macOS zusammen mit Quellcode- und
Referenzprüfung 3/3 unter
`build/contact-world-language-release-mac/test-results/run-wp5pe_w2` und unter
Linux ASan+UBSan im erweiterten Lernpfad unter
`build/spring-tutorial-asan-linux/test-results/run-w8c9qcv3`.

macOS besteht die drei ausgewählten Fensterabläufe unter
`build/contact-world-language-release-mac/test-results/run-hp6wgi1q`, Linux unter
`build/contact-world-language-release-linux/test-results/run-vx1_cjf7`.
Die neue Prüfung erzeugt beide App-Projektsprachen aus Vorlagen, übernimmt die
getesteten Tutorialquellen, baut, wählt Verlet, führt 40 angehaltene Einzelschritte
aus und öffnet die 41 gespeicherten Messzeilen mit Szene und Bericht erneut.
Zusätzlich bestehen der bestehende adaptive Fensterablauf und die gesamte
Offline-Dokumentnavigation einschließlich des neuen Lernpfads. Die macOS-Fenster
wurden nativ ausgeführt; Linux nutzte X11, Xvfb, Openbox und Mesa. Die Plattform-
und Compiler-Versionen entsprechen den folgenden Intel-/Debian-12-Nachweisen.
Screenshots von Pendel, Beschriftungen und Energieanalyse wurden visuell geprüft.
Der kleine Bildschirm erfordert weiterhin Scrollen zu den unteren Diagrammteilen.

Beide SDKs wurden neu installiert, in Ordner mit Leerzeichen/Umlauten kopiert
und unter einem anderen absoluten Pfad vollständig geprüft. macOS-Nachweis:
`build/pendulum-tutorial-sdk-final-mac/Native SDK ä it4e0vz5`; Linux-Nachweis:
`build/pendulum-tutorial-sdk-final-linux/Native SDK ä 5lzt4zzx`. Der Gesamtprüfer
kontrolliert alle Manifestdateien und eigenständigen öffentlichen Header, baut
Core und sämtliche Sprachmodule aus den mitgelieferten Quellen neu und prüft
die vorhandenen Archiv-, Kontakt-, Diagnose-, Serien- und Beispielproben.
Für den neuen Lernpfad laufen fünf Integratoren und acht gemischte
Mehrlaufanalysen jeweils gegen installiertes und neu gebautes Core. Außerdem
führt der Prüfer genau den dokumentierten nativen C-Projektbuild mit
`physim-build` aus und prüft dessen Experiment-/Analysemodul erneut mit denselben
unabhängigen Oracles. Die App wurde aus derselben Release-Binärdatei geprüft;
Code-/Beispieldateien der endgültigen Pakete werden bytegleich zur verifizierten
Installation erhalten. Nur die aktuellen Dokumentationsnachweise und ihre
SHA-256-Manifestwerte werden danach aufgefrischt.

Der Katalog umfasst jetzt 557 Prüfungen ohne Fenster (542 ohne SDL), 63
Fensterfälle und 18 eigenständige Sprachprogramme plus 38 Experiment-/
Analysemodule. Beide Release-Builds mit `--examples` erstellen diese Programme
und Module. Diese Sitzung behauptet keinen Gesamtlauf aller 557/63 Fälle;
der vorherige Gesamtlauf aller 555 Fälle ist im folgenden Abschnitt belegt.
Der Projektplan bleibt offen; dieser Lernpfad schließt keine anderen
Roadmap-Anforderungen automatisch ab.

## Korrekturen des Prüfberichts am 6. Oktober 2026

Die Befunde CR-001 bis CR-007 aus dem externen Prüfbericht wurden am lokalen
Stand nachvollzogen und korrigiert. Die archivierte alte Bibliothek scheitert
an der neuen Sekantenregression. Der konkrete Materialfall mit Dichte 10000,
Fluiddichte `1e-20`, Radius 1 und Viskosität `1e-8` ergibt bei t=0,001 s in beiden
korrigierten Sprachen `reference.y=0.499995096675` und
`reference.velocity=-0.009806649999999978`. Die alte Physim-Quelle ergibt dort
`25141055.324947417` m; die alte C-Quelle `0.49999480194206297` m.

| Befund | Korrektur und Nachweis |
| --- | --- |
| CR-001 | Editor und Autosave erzeugen eindeutige Geschwisterdateien exklusiv. Vorhandene `.tmp`-Dateien, Verzeichnisse, Symlinks und Hardlinks bleiben erhalten. Fehlgeschlagene Veröffentlichung entfernt nur eigene Zwischenfiles. Der App-Selbsttest ruft den tatsächlichen Hauptquellen-Speicherpfad auf und prüft das fremde Symlinkziel. |
| CR-002 | Auf POSIX starten Zwischenfiles mit `0600`; Quelle und Backup übernehmen gewöhnliche rwx-Bits. Tests erhalten `0600`, `0640`, `0700`, `0755`, einschließlich Ausführbarkeit. ACLs, xattrs und besondere Modusbits werden nicht übernommen; Windows-Rechte wurden nicht geprüft. |
| CR-003 | Zwei Taylorfaktoren bis Ordnung acht vermeiden Auslöschung für `λt≤0,1`. 18 vollständige C-/Physim-Läufe mit logarithmischen Viskositäten, Nullfall und Übergangsbereich werden gegen ein unabhängiges Decimal-Oracle mit 80 Stellen geprüft. |
| CR-004 | Konstante Kurven liegen in der Mitte der Zeichenfläche; skalierte Brüche vermeiden überlaufende Achsenspannen. Sieben Konstanten einschließlich `±DBL_MAX` exportieren endliche SVG-Punkte. |
| CR-005 | Energie startet beim ersten gültigen Messwert; nur gültige Messungen tragen zur Drift bei. Lücken unterbrechen Periodenintervalle. Tests decken Status 0/2 am Anfang, in der Mitte und am Ende sowie Nulldurchgänge ab. Fehlende Kennzahlen sind im Manifest `nan`. |
| CR-006 | Nichtendliche Statistikakkumulatoren führen vor dem Export zu `PS_NUMERIC`. Die bestehende `ps_statistics`-ABI bleibt unverändert; ihr unskaliertes `m2` begrenzt den Wertebereich. `±1` und `±1e150` stimmen mit der unabhängigen Streuung überein, `±1e200` wird kontrolliert abgewiesen. |
| CR-007 | Skalierte Differenzen und Produkte liefern Sekante 1 für `x=y={-1e308,1e308}` und Trapezintegral `1e108`. Nichtdarstellbare Ableitungen lassen die ganze Ausgabe unverändert; Eingabe-/Ausgabealiase werden unterstützt. |

Beide Release-Gesamtläufe bestehen den vollständigen aktuellen Katalog ohne
Fenster 555/555: macOS unter
`build/contact-world-language-release-mac/test-results/run-y6el5jvy`, Linux unter
`build/contact-world-language-release-linux/test-results/run-7e671h5x`.
Die C-/Physim-Module, Prozessabläufe, nativen Projektbuilds, Messarchive und
vorhandenen Regressionen sind damit vollständig im aktuellen Katalog geprüft.
Die 62 Fensterfälle wurden hier nur im beschriebenen Ausschnitt ausgeführt.

macOS Release besteht sieben gezielte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-wramilpq`:
Core, neue Analysegrenzen, Autosave, Textdokumente, Projektdateien,
Materialtutorial und dessen dokumentierte Quellen. Die App besteht die beiden
Abläufe Autosave und Physim-Feder einschließlich der neuen Prüfung des echten
Editor-Speicherpfads unter
`build/contact-world-language-release-mac/test-results/run-fgh4df4a`.

Zusätzlich bestehen auf macOS Dokumentwiederherstellung und Dokumentneubau
unter `build/review-fixes-gui-mac/documents_recovery` beziehungsweise
`build/review-fixes-gui-mac/documents_build`, jeweils mit `app-steps.json`.
Unter Linux bestehen alle vier App-Abläufe (Autosave, Physim-Feder,
Dokumentwiederherstellung und Dokumentneubau) unter
`build/review-fixes-gui-linux/<Fall>`; der Sammellog ist
`build/review-fixes-gui-linux.log`. Die Linux-Fenster liefen tatsächlich unter
X11, Xvfb und Openbox mit Mesa; die macOS-Fenster im nativen Desktop.

Die abschließenden zusätzlichen Kollisionszusicherungen prüfen auch die
Identität beziehungsweise den Inhalt des vorbestehenden temporären Pfades.
Autosave, Textdokumente und Analysegrenzen bestehen erneut 3/3 auf macOS unter
`build/contact-world-language-release-mac/test-results/run-8vcz30cv` und Linux
unter `build/contact-world-language-release-linux/test-results/run-3wpn5bu8`.
Der tatsächliche Editorpfad mit erhaltener Symlinkidentität besteht erneut im
Physim-Federablauf unter `build/review-final-gui-mac` und
`build/review-final-gui-linux`, jeweils mit `app-steps.json`. Eine getrennte
macOS-Probe gegen den aktuellen Core prüft die vollständige Ausgabeerhaltung
auch nach bereits berechneten gültigen Ableitungen unter
`build/review-final-analysis-mac`.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
Core, Analysegrenzen, Messmasken und Materialtutorial 4/4 unter
`build/spring-tutorial-asan-linux/test-results/run-n_2va9lb`.
Die Compiler-, Betriebssystem- und SDL-Versionen entsprechen den folgenden
macOS-Intel- und Debian-12-Nachweisen. Core-API/ABI, Sprachvertrag und Dateiformate
bleiben unverändert. Diese Nachprüfung ist keine Fehlerfreiheitszusage für den
übrigen Code und keine vollständige Abnahme des Projektplans.

## Feder-Lernpfad in beiden Sprachen am 6. Oktober 2026

C- und Physim-Federvorlagen definieren die Dämpfung jetzt als typisierten
Experimentparameter `damping` in N s/m, Bereich 0..64. Wert und Metadaten
gehören zum Modellkontext; mehrere Instanzen teilen keinen veränderlichen
Parameter. Der C-Makrostandard bleibt für vorhandene Referenzvarianten erhalten.
Physim prüft positive Schrittweiten ausdrücklich. Die C-Punktmarkierung speichert
nun denselben zweiten Punktwert wie die Sprachbindung. Modell, RK4-Verfahren,
Kanäle und Standards sind erhalten; öffentliche Core-API, ABI 3, Sprachvertrag
0.177.0 und alle Dateiformate bleiben unverändert.

Der vollständige Lernpfad enthält beide Experiment- und Analysequellen sowie
Ablauf, Gleichungen, vier Dämpfungsfälle, Einheiten, Ergebnisse und Modellgrenzen.
Die Analysen erzeugen drei Plots mit fünf vollständigen Kurven, eine Tabelle
mit Zeilenzahl und maximaler absoluter Bilanzabweichung und vollständiges CSV.
Ein unabhängiger Pythonprüfer vergleicht 16.008 Messzeilen, alle elf Kanäle mit
SI-Dimensionen, komplette Szenen mit 13 Objekten und 65 Helixpunkten sowie
Parameterprovenienz. Er prüft die geschlossenen Lösungen für Dämpfung 0, 1,2, 8
und 12 und eine RK4-Verfeinerung zwischen Faktor 14 und 18. Sechzehn gemischte
Analysewege werden durch einen getrennten C-Prüfer kanalweise samt Einheiten und
exakter Tabellenstatistik mit den Rohdaten verglichen. Die Instanzprobe löst zwei
verschiedene Dämpfungen im selben geladenen Modul, prüft Reset und Fehlererhaltung.
Alle vier dokumentierten Codeblöcke entsprechen den tatsächlich gebauten Quellen.

macOS Release besteht neue Tutorial-, Quellcode- und Referenzprüfung sowie die
bisherigen Federreferenzen und den Sprachlebenszyklus 5/5 unter
`build/contact-world-language-release-mac/test-results/run-b1ltqy4u`.
Linux Release besteht dieselben fünf Prüfungen unter
`build/contact-world-language-release-linux/test-results/run-gl65_h83`.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
Federreferenz, Sprachlebenszyklus und vollständigen neuen Tutorialablauf 3/3 unter
`build/spring-tutorial-asan-linux/test-results/run-otgywszl`.
macOS Release besteht neues C-/Physim-Tutorial, bisherige Sprach-Federvorlage
und Handbuchfenster 3/3 unter
`build/contact-world-language-release-mac/test-results/run-_ow7csno`.
Die neuen Projekte verwenden tatsächlich Parameterwert 8, 40 kontrollierte
Schritte, 41 gespeicherte Messungen/Szenen und eine in der App wieder geöffnete
Energieanalyse. Die Physim-Energieansicht wurde auf macOS und Linux visuell geprüft.

Linux Release besteht dieselben drei Fensterprüfungen unter
`build/contact-world-language-release-linux/test-results/run-uz1gqxof` mit
X11/Mesa, Xvfb und Openbox. Die Paket-Apps sind bytegleich mit den zuvor
geprüften GUI-Apps. Beide ersten SDK-Manifeste enthalten 330 geprüfte Dateien,
darunter 134 Code-/Binärdateien.

Die vollständige verschobene SDK-Prüfung besteht auf macOS unter
`build/spring-tutorial-sdk-proof-mac/Native SDK ä uro52_vz`. Sie kompiliert alle
öffentlichen Header und 21 Core-Einheiten, prüft 18 eigenständige Sprachprogramme
und 36 Module und führt den neuen Federlernweg gegen ausgelieferte und neu
gebaute Bibliotheken aus. Zusätzlich bestehen die bisherigen Spezialproben,
Material-/Medium-Lernpfade, adaptive Studien und neun eigenständige Projektneubauten.
Diese SDK-Prüfung enthält keine zusätzliche GUI-Abnahme.

Linux besteht die vollständige SDK-Prüfung mit demselben Umfang unter
`build/spring-tutorial-sdk-proof-linux/Native SDK ä mjxnz2wi`.
Die finalen Pakete `build/Spring tutorial clean SDK ä mac` und
`build/Spring tutorial clean SDK ä linux` übernehmen die geprüften SDKs und
aktualisieren die Dokumentation. Ihre SHA-256-Manifeste wurden für alle 330
Dateien geprüft. Alle 134 Dateien unter `include`, `src`, `lib` und `bin` sowie
sämtliche Beispielquellen stimmen mit dem jeweiligen vollständig geprüften
verschobenen SDK überein. Die Apps entsprechen bytegenau den geprüften GUI-Apps.
Seit den erfolgreichen Rechen-, Fenster- und SDK-Prüfungen wurde kein
Produktionscode verändert.

Der Katalog umfasst 554 Prüfungen ohne Fenster (539 ohne SDL) und 62 Fensterfälle.
Der Beispiel-/SDK-Katalog umfasst 18 eigenständige Sprachprogramme und 36 Module.
Diese Änderung verwendet gezielte Regressionen und SDK-Prüfungen; ein neuer
Gesamtlauf aller 554 beziehungsweise 62 Fälle ist nicht behauptet. Die frühere
550/550-Abnahme bleibt für ihren benannten Stand erhalten. Dieser Lernpfad
schließt weiterhin weder steife/nichtlineare Federn noch allgemeine Führungs-
Constraints oder weitere offene Produktziele ab.

## Lernpfad eigenes Material und Medium am 6. Oktober 2026

Der achte geforderte Lernpfad liefert vollständige C-/Physim-Experimente und
Auswertungen. Ein benutzerdefiniertes Material bestimmt die Kugelmasse über
Dichte und Volumen; benutzerdefinierte Fluiddichte und Viskosität bestimmen
Auftrieb und Stokes-Widerstand. RK4 integriert Ort, Geschwindigkeit und
Dissipation unabhängig von den analytischen Kontrollkanälen. Eigene,
konservative Grenzen Re<=0,1 und lambda*dt<=0,25 werden durchgesetzt.
Quelle, Parameter, Kräfte, Integrator und ausgeschlossene hydrodynamische
Effekte sind dokumentiert und im Laufmanifest gespeichert. ABI, öffentliche
Core-API, Sprachvertrag 0.177.0 und alle Dateiformate bleiben unverändert.

Der unabhängige Pythonprüfer validiert CRC, alle 1001 Messzeilen und sämtliche
Szenenfelder für sieben Konfigurationen in beiden Sprachen: Standard, Aufstieg,
Dichtegleichheit, Viskosität, Radius, Vakuum und halbierter Zeitschritt.
Masse, Kräfte, analytische Lösung und Energiebilanz werden unabhängig gerechnet.
Die Halbierung verlangt mindestens Faktor 14 weniger Geschwindigkeitsfehler.
Alle vier Experiment-/Analysesprache-Kombinationen erzeugen drei Plots mit
sieben vollständigen Kurven und CSV; ein separater C-Prüfer vergleicht jeden
Kurvenwert und seine SI-Dimension mit den ursprünglichen Messungen.
Ungültige Reynolds-Zahl, Viskosität und Schrittweite werden in beiden Sprachen
abgewiesen. Die Codeblöcke des Handbuchs werden gegen alle vier Quellen geprüft.

macOS Release besteht zunächst den neuen Rechenablauf 1/1 unter
`build/material-tutorial-release-mac/test-results/run-afa49mvr`.
Die abschließende Paketprüfung besteht Materiallernweg, Quellcodeabgleich,
C-Dokumentmodell und generierte Funktionsreferenz 4/4 unter
`build/contact-world-language-release-mac/test-results/run-cd4q13l8`.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
den vollständigen neuen Referenzablauf 1/1 unter
`build/material-tutorial-asan-linux/test-results/run-yiurqtb1`.
macOS Release besteht Handbuchfenster und neuen C-/Physim-Fensterablauf 2/2
unter `build/material-tutorial-release-mac/test-results/run-gpwxchvj`.
Linux Release besteht dieselben Fensterprüfungen 2/2 mit X11/Mesa, Xvfb und
Openbox unter `build/material-tutorial-release-linux/test-results/run-het1psio`.
Beide Sprachprojekte werden tatsächlich angelegt, gebaut, 40 Schritte ausgeführt,
41 Messzeilen und Szenen wieder geöffnet sowie der gespeicherte Analysebericht
in der App geladen. Die Physim-Auswertung wurde auf macOS und Linux visuell geprüft.
Der erste Fensteraufbau markierte die ersetzte Analysequelle nicht als geändert
und baute deshalb die Vorlagenanalyse; die korrigierte Probe markiert beide
Editoren und überprüft das tatsächliche Tutorialergebnis.

Linux Release besteht die abschließende gezielte Paketprüfung 4/4 unter
`build/contact-world-language-release-linux/test-results/run-4y9miqrn`.
Die ersten SDK-Manifeste enthalten jeweils 327 Dateien und 133 Code-/Binärdateien.
Da Apps aus unterschiedlichen Buildordnern verschiedene Binärhashes hatten,
wurden die ausgelieferten Programme direkt geprüft: jeweils vollständiger
C-/Physim-Materialablauf unter `build/material-tutorial-installed-gui-mac`
beziehungsweise `build/material-tutorial-installed-gui-linux`. Beide bestehen.

Die erste erweiterte SDK-Prüfung bestand den neuen Materialablauf, scheiterte
aber danach an einem überschriebenen lokalen Prüferpfad: die Tutorialprobe
ersetzte den allgemeinen Analyseprüfer. Ein eigener Variablenname erhält
nun beide Prüfer; der SDK-Gesamtaufruf wurde vollständig wiederholt.
macOS besteht diese Wiederholung unter
`build/material-tutorial-sdk-final-proof-mac/Native SDK ä rj4l769l`.
Sie prüft alle Header, 21 Core-Einheiten, 18 Sprachprogramme, 35 Module,
installierte und neu gebaute C-/Physim-Materialversuche, alle vier Analysesprachen-
Kombinationen sowie die bisherigen Spezialproben, adaptiven Studien und neun
unabhängigen Projektneubauten. Die SDK-Prüfung selbst enthält keine GUI-Abnahme.

Linux besteht den vollständig wiederholten SDK-Aufruf unter
`build/material-tutorial-sdk-final-proof-linux/Native SDK ä s6j__27_` mit
identischem Umfang. Der abschließend verschärfte Messdateiprüfer verlangt
Namen und SI-Dimensionen aller 13 Kanäle; macOS besteht damit erneut 4/4 unter
`build/contact-world-language-release-mac/test-results/run-jvgg_51u`.

Linux besteht dieselben vier abschließenden Prüfungen unter
`build/contact-world-language-release-linux/test-results/run-epardf8l`.
Die finalen Pakete `build/Material tutorial clean SDK ä mac` und
`build/Material tutorial clean SDK ä linux` übernehmen die geprüften SDKs
und aktualisieren die Dokumentation. Ihre Manifeste enthalten jeweils 327
SHA-256-geprüfte Dateien. Alle 133 Dateien unter `include`, `src`, `lib` und
`bin` sowie alle Experiment-/Analysequellen stimmen bytegenau mit dem jeweiligen
vollständig geprüften verschobenen SDK überein. Die App entspricht der direkt
geprüften installierten App; der Produktionscode blieb unverändert.

Der Katalog wächst auf 552 Fälle ohne Fenster (537 ohne SDL) und 61 Fensterfälle.
Diese Änderung nutzt gezielte Prüfungen und die vollständigen SDK-Prüfungen;
ein neuer Gesamtaufruf aller 552 beziehungsweise 61 Fälle ist nicht behauptet.
Die vorausgehenden 550/550-Gesamtprüfungen bleiben für ihren benannten Stand
erhalten. Materialkennwerte sind Demonstrationsdaten, keine gemessene
Stoffdatenbank; der Lernweg übernimmt keine vollständige Flüssigkeitsdynamik.

## Physim-Kontaktzustände am 6. Oktober 2026

Sprachvertrag 0.177.0 bindet die öffentliche persistente Kontaktwelt und den
manuellen Warm-Kontaktgraphen an denselben C-Core. Collider sind kleine Werte;
Kontaktzustände besitzen unveränderliche Snapshots im gemeinsamen 64-MiB-Budget.
`solve()` und `reset()` erzeugen neue Werte und ändern weder Receiver noch
Eingabekörper. Arrays, optionale Werte, Strukturfelder, Rückgaben und Closures
behalten unabhängig gespeicherte Zustände und geben sie automatisch frei.
API/ABI 3 und alle Mess-/Snapshot-/Wireformate bleiben unverändert.

Die abschließende gezielte macOS-Debug-Prüfung besteht 19/19 unter
`build/contact-world-language-debug-mac/test-results/run-jnqomjeg`.
Sie enthält analytische Warm-/Kaltreste, Zeitschrittskalierung, Kontaktgeometrie,
ID-Grenzen, Kopien/Closures und 2000 Ersetzungszyklen mit tatsächlicher Kontrolle
des verbleibenden Sprachspeichers. Negative Compilerfälle prüfen reservierte
Namen und Arrayelementtypen. Runtimefehler prüfen Collidergrenzen, Formen,
Normalen, Weltsettings, doppelte IDs, dynamische Ebenen, Schrittweiten, Zugriffe
und Seedanzahlen, jeweils mit Freigabe bestehender Besitzer. Ein eigener
Allocator erzwingt alle Allokationsfehler von Konstruktor, Solve, Reset,
Körperarray und Warmgraph; Eingabesnapshots bleiben bytegleich und alle Besitzer
werden anschließend vollständig freigegeben.
Der erste gezielte Lauf scheiterte ausschließlich beim Testinstrumentieren von
POD-Programmen ohne Sprachspeicher. Die betroffenen Fehlerproben besitzen nun
zusätzlich einen echten Weltzustand und prüfen auch dessen Freigabe.

Ein unabhängiger CRC-/Wireparser vergleicht C und Physim in warmem und kaltem
Modus über je 1001 vollständige Messzeilen und sämtliche aufgezeichneten
Objektfelder mit 1e-12 relativer/absoluter Toleranz. Die C-Punktmarker verwenden
nun denselben zweiten Positionswert wie die Sprachbindung. Die Quaternion-
normalisierung der Sprach-Szenenkonstruktoren kann letzte Bits verändern;
Formen, Farben, IDs, Texte und Strukturfelder werden exakt verglichen.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
alle 19 gezielten Prüfungen unter
`build/contact-world-language-asan-linux/test-results/run-f1wai2ee`.
macOS Debug besteht die C- und Physim-Stapelfenster 2/2 unter
`build/contact-world-language-debug-mac/test-results/run-0zkbhpx3`.
Die App erstellt echte Projekte, baut sie, führt 40 kontrollierte Schritte aus
und öffnet 41 aufgezeichnete Messzeilen mit Szenen wieder. Die Physim-
Stapelansicht wurde visuell geprüft.

Die ersten Release-Gesamtläufe bestanden auf beiden Plattformen 549/550;
allein der CLI-Korpus traf zwei alte
Erwartungen an die Sprachversionsausgabe 0.176.0. Diese ersten Läufe liegen unter
`build/contact-world-language-release-mac/test-results/run-w5mvh34p` und in der VM
`build/contact-world-language-release-linux/test-results/run-cp09edzv`. Der Compiler meldete bereits
korrekt 0.177.0. Diese Erwartungen wurden aktualisiert; der Produktionscode
blieb unverändert. Erst die anschließenden Gesamtprüfungen belegen den
abschließenden Katalog mit 550 Fällen ohne Fenster und 60 Fensterfällen.

macOS Release besteht die abschließende Gesamtprüfung 550/550 unter
`build/contact-world-language-release-mac/test-results/run-z18rl376` mit
Apple Clang 16 auf Intel macOS 14.6.1 und SDL 3.2.30.
Linux Release besteht die abschließende Gesamtprüfung 550/550 unter
`build/contact-world-language-release-linux/test-results/run-4crv23h0`.
Der abschließende Katalog enthält 18 eigenständige Sprachprogramme und 33 Module.
Dieselbe Release-App besteht C-/Physim-Stapel und den bisherigen Kontaktablauf
3/3 mit X11/Mesa, Xvfb und Openbox unter
`build/contact-world-language-release-linux/test-results/run-barcukyy`.
Die Physim-Stapelansicht wurde auch auf Linux visuell geprüft; dies ersetzt
keine Gesamtprüfung sämtlicher 60 Fensterfälle.
macOS Release besteht dieselben drei Fensterabläufe 3/3 unter
`build/contact-world-language-release-mac/test-results/run-511vesok`.
Die SHA-256-Manifeste der ersten Pakete enthalten auf beiden Plattformen
320 geprüfte Dateien, darunter 131 Code-/Binärdateien.

Die vollständige verschobene SDK-Prüfung besteht auf macOS unter
`build/contact-world-language-sdk-proof-mac/Native SDK ä paqeof_i` und auf
Debian unter `build/contact-world-language-sdk-proof-linux/Native SDK ä _63wzqhx`.
Beide prüfen unabhängig alle Header, 21 Core-Quellen, 18 Sprachprogramme,
33 Sprachmodule, installierte und neu gebaute Kontaktwelten mit Allokations-
fehlern sowie die vollständigen warmen/kalten C-/Physim-Stapelvergleiche.
Bestehende Logging-/Frame-/Diagnostik-/Indexproben, gemeinsame Analysen,
adaptive Studien und neun eigenständige native Projektneubauten bestehen.
Diese SDK-Prüfungen enthalten keine zusätzliche GUI-Abnahme.

Die abschließenden Pakete `build/Contact Language clean SDK ä mac` und
`build/Contact Language clean SDK ä linux` enthalten die finalen Nachweise.
Ihre SHA-256-Manifeste wurden für alle 320 Dateien geprüft; alle 131 Dateien
unter `include`, `src`, `lib` und `bin` stimmen mit dem jeweils vollständig
geprüften verschobenen SDK überein. Die Apps entsprechen den geprüften
Release-Programmen. Der Produktionscode blieb seit den Gesamtprüfungen
unverändert; die finalen Pakete aktualisieren nur Dokumentation.

Kontaktverwaltung und Warmstart bleiben diskret; Feature-IDs, Kontaktinseln,
Gelenk-Warmstart und nichtlineare Rotationsprojektion sind weiter offen.

## Persistente Kontaktverwaltung am 6. Oktober 2026

Der neue caller-eigene C-Kontaktzustand regeneriert diskrete Kugel-/Box-/
Ebenenkontakte durch die bestehenden Broad-/Narrow-Phase-Funktionen. Stabile
Collider-IDs, beide Körperlokalanker und ein Normalenvergleich ordnen Kontakte
eins zu eins zwischen erfolgreichen Aufrufen zu. Masse-/Trägheits-/Formänderungen
verwerfen die Zuordnung; fehlende Kontakte laufen sofort aus. Startimpulse werden
mit dem Zeitschrittverhältnis skaliert und auf den aktuellen Coulomb-Kegel
projiziert. Restitutionsziele werden vor sämtlichen Warmimpulsen vorbereitet.
Körper, Cache und Ergebnis ändern sich bei Fehlern nicht. Es gibt keine eigenen
Heapallokationen oder implizite Integration. Alte Solver bleiben kalt; ABI 3,
Physim 0.176.0 und alle Daten-/Wireformate bleiben erhalten.

macOS Debug besteht die abschließenden Core-, Kontaktgraph-, gemischten Graph-
und Runnerprüfungen 4/4 unter
`build/contact-world-debug-mac/test-results/run-i_b62wvv`. Ein analytischer
Vier-Körper-Stapel erreicht mit gespeicherten Impulsen bereits nach einer
Geschwindigkeitsiteration einen Rest unter 1e-12, während der kalte Lauf einen
Rest über .04 behält. Timestep-Skalierung, Umordnung von Körper-/Colliderarrays,
Kontaktablauf, geänderte Masse/Trägheit, rotierte Ebenen, alle unterstützten
Formpaare, aktuelle Reibung, Restitution vor Warmstart sowie ungültige Seeds,
Kapazitäts- und numerische Überläufe werden ausdrücklich geprüft.

Der unabhängige Runnerprüfer liest jeweils 1001 Messzeilen eines warmen und
kalten Boxenstapels und CRC-validierte aufgezeichnete Szenen. Kontaktzähler,
Höhen, SI-Metadaten, Abschlussindex und geringerer mittlerer Normalenrest des
warmen Laufs werden geprüft. Die Debug-App-Prüfung besteht 1/1 unter
`build/contact-world-debug-mac/test-results/run-99qjwffa`: neues Projekt bauen,
40 kontrollierte Schritte, aktive Cachezähler und vollständige Wiederöffnung
der 41 Messzeilen/Szenen. Der Screenshot der Körper und Kontaktnormalen wurde
visuell geprüft; die Körperfarben wurden anschließend zur Unterscheidung variiert.

Für weitere Prüfungen wurde die eigene Debian-Testplatte von 16 auf 32 GiB
erweitert. Das vorherige Disk-Image wurde unter
`build/linux-vm/debian-dialogs-before-contact-world.qcow2` erhalten; Debian
erweiterte die bestehende Ext4-Rootpartition automatisch. Betriebssystem und
Compiler bleiben Debian 12/GCC 12.2.0 mit Kernel 6.1.0-53-cloud-amd64.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
6/6 unter `build/contact-world-asan-linux/test-results/run-yni30whx`: neuer
Kontaktzustand und Runner, bisherige Kontakt-/Gelenkgraphen, Boxkontakte und
Broad Phase. Die tatsächlichen warmen/kalten Simulationen laufen ebenfalls
mit instrumentiertem Runner und Modul.
Linux Release besteht die vollständige Prüfung 533/533 unter
`build/contact-world-release-linux/test-results/run-9b3v74yv` in der VM.
Der Lauf baut alle 17 eigenständigen Sprachprogramme und 32 Module sowie
den nativen Projektbuilder mit nun 21 Core-Einheiten.
macOS Release besteht ebenfalls 533/533 unter
`build/contact-world-release-mac/test-results/run-n2idnt61` mit Apple Clang 16,
Intel macOS 14.6.1 und SDL 3.2.30. Beide Gesamtprüfungen verwenden den
abschließenden unveränderten Produktionscode.
Debian Release mit X11/Mesa, Xvfb und Openbox besteht neue Stapelansicht,
bisherigen C-/Physim-Kontaktablauf und Boxstoß 3/3 unter
`build/contact-world-release-linux/test-results/run-cywzwx6t`. Dies ist keine
vollständige Abnahme aller 59 Fensterfälle.
macOS Release besteht dieselben drei Fensterprüfungen 3/3 unter
`build/contact-world-release-mac/test-results/run-ov7_9k70`.
Die abschließende Linux-Stapelansicht mit getrennten Körperfarben wurde
visuell geprüft; gespeicherte Kontakte und Normale bleiben sichtbar.

Die vollständige verschobene SDK-Prüfung besteht auf macOS unter
`build/contact-world-sdk-proof-mac/Native SDK ä bdjem_8b` und auf Debian unter
`build/contact-world-sdk-proof-linux/Native SDK ä gi_i7c87` in der VM.
Alle öffentlichen Header werden unabhängig kompiliert; die ausgelieferte
Bibliothek und ein Neubau aus 21 Core-Einheiten bestehen die analytische
Kontaktweltprüfung und tatsächliche warme/kalte Stapelläufe mit aufgezeichneten
Szenen. Außerdem bestehen 17 eigenständige Sprachprogramme, 32 Module,
Logging-/Frame-/Diagnostik-/Indexproben, C-/Physim-Analysen, adaptive Studien
und neun unabhängige Projektneubauten. Diese SDK-Prüfungen führen keine
zusätzliche GUI-Prüfung aus.

Die abschließenden Pakete `build/Contact World clean SDK ä mac` und
`build/Contact World clean SDK ä linux` enthalten die aktualisierten Nachweise.
Ihre SHA-256-Manifeste wurden jeweils für alle 315 Dateien geprüft. Alle 128
Dateien unter `include`, `lib`, `src` und `bin` stimmen bytegenau mit dem
jeweils vollständig geprüften verschobenen SDK überein; die App entspricht
der zuvor geprüften Release-App. Die aktualisierte Dokumentation erfordert
keinen erneuten Programmneubau.

Die neue Verwaltung ist diskret, synchron und begrenzt auf 128 Körper/Collider
und 512 Kontakte. Kein CCD, Compound-/konvexe Formen, Feature-IDs/Kontaktinseln,
Gelenk-Warmstart, nichtlineare Rotationsprojektion oder direkte neue Physim-
Bindungen sind damit abgenommen. Der feste Iterationsetat garantiert keine
Konvergenz; Normalen- und Projektionsreste bleiben ausdrücklich sichtbar.

## Physim-Indexabfragen am 6. Oktober 2026

Sprachvertrag 0.176.0 ergänzt `RunIndex`, `RunBlock` und vollständige
`RunSnapshot`-Werte. Indexkopien teilen einen automatisch freigegebenen
Dateibesitzer; `close()` gibt nur die jeweilige Referenz frei. Arrays,
optionale Werte, Strukturfelder, Rückgaben und Closures behalten ihre
Besitzregeln. Blockdaten und Snapshotdaten bleiben nach dem Schließen des
Index verwendbar. Abfragen, Metadaten und SI-Dimensionen verwenden den
geprüften C-Core. `inputPath(index)` erschließt die explizite Analyseauswahl
als eigenen String. API/ABI 3 und alle Datei-/Wireformate bleiben erhalten.

Die Debug-Prüfung auf macOS besteht neue Werte, alte/recovered Dateien,
Arraykopien mit unabhängigem Schließen, optionale Werte und Strukturfelder,
einen aus der Funktion zurückgegebenen Closure sowie 2000 Aufräumschleifen.
Eine direkte C-Prüfung erzwingt jede Allokationsfehlerstelle beim Öffnen und
eine fehlgeschlagene Blockallokation und verlangt null verbleibende Besitzer.
Ein echtes Physim-Analysemodul liest die letzten 256 Zeilen eines
Million-Zeilen-Laufs; ein unabhängiger C-Prüfer vergleicht alle Zeiten/Werte
und die SI-Dimensionen des Berichts. Sechs Compilerchecks prüfen reservierte
Typnamen, Parametertypen, unveränderbare Receiver und den Analyse-Hostvertrag.
Die abschließenden beiden Debugfälle bestehen 2/2 unter
`build/run-index-language-debug-mac/test-results/run-jooqqt1l`.
Ein früher Analyseentwurf verwendete einen nicht vorhandenen Series-Konstruktor
und danach zwei voneinander unabhängige Reihen. Das Beispiel verwendet nun
`Series.fromValues` und ausdrücklich `alignedValues`, bevor es die Kurve erzeugt.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
5/5 unter `build/run-index-language-asan-linux/test-results/run-u71pu0nh`:
neue Indexwerte samt Analyse und Speicherprobes, sechs Compilerchecks,
abgefangene Modulfehler und beide bisherigen Speicherleck-Abläufe.
Die vollständigen Release-Läufe bestehen jeweils 531/531: macOS unter
`build/run-index-language-release-mac/test-results/run-a40iew2d`, Debian unter
`build/run-index-language-release-linux/test-results/run-frpk9b9o` in der VM.
Beide bauen zusätzlich 17 eigenständige Sprachprogramme und 32 Module.
Die Umgebungen sind Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2.0
mit Kernel 6.1.0-53-cloud-amd64, jeweils SDL 3.2.30.
Debian Release mit X11/Mesa, Xvfb und Openbox besteht vollständigen Sprachworkflow
und eigenständige Analyseprojekte 2/2 unter
`build/run-index-language-release-linux/test-results/run-069l_kgq`. Dies ist keine
erneute vollständige Prüfung aller 58 Fensterabläufe.
macOS Release besteht dieselben beiden App-Prüfungen 2/2 unter
`build/run-index-language-release-mac/test-results/run-0oncir9q`.
Das verschobene Linux-SDK besteht die vollständige unabhängige Prüfung unter
`build/run-index-language-sdk-proof-linux/Native SDK ä q6o953s4` in der VM.
Sie kompiliert alle ausgelieferten Header, baut die 20 Core-Quellen neu, führt
17 Sprachprogramme und 32 Module aus bzw. baut sie neu und prüft neun native
Projektbuilds. Die neuen Sprachwerte laufen mit den erzeugten Million-Zeilen-
Fixtures; Besitzer-/Fehlerprobes werden gegen beide Archive gelinkt. Das neue
Analysemodul wird tatsächlich ausgeführt; ein aus dem SDK gelinkter C-Prüfer
vergleicht alle 256 Kurvenwerte und SI-Einheiten.
Das macOS-SDK besteht dieselbe vollständige Prüfung unter
`build/run-index-language-sdk-proof-mac/Native SDK ä wj2v1yh2`, einschließlich
beider Besitzer-/Fehlerprobes und des tatsächlich ausgeführten Analysemoduls.
Die SDK-Manifeste enthalten jeweils 310 geprüfte SHA-256-Dateieinträge.
Abschließende Pakete liegen unter `build/Run Index Language clean SDK ä mac`
bzw. `build/Run Index Language clean SDK ä linux` in der VM; ihre Manifestdateien
werden erneut gehasht. Ausgelieferte Header, Archive und Core-Quellen stimmen
mit den vollständig geprüften SDKs überein, die App mit dem abschließend
geprüften Fensterprogramm. Nach dem Paketneubau unterscheiden sich unter Linux
vier Programmdateien vom ersten SDK-Proof. Das abschließend gebaute Linux-SDK
besteht deshalb die komplette Prüfung erneut unter
`build/run-index-language-clean-sdk-proof-linux/Native SDK ä kxonlhz6`.
Endgültige Pakete mit diesen Nachweistexte liegen unter
`build/Run Index Language final SDK ä mac` bzw.
`build/Run Index Language final SDK ä linux` in der VM. Ihre 310 Manifestdateien
werden erneut geprüft; sämtliche 126 Code-/Programmdateien stimmen bytegenau
mit dem jeweiligen abschließend geprüften SDK überein.

## Rekonstruierbare Laufdatei-Indizes am 6. Oktober 2026

Format 1 erhält optionale Abschlusschunks 6/7 vor dem bisherigen Footer.
Mess- und Szenenpayloads, Reader-/Writerstrukturen, API/ABI 3, Snapshot 3,
IPC 5 und Physim 0.175.0 bleiben erhalten. Writerabschluss validiert einmal
den aufgezeichneten Präfix und erzeugt begrenzte Indexseiten ohne Allokation.
Die neue besitzende C-API validiert beim Öffnen den lesbaren Präfix und
rekonstruiert Checkpoints mit einem expliziten Allocator. Gezielte Messblöcke
und Szenen werden beim Abfragen erneut geprüft; alle Fehler erhalten Ausgaben.
Ein gespeicherter Index gilt erst nach vollständigem Vergleich mit den
tatsächlich gelesenen Daten als bestätigt. Quelllaufdateien bleiben unverändert.
Der native Projektbuilder und ausgelieferte Quellen umfassen nun 20 Core-Einheiten.

Die gezielten macOS-Debug-Prüfungen von Snapshots, Runnern, Frame-Aufzeichnung,
Logging und dem neuen Index bestehen 7/7 unter
`build/run-index-debug-mac/test-results/run-5wfp3ltu`. Nach Ergänzung der
Checkpointprüfung und absichtlicher Beschädigung nach dem Öffnen bestehen
Index und alte Szenenversionen 2/2 unter `run-6fge4idn`.
Die unabhängige Pythonprüfung liest eine Million Messzeilen, 3907 Checkpoints,
16 Indexseiten und den unveränderten Footer mit korrekten CRCs und Bytepositionen.
Sie kontrolliert außerdem Indexfreiheit, Recovery, semantisch falsche
Checkpoints trotz korrekter CRC und eine unbekannte Indexversion.
Die C-Prüfung erzwingt jede Allokationsfehlerstelle und ein Budget von 600000
Bytes beim Million-Zeilen-Lauf; sämtliche Besitzer werden korrekt freigegeben.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
5/5 unter `build/run-index-asan-linux/test-results/run-rea3kiur` in der Debian-VM:
Core, Snapshots, alte Szenenversionen, C-/Physim-Frame-Runner und Index einschließlich
Million-Zeilen- und unabhängiger Codec-Prüfung.
Ein unveränderter Analyse-Runner und das Analysemodul aus dem vorherigen macOS-SDK
`build/Diagnostics clean SDK ä mac` verarbeiten die neue Million-Zeilen-Datei
erfolgreich (`build/run-index-legacy-million-proof.*`). Ein erster Versuch mit
der absichtlich nichtmonotonen Prüffixture bestätigt zwar 1000 lesbare Zeilen,
wird aber erwartbar von deren strenger Analyse-Zeitprüfung abgewiesen.

Die ersten Release-Gesamtläufe enden auf beiden Plattformen 528/529:
`build/run-index-release-mac/test-results/run-4ms85qtl` und in der VM
`build/run-index-release-linux/test-results/run-_jy4js73`. Der bestehende
Mutationsprüfer erwartete noch sechs Chunks; Indexseite und Indexkopf ergeben
nun acht. Nach Korrektur und Erweiterung um die Index-API bestehen dessen
8660 Varianten unter macOS Debug (`run-f1vlwacq`) und zusätzlich unter Linux
ASan/UBSan (`build/run-index-asan-linux/test-results/run-o5qx3yw0`).
Der anschließende Linux-Release-Gesamtlauf besteht 529/529 unter
`build/run-index-release-linux/test-results/run-a0u2wsxq`. Er baut auch alle
16 eigenständigen Sprachprogramme und 31 Experiment-/Analysemodule.
Linux Release mit X11/Mesa, Xvfb und Openbox besteht vollständigen Physim-Ablauf
und C-/Physim-Zeitleiste 2/2 unter
`build/run-index-release-linux/test-results/run-3bqae85o`. Die Abläufe öffnen
aufgezeichnete Szenen erneut und kontrollieren Zeiten, Werte, Auswahl und
unveränderte Rohdateien. Es ist keine vollständige Abnahme aller 58 Fensterfälle.
macOS Release besteht ebenfalls 529/529 unter
`build/run-index-release-mac/test-results/run-sqfa86sl` mit Apple Clang 16,
Intel macOS 14.6.1 und SDL 3.2.30; auch dort werden alle 16 Sprachprogramme und
31 Module gebaut. Debian 12 verwendet GCC 12.2.0, Kernel 6.1.0-53-cloud-amd64
und SDL 3.2.30. Die Wiederholungen verwenden den unveränderten Produktionscode
mit dem korrigierten und erweiterten Mutationsprüfer.
macOS Release besteht die beiden App-Prüfungen ebenfalls 2/2 unter
`build/run-index-release-mac/test-results/run-fx8u_vc0`.
Das verschobene Linux-SDK besteht die vollständige unabhängige Prüfung unter
`build/run-index-sdk-proof-linux/Native SDK ä k2jnfyn3` in der VM. Sie prüft alle
gelieferten Header separat, baut die 20 Core-Quellen neu, führt 16 Sprachprogramme,
31 Module und neun native Projektbuilds aus. Neue Indexprobes laufen gegen das
installierte und das neu gebaute Archiv, jeweils mit Million-Zeilen-Fixture,
Allocatorfehlern, Recovery und unabhängigem Pythoncodec. Alle 305 Manifestdateien
des SDKs stimmen mit ihren SHA-256-Werten überein.
Das macOS-SDK besteht dieselbe vollständige Prüfung unter
`build/run-index-sdk-proof-mac/Native SDK ä ycjz0z88`, ebenfalls einschließlich
beider Indexarchive, Million-Zeilen-Dateien und unabhängiger Codec-Prüfung.
Die abschließenden Pakete liegen unter `build/Run Index clean SDK ä mac` bzw.
`build/Run Index clean SDK ä linux` in der VM. Ihre jeweils 305 Manifestdateien
werden erneut gehasht. Header, Archive, ausgelieferte Core-Quellen und alle
Programme stimmen bytegenau mit den vollständig geprüften verschobenen SDKs
überein; die Pakete enthalten die abschließenden Nachweistexte.

Die Indexfinalisierung und das geprüfte Öffnen sind zusätzliche lineare Scans;
gezielte nachfolgende Abfragen profitieren von Checkpoints. Kompression,
weitere Messdatentypen, schnelleres ungeprüftes Öffnen und eigene direkte
Sprachmethoden für die gezielten C-Abfragen sind damit nicht abgenommen.

## Strukturierte Diagnosen am 6. Oktober 2026

Der neue besitzende UTF-8-Wert trägt Fehlercode, Operation, Argument und
ursprüngliche Quellposition. Ein optionaler Experimentcontext-Tail und ein
optionaler Analysecallback erweitern API/ABI 3 ohne verschobene bestehende
Felder. Physim 0.175.0 bindet Kopien, Formatierung, Bytes, Dateien und Auslösen.
CRC-geschützte `.psdiag`-Sidecars entstehen exklusiv; bereits vorhandene Dateien
bleiben erhalten. IPC 5 sendet Typ 12 nur mit `--diagnostics`, ansonsten weiterhin
Typ 7 mit bisherigem Fehlertext. Messdateien und Snapshots bleiben unverändert.

Die Core-Prüfung kontrolliert UTF-8, Feldgrenzen, Fehlercodes, Koordinaten,
unveränderte Ausgaben bei ungültigen Eingaben, sämtliche gekürzten Payloads,
CRC und Versionen sowie alte Context-Grenzen. Die Runnerprüfung vergleicht
C-/Physim-Fehler beim Schritt und bei Analysen mit ihren gespeicherten Feldern,
beide Wire-Modi und unveränderte fremde Sidecars. Ein Szenenfehler behält seinen
Code `PS_SINGULAR`; ein eingefrorener alter ABI-3-Analysetail mit `run_many`
bleibt für null Eingaben verwendbar. Abgefangene Sprachfehler hinterlassen keine
Hostdiagnose; Rücksetzen und Wertkopien werden ebenfalls ausgeführt.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
4/4 unter `build/diagnostics-asan-linux/test-results/run-dgsghrph` in der
Debian-12-VM (GCC 12.2.0, Kernel 6.1.0-53-cloud-amd64).
Linux Release besteht alle 528 Prüfungen unter
`build/diagnostics-release-linux/test-results/run-c20csgrl`.
Die X11/Mesa-Fensterprüfung unter Xvfb und Openbox besteht 3/3 unter
`build/diagnostics-release-linux/test-results/run-y6ztxcgb`: vollständiger
Physim-Ablauf, eigenständige Analyseprojekte und Diagnosen. Letztere bauen und
führen vier fehlschlagende C-/Physim-Experiment-/Analyseprojekte aus, prüfen
Sidecars und Quellmarkierungen; ein fünfter Ablauf öffnet eine zusätzliche
Unicode-Quelldatei. Es ist keine vollständige Prüfung aller 58 Fensterfälle.

Der erste macOS-Gesamtlauf endet 527/528 unter `run-ujep0jyy`: der während
des laufenden Builds ergänzte Szenenfehler-Test verwendet dort noch den vorher
gebauten Runner. Dieser Lauf bestätigt nicht die abschließende Korrektur.
Der korrigierte Runner gibt die veröffentlichte Szenendiagnose statt des
bisherigen pauschalen `PS_NUMERIC` zurück.
Der anschließende macOS-Release-Gesamtlauf besteht 528/528 unter
`build/diagnostics-release-mac/test-results/run-fg394xdy` (Intel macOS 14.6.1,
Apple Clang 16, SDL 3.2.30). Beide Gesamtläufe bauen außerdem 16 eigenständige
Physim-Programme und 31 Experiment-/Analysemodule.

Das verschobene Linux-SDK besteht die vollständige unabhängige Prüfung unter
`build/diagnostics-sdk-proof-linux/Native SDK ä 3koqddhm` in der VM. Sie kompiliert
alle gelieferten Header separat, baut die Core-Bibliothek aus den gelieferten
Quellen neu und führt Programme, Module und neun native Projektbuilds aus.
Neue Diagnoseprobes werden gegen das installierte und das neu gebaute Archiv
gelinkt. C-/Physim-Runner, beide Wire-Modi, `.psdiag`, Szene und eingefrorenes
Analyse-ABI werden auch aus dieser Installation ausgeführt. Das SDK-Manifest
enthält 300 Dateien; alle SHA-256-Werte sind geprüft.
Die Sichtprüfung fand lange absolute Pfade, welche die Fehlermeldung aus der
anklickbaren Zeile verdrängten, sowie sichtbare Ersatzglyphen für Zeilenumbrüche.
Die App zeigt dort jetzt den Dateinamen und Leerzeichen; die vollständigen
Quellpfade und Nachrichten bleiben in den Diagnosewerten erhalten.
Die abschließenden fünf Linux-Diagnoseabläufe bestehen erneut 1/1 unter
`build/diagnostics-release-linux/test-results/run-qrbgnbgl`;
Screenshots der Fehleranzeige und zusätzlichen Quelldatei wurden geprüft.
macOS Release besteht die drei App-Prüfungen mit der abschließenden Anzeige
ebenfalls 3/3 unter `build/diagnostics-release-mac/test-results/run-i8kxzhzp`.
Der Physim-Screenshot zeigt die ursprüngliche Erstellungszeile des Diagnosewerts,
Code/Operation/Argument und den lesbaren UTF-8-Fehlertext.
Das macOS-SDK mit der abschließend geprüften App besteht dieselbe vollständige
unabhängige Prüfung unter
`build/diagnostics-sdk-proof-mac/Native SDK ä efo47ad0`: 16 Sprachprogramme,
31 Module, Diagnoseprobes gegen beide Archive, C-/Physim- und alte ABI-Runner
sowie neun native Projektbuilds.

Abschließende Pakete liegen unter `build/Diagnostics clean SDK ä mac` bzw.
`build/Diagnostics clean SDK ä linux` in der VM. Ihre jeweils 300 Manifestdateien
werden erneut per SHA-256 geprüft. Header, Core, Compiler, Runner, Builder und
gelieferte Core-Quellen stimmen mit den vollständig geprüften verschobenen SDKs
überein. Die aktualisierte App stimmt jeweils mit dem Programm der abschließenden
Fensterprüfung überein; die Pakete enthalten die abschließenden Nachweistexte.

Die Implementierung ergänzt explizite Diagnosen an Modulgrenzen; bestehende
niedrige C-Funktionen liefern weiterhin ihre dokumentierten Ergebniswerte.
Prozessabstürze und Zeitüberschreitungen besitzen damit noch keine originale
Sprachquellposition. Die Dateiablage ist keine Stromausfall-Durabilitätsgarantie.


Ältere Plattformnachweise stehen vollständig unter
[Historische lokale Prüfungen](status-history.md). Die genannten Laufstände
und Dateipfade behalten ihren ursprünglichen Beweisumfang.


## Gemeinsame Series-Numerik am 7. Oktober 2026

Die Folgekorrektur zu CR-007 verwendet eine gemeinsame interne Rechnung in
`src/analysis_numeric.h` für die bisherigen Array-Helfer und die C-/Physim-Series.
Die Sekantenmethode bleibt erhalten. Extreme darstellbare Steigungen und
Trapezflächen werden nicht mehr durch überlaufende Differenzen oder vorzeitig
zu null gerundete subnormale Mittelwerte abgewiesen. Echte Überläufe nehmen
Handle und Scratch-Speicher vollständig zurück. Ein fehlender Messwert bleibt
im kumulativen Integral auch über extreme Achsenabstände unbekannt.
[Verfahren, Einheiten und Grenzen](numerics.md) erklären die Sekanten- und
Trapezapproximation; PP-0363 besitzt jetzt eine explizite Zuordnung.
API/ABI 3 und sämtliche Datenformate bleiben erhalten.

### Lokale Tests und ihre genaue Reichweite

Der unveränderte Series-Pfad scheitert am neuen Gegenbeispiel unter
`build/contact-world-language-release-mac/test-results/run-wo9rwlxg`:
`x=y={-1e308,1e308}` muss die Steigung 1 ergeben. Der korrigierte gezielte
Sieben-Fälle-Lauf besteht unter `run-19yzguj5`. Ein weiterer Zwischenlauf
`run-o8t_3nt1` enthielt doppelte Achsenwerte durch einen zu kleinen Testabstand;
das Raster wurde korrigiert und dieser Lauf bleibt fehlgeschlagen.

Der vollständige macOS-Release-Lauf besteht mit **609/609** unter
`build/contact-world-language-release-mac/test-results/run-mrbtiqsb`.
Unter Debian/GCC enthält der vollständige Lauf `run-mbhcuca2` **608/609**:
alle Laufzeitfälle bestehen, `documentation_reference` scheitert an
Apple-Metadatendateien aus der Dateiübertragung. Die zugehörigen erzeugten
Begleitdateien wurden vor dem Entfernen in
`build/series-numeric-transfer-metadata.tar.gz` gesichert und gegen das Archiv
geprüft (SHA-256 `a2e35542025afb38317c72f3f56ba4312f89aa677fc038179209e6c5c67d8021`).
Der ursprüngliche Lauf wird dadurch nicht nachträglich grün.

Die vier abschließenden Fälle `series_numeric_extremes`,
`language_analysis_numeric_extremes`, `verification_kit` und
`documentation_reference` bestehen mit **4/4** auf macOS unter `run-y4a44_hx`
und Linux unter `run-00f11gmw`. Sie verwenden den endgültigen Teststand mit
Polynomreferenzen, Konvergenzordnung, Blockgrenzen, Messmasken, subnormalen
Flächen, Vorzeichenauslöschung, Einheiten und Fehler-Rücknahme. Der separate
Physim-Analyseprozess prüft selbst erzeugte Reihen ohne Eingabedatensatz.
29 Referenzseiten sind aktuell. Der Katalog enthält jetzt 610 Fälle ohne
Fenster, 591 ohne SDL und 75 Fensterfälle; die nach dem Gesamtlauf ergänzte
separate Sprachprüfung wird durch den Vier-Fälle-Lauf belegt.

### Noch offene Remote-Befunde

Die vor dieser Korrektur abgefragte
[Linux-Paket-CI für `37a55bd`](https://github.com/PhysicSimulator/physim/actions/runs/37609456711)
scheitert ausschließlich an `runner_pacing_phys`: der Offline-Lauf benötigt
1,426 Sekunden gegenüber 2,299 Sekunden bei 0,5× und verletzt den bisherigen
relativen Zeitvergleich. Die Ursache ist noch nicht nachgeprüft; ein
Hardware-/Lastproblem wird nicht als bewiesen behandelt. Das Artefakt mit
608 Ergebnisdatensätzen stimmt mit dem veröffentlichten SHA-256
`e73958f216105338e4d5564b643fdf1b957b19c08b614fda556d726da79b9f46` überein.

Die [C17-CI für `0fd3ee7`](https://github.com/PhysicSimulator/physim/actions/runs/37603493468)
enthält weitere Fehler. Der geprüfte Intel-macOS-SDK-Log endet bei der
Handbuch-Tastaturprüfung an Stufe 13 nach 15,029 Sekunden; die 608 nativen
Debug-Fälle bestehen. Das Artefakt besitzt SHA-256
`dc1b6f65b5dd6150001e169dd223d10417a5002c5a6aa892d3b3940b2a36c08e`.
Das separat geprüfte Windows/v143-Release-Artefakt besitzt SHA-256
`f17510e40bd81f6d17de142149924bd19448721820c47c59010ca960bd2f4ffd`;
607/608 Fälle bestehen, die Referenzprüfung weist die damals zu große
Plattformseite zurück. Die anschließende Archivierung in `37a55bd` hat die
Seite verkleinert; eine neue Windows-Gesamtabnahme wird damit nicht behauptet.
Diese Befunde bleiben von der lokalen Numerikkorrektur getrennt offen.


### Frische verschobene SDKs

Beide vollständigen SDK-Prüfverfahren **ohne Fensterprüfungen** bestehen:

- macOS: `build/Series numeric SDK ä mac tdu_luc2`, verschobenes SDK unter
  `proof/Native SDK ä o3ewk32f/Relocated SDK ä`.
- Linux: `build/Series numeric SDK ä linux p5iekc7w`; der genaue verschobene
  Pfad steht im zugehörigen `verify.log` und `PASSED.json`.

Jeder Lauf prüft ein frisches SDK mit 430 SHA-256-erfassten Dateien und ein
isoliertes Kit mit 60 Eingaben. Die neue private Datei `src/analysis_numeric.h`
ist in der Source-Distribution enthalten. Header und alle 26 Core-Module werden
ausschließlich aus dem verschobenen SDK kompiliert. Die neuen C- und
Physim-Gegenbeispiele bestehen jeweils gegen das installierte und das neu
kompilierte Archiv. Alle bisherigen SDK-Gates bleiben aktiv, darunter die
gekoppelten Lernpfade, unabhängigen Referenzprüfer und kalten nativen Domänenbuilds.
`verify.log` enthält den ausdrücklichen Numerik-Erfolgsmarker; das jeweils
verschobene SDK-Prüfverzeichnis enthält `PASSED.txt` und den vollständigen
Befehlslog. Die übergeordneten Receipts stehen unter
`build/series-numeric-sdk-mac-PASSED.json` und
`build/series-numeric-sdk-linux-PASSED.json`.

Der erste Linux-Hilfsaufruf endete **vor** der SDK-Verifikation an der unter
Python 3.11 nicht verfügbaren `tarfile.extractall(filter=...)`-Option. Die
Fortsetzung prüft die Archivpfade, regulären Dateien und Kit-Hashes explizit
und verwendet dasselbe bereits erfolgreich gebaute SDK. Dieser ursprüngliche
Hilfsfehler bleibt in `series-numeric-sdk-proof-linux.log` erhalten; die
Fortsetzung steht in `series-numeric-sdk-resume-linux.log`.

Die zwölf implementierungs- und prüfrelevanten Dateien stimmen auf beiden
Systemen mit `build/series-numeric-source-freeze.json` überein. Nach dem Entfernen
der Übertragungsmetadaten wurde zusätzlich der gesamte erwartete
Repository-Dateibestand auf Linux byteweise mit dem Mac abgeglichen.
Neue Fenster-, Windows- und Apple-Silicon-Prüfungen gehören nicht zum
Nachweis dieser Runde. Die oben aufgeführten Remote-Fehler bleiben zu bearbeiten.


## CI-Timingprüfungen am 7. Oktober 2026

Die [C17-CI für `37a55bd`](https://github.com/PhysicSimulator/physim/actions/runs/37609456911)
ist abgeschlossen: beide Linux- und alle vier Windows-Jobs bestehen. Die beiden
macOS-Jobs scheitern weiterhin in der SDK-Handbuchprüfung an Stufe 13 nach
15,061 beziehungsweise 15,055 Sekunden. Die gezielt gelesenen Artefakte stimmen
mit den veröffentlichten SHA-256-Digests überein: Intel
`5a41d123f7a87ef8b2382ba176bbbfbf9d623b8f9f2ac2e5a6761c1e509ddaaa`,
Apple Silicon `3227a33d7ea1ebdc21afa9ca5f1bf31dcafc46b7cf5a7139041d666dfa3e9d0d`.

### Reproduzierte Testannahmen

Ein deterministisch verzögertes Zeichnen mit 80 ms pro Frame reproduziert unter
Intel macOS genau die Handbuchstufe 13 und den bisherigen 15-Sekunden-Abbruch:
`build/contact-world-language-release-mac/test-results/run-8x2xqqpy`.
Der Trace zeigt weiter wechselnde Fokusziele; die Navigation hängt dort nicht.
Die genaue Ursache des langsameren Zeichnens auf den Remote-Macs bleibt ohne
deren detaillierten Trace unbewiesen. Die Prüfung muss langsames Zeichnen
zulassen, ohne fehlende oder unerreichbare Bedienelemente zu akzeptieren.

Die Handbuch-Tastaturtests besitzen nun ein eigenes 60-Sekunden-Gesamtlimit
(innerhalb der 75-/85-Sekunden-Prozessgrenzen). Jede Fokussuche prüft zusätzlich,
dass das semantische Ziel existiert, und begrenzt die Versuche anhand der
aktuellen Zahl navigierbarer Elemente. Fehlschläge melden Stufe, Ziel, Fokus,
Sichtbarkeitszustand, Thema und Versuchszahl. Eine neue negative Prüfung erzwingt
ein fehlendes Ziel und verlangt Exitcode 1 mit der passenden Diagnose.
`docs-keyboard-delayed` verwendet die gesamte bisherige Tastaturfolge und ist
auch im SDK-Prüfverfahren enthalten; der normale Produktbetrieb bekommt keine
künstliche Verzögerung.

Der unveränderte Offline-Zeitvergleich scheitert mit einem um 1,5 Sekunden
verzögerten Beobachter unter `run-bkhk2803`: 0,5× benötigt 2,003 Sekunden,
4× 0,260 Sekunden, Offline 1,501 Sekunden. Datenvergleich, Pause und Einzelschritt
sind bereits erfolgreich; nur die bisher geforderte relative Beobachtungszeit
scheitert. Offline bedeutet fehlende Scheduling-Wartezeit, keine garantierte
Schranke für Experimentrechnung, Datenträger, Pipes und den Beobachter.
Die genaue zusätzliche Latenz des früheren Linux-CI-Laufs bleibt unbewiesen.
Der neue Vertragstest fordert 100.000 fällige Schritte bei **unveränderter** Uhr
und prüft zusätzlich die tatsächlichen Runner-Daten und Steuerungen mit dem
verzögerten Beobachter. Die Zeitprüfungen der realen 0,5×-/4×-Raten und die
Prüfung des langsamen Lesers bleiben erhalten. Runner und Pacer wurden nicht
geändert.

### Ausgeführte Nachweise

Die vier gezielten Runner-Fälle bestehen auf macOS unter `run-yfpgwbav` und
Linux unter `run-sst862ow`. Die sieben bisherigen Handbuch-/Menüprüfungen
mit dem neuen langsamen Ablauf bestehen unter macOS `run-8_xgmqrz` und
Linux `run-plnrml_9`. Nach Ergänzung der negativen Kontrolle bestehen sämtliche
vier Tastaturfälle am endgültigen App-Stand unter macOS `run-cdtaiyvk` und
Linux `run-_n21xkus`. Der langsame macOS-Fall benötigt 25,417 Sekunden.
Ein einzelner neuer Gesamtlauf aller Fälle wird damit nicht behauptet.

Beide gezielten SDK-App-Prüfungen bestehen mit vier erfolgreichen Abläufen
(16 px, 22 px, langsames Zeichnen, sämtliche Handbuchthemen/Lernwege) sowie der
korrekt fehlschlagenden Fokus-Kontrolle:

- macOS: `build/CI regression SDK ä mac vxc7i35j/Relocated SDK ä`,
  App-SHA-256 `f3b748b31e2eb709aab78378f28b82951a22c451439519f6c57034e22d8a0b81`.
- Linux: `build/CI regression SDK ä linux h05lz9j2/Relocated SDK ä`,
  App-SHA-256 `53fbb29c403b639ed88a7c04e03c2e1403df5ed06cea42d49031cff6b02a50e8`.

Sie verwenden die zuvor vollständig geprüften numerischen SDKs mit neuer App;
alle anderen 429 manifestierten Dateien bleiben bytegleich. Die fünf
Befehle, erwarteten Exitcodes und acht identischen Implementierungs-/Prüfquellen
stehen in `build/ci-followup-sdk-mac-PASSED.json`,
`build/ci-followup-sdk-linux-PASSED.json` und `ci-followup-source-freeze.json`.
Vorherige Hilfsaufrufe endeten vor dem App-Start an fehlenden Arbeitsordnern;
die Korrektur und Fortsetzung sind in `ci-followup-sdk-ui-proof-v3-*.log`
festgehalten. Diese Hilfsfehler sind keine erfolgreichen App-Prüfungen.

29 Referenzseiten und das isolierte Kit mit 60 Eingaben bestehen auf beiden
Systemen. Der Katalog enthält jetzt 611 Fälle ohne Fenster, 592 ohne SDL und
77 Fensterfälle. Bei der letzten Abfrage ist der Paketbuild der
[Linux-CI für `d4ac0f3`](https://github.com/PhysicSimulator/physim/actions/runs/37619178591)
erfolgreich; die beiden installierten Systemprüfungen und der Großteil der
[C17-Matrix](https://github.com/PhysicSimulator/physim/actions/runs/37619178905)
laufen noch. Das bestätigt weder diese neue Korrektur noch die vollständige
Plattformabnahme; die nächste Remote-Matrix bleibt erforderlich.


## Einzelabgleich der Basis und RNG-Vertrag am 7. Oktober 2026

PP-0348/0349/0350/0351/0353 sind jetzt einzeln vorhandenen APIs, Quellen und
Referenztests zugeordnet. Feste Integer-/Ergebniswerte, Allocatoren, alle vier
Container, explizite Logger und RNG-Zustände besitzen konkrete Implementierungen.
Die allgemeine Handle-Anforderung PP-0352 bleibt ungeprüft: sichere generationale
Series-/Dataset-/Report-APIs ersetzen keine Einzelabnahme sämtlicher öffentlicher
Zustandsdeskriptoren. Der Abgleich bewahrt alle 531 Originalblöcke und enthält
jetzt 20 implementierte, fünf unvollständige und 506 ungeprüfte Blöcke.

Der Normalgenerator zieht den Radius und anschließend den Winkel jetzt in
getrennten C-Anweisungen. Die Probe des vorherigen Ausdrucks auf Debian/GCC und
Clang liefert dieselben sechs Werte; ein tatsächlicher Unterschied dieser Builds
wird **nicht** behauptet. Die Änderung macht die Reihenfolge ausdrücklich zum
Vertrag, statt sie der Operandenauswertung zu überlassen. [RNG-Vertrag](measurement.md)
unterscheidet exakt prüfbare Integerzustände von libm-Rundung und dem ungeprüften
C-Helfer von geprüften, verbrauchsfreien degenerierten Verteilungen. API/ABI 3,
Sprachversion und Dateiformate bleiben erhalten.

### Lokale Referenzen und Sanitizer

Die neuen unabhängigen Probes prüfen vier vollständige 64-Bit-Seeds, 384 C-Referenzwerte
mit ihren jeweiligen Integerzuständen und 128 Physim-Referenzwerte. Dazu kommen
Snapshots, abwechselnd genutzte unabhängige Ströme, degenerierte Verteilungen und die
transaktionale Ablehnung ungültiger Parameter. Integerwerte und Zustände werden
exakt, Normalwerte mit absoluter/relativer Toleranz `2e-14` geprüft.
Die erste Sprachfixture enthielt eine nicht unterstützte if-Ausdruckssyntax und
scheiterte unter `run-fpl88l8n`; sie wurde auf reguläre if-Anweisungen korrigiert.
Dieser ursprüngliche Lauf bleibt fehlgeschlagen.

Die 24 gezielten Basis-/RNG-/Sensor-/Monte-Carlo-/Sprachfälle bestehen auf Intel
macOS unter `build/contact-world-language-release-mac/test-results/run-2_wbxiq7`
und Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-t24apbz3`.
Der frische Linux/Clang-Debuglauf mit ASan/UBSan besteht mit 9/9 unter
`build/base-contract-asan-linux/test-results/run-58n1v87k`. Dies umfasst Core,
Memory, Memory-Owners, Array, Hashmap, String-View, Logging, Measurement und die
C-/Physim-RNG-Referenz. Die separate direkte C-Probe bestätigt bei sd=0 genau
zwei Ziehungen auf beiden Systemen (`build/rng-zero-probe-*.log`).
Ein neuer vollständiger Gesamtlauf sämtlicher Fälle wird damit nicht behauptet.

Die erweiterten Handbuchseiten und Tastaturnavigation bei 16/22 px bestehen
jeweils mit 3/3 auf macOS unter `run-znrkp4yn` und Linux unter `run-3ea1l5qg`.
Alle 29 Referenzseiten sind aktuell. Der Katalog besitzt jetzt 612 Fälle ohne
Fenster, 593 ohne SDL und 77 Fensterfälle.

### Frische verschobene SDKs

Das vollständige isolierte SDK-Prüfverfahren ohne Fensterprüfungen besteht
auf beiden Systemen mit jeweils 360 erfolgreich ausgeführten Prüfbefehlen:

- macOS: `build/Base RNG SDK ä mac 6er9kgzl`, verschobenes SDK unter
  `proof/Native SDK ä 0qtgust5/Relocated SDK ä`.
- Linux: `build/Base RNG SDK ä linux qfr5yce6`, verschobenes SDK unter
  `proof/Native SDK ä 08t06e77/Relocated SDK ä`.

Das frische SDK besitzt 430 manifestierte Dateien, das unabhängige Kit 63
Eingaben. Die RNG-Referenzen laufen gegen das installierte **und** aus den 26
mitgelieferten Core-Modulen neu gebaute Archiv, jeweils in C und Physim.
Die bisherigen Domänen-, Lernweg-, Analyse- und nativen Projektbuild-Gates
bleiben aktiv. Receipts und Quellhashes stehen in
`build/base-contract-sdk-mac-PASSED.json`,
`build/base-contract-sdk-linux-PASSED.json` und
`build/base-contract-source-freeze.json`. Neue Windows-/Apple-Silicon- oder
SDK-Fensterprüfungen werden damit nicht behauptet.

### Weiterer CI-Abbruch nach der nativen Suite

Die [CI für `2564e38`](https://github.com/PhysicSimulator/physim/actions/runs/37621422129)
meldet Fehler im zusammengesetzten Schritt „Direct compiler build“. Die
geprüften Linux/GCC- und Apple-Silicon-Artefakte enthalten jeweils **611/611**
bestandene native Fälle; deren SHA-256 stimmt mit den veröffentlichten Digests
überein: `f91ca69bf965c1b6fac4a8e507ef8bfd73cf474df56bab723542800d3b04891a`
und `6e464e3601665ff577806574eab4458009da6555916a0a784e01a09c2eb8e7c4`.
Die absichtlich ausgelösten Sanitizerfehler der dortigen Probe sind Testdaten;
auch die Fuzzerkampagne ist erfolgreich.

Ein deterministischer nachfolgender Abbruch ist lokal belegt:
`test_native_test_runner.py` vergleicht den vollständigen Fensterkatalog mit
Python-Workflows und kennt die neue direkt gestartete negative Handbuchprüfung
nicht. Der unveränderte Prüfer scheitert an diesem Mengenvergleich unter
`build/base-contract-test-runner-audit.log`. Die erwartete Liste erfasst nun die
negative direkte Prüfung ausdrücklich und kontrolliert deren Exitcode und
Diagnose. Fehlende/zusätzliche künftige Fälle bleiben Fehler, mit konkreter Liste.
Die vollständige Selbstprüfung besteht auf macOS und Linux unter
`base-contract-test-runner-final-*.log`; ihre absichtlich fehlschlagende
Drei-Fälle-Suite ist kein Produktfehler. Weitere mögliche Fehler des
zusammengesetzten Remote-Schritts und die neue Gesamtmatrix bleiben zu bestätigen.


## Materialeigenschaften als SI-Daten am 7. Oktober 2026

`properties.h` und der neue Core-Baustein `properties.c` beschreiben eine
Eigenschaft durch Quelle, Einheit, geschlossenen Temperatur-/Druckbereich und
konstantes oder tabellarisches Modell. 1–64 Achsenpunkte, maximal 4096 Werte,
Einpunktachsen für unabhängige Koordinaten, keine Extrapolation und eine
beschränkte konvexe Interpolation bilden den expliziten Vertrag.
[Verträge und vollständige C-/Physim-Quellen](properties.md) verwenden synthetische
Referenzdaten; es wird kein gemessener Stoffdatensatz erfunden. Beide wirklichen
Experimentsprachen speichern Quellen, Bereich, Einheit, Achsen, Eckwerte und
Methode im Lauf. Die unabhängige Prüfung liest CRC, Schema, sämtliche Messwerte
und Metadaten und kontrolliert alle vier Analyse-Kombinationen.
API/ABI 3 und Datenformate bleiben erhalten; die Library und der native
Projektbuilder verwenden jetzt denselben Satz von **27 Core-Modulen**.

### Lokale Builds, Referenzen und genaue Reichweite

Die vollständigen Release-Suiten bestehen mit **620/620** auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-i9b1hv9g` und
Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-yx2gke1o`.
Danach wurde die zusätzliche kalte Eigenschaftsprojekt-Prüfung registriert und
eine Header-Abhängigkeit korrigiert. Diese ursprünglichen Gesamtläufe werden
damit nicht zu einem neuen 621-Fälle-Lauf umbenannt.

Die endgültigen zehn gezielten Fälle bestehen unter macOS `run-of580rj2` und
Linux `run-0breuqjc`. Sie umfassen C-Eigenschaften, fünf Physim-Wert-/Fehlerfälle,
den tatsächlichen Experiment-/Analyseablauf, kalte native C-/Physim-Projekte,
Referenzseiten und das isolierte Kit. Der separate Quellenvergleich der vier
publizierten Programmquellen besteht ebenfalls. Die C-Prüfung verwendet über
10000 unabhängige T/P-Paare, einen nichtquadratischen 3×2-Aufbau mit echtem
T·P-Term, Extremwerte und das maximale 64×64-Raster. Quellen und Arrays bleiben
bei fehlgeschlagenen Abfragen erhalten; Ausgaben werden nicht teilweise geändert.

Die kalten Eigenschaftsprojekte samt unverändertem Cache und Quellen bestehen
separat unter macOS `run-t_bp6a00` und Linux `run-_k3_pnz7`. Unter Linux/Clang
bestehen die sieben neuen C-/Physim-Eigenschaftsfälle einschließlich des wirklichen
Analyseablaufs mit ASan/UBSan unter
`build/base-contract-asan-linux/test-results/run-i81ymy_i`.
Die vier Handbuchfenster-/Tastaturfälle (alle Themen, 16 px, 22 px, langsames
Zeichnen) bestehen unter macOS `run-cvq8czm3` und Linux `run-s4k2glpn`.
30 Referenzseiten und das isolierte Kit mit 67 Eingaben sind geprüft.
Der Katalog enthält jetzt 621 Fälle ohne Fenster, 601 ohne SDL und 77 Fensterfälle.

### Erhaltene Fehlversuche

Die erste Sprachfixture verwendete eine nicht vorhandene `unitOne`-Funktion
und einen falschen Optionalzugriff (`run-fwo1icnr`); beides wurde auf die vorhandene
Unit-/attempt-Syntax korrigiert. Die ersten SDK-Läufe
`Material properties SDK ä mac p9epbn11` und
`Material properties SDK ä linux f1f3gvqi` scheitern an der eigenständigen
Einbindung von `language_properties.h`: `ps_vec4` war nur über den Sammelheader
bekannt. Der eigene Header bindet `math.h` jetzt selbst ein. Die fehlgeschlagenen
Läufe bleiben erhalten und werden nicht als erfolgreiche SDK-Nachweise gezählt.

Die erste Registrierung der kalten Eigenschaftsprojekte referenzierte den
falsch benannten Oracle `test_property_tutorial.py`. Die Projekte bauen bereits,
doch der Folgeprüfer startet nicht; die Läufe `run-nfhq9k7k` und `run-uflqhux9`
bleiben mit 10/11 fehlgeschlagen. Die Registrierung verwendet jetzt den tatsächlich
vorhandenen `test_property_workflow.py`; die Folgeprüfungen oben bestehen.

### Frische verschobene SDKs mit der Korrektur

Beide vollständigen isolierten SDK-Prüfverfahren ohne Fensterprüfungen bestehen
mit jeweils **370 erfolgreichen Prüfbefehlen**:

- macOS: `build/Material properties SDK ä mac 9yakyf10`, verschobenes SDK unter
  `proof/Native SDK ä bptlzb_k/Relocated SDK ä`.
- Linux: `build/Material properties SDK ä linux 0d75c6hg`, verschobenes SDK unter
  `proof/Native SDK ä eiggnpef/Relocated SDK ä`.

Das SDK besitzt 441 SHA-256-erfasste Dateien. Der Prüfer verwendet ein isoliertes
Kit mit 67 Eingaben; Header und alle 27 Core-Module stammen ausschließlich aus
dem SDK. Die neuen C-/Physim-Eigenschaften und gespeicherten Metadaten bestehen
gegen das installierte und das neu gebaute Archiv. Zehn kalte native Projekte
(davon die beiden neuen Eigenschaftsprojekte), unveränderter Cache und fünf
vollständige unabhängige Domänenorakel bestehen. Alle früheren SDK-Gates bleiben
aktiv. Receipts und 27 eingefrorene Quellen stehen in
`build/properties-sdk-mac-PASSED.json`, `build/properties-sdk-linux-PASSED.json`
und `build/properties-source-freeze.json`. Das belegt diese lokalen Systeme und
diesen Umfang, keine neue Windows-/Apple-Silicon-Gesamtabnahme.

Die letzte Abfrage der
[früheren C17-CI für `915e431`](https://github.com/PhysicSimulator/physim/actions/runs/37625776693)
bestätigt alle vier Windows-Jobs und Linux/Clang; macOS und Linux/GCC laufen
noch. Die [frische Linux-Paket-CI](https://github.com/PhysicSimulator/physim/actions/runs/37625776779)
besteht vollständig einschließlich der installierten Debian-/Ubuntu-Prüfungen.
Diese Ergebnisse gelten für die frühere Revision. Die nächste gesamte Matrix
mit dem neuen Eigenschaftsbaustein bleibt erforderlich.

## Homogenes Van-der-Waals-Modell am 7. Oktober 2026

Dieser begrenzte reale Gasbaustein ergänzt das bestehende Thermodynamikmodul;
Core bleibt bei 27 Modulen, API/ABI 3, Wire 5, Snapshot 3 und psrun 1 bleiben
unverändert. Vier C-/Physim-Funktionen berechnen Druck, Druckableitung,
innere Energie und Entropiedifferenz mit konstanten molaren SI-Koeffizienten.
[Modell, Primärquelle und vollständige Beispiele](real-gas.md) dokumentieren
insbesondere die fehlende Phasenauswahl und Kalibrierung. PP-0412 bleibt
unvollständig; der gesamte Projektplan ist weiterhin nicht abgenommen.

### Tatsächlich ausgeführte gezielte Prüfungen

Die acht Release-Fälle bestehen auf macOS/Apple Clang unter `run-rds54d3j`
und auf Debian 12/GCC unter `run-c481q_od`. Enthalten sind C-Grenzfälle,
Physim-Werte/attempt, das bestehende Thermodynamikmodul, eine unabhängige
60-stellige Decimal-Referenz für reale aufgezeichnete C-/Physim-Läufe, vier
gemischte Analysen, zwei kalte native Projekte samt unverändertem Cache,
publizierte Quellen, Referenzseiten und das isolierte Kit.
Die Läufe speichern sieben geprüfte SI-Kanäle und synthetische Koeffizienten;
sie setzen die Kompression nach einer Sekunde auf ein festes Volumen fort.
Es werden keine gemessenen Gasdaten behauptet.

Vier Fälle bestehen zusätzlich unter Linux/Clang mit ASan/UBSan in
`build/base-contract-asan-linux/test-results/run-homy18es`: C-Gasmodell,
Physim-Bindungen, bisherige Thermodynamik und der wirkliche Experiment-/Analyseablauf.
Die C-Prüfung enthält idealen Grenzfall, Druckableitung per endlicher Differenz,
Stoffmengen-/Volumenskalierung, Entropieumkehr, mechanisch instabile homogene
Algebra, negative Ausgaben, Domainfehler und endliche Rechnung trotz separatem
Überlauf der Energieterme. Bei Fehlern bleibt die Ausgabe erhalten.

Vier Handbuchprüfungen bestehen je Plattform: sämtliche Themen, Tastatur mit
16/22 px und künstlich verzögerte Tastaturnavigation. Belege liegen unter
`build/real-gas-handbook-mac-wlhwzc34` und auf Linux unter
`build/real-gas-handbook-linux-ssktfa0m`; der Linux-Receipt ist zusätzlich als
`build/real-gas-handbook-linux-PASSED.json` lokal erhalten. 30 generierte
Referenzseiten bestehen einschließlich neuer C-/Physim-Einträge.

### Vollständige verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **380 erfolgreichen Prüfbefehlen**:

- macOS: `build/Real gas SDK ä mac nu580m90`, verschoben unter
  `proof/Native SDK ä zwjx2pql/Relocated SDK ä`.
- Linux: `build/Real gas SDK ä linux d33yv2j1`, verschoben unter
  `proof/Native SDK ä _7m5hk7u/Relocated SDK ä`.

Jedes SDK enthält 448 SHA-256-erfasste Dateien; das isolierte Kit hat 71 Eingaben.
Alle 27 Core-Module und öffentlichen Header stammen aus dem SDK. Das neue
Gasmodell und seine Physim-Bindungen bestehen gegen das installierte und das
neu gebaute Archiv. Zwölf kalte native C-/Physim-Projekte einschließlich beider
Gasprojekte, unveränderter Cache und sechs unabhängige Domänenorakel bestehen.
Alle früheren SDK-Gates bleiben erhalten. Receipts stehen in
`build/real-gas-sdk-mac-PASSED.json` und `build/real-gas-sdk-linux-PASSED.json`.
26 Implementierungs-/Test-/Dokumentationsdateien sind in
`build/real-gas-source-freeze.json` festgehalten. Nach den SDK-Läufen wurde nur
Text im abschließenden Verifier-Receipt auf zwölf Projekte und die tatsächlich
geprüften Material-/Gasfälle berichtigt; die geprüften Operationen sind identisch.
Das erneut paketierte Kit hat ebenfalls 71 Eingaben.

### Erhaltene Fehlversuche und Grenzen

Die erste Sprachbindung rief `psrt_raise` ohne dessen viertes Argument auf
(`run-41b7k717`); der Aufruf wurde auf den vorhandenen Vertrag korrigiert.
Die ersten C-Experiment-/Projektfälle (`run-1h0gckvs`) zeigten die fehlende
eigenständige Einbindung von `units.h` für `PS_PASCAL`. Beide C-Beispiele binden
sie nun ausdrücklich ein. Die erfolgreichen Folgeprüfungen verwenden diese
Korrekturen. Zwei früh gestartete SDK-Läufe (`l5dn54s5`, `en_fcx5k`) wurden vom
Build-Lock abgewiesen; ihre Logs bleiben erhalten. Die erfolgreichen SDK-Läufe
starteten erst nach Abschluss der vorherigen Builds.

Diese Belege gelten für den lokalen Intel-Mac und Debian-VM-Lauf. Sie beweisen
keine neue Windows-/Apple-Silicon-Gesamtabnahme, kein Phasengleichgewicht und
keine Stoffkalibrierung. Der Katalog enthält jetzt 626 Fälle ohne Fenster und
77 Fensterfälle; Katalogzahlen allein sind keine bestandenen Gesamtläufe.

### Abschließende Gesamtläufe

Die tatsächlichen Release-Gesamtläufe mit diesem Baustein bestehen auf beiden
lokalen Systemen vollständig, jeweils **626/626**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-ngt3aqiy`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-w7lsv7gh`.

Sie enthalten auch die früheren Compiler-, Runtime-, Runner-, Batch-, Format-,
Domänen- und Projektbuildfälle. Der Linux-Ergebnisreceipt ist zusätzlich als
`build/real-gas-full-linux-results.json` lokal erhalten. Die zuvor genannten
acht gezielten Fälle, Sanitizer- und Handbuchprüfungen sind separate Belege;
sie werden nicht zu einer erfundenen größeren Testsuite zusammengezählt.

## Gemeinsame SI-Kanaldeklaration am 7. Oktober 2026

Der Einzelabgleich der Einheitenanforderungen deckt einen konkreten Unterschied
zwischen C und Physim auf: `ps_channel_add` nahm in C eine Skala ungleich 1 an,
speicherte aber nur Dimensionen und Symbol. Die Skala ging damit verloren.
Die unveränderte Ausgangsrevision `d9b167a` scheitert reproduzierbar an der neuen
C-Prüfung: Ein Zentimeterkanal wird angenommen (`run-58yp6qw6`). Das ist ein
belegter Fehler an dieser API-Grenze, keine Aussage über alle historischen Läufe.

C und Physim verwenden nun denselben Deklarationsvalidator: Skala exakt 1,
begrenzte UTF-8-Metadaten ohne Kürzung, eindeutiger nichtleerer Name und höchstens
16 Kanäle. Beschreibungen erlauben Tabs/Zeilenumbrüche. Fehler ändern keinen
Kontextwert und verbrauchen keinen Slot. Texte werden vor der Zuweisung kopiert,
auch wenn Eingaben auf den eigenen Zielslot zeigen. Das bestehende Rohschema-
und Dateiformat bleibt erhalten. [Vertrag und ausdrückliche SI-Umrechnung](numerics.md)
unterscheiden Kanalwerte von Anzeigeeinheiten und physikalischer Bedeutung.
Eine nackte Zahl kann die Bibliothek nicht auf ihre richtige Einheit prüfen.

### Gezielte tatsächlich ausgeführte Nachweise

Je **22/22** Release-Fälle bestehen auf dem Intel-Mac/Apple Clang
(`run-ixa2ludj`) und Debian 12/GCC (`run-83jdka8c`). Sie enthalten Kern-/Numerik-,
Kanal-/Anzeigeeinheiten-, Runner-, Katalog-, Referenz- und Kitprüfungen. Die neuen
Fälle behandeln exakte Byte-Grenzen, UTF-8/Steuerzeichen, Duplikate, nichtkanonische und nichtendliche
Skalen, volle/ungültige Zähler, Input-Aliasing und unveränderte Fehlerausgaben.

Tatsächliche C-/Physim-Experimentmodule weisen ungültige Deklarationen ab und
speichern nach ausdrücklicher Konvertierung von 125 cm exakt 1,25 m.
Ein unabhängiger Python-Prüfer liest Magic, CRCs, zwei Kanalschemata samt sieben
SI-Exponenten, drei reale Zeilen und Footer und vergleicht den fertigen CSV-Export.
Er bestätigt die kompakten Indizes nach abgefangenen Fehlern und gleiche Werte
in beiden Sprachen. Der CSV-Prüfer benutzt die öffentliche Export-API, keinen
nicht vorhandenen Runner-Schalter.

Vier Fälle bestehen zusätzlich unter Linux/Clang mit ASan/UBSan:
`build/base-contract-asan-linux/test-results/run-93fmxd0p`. Sie umfassen Core,
Numerik, C-Deklarationen und den echten C-/Physim-Runner-/CSV-Ablauf.
Linux-Receipts liegen zusätzlich lokal unter `build/channel-targeted-linux-results.json`
und `build/channel-asan-linux-results.json`. 30 Referenzseiten bestehen.

Die erste Sprachfixture nutzte eine in diesem Bindungskontext nicht globale
`convert`-Funktion (`run-2ibf7ctf`). Die korrigierte Quelle verwendet die vorhandene
Methode `centimetre.convert(125,metre)` und besteht unter `run-j404182x` sowie in
allen gezielten und SDK-Folgeläufen. Fehlversuche und Quellen bleiben erhalten.

### Vollständige frische und verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **385 erfolgreichen Prüfbefehlen**:

- macOS: `build/Canonical channels SDK ä mac 7itzrr5z`, verschoben unter
  `proof/Native SDK ä 0n6v3i18/Relocated SDK ä`.
- Linux: `build/Canonical channels SDK ä linux 9gpwkv9q`, verschoben unter
  `proof/Native SDK ä aee01x5u/Relocated SDK ä`.

Je 448 SDK-Dateien und 76 isolierte Kit-Eingaben sind SHA-256-erfasst. Alle
27 Core-Module und öffentlichen Header stammen aus dem SDK. Die C-Grenzfälle
und tatsächlich aufgezeichneten C-/Physim-Läufe samt CSV bestehen gegen das
installierte und das aus SDK-Quellen neu gebaute Archiv. Alle früheren Gates,
zwölf kalte native Projekte, unveränderte Cache-Wiederverwendung und sechs
vollständige unabhängige Domänenorakel bleiben aktiv. Receipts stehen in
`build/channel-declaration-sdk-mac-PASSED.json` und
`build/channel-declaration-sdk-linux-PASSED.json`. Alle 19 in
`build/channel-declaration-source-freeze.json` festgehaltenen Implementierungs-,
Test- und Dokumentationsdateien sind während beider Prüfungen unverändert.
API/ABI 3, Wire 5, Snapshot 3 und psrun 1 bleiben unverändert.

PP-0372 bleibt für sämtliche API-/Analysegrenzen ungeprüft. Auch die gesamte
Einheiten-, Windows-/Apple-Silicon- und Produktabnahme bleibt offen. Die letzte
Abfrage der [C17-CI für die vorherige Revision `d9b167a`](https://github.com/PhysicSimulator/physim/actions/runs/37641732728)
zeigt drei erfolgreiche Windows-Jobs; die übrigen Jobs laufen noch. Das ist kein
Windows-Nachweis der aktuellen Kanalkorrektur.

### Abschließende Gesamtläufe der Kanalkorrektur

Die tatsächlichen Release-Gesamtläufe bestehen vollständig, jeweils **628/628**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-5f9izwxu`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-ovk8fbvv`.

Sie enthalten die früheren Compiler-, Runtime-, Format-, Runner-, Batch-,
Domänen- und Projektbuildfälle sowie beide neuen Kanaldeklarationsfälle.
Der Linux-Receipt ist zusätzlich lokal unter `build/channel-full-linux-results.json`
erhalten. Die 22 gezielten Fälle, vier Sanitizerfälle und beide SDK-Prüfungen
sind separate Nachweise; sie werden nicht zu einer erfundenen Gesamtsuite
zusammengezählt. Die Fensterfallzahl im Katalog bleibt 77; dieser Schritt
behauptet keine erneut ausgeführte gesamte Grafikabnahme.

## Normierte und kompensierte Quantity-Summen am 7. Oktober 2026

Eine neue C-Regression am unveränderten Ausgangsstand `934d5fc` reproduziert
`PS_NUMERIC` für die endliche Summe `-2^1023 m + 2^1023 (Skala 2 m)`
(`run-tq5m9snf`). Die Einzelkonvertierung läuft zuvor über. C und Physim
kombinieren jetzt normierte Operanden und Konvertierungs-/Summenreste vor der
Rückskalierung. Subnormale Ergebnisse werden auf dem endgültigen Gitter gerundet;
sehr kleine Beiträge können dadurch einen Rundungsgleichstand beeinflussen.
[Vertrag, Primärquelle und Grenzen](numerics.md) nennen die normale Rundung zur
nächsten Zahl, Alias-/Fehlervertrag und fehlende allgemeine Exaktheitsgarantie.
Die linke Einheit und ihr geliehenes Symbol bleiben erhalten. Eigenständige
Konvertierung sowie Produkt-/Quotientenoperationen behalten ihren Vertrag.

### Tatsächlich ausgeführte gezielte Nachweise

Je **16/16** Release-Fälle bestehen auf dem Intel-Mac/Apple Clang
(`run-2uvhcho3`) und Debian 12/GCC (`run-qonkc1zv`). Sie enthalten C-Grenzfälle,
bestehende Quantity-Operatoren/-Fehler, numerische Kernprüfungen, Kanaldeklarationen,
Anzeigeeinheiten, Referenz und isoliertes Kit. Der neue Prüfer benutzt Python
`Fraction` für exakte rationale Werte der tatsächlich eingelesenen Double-Eingaben:
**20038 C-Eingaben und 38 tatsächliche Physim-Operatorfälle**. Die gezielten 38
Fälle verlangen exakt den aus der rationalen Referenz gerundeten Double-Wert;
in beiden Sprachen werden erfolgreiche Werte, Fehler und linke Skala geprüft.

Die erzeugten Fälle decken 10000 dyadische Skalenverhältnisse und 10000 weitere
breit verteilte endliche Werte/Skalen ab. Dafür werden Bereich und dokumentierte
Double-Abweichung unabhängig geprüft; unvermeidbare Endrundung wird bei der
Fehlerschranke berücksichtigt. Dies ist keine Aussage über sämtliche möglichen
Double-Paare. Die C-Prüfung verlangt zusätzlich volle unveränderte Fehlerausgaben,
Symbol-/Dimensionsintegrität, Aliasverhalten, Auslöschung und vorzeichenbehaftete
Null. Subnormale Fälle auf und beiderseits einer halben Gitterweite sind enthalten.

Unter Linux/Clang mit ASan/UBSan bestehen die elf ausgewählten Fälle in
`build/base-contract-asan-linux/test-results/run-s2j2pkmr`. Linux-Receipts sind
zusätzlich unter `build/quantity-targeted-linux-results.json` und
`build/quantity-asan-linux-results.json` lokal erhalten. 30 Referenzseiten bestehen.

Die erste einfache normierte Version besteht die Überlaufregression, verliert
aber bei einer Subnormal-Grenze den Rundungsrest. Dieser konkrete Probe führte
zur Kompensation und abschließenden Gitterrundung. Der erste kompensierte
Oracle-Lauf (`run-nzu0y43_`) scheitert an einer zu engen Oracle-Fehlerschranke,
die unvermeidbare Subnormal-Endrundung nicht berücksichtigt. Die Schranke ist nun
mindestens eine halbe finale ULP; die 38 gezielten Fälle verlangen zusätzlich
exakte Gleichheit zur rationalen Referenz. Die erfolgreichen Folgeprüfungen
verwenden diese strengeren gezielten und korrigierten allgemeinen Prüfungen.
Fehlversuche und Quellen bleiben erhalten.

### Vollständige verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **390 erfolgreichen Prüfbefehlen**:

- macOS: `build/Quantity sums SDK ä mac cv7rk3x8`, verschoben unter
  `proof/Native SDK ä eegxtvz7/Relocated SDK ä`.
- Linux: `build/Quantity sums SDK ä linux p5n8cppa`, verschoben unter
  `proof/Native SDK ä gmosenm4/Relocated SDK ä`.

Je 448 SDK-Dateien und 81 Eingaben im isolierten Kit sind SHA-256-erfasst.
Die neuen C-/Physim-Summen bestehen gegen installierte und aus SDK-Quellen
neu gebaute Archive. Alle früheren Gates, zwölf kalte native Projekte, unveränderter
Cache und sechs unabhängige Domänenorakel bleiben aktiv. Die 18 in
`build/quantity-sum-source-freeze.json` festgehaltenen Implementierungs-/Test-/
Dokumentationsdateien waren während beider Prüfungen unverändert. Receipts liegen
in `build/quantity-sum-sdk-mac-PASSED.json` und `build/quantity-sum-sdk-linux-PASSED.json`.
Der anschließende Planabgleich ergänzt ausschließlich den unten belegten Series-
Gegenbefund. API/ABI 3, Wire 5, Snapshot 3 und psrun 1 bleiben unverändert.

### Offene Analysegrenze und Windows-Nachweis

Ein tatsächlicher C-Probe gegen das frisch installierte macOS-SDK addiert eine
Series mit 1 m und eine ausgerichtete Series mit 50 cm als **51 statt 1,5 m**.
`import_values` speichert rohe Werte mit ihrer Skala; `ps_series_combine` prüft
Dimensionen, berücksichtigt beim Addieren aber die rechte Skala nicht.
Quellen und Ergebnis stehen in `build/series-unit-scale-probe.c`,
`build/series-unit-scale-probe.txt` und `build/series-unit-scale-audit.json`.
PP-0370 ist deshalb jetzt konkret unvollständig. Der Plan enthält unverändert
24 implementierte, nun sechs unvollständige und 501 ungeprüfte Blöcke.
Diese Series-/SI-Speicherlücke wird als nächstes bearbeitet; die Quantity-Korrektur
ist keine gesamte Einheiten-/Analyseabnahme.

Der [Windows-v143-Debug-Job der vorherigen Revision `934d5fc`](https://github.com/PhysicSimulator/physim/actions/runs/37645634144/job/112875535813)
ist fehlgeschlagen. Seine öffentlichen Check-Annotationen melden mehrfach ein
nicht mehr vorhandenes Laufwerk für Runner-Dateien auf `D:` und einen Prozess-
Exitcode 1. Der Compilerlog ist über die öffentliche API nicht abrufbar (403).
Daraus wird weder ein konkreter Produktcodefehler noch ein bestandener Job
abgeleitet. Die neue gesamte Windows-/Apple-Silicon-/Plattformabnahme bleibt offen.

### Abschließende Gesamtläufe der Quantity-Korrektur

Die tatsächlichen Release-Gesamtläufe bestehen auf beiden lokalen Systemen
vollständig, jeweils **630/630**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-ptuom6qd`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-hp1counu`.

Sie enthalten die früheren Compiler-, Runtime-, Format-, Runner-, Batch-,
Domänen- und Projektbuildfälle sowie beide neuen Quantity-Summenfälle.
Der Linux-Receipt ist zusätzlich unter `build/quantity-full-linux-results.json`
lokal erhalten. Die 16 gezielten Fälle, elf Sanitizerfälle und SDK-Läufe sind
separate Nachweise. Die 77 Fensterfälle im Katalog sind in diesem Schritt
nicht als erneut ausgeführte Grafikgesamtabnahme ausgewiesen. Der weiterhin
belegte Series-Skalierungsfehler wird durch diese grünen Tests nicht widerlegt;
der Katalog erhält dafür als nächsten Schritt einen eigenen Gegenbeleg.

## Kanonische SI-Series am 7. Oktober 2026

Die neue C-Regression am unveränderten Ausgangsstand `00fec46` reproduziert
**1 m + 50 cm = 51 statt 1,5 m** (`run-z8v7c6hg`). Eigene Series speicherten
Rohwerte mit Eingabeskala; die Addition prüfte Dimensionen und addierte die
Rohzahlen ohne Skalenumrechnung. Beide Importfunktionen übernehmen deklarierte
Eingabeeinheiten jetzt blockweise nach SI. Eingabearrays bleiben erhalten;
Ausgabe-Metadaten und Werte verwenden Skala 1 und SI-Dimensionssymbole.

[Vertrag und Umstellung](series.md) beschreiben die absichtliche Änderung von
`value()/unitScale()`: Bestehende Analysen müssen eine anschließende eigene
Umrechnung entfernen. Die bisherige Physim-Referenzfixture erwartet für einen
Zentimeterimport nun Skala 1 und Werte 0,02/0,03 statt 2/3. Sie bleibt Teil
der tatsächlichen Sprach-/Analyseprüfung; der geänderte Wertvertrag wird nicht
mit einer bloßen Korrektur der fehlerhaften Summe verdeckt. Historische Laufdateien
werden nicht umgeschrieben. Plot-/Report-Anzeigeeinheiten bleiben eine ausdrückliche
Darstellungskonvertierung und ändern die Series nicht.

### Tatsächlich ausgeführte gezielte Nachweise

Je **21/21** Release-Fälle bestehen auf dem Intel-Mac/Apple Clang
(`run-mbago8z8`) und Debian 12/GCC (`run-ug7tw11k`). Sie enthalten eigene und
Dataset-Series, Masks, Auswahl/Quantile, alle Resampling-Verfahren, PCHIP,
numerische Extremfälle, die bestehende Physim-Analyse, Referenz und isoliertes Kit.
Unter Linux/Clang mit ASan/UBSan bestehen die 19 ausgewählten Fälle in
`build/base-contract-asan-linux/test-results/run-8pf3i131`. Linux-Receipts sind
zusätzlich lokal als `build/series-si-targeted-linux-results.json` und
`build/series-si-asan-linux-results.json` erhalten.

Die neue C-Prüfung verarbeitet **513 Zeilen über mehrere 256er-Blöcke** aus
cm-/ms-Eingaben. Sie prüft SI-Import, Addition/Subtraktion/Produkt/Quotient,
Affine, Ableitung, Integral mit nichtkanonischem Anfangswert, Prozent-Selektoren,
Masken, leere Reihen sowie **1025 Ziele** für linear/nearest/previous/PCHIP.
Die affine Rechnung verwendet Quantity-Addition pro gültiger Zeile, sodass ein
allein nichtdarstellbarer Offset noch eine endliche Summe ergeben kann. Maskierte
Zeilen werden nicht ausgewertet; factor*value muss selbst endlich sein.

Rangefehler im letzten Importblock, nichtdarstellbarer nichtnulliger Unterlauf
und erschöpfte Quota erhalten Ausgabehandles, vorhandene Werte und belegte
Scratch-Bytes. Der unveröffentlichte Entwurf wird geschlossen. Ein tatsächliches
Physim-Analysemodul prüft dieselben Eingaben und skalierte Einheiten, abgefangene
Rangefehler sowie lineare/PCHIP-Ausgaben. Ein unabhängiger 64-stelliger Decimal-
Prüfer liest beide fertigen CSVs: alle 513 Zeilen, fünf Spalten und kanonische
Kopfzeilen. Ein separater Report-Prüfer lädt beide Berichte und prüft jede
Kurvenkoordinate sowie alle sieben Dimensionsfelder und Skala 1.

### Erhaltene Fehlversuche

Die erste neue Sprachfixture verwendete nicht unterstützte Series-Operatoren
(`run-4b_irekn`) und danach in diesem Kontext nicht globale Methodennamen
(`run-8hwak2fh`). Sie verwendet nun die vorhandenen adding/subtracting/multiplied/
divided-Methoden und besteht in allen Folgeprüfungen. Der zusätzliche Nearest-
Test nahm anfangs den späteren Punkt am Abstandsgleichstand an (`run-p_xpc4fm`);
laut bestehendem Headervertrag ist der frühere Punkt richtig. Die Erwartung
wurde korrigiert, das Verfahren bleibt erhalten. Der erste breitere Lauf
(`run-o2v_sw8e`) besteht funktional mit 20/21, scheitert aber an noch nicht
regenerierten Referenzseiten. Die endgültigen 21/21 verwenden die aktualisierten
30 Referenzseiten. Alle Fehlversuche und Quellen bleiben erhalten.

### Vollständige verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **393 erfolgreichen Prüfbefehlen**:

- macOS: `build/Series SI SDK ä mac 198cckdp`, verschoben unter
  `proof/Native SDK ä 3pbff4ir/Relocated SDK ä`.
- Linux: `build/Series SI SDK ä linux gzozrs0z`, verschoben unter
  `proof/Native SDK ä ep8tyb0h/Relocated SDK ä`.

Je 448 SDK-Dateien und 85 Kit-Eingaben sind SHA-256-erfasst. C-/Physim-SI-Import,
Analysen und Report-/CSV-Ausgaben bestehen gegen das installierte und das aus
SDK-Quellen neu gebaute Archiv. Alle früheren Gates, zwölf kalte native Projekte,
unveränderter Cache und sechs unabhängige Domänenorakel bleiben aktiv.
Die 18 in `build/series-si-source-freeze.json` festgehaltenen Implementierungs-/
Test-/Dokumentationsdateien waren während beider SDK-Prüfungen unverändert.
Receipts: `build/series-si-sdk-mac-PASSED.json` und
`build/series-si-sdk-linux-PASSED.json`. API/ABI 3, Wire 5, Snapshot 3 und psrun 1
bleiben unverändert; die numerische Importsemantik ist ausdrücklich dokumentiert.

Der konkrete Gegenbefund zu PP-0370 ist geschlossen. Der Block kehrt zu
`unverified` zurück, weil sämtliche übrigen internen Speicherschichten noch
nicht einzeln gegen den gesamten Planumfang abgenommen sind. Aktuell 24
implementierte, fünf unvollständige und 502 ungeprüfte Blöcke. Die letzte Abfrage
der [C17-CI der vorherigen Quantity-Revision `00fec46`](https://github.com/PhysicSimulator/physim/actions/runs/37651910156)
bestätigt Windows/MSVC und Windows/ClangCL in Debug; übrige Jobs laufen noch.
Das ist kein Windows-Nachweis der aktuellen SI-Series-Änderung. Die gesamte
Einheiten-, Grafik- und Plattformabnahme bleibt offen.

### Abschließende SI-Series-Gesamtläufe

Die tatsächlichen Release-Gesamtläufe bestehen vollständig, jeweils **632/632**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-c052_5rb`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-uj9k60h2`.

Sie enthalten sämtliche bisherigen Compiler-, Runtime-, Runner-, Batch-, Format-,
Domänen- und Projektbuildfälle sowie beide neuen SI-Series-Fälle. Der Linux-
Receipt ist zusätzlich lokal als `build/series-si-full-linux-results.json`
erhalten. Die 21 gezielten Fälle, 19 Sanitizerfälle und SDK-Prüfungen sind
separate Nachweise. 77 Fensterfälle bleiben im Katalog; dieser Schritt behauptet
keine erneut ausgeführte gesamte Grafikabnahme. Die Semantikumstellung für eigene
Series bleibt trotz grüner Gesamtläufe ausdrücklich dokumentiert.


## Gewichtete ODE-Arithmetik (§7.2 / PP-0362)

Ausgangsrevision `f604d2a`. Ein getrennt gegen das vorherige installierte Archiv
gelinkter Probe liefert für `y'=1e308`, `y(0)=0`, `dt=1e-308` den Fehlercode 10
und Zustand 0 statt eines darstellbaren Zuwachses nahe 1. Die neue Regression
scheitert am unveränderten Stand in `run-oh2dqpir`. Die frühere RK4-Summe
`a+2b+2c+d` läuft vor Multiplikation mit der kleinen Schrittweite über.

Der gemeinsame interne Baustein kombiniert endliche Gewichte, Ableitungen und
Schrittweite mit kompensierten Summen und FMA. Bei problematischen Größenordnungen
verwendet er Mantissen/Exponenten. Euler, symplektischer Euler, RK4, Verlet und
RK45 samt Fehlerschätzung verwenden ihn. Echte nichtdarstellbare Stufen oder
Endzustände bleiben Fehler; die geprüften Schnittstellen erhalten ihre Zustände.
[Methoden, Einheiten und Grenzen](numerics.md) erklären weiterhin lokale statt
globale Fehlerschätzung, nichtsteife Probleme und gewöhnliche Double-Rundung.

### Gezielte Laufzeitprüfungen

Je **49/49** Release-Fälle bestehen auf Intel macOS/Apple Clang
(`run-6gcg9jz5`) und Debian 12/GCC (`run-j25bww01`). Unter Linux/Clang Debug
mit ASan/UBSan bestehen **44/44** (`run-ukgrsnk_`). Diese Läufe enthalten
Core/Numerics, tatsächliche Sprachprogramme, RK45-Diagnosen, Verlet sowie
bestehende nichtkonstante Pendel- und Konvergenzprüfungen.

Der zusätzliche Fraction-Prüfer vergleicht jeweils **244 C- und Physim-Fälle**
aller fünf Verfahren gegen unabhängige rationale Lösungen für konstante
Ableitungen/Beschleunigungen. Hex-Eingaben und die erzeugte Sprachfixture
bewahren dieselben binären Eingaben. Geprüft werden große und kleine Größen,
Vorzeichen, endliche Auslöschung und echte Rangefehler. Das Fehlerbudget ist
eine ausdrücklich begrenzte Prüfung, keine Garantie exakt gerundeter Summen.
Eine separate C-Prüfung enthält außerdem 32 Zustände und atomare Fehlerausgaben.

### Gemessene Kosten und erste SDK-Lücke

`tools/ode_range_benchmark.c` führt 200000 zurückgesetzte, eindimensionale RK4-
Schritte mit konstanter Ableitung aus. Auf diesem Intel-Mac ergeben sich als
CPU-Zeiten 0,005608 s am vorherigen Archiv, 0,036980 s mit durchgehend skalierter
Rechnung und 0,017237 s mit dem geprüften direkten Rechenweg für normale Größen.
Alle drei akkumulierten Werte sind 200,00000000059092. Das ist ein einzelner
synthetischer Vergleich: Der endgültige Weg kostet hier etwa das Dreifache des
vorherigen Wegs. Er beweist weder allgemeine Leistungsgleichheit noch ein
Leistungsbudget für vollständige Experimente. Die Messausgaben bleiben unter
`build/ode-cost-before.txt`, `ode-cost-after.txt` und `ode-cost-fast.txt` erhalten.

Der erste isolierte macOS-SDK-Versuch (`ODE range SDK ä mac _a6b6q2f`) entdeckt
einen fehlenden privaten Header beim Neubau der installierten Quellen.
`tools/build.py` übernimmt jetzt auch `src/ode_numeric.h` ins SDK. Dieser
Fehlversuch bleibt erhalten; die endgültige SDK-Abnahme benötigt einen frischen
vollständigen Lauf.

### Wiederherstellbare ältere Linux-Nachweise

Zwei abgeschlossene generierte Linux-Prüfverzeichnisse wurden nach vollständiger
Inventar-/SHA-256-Prüfung archiviert: `Base RNG SDK ä linux qfr5yce6` und
`Material properties SDK ä linux 0d75c6hg`. Sie liegen jetzt in
`build/archived-proofs-rng-properties.tar.gz` auf Linux und macOS. Das Archiv
enthält 28166 Dateien/Links, ist 602375720 Byte groß und hat SHA-256
`e4194290dadf4ca95fc095710f38ad7480955882bdf0c7975bba11ac663ebcc0`.
`build/archived-proofs-rng-properties.json` enthält das vollständige Inventar.
Alle Inhalte wurden vor Entfernen der beiden ausgepackten Linux-Verzeichnisse
gegen das Archiv geprüft; die Mac-Kopie wurde zusätzlich nach Größe/SHA geprüft.
Die historischen Nachweise bleiben damit wiederherstellbar.

Die Linux-Paket-CI der Ausgangsrevision `f604d2a`
([Lauf 37657198234](https://github.com/PhysicSimulator/physim/actions/runs/37657198234))
ist erfolgreich abgeschlossen. Ihre C17-Matrix läuft bei der letzten Abfrage
noch. Beide Aussagen betreffen die vorherige Revision und ersetzen keine
Windows-/Apple-Silicon- oder gesamte Grafikabnahme der ODE-Änderung.

### Korrigierte Linux-SDK-Abnahme und macOS-Gesamtlauf

Das korrigierte Linux-SDK besteht vollständig unter
`build/ODE range SDK ä linux lpk4sq92`, verschoben unter
`proof/Native SDK ä r8gpcgqf/Relocated SDK ä`. Alle **398 Prüfbefehle**
enden erfolgreich. 449 SDK-Dateien und 90 Kit-Eingaben sind SHA-256-erfasst.
`build/ode-range-sdk-linux-PASSED.json` und der zusätzlich lokal erhaltene
`build/ode-range-sdk-linux-verification.log` enthalten die Receipts.
Das Kit läuft außerhalb der Repositoryquellen und prüft sowohl das installierte
Archiv als auch den Neubau aus ausgelieferten SDK-Quellen. Die 21 eingefrorenen
Implementierungs-/Test-/Dokumentationsdateien bleiben während dieses Nachweises
unverändert. Die laufend ergänzte Plattformchronik ist nicht Teil dieses Freeze.

Der vollständige macOS/Apple-Clang-Release-Lauf besteht mit **634/634** unter
`build/contact-world-language-release-mac/test-results/run-r8al1p35`.
API/ABI 3, Wire 5, Snapshot 3, psrun 1 und Sprache 0.182.0 bleiben unverändert.
27 Core-Module und sämtliche bisherigen Compiler-, Runtime-, Format-, Batch-,
Projekt- und Domänenprüfungen bleiben enthalten. Die 77 Fensterfälle im Katalog
sind durch diesen Lauf ohne Fenster nicht erneut abgenommen.

Der vollständige Debian-12/GCC-Release-Lauf besteht ebenfalls mit **634/634**
unter `build/contact-world-language-release-linux/test-results/run-bzxu9w5y`.
Der zusätzliche lokale Receipt ist `build/ode-range-full-linux-results.json`.
Die beiden gezielten Linux-Receipts liegen lokal unter
`build/ode-range-targeted-linux-results.json` (49/49) und
`build/ode-range-asan-linux-results.json` (44/44). Beide vollständigen
Release-Läufe enden erfolgreich; die SDK- und Sanitizerprüfungen sind getrennte
Nachweise mit dem jeweils beschriebenen Umfang.

Die spätere Jobabfrage derselben C17-Matrix bestätigt Windows/v143 und
Windows/ClangCL jeweils Debug und Release erfolgreich. Der macOS-Intel-Job
[112915204504](https://github.com/PhysicSimulator/physim/actions/runs/37657198365/job/112915204504)
scheitert im SDK-/Relocation-Schritt, nachdem der direkte Build bestanden ist.
Die öffentliche Annotation nennt nur Exitcode 1; daraus folgt keine belegte
Ursache. macOS-ARM und beide Linux-Jobs laufen bei dieser Abfrage noch.
Receipts: `build/ode-range-prior-c17-jobs.json` und
`build/ode-range-prior-mac-annotations.json`. Der Fehler bleibt offen, bis
vollständige Logs oder ein entsprechender reproduzierbarer Gegenbefund vorliegen.
Die grünen lokalen Prüfungen ersetzen diesen Remote-Fehler nicht.

Der öffentliche Abruf der vollständigen Logs für den älteren macOS-Intel-Job
liefert HTTP 403 (`build/ode-range-prior-mac-log-response.txt`). Die Annotation
ersetzt keinen Compiler-/Testlog; es wurde daher keine spekulative CI-Korrektur
vorgenommen.

### Abschließender macOS-SDK-Nachweis

Das korrigierte macOS-SDK besteht vollständig unter
`build/ODE range SDK ä mac tk_g682x`, verschoben unter
`proof/Native SDK ä j3kb1qwy/Relocated SDK ä`. Auch hier enden alle
**398 Prüfbefehle** erfolgreich; 449 SDK-Dateien und 90 Kit-Eingaben sind
SHA-256-erfasst. Receipt: `build/ode-range-sdk-mac-PASSED.json`. Beide
SDK-Prüfungen verwenden die gleichen 21 eingefrorenen Dateien und alle 27
Core-Module. Gegen installierte und neu gebaute Archive bestehen die fünf
ODE-Verfahren, unabhängige C-/Physim-Referenzen, tatsächliche Lernpfade und
sämtliche bisherigen SDK-Gates.

Der endgültige Implementierungsnachweis für PP-0362 ist damit lokal auf Intel
macOS und Debian belegt. Der vollständige Plan bleibt mit 25 implementierten,
fünf unvollständigen und 501 ungeprüften Blöcken offen. Eine neue gesamte
Remote-/Grafik-/Produktabnahme wird durch diesen Schritt nicht behauptet.
