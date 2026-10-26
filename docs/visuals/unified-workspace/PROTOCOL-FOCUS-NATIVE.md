# Protokoll C – Fokusnachlauf vom 26. Oktober 2026

Prüfstand dieses Berichts: Phasen 16–19. Den späteren Anschluss echter Diagnosen
und die isolierte Prüfung der versionierten Umsetzung dokumentiert der
[Diagnose- und Integrationsbericht](PROTOCOL-DIAGNOSTICS-NATIVE.md).

Die verschärfte native Tastaturprüfung verlangt jetzt eine Fokusübergabe aus
einem tatsächlich aktivierten Suchfeld zur Höhenleiste. Der alte Eingabeweg
verliert diesen Fokus; die Korrektur im SDL-Adapter besteht die Windows-Prüfung.
**Die direkte Mausprobe ist weiterhin nicht abgenommen:** Nach dem Anklicken
der Höhenleiste meldet die zugängliche Oberfläche weiterhin das Suchfeld als
Fokusziel. Der native Arbeitsstand bleibt unveröffentlicht.

Die [Ergebnisliste](../../verification/protocol-focus-2026-10-26.json) verknüpft
die tatsächlichen Prozesse, Quell-/Binärhashes und Originalberichte. Der
[vorherige Teilbericht](PROTOCOL-C-NATIVE.md) und dessen eingefrorene JSON-Datei
behalten ihren damaligen Geltungsbereich. Neue Ausgaben liegen ausschließlich
unter `D:/Physim/build/validation/protocol-ui-2026-10-25`.

## Ursache und begrenzte Korrektur

Die Höhenleiste liegt in einem eigenen Nuklear-Fenster. Bei einer gewährten
Bereichsfokussierung beendete `accessibility_range_widget` nur dessen
Editiercache. Ein aktives Suchfeld in einem anderen Fenster konnte beim nächsten
Rendern den Modellfokus erneut übernehmen. Der Adapter beendet bei dieser
Übergabe jetzt die bestehenden Fenster-Editiercaches und den aktiven Texteditor.
Der sichtbare Aufbau wird dadurch nicht geändert.

Die erste Korrektur im nachgelagerten Zeichenweg genügte nicht. Die gezielte
Gegenprobe mit aktiver Suche scheitert in Phase 17, Lauf `run-x_mu1imn`, bei
Schritt 39: `actual=1 separator=0`, zuvor `search=1`. Die Adapterkorrektur in
Phase 18 besteht denselben Ablauf einschließlich Shift+Auf um 40 Pixel.
Eine spätere Ergänzung fordert den Fokus bereits beim Beginn einer Mausziehung
an. Die direkte Computer-Use-Probe bestätigt damit noch keine vollständige
Korrektur des Mausklicks; diese Grenze bleibt ausdrücklich offen.

## Tatsächliche Ergebnisse des Nachlaufs

| Stand / Prüfung | Windows | Linux |
| --- | --- | --- |
| Phase 16, Navigator ohne erzwungene aktive Suche | 8/8 bestanden | 8/8 bestanden |
| Phase 16, echte Protokoll-Datenquellen | bestanden | nicht erneut ausgeführt |
| Phase 17, aktive Suche vor Bereichsfokus | Fehler Schritt 39 | nur gebaut |
| Phase 18, Adapterkorrektur | 8/8 bestanden | nur gebaut |
| Phase 19, aktueller Navigator mit verschärfter Fokusprüfung | 8/8 bestanden | 8/8 bestanden |
| Phase 19, bisheriger Diagnoseablauf | Fehler Schritt 2 | nicht erneut ausgeführt |

Im fehlgeschlagenen Diagnoseablauf ist die echte Compilerdiagnose aus
`helper α.h` bereits vorhanden (`diagnostics=3 selected=2`). Vor dem eigentlichen
Quellsprung verlangt der alte Test ein nichtleeres `diagnostic_bounds`-Rechteck
der bisherigen Anzeige. Diesen Ablauf auf die tatsächliche Auswahl der
Navigator-Meldung und den expliziten Quellsprung umstellen; Originaldiagnose,
Datei, Zeile, Reparatur und anschließenden echten Lauf weiterhin prüfen.
Der fehlgeschlagene Test beweist noch nicht, dass diese neue Benutzeraktion
funktioniert oder fehlschlägt.

