# Windows-Fokus: erneute Prüfung am 21. Oktober 2026

Nach der Nutzerfreigabe für ein ungestörtes Testfenster wurde die aktuelle native
Windows-Anwendung erneut geprüft. Alle **24 F1-Bereichszustände** bestehen,
Prozess-Exitcode **0**. Der getrennte, ausschließlich lesende Win32-Beobachter
bestätigt das Dokumentationsfenster gleichzeitig als Vordergrundfenster,
aktives Fenster und Tastaturfokus des tatsächlichen GUI-Threads.

Der erste vollständig bestätigte Dokumentationsfokus liegt bei 5,985 Sekunden.
Die letzte Zustandsbeobachtung liegt bei 25,375 Sekunden; der Test benötigt damit
keine vollen 90 Sekunden. Danach kann der Nutzer wieder Fenster wechseln.

Die App-Matrix erzeugt ihre SDL-Testereignisse. Der externe Beobachter aktiviert
keine Fenster und injiziert keine Betriebssystemeingaben. Er speichert Änderungen
abgetasteter Fensterzustände; daraus folgt kein eigener externer Fokusnachweis
für jeden einzelnen der 24 Fälle oder eine durchgehende 90-Sekunden-Messung.
Frühere sporadische Aktivierungsfehler sind durch diesen Erfolg nicht ursächlich
erklärt oder dauerhaft behoben.

Geprüft wurde `build/native/Design-analysis/bin/physim.exe` mit SHA-256
`e513297f5f21d682f67a7bccddc08a6c6acbc7700295df2a097a5663fe41998c`.
Die Quellbasis entspricht dem geprüften Grafikdatenbaustein aus `75eb51f`;
die F1-Darstellung bleibt auf dem Stand der
[Kontextdetails](CONTEXT-DETAILS.md). Zehn bestehende lokale Accessibility-
Änderungen sind separat gehasht und enthalten. Dies ist kein Nachweis eines
sauberen Checkouts ausschließlich des genannten Commits.

[Befehl, rohe Beobachtungen, App-Ausgaben, Quellen und Hashes](windows-focus-2026-10-21/validation.json).
Es gibt dadurch keine neue praktische Screenreader-Abnahme, keinen physischen
DPI-/Monitornachweis und keine vollständige Designabnahme. Die
[gesamte native Umsetzung](../unified-workspace/NATIVE-FIDELITY.md) bleibt offen.
