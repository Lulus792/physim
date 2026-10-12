# Historische Plattformnachweise – Band 2

Vollständige ältere Belege aus [Band 1](platform-validation-history.md).
Die [aktuellen Nachweise](platform-validation.md) enthalten neuere Prüfungen.
Die folgenden Texte und Belegpfade sind unverändert übernommen.

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



## Lineare Gleichungssysteme (§7.2 / PP-0360)

Ausgangsrevision `03c42f6`. Ein getrennt gegen ihr vorheriges SDK-Archiv
gelinkter C-Probe bestätigt den Fehler im Dreieckssystem mit Zeilen
`(1,-1,1)`, `(0,1,0)`, `(0,0,1)` und rechter Seite `(1e308,1e308,1e308)`.
Die endliche Lösung `(1e308,1e308,1e308)` ergibt zuvor `PS_NUMERIC` und
unveränderte Ausgabe `(7,8,9)`. Die neue Regression scheitert am unveränderten
Stand in `run-1s9ckng5`; der separate Probe liegt in
`build/linear-range-before.c`/`.txt`. Auch die Elimination des Systems
`(1,1)`, `(-1,1)` mit rechter Seite `(1e308,1e308)` läuft früher über,
obwohl die Lösung `(0,1e308)` endlich ist. Die Ergebnisse aller 53 neuen
Eingabesysteme gegen das alte SDK sind unter
`build/linear-range-old-sdk-results.json` erhalten.

Die Koeffizienten bleiben zeilennormierte Double-Werte mit partieller
Pivotisierung. Jede rechte Seite und Teillösung erhält jetzt eine eigene
Mantisse/Exponent, kompensierte gewichtete Summen und eine abschließende
Rückskalierung. Das bewahrt auch gleichzeitig große und kleine Komponenten.
Die vorhandene Pivottoleranz und der Zustandserhalt auf allen Fehlern bleiben.
[Einheiten, Algorithmus und Grenzen](numerics.md) dokumentieren mögliche
Koeffizientenverluste bei der Normierung, schlechte Kondition, subnormale
Ergebnisrundung und die fehlende allgemeine Fehlerschranke.

Der unabhängige Prüfer löst 53 binäre Eingabesysteme durch rationale Elimination
und prüft zusätzlich ihre Residuen in C und tatsächlichem Physim. Die Systeme
haben 1–32 Unbekannte, starke Zeilenskalierung, Zeilentausch, Auslöschung,
Eliminationswachstum und echte Range-/Singularitätsfehler. Der separate C-Test
prüft Eingabeintegrität, Alias x=A/x=b, unabhängige Exponenten, subnormale Eingaben
und Zustandserhalt bei ungültigen Werten, Toleranz, Dimension oder Singularität.
Der erste breitere macOS-Lauf besteht mit 13/13 (`run-g253hgl5`); die endgültige
Prüfung ergänzt Dokumentation, Kit und zusätzliche C-Fehler-/Aliasfälle.