Die Laufzeitarchive des aktuellen Stands in Phase 19 wurden vor weiteren
Builds vollständig nachgelesen und gegen die verwendeten Programme geprüft.

## Direkte Bedienung mit Computer Use

Verwendet wurde der lokale
`computer-use/26.1021.31345/skills/computer-use/SKILL.md` mit `@oai/sky`,
jeweils am eindeutig ausgewählten Physim-Testfenster. Der neue Modus
`--workspace-state-test <neuer D:-Prüfordner> protocol-ui-interactive` verwendet
isolierte Einstellungen, ein Prüfprojekt und ausdrücklich markierte Prüfmeldungen.
Er lässt normale Eingaben zu, erzeugt keine automatischen Bilder und endet
spätestens nach 900 Sekunden. Die Fenster wurden regulär geschlossen; Exitcode 0.

- Phase 16: Eine echte Ziehung von `(720,632)` nach `(720,602)` vergrößert den
  Bereich von 340 auf 370. Zugänglicher Wert und nativer Zustandsbericht stimmen
  überein. Das belegt diese abgeschlossene Ziehung, keinen Escape-Abbruch während
  einer physisch gehaltenen Maustaste.
- Die getrennten Fokusproben der Phasen 17–19 zeigen nach dem Klick weiter
  „Meldung oder Quelle“. Für Phase 19 bleibt auch nach Shift+Auf der beobachtete
  Höhenwert 340. Kein bestandener physischer Tastaturnachweis daraus ableiten.
- Die Fenstermethode benötigt teilweise eine neue Beobachtung nach der Aktion;
  die direkte Rückgabe kann noch den alten Zustand zeigen. Daher wurde vor
  weiteren Eingaben erneut beobachtet. Die Proben wurden nicht allein anhand
  der unmittelbaren Aktionsantwort bewertet.

Es wurden keine weiteren Bildschirmdateien für diesen Bericht gespeichert.
Die kompakte Ergebnisliste trennt manuell beobachtete Zustände von automatisch
aufgezeichneten nativen Zustandswechseln.

## Nächster prüfbarer Schritt

Vor einer weiteren Änderung den Rückgabewert der Maus-Fokusanforderung und die
Fokus-/Pending-Kennungen vor der Trennleistenregistrierung, danach und nach
Veröffentlichung erfassen. Damit zwischen abgewiesener Anforderung, erneuter
Fokusübernahme und abweichender nativer Veröffentlichung unterscheiden.
Erwartete Bewegungswerte und Host-Fokusvoraussetzungen beibehalten.

Die ältere Prüfung mit echten Compiler-/Laufmeldungen besteht unter Windows in
Phase 16 (`protocol_sources`, Lauf `run-1otd0nhd`). Die bisherigen Diagnoseabläufe
mit implizitem Zeilen-Quellsprung und altem Rohtextschalter müssen weiterhin
gegen die neuen expliziten Navigatoraktionen geprüft werden. Bildschirmbedienung,
Dateidialog/Abbrechen, Zwischenablage, Screenreader, macOS und vollständige
Designgleichheit bleiben getrennte Abnahmepunkte.

## Nachweisgrenze der Zwischensicherung

Die Laufzeitarchive der Phasen 16 und 17 wurden vollständig nachgelesen und
hashgeprüft. Der Sicherungshelfer für Phase 18 las eine UTF-8-Quelldatei zunächst
mit der Windows-Standardkodierung; dadurch wurde der Umlaut im Prüfpfad falsch
interpretiert. Der bereits gestartete Folgebuild ersetzte anschließend die
Programmdatei. Die korrigierte Sicherung wies den abweichenden Binärhash ab,
anstatt den neuen Build als alten Nachweis auszugeben. Phase 18 behält Quellen,
Prüflogs und den tatsächlich gemessenen Binärhash, aber kein separates
Laufzeitarchiv. Diese Lücke nicht als vollständige eingefrorene Programmsicherung
darstellen.
