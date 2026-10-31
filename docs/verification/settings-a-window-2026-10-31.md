# Einstellungen A: eigenes natives Kategorienfenster

31. Oktober 2026. **Geprüfter nativer Teilstand, keine vollständige Abnahme.**
[Implementierung und offene Bestandteile](../visuals/unified-workspace/SETTINGS-A-NATIVE.md),
[Kommandos, Quell-/Runtime-Manifeste, Hashes, ursprüngliche Fehler und Ergebnisse](settings-a-window-2026-10-31.json).
Die bisherigen eingefrorenen Nachweise bleiben unverändert. Der frühere
[Tab-6-Bildbefund](settings-a-native-gap-2026-10-31.md) beschreibt seinen damaligen
Stand; der neue Renderer verwendet ein eigenes nichtmodales OS-Aufgabenfenster.

## Ergebnis und tatsächlich geprüfter Umfang

Eigene Umsetzung auf Basis `fa97b315`, 141 erfasste eigene Eingaben in einem
isolierten Compilerbaum. Fremde AT-SPI-/Orca-Arbeit wurde nicht übernommen.
Neuer eingefrorener Anschluss: **`settings-a-window-visual`**. Pro Anschluss
215 Windows-/113 Linux-Runtimedateien, alle referenzierten Archivbytes tatsächlich
zurückgelesen. Nicht aus einem alten Anschluss überschreiben.

| Normaler Aufruf | Windows Release MSVC | Linux Debug GCC ASan/UBSan |
| --- | --- | --- |
| `settings-a-window-ready`: sieben registrierte Gruppen | **7/7**, `run-hxya7sl3` | **7/7**, `run-8qt40qsk` |
| Anschließende reine Bildkorrektur: `settings_a_window`, je zwei App-Prozesse | **1/1**, `run-mgnwir2w` | **1/1**, `run-cv0mpuxb` |

Die sieben Gruppen sind `settings_transactions`, `settings_a_window`,
`settings_keyboard_workflow`, `ui_typography_workflow`, `themes_workflow`,
`keyboard_menu_small` und `keyboard_menu_large`. Linux verwendet X11/Xvfb,
Openbox, Software-GL und `detect_leaks=0:halt_on_error=1`; dies ist keine neue
Leak-Abnahme. Windows läuft mit tatsächlichem SDL/OpenGL und NVIDIA-Treiber.

Die Fensterprüfung verwendet zwei tatsächliche App-Prozesse: breites dunkles
Fenster bei UI 16 und schmales helles Fenster bei UI 22. Sie prüft verschiedene
OS-Fenster-IDs und eigene Eingabe-/Renderkontexte, anfänglichen Suchfokus,
tatsächlich empfangenen Text **SCHRIFT**, Ergebnisse über zwei Kategorien und
erhaltene Suche beim Wiederaufruf. Ein Befehl im Hauptfenster wechselt parallel
nach Simulieren, ohne das Aufgabenfenster oder dessen Entwurf zu schließen.
Themeänderung bleibt zunächst Entwurf; die Änderungsprüfung belegt die volle
Tabelle. Escape geht zurück und verwirft anschließend. OS-Schließen verwirft
ebenfalls. Eine tatsächlich eingegebene Seitenbreite **139** wird samt Fehler
behalten, auf **300** korrigiert, gespeichert und erneut gelesen. Die aktuelle
Simulieren-Seitenbreite bleibt dabei unverändert.

Die Transaktionsgruppe erhält spätere Geometrie/Sichtbarkeit, beide Workspace-
Sitzungen und das bisherige Preferences-File bei einem tatsächlich fehlgeschlagenen
Dateiersatz. Wiederholung speichert Entwickeln-Ausgangswerte; diese bleiben auch
nach Programmende korrekt. Die Tastaturgruppe verwendet die sichtbare Kategorien-
und Aktionsfolge statt alter Tab-6-Radios. Typografie umfasst zwölf tatsächliche
App-Unterprozesse mit Schreiben/Neulesen, Abbruch und Defaults; Themeprüfung
verwendet sieben App-Unterprozesse. Die beiden F10-Menüfälle schließen nun das
eigene Einstellungsfenster, bevor sie die Bedienung im Hauptfenster fortsetzen.

