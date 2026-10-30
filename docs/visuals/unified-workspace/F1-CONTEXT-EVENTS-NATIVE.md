# F1-Kontextcode: vollständig initialisierte Testereignisse

Stand: 30. Oktober 2026. Dies korrigiert einen Fehler im nativen Testprogramm,
keine nachgewiesene Beschädigung echter Tastaturereignisse durch die Anwendung.
Der [kompakte Nachweis](../../verification/f1-context-events-2026-10-30.json)
verbindet Originalfehler, Rohdaten, Korrektur und tatsächlich ausgeführte Prüfungen.

## Ursache und Änderung

Der Linux-Lauf mit AddressSanitizer und UndefinedBehaviorSanitizer scheiterte
im vierten Kontextcodefall: `SDL_KeyboardEvent.repeat` enthielt 159. Die Diagnose
liest die Darstellung der Bool-Felder ausschließlich als Bytes. Bereits vor
`SDL_PushEvent` stehen `repeat=159` und `down=220` im künstlichen Shift-Ereignis.
Nach dem Einreihen und nach `SDL_PollEvent` sind dieselben Werte vorhanden.
SDL-Header und geladene Bibliothek sind jeweils 3.2.30. Eine unterschiedliche
SDL-Version erklärt diesen Befund nicht.

Die bisherige Initialisierung `SDL_Event e={0}` stellt im beobachteten nativen
Build nicht die benötigten Nullbytes des anschließend verwendeten Union-Members
bereit. Auch andere nicht ausdrücklich gesetzte Ereignisfelder enthalten
Restbytes. Deshalb initialisiert der Kontextcode-Test nun alle vier Konstruktoren
für Mausbewegung, Mausrad sowie Shift-Drücken/-Loslassen mit `SDL_zero`.
Das Drücken setzt zusätzlich `key.down=true`.

Der Anwendungscode zum Lesen und Verarbeiten der Eingaben bleibt unverändert.
Es gibt weder eine Bool-Wertbegrenzung noch eine abgeschaltete Sanitizerprüfung
oder gelockerte Fokus-, Scroll-, Zwischenablage- und Bildprüfung.

## Prüfung und Grenzen

| Umgebung | Tatsächliches Ergebnis |
| --- | --- |
| Linux Debug, ASan/UBSan, normale Prüfung | Bestanden: alle sieben Kontextcodefälle |
| Windows Release | Build erfolgreich; Prüfung weiterhin bei Fall 0 am echten Fensterfokus gescheitert |
| Windows Computer Use | Zwei leichte Verbindungsversuche liefen in eine Zeitüberschreitung; kein Eingabenachweis |

Die sieben Linux-Fälle umfassen beide Dokumentationssprachen, Hell/Dunkel/hohen
Kontrast, 16/22 px, vollständigen langen UTF-8-Code sowie Ende/Anfang,
Mausrad/Umschalt-Mausrad, horizontales Ziehen und Kopieren. Die unveränderte
Prüfung kontrolliert außerdem die Bilder und die unveränderten Editorinhalte.
Das ist keine vollständige F1-, Screenreader-, Monitor-/DPI- oder macOS-Abnahme.
Der Windows-Befund bleibt offen; logischer Dokumentationsfokus ersetzt dort
keinen nachgewiesenen nativen Tastaturfokus.

## Erneute Diagnose

Die optionale Linux-Probe [trace_sdl_shift_events.c](../../../tests/trace_sdl_shift_events.c)
protokolliert nur Shift-Tastenereignisse und leitet alle Ereignisse unverändert
weiter. Sie gehört nicht zum normalen Anwendungsbuild. Beispiel aus dem
Projektverzeichnis:

```sh
clang -shared -fPIC -std=c17 -O0 -g -Wall -Wextra -Werror \
  -I build/dependencies/SDL3-linux-3.2.30/include \
  tests/trace_sdl_shift_events.c -ldl -o build/event-trace.so
```

Bei einem ASan-Programm muss dessen tatsächliche ASan-Laufzeit im
`LD_PRELOAD` vor der Diagnosebibliothek stehen. Die verwendeten Befehle,
Bibliothekspfade und Byte-Hashes stehen im Nachweis. Die vorangegangenen
ungültigen Diagnoseversuche (fehlende Probe bzw. falsche ASan-Ladereihenfolge)
sind dort getrennt erfasst und belegen keine erfolgreiche Ereignisbeobachtung.

Die eingefrorenen A2-Nachweise werden nicht verändert. Die gesamte Oberfläche,
alle bereits freigegebenen Designs und die praktischen Plattformprüfungen
bleiben Teil des laufenden Auftrags.