Die aktuelle Remote-CI der Ausgangsrevision
([C17 37666123958](https://github.com/PhysicSimulator/physim/actions/runs/37666123958),
[Linux-Paket 37666123932](https://github.com/PhysicSimulator/physim/actions/runs/37666123932))
läuft bei der ersten Abfrage noch. Die ältere fehlgeschlagene macOS-Intel-SDK-
Abnahme bleibt getrennt offen; die lokalen Nachweise ersetzen sie nicht.

### Gezielte endgültige Nachweise

Intel macOS/Apple Clang besteht **15/15** Release-Fälle in
`build/contact-world-language-release-mac/test-results/run-3eouq3yf`.
Debian/Clang Debug mit ASan/UBSan besteht **13/13** unter
`build/base-contract-asan-linux/test-results/run-j7kubmeb`. Die Läufe enthalten
Core, Numerics, sämtliche bestehenden Linear-Solve-Sprachfälle, beide neuen
C-/Physim-Prüfungen und die vollständigen ODE-Range-Referenzen. Die Release-
Auswahl enthält zusätzlich Referenz- und Kitprüfung. Eine lediglich unklare
Einrückung im stdin-Probe wurde anschließend korrigiert; die endgültigen
Gesamtläufe und SDKs verwenden diesen unverändert funktionalen Probe.

Die zusätzliche Linux/Clang-Debug-ASan/UBSan-Auswahl für Mathematik,
Matrixinversionen und Transformationen besteht mit **15/15** in
`build/base-contract-asan-linux/test-results/run-bfl5a3y7`. Das ist erforderlich,
weil Matrixinversionen denselben linearen Solver verwenden.

Eine spätere Jobabfrage der Ausgangsrevision `03c42f6` bestätigt beide
Windows/ClangCL-Jobs erfolgreich. Windows/v143 Debug scheitert dagegen im
separaten Crash-Dump-Nachweis
([Job 112945682723](https://github.com/PhysicSimulator/physim/actions/runs/37666123958/job/112945682723));
die öffentliche Annotation nennt nur Exitcode 1. Die übrigen Jobs laufen
bei dieser Abfrage noch. Receipts: `build/linear-audit-prior-ci-jobs.json`,
`build/linear-audit-prior-failed-job.json` und
`build/linear-audit-prior-failed-annotations.json`. Dieser Remote-Fehler
bleibt getrennt offen; die lokalen Solver-/SDK-Prüfungen ersetzen ihn nicht.

### Kompakte Sprachfixture mit identischen Eingaben

Die erste explizite Fixture erzeugt 67725 C-Zeilen mit 12535867 Byte. Der
Linux/GCC-Compiler arbeitet damit nachweislich über 17 Minuten bei nahezu
100 % CPU und ungefähr 1,5 GB RSS. Die Fixture wurde deshalb strukturell
verkleinert: Eine kleine Tabelle eindeutiger binärer Koeffizienten und ein
ASCII-Indextext rekonstruieren dieselben 53 vollständigen Matrizen. Der
Referenzprüfer bestätigt die identische Hex-Eingabefolge beim Erzeugen und
prüft weiterhin sämtliche Lösungen und Residuen in beiden Sprachen.
Die kompakte Fixture erzeugt 2105931 Byte C-Code. Kein Testfall, Koeffizient,
Toleranzwert oder Solververhalten wurde entfernt oder vereinfacht.

Der überholte Linux-Build wurde gezielt wegen dieser Fixture-Änderung beendet
(Exit 143), nicht aufgrund eines Beobachtungstimeouts. Prozess-/Ressourcendaten
liegen in `build/linear-fixture-expanded-termination.json`, sein erzeugter Code
in `build/linear-fixture-expanded-linux.c`. Der ursprüngliche Log bleibt unter
`build/linear-range-targeted-linux.log` erhalten. Die Solverquellen sind während
dieser Umstellung unverändert geblieben. Die kompakte macOS-Referenzprüfung
besteht in `build/linear-fixture-check-mac/test-results/run-duz3g303` (1/1).

Das vollständige macOS-SDK mit der ersten Fixture besteht mit **403 erfolgreichen
Prüfbefehlen**, 449 SDK-Dateien und 95 Kit-Eingaben unter
`build/Linear systems SDK ä mac izmu2853`, verschoben unter
`proof/Native SDK ä 7n_29a1f/Relocated SDK ä`. Receipt:
`build/linear-range-sdk-mac-PASSED.json`. Seine 15 ursprünglichen eingefrorenen
Dateien bleiben im Receipt und in `build/linear-range-source-freeze-first.json`
erhalten. Die einzige anschließende funktionale Prüferänderung ist die kompakte
Fixture samt deren Erzeuger; die Implementierung bleibt identisch.

Die kompakte Fixture besteht anschließend gegen beide bereits geprüften
macOS-SDK-Archive in `build/Linear compact SDK ä mac 66zz_exu`. Alle sechs
zusätzlichen Befehle bestehen: Emission durch den ausgelieferten Compiler,
zwei unabhängige Consumer-Builds, zwei rationale C-/Physim-Prüfungen und ein
frisches Kit mit denselben 95 erfassten Eingaben. Das SDK-Manifest bleibt
unverändert SHA-256-validiert. Receipt:
`build/linear-compact-sdk-mac-PASSED.json`. Die aktualisierten 15 Dateien sind
in `build/linear-range-source-freeze.json` erfasst; die Plattformchronik wird
separat ergänzt.

Die endgültige kompakte Linux/GCC-Release-Auswahl besteht mit **15/15** unter
`build/contact-world-language-release-linux/test-results/run-wdkjpvr0`. Die
kompakte Sprachfixture besteht erneut unter Clang/ASan/UBSan (1/1) in
`build/base-contract-asan-linux/test-results/run-6vz08jmm`. Die vorherigen 13
Solver-/ODE-Sanitizerfälle und 15 Matrix-/Transformations-Sanitizerfälle bleiben
separate Nachweise. Alle vier Linux-Receipts sind zusätzlich auf dem Mac
als `build/linear-range-targeted-linux-results.json`,
`linear-range-asan-linux-results.json`, `linear-range-math-asan-linux-results.json`
und `linear-compact-asan-linux-results.json` erhalten.

### Abschließender macOS-Gesamtlauf

Der vollständige Intel-macOS/Apple-Clang-Release-Lauf besteht mit **636/636**
unter `build/contact-world-language-release-mac/test-results/run-g99aohmt`.
Er verwendet bereits die kompakte endgültige Fixture und enthält sämtliche
bisherigen Compiler-, Runtime-, Format-, Batch-, Lernpfad- und Projektfälle.
Zusammen mit den 403 vollständigen SDK-Prüfbefehlen, sechs ergänzenden
SDK-/Kit-Befehlen und den gezielten Sanitizerprüfungen belegt dies den
beschriebenen lokalen Umfang. Die 77 Fensterfälle wurden in diesem Schritt
nicht erneut vollständig ausgeführt. API/ABI 3, Wire 5, Snapshot 3, psrun 1,
Sprache 0.182.0 und die 27 Core-Module bleiben unverändert.

### Endgültiges vollständiges Linux-SDK

Das frische Linux-SDK mit der kompakten endgültigen Fixture besteht vollständig
unter `build/Linear systems SDK ä linux lk2mfuc5`, verschoben unter
`proof/Native SDK ä fl1nbqwq/Relocated SDK ä`. Alle **403 Prüfbefehle** enden
erfolgreich; 449 SDK-Dateien und 95 Kit-Eingaben sind SHA-256-erfasst. Der
Receipt `build/linear-range-sdk-linux-PASSED.json` stimmt exakt mit den 15
endgültigen Dateien in `build/linear-range-source-freeze.json` überein. Alle
Dateihashes bleiben auch lokal unverändert. Das vollständige Befehlslog liegt
zusätzlich auf dem Mac als `build/linear-range-sdk-linux-verification.log`.
Die C-/Physim-Systeme, beide Archive, zwölf kalte native Projektbuilds, Cache-
Wiederverwendung und alle bisherigen SDK-Gates bleiben in der Abnahme enthalten.

Der begrenzte Implementierungsnachweis zu PP-0360 umfasst damit die tatsächliche
Solverausführung und SDK-Verteilung auf Intel macOS und Debian. Der Plan bleibt
mit 26 implementierten, fünf unvollständigen und 500 ungeprüften Blöcken offen.
Die neue gesamte Remote-/Grafik-/Produktabnahme und die dokumentierten alten
Remote-Fehler bleiben gesonderte Anforderungen.

### Abschließender Linux-Gesamtlauf

Der vollständige Debian-12/GCC-Release-Lauf besteht mit **636/636** unter
`build/contact-world-language-release-linux/test-results/run-iubzzpfg`.
Der zusätzlich lokal erhaltene Receipt ist `build/linear-range-full-linux-results.json`.
Er enthält die endgültige kompakte Fixture und sämtliche bisherigen Compiler-,
Runtime-, Format-, Runner-, Batch-, Domänen- und Projektprüfungen. Zusammen mit
dem vollständigen Linux-SDK, dem vollständigen macOS-Gesamtlauf, dem macOS-SDK
samt kompakter Ergänzung und den getrennten Sanitizerprüfungen belegt dies
den dokumentierten Umfang. Die Originaltexte und SHA-256 des vollständigen
Projektplans bleiben erhalten; die gesamte Projektabnahme bleibt offen.

## Expliziter 2D-Ebenenmodus (PP-0295)

Die tatsächliche App bietet unter Darstellung einen 2D-Modus. Vorne/Seite/Oben
wählt XY/YZ/XZ; orthografische und perspektivische Projektion sind separat
wählbar. Rechte/mittlere Maustaste und Alt-Pfeile verschieben parallel zur
Ebene, das Mausrad zoomt. Exakte Ebenenachsen vermeiden senkrechte Drift durch
trigonometrische Rundung. Alt+2 toggelt ohne Wiederholung; Standard/Alt+Pos1
kehrt zur freien 3D-Kamera zurück. Die Ebene und die passenden Gesten sind im
Kamerabereich sichtbar beschrieben.

Intel-macOS-/AppleClang-Release-Nachweise:

- Bisherige Diagramm-/3D-Tastatur- und neue Kameraabläufe **4/4**, `run-98768cid`,
  `build/current-camera-plane-final-ui-mac.log`.
- Verstärkte tatsächliche 16/22-Pixel-Kameraprüfung **2/2**, `run-cmv8u5_y`,
  `build/current-camera-plane-rendered-ui-mac.log`: wirkliche Maus-/Tastaturwege,
  alle drei Ebenen, beide Projektionen, feste Normalenkoordinate, Zoom, Home,
  Modifier-/Editor-/Analyseisolation und bytegleiche Szene/Messwerte. Eine
  anfänglich noch aktive Archivansicht wurde im Prüffixture korrigiert;
  Rendererstatistik fordert jetzt tatsächlich gezeichnete Kugel-/Vektorgeometrie.
  Die aufgenommenen Bilder wurden visuell geprüft.
- SDK-Installation und Dokumentations-/Kit-/Identitäts-/Seriengruppen **5/5**,
  `run-dia9e5_y`, `build/current-camera-plane-install-mac.log`.
- Unabhängiges **204**-Dateien-Kit, verschobenes SDK und tatsächliche 16/22-Pixel-
  App-Bedienung **2/2** Befehle, `build/current-camera-plane-sdk-verify-mac.log`,
  Verbraucher `build/current-camera-plane-sdk-consumer-mac/Native SDK ä byx4eyds`.
  `--camera-only` prüft diesen UI-Vertrag gezielt; vollständige SDK-Prüfungen
  führen die beiden Kamerafälle ebenfalls aus.

PP-0295 ist einzeln umgesetzt/geprüft. Die ganze aktuelle Plattformabnahme
bleibt offen: **123 implemented**, **14 incomplete**, **394 unverified**,
531 unveränderte Originaltexte/Planprüfsumme, `completion_proven=false`.

Abschließende Dokumentations-/Referenz-/Kitprüfung: **3/3**, `run-cgnrc5bf`,
`build/current-camera-plane-final-docs-mac.log`.