Nach dem 7/7-Nachlauf wurde nur der kompakte Änderungszähler einzeilig gezeichnet,
Singular ergänzt, CSS-Auswahltransparenz übernommen und der Zurück-Button normal
dargestellt. Dafür wurde gezielt die Fenstergruppe wiederholt. **Keinen neuen
vollständigen Siebenerlauf mit der letzten Binärdatei behaupten.** Je Betriebssystem
bleiben drei PNGs für breite Ansicht, schmale große Schrift und Änderungsprüfung
als gezielte Referenz erhalten. Der Zähler bleibt innerhalb seines Feldes.
Dies ist kein Nachweis einer pixelidentischen gesamten Anwendung.

## Ursprüngliche Fehler und Ursachen

Alle Zwischenphasen bleiben im JSON mit ursprünglichem Status erhalten:
`first` je 0/2, `focus` je 3/5, `review`, `input` und `edit` je 6/7.
Sie werden nicht nachträglich zu bestandenen Aufrufen erklärt.

Der erste Nachlauf zeigte außerdem einen echten Persistenzfehler: Beim Beenden
überschrieb die aktuelle Protokollhöhe des anderen Workspaces den gerade
gespeicherten Entwickeln-Ausgangswert. Ausgangswert und aktuelle gemeinsame
Sitzung werden jetzt ausdrücklich getrennt; bewusstes späteres Resizing oder
Umschalten hat Vorrang. Die Protokoll-Trennlinie selbst wurde nicht verändert.

Bei der Suche kamen SDL-Ereignis, Fenster-ID und die sieben Bytes SCHRIFT korrekt
an; `SDL_TextInputActive` war wahr. Trotzdem blieben Nuklear-Fenster/Layout auf
**4128 = NO_SCROLLBAR | ROM**. Passiver Hover auf dem Header hatte den aktiven
Bereich gewechselt. Aktivieren nur vor `nk_begin` reichte nicht. Die tatsächliche
fokussierte Eingabefläche wird jetzt nach Bereichsbeginn und vor dem Edit gezielt
aktiviert und entsperrt. Ein GDB-Gegenversuch ergab Flags **32** und den realen
Suchtext. Debugger-veränderte Abläufe zählen ausschließlich als Ursachenbeleg,
niemals als normale bestandene Prüfung.

Nach dieser Produktkorrektur waren zwei Fixture-Fehler sichtbar: Die Prüfung
lief vor dem nächsten Workspace-/Review-Layout; außerdem wurden Ctrl+A und
Backspace in einem Frame nach Nuklear-Tastenreihenfolge abgearbeitet. Begrenztes
Warten auf das tatsächliche Layout und getrennte Eingabeframes bilden nun die
Benutzerfolge ab. Die Texterwartung wurde nicht gelockert. Die Theme-Fixture
verwendet für Auswerten die bereits bestehende freigegebene helle Werkbankfläche
246/247/249 statt eines alten globalen Hintergrundwerts.

## Offene Abnahmen und Anschluss

Eigene Einheitenaufgabe und Completion-D-Preferences sind noch umzusetzen.
Historische `settings_workflow`-/AX-Fixtures brauchen eine Portierung unter Erhalt
ihrer Wiederherstellungs-/Geometrie-/Maximieren-Abdeckung. Sie gehören nicht zur
obigen grünen Siebenergruppe; die gesamte GUI-Suite ist nicht abgenommen.
Die zwei Suchrücksetz-Aktionen teilen derzeit Fokus-ID 26; den Tastatureigentümer
im leeren Ergebniszustand gezielt nachprüfen. Weitere visuelle Feinheiten sowie
Screenreader, physische Eingaben, DPI und Monitorwechsel bleiben offen.

Der Nutzer-Mac ist ausgeschaltet und wurde nicht angesprochen. Neue gehostete
Intel-/Apple-Silicon-Läufe sind erst nach tatsächlich gelesenem Ergebnis ein
macOS-Nachweis; eine ergänzte Workflow-Datei allein beweist nichts.

Nach Ende aller Build-/Test-/Archivhandles wurden **656 redundante erfolgreiche
BMP-Rohbilder, 2.093.854.368 Bytes**, entfernt. Pfade, Größen und
Hashes wurden vor Löschung vollständig geprüft; Ergebnis und genaue Dateiliste
stehen unter `build/validation/settings-window-2026-10-31/cleanup-{plan,result}.json`.
Fehlgeschlagene Fälle, Settings-A-PNGs, bisher geschützte Referenzen, kompakte
Berichte, Quellen, Runtime-Archive und benötigte Symbole bleiben erhalten.
