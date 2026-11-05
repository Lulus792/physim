# F1-Schriftgrößen: Fehlerprobe im tatsächlichen Einstellungsfenster

Der bisherige Test rief `settings_commit` bei geschlossenem Einstellungsfenster
auf. Seit der Umstellung auf eigenständige Aufgabenfenster kehrt dieser Aufruf
sofort zurück. Die behauptete Allokationsfehlerprobe fand dadurch nicht statt.
Windows Release und Alpine/X11 ASan/UBSan reproduzierten denselben Fehler an
Stufe3 mit unveränderten bisherigen Programmen.

Die Probe öffnet jetzt das reale, separate Settings-Fenster und löst denselben
begrenzten Allokationsfehler aus. Mindestens drei tatsächliche Anfragen müssen
erreicht sein. Sie prüft weiterhin die unveränderte Präferenzdatei, aktive
Schriftgrößen, alten Fonts, Leseversion, GL-Kontext und vollständige Freigabe der
fehlgeschlagenen Fonttransaktion. Das Fenster bleibt mit Fehler und Entwurf offen;
nach einem normalen Zeichenframe wird es abgebrochen. Die vorhandenen Prüfungen
für Wiederholung, unabhängige UI-/Codegrößen und Dichtewechsel bleiben erhalten.
Der unabhängige Python-Prüfer verlangt den neuen Nachweis des tatsächlich
geöffneten Fensters und der erreichten Allokationen.

Nach der Korrektur bestehen auf **Windows Release und Alpine/X11 ASan/UBSan je
3/3 Gruppen**: Einstellungen per Tastatur, F1-Zoom und Fontbedarf. Alle306 eigenen
Eingaben wurden vor Build zwischen Projekt, Isolation und Manifest bytegleich
geprüft. Nur zwei Testdateien geändert; Produktlayout und Fontimplementierung
blieben erhalten. [Befehle, Hashes und Originalergebnisse](font-settings-fault-2026-11-05.json).

Zusätzlich wurden beide vollständig abgeschlossenen Linux-CI-Joblogs zur Quelle
01d2c0d0 gelesen: GCC127467266200 und Clang127467266784, Lauf42090594921. Beide
erreichen **26/26 Daten und46/48 UI**. Offen sind dort nur diese alte Fontprobe
und die zugängliche Fehlermeldung beim ungültigen F1-Link. Die Exportabläufe und
der zuvor einmal gescheiterte allgemeine AT-SPI-Fokus bestehen in beiden neuen
Jobs. Die großen Original-ZIPs wurden hierfür nicht zusätzlich heruntergeladen;
die Aussage beruht auf den vollständig gelesenen und lokal gesicherten Joblogs.

Die neue glibc-Ausführung der Fontkorrektur, vollständige assistive Bedienung und
physische Eingabe bleiben offen. Linux-GUI-ASan/UBSan lief mit `detect_leaks=0`
für die externe Grafiklaufzeit; daraus folgt keine LeakSanitizer-Abnahme.
