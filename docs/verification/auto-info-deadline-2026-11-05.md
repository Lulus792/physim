# Automatische Kurzinfo: Fristrennen im Prüfer · 5. November 2026

Der native Automatikprüfer wartet jetzt das Ergebnis einer bereits vorgemerkten
Kurzinfo ab, bevor er gegebenenfalls Strg+Leertaste plant. Die App-Optik,
Produktionssteuerung und sämtliche bisherigen Daten-/Undo-/Geometrieassertionen
bleiben erhalten. Keine Fristverlängerung: weiterhin 3 s je Prüfstufe und
60 s im vorhandenen `completion-*`-Watchdog.

Quellcommit **`0c525422`** in beiden Repositories. Alle erfassten 1.124 Compiler-/
Testdateien und 174 eigenen Eingaben gegen tatsächliche Git-Blobs geprüft,
nur CRLF/LF normalisiert. [Commitabschluss und offene CI-Handles](auto-info-deadline-commit-2026-11-05.json).

Auslöser war der gehostete ARM-Befund zu `f01f25c1`: Automatikablauf an Stufe 71,
Editorgruppe 5/6; [Originalarchiv und Einzelresultate](completion-info-macos-2026-11-05.json).
Der damalige Fehlertext enthält keine Kurzinfo-Mode-/Fristdaten. Die folgende
Gegenprobe beweist ein Rennen im selben Prüfer; sie ist keine nachträgliche
Aufzeichnung des ursprünglichen ARM-Ereignisablaufs.

## Tatsächliche Gegenprobe

`completion-auto-deadline-c/phys` nutzt den echten Controller und die echten
SDL-Eingaben. Nur in diesem Testmodus wird eine Auswahlvormerkung genau in der
Zeichnung zwischen Zustandsprüfung und Verarbeitung der geplanten Taste fällig.
Kein künstliches Setzen von Sichtbarkeit oder Kartenbounds, keine Quelländerung.

Mit dem vorherigen Prüfer ergibt sich tatsächlich:

| Beobachtung | Kurzinfo | Wirkung |
| --- | --- | --- |
| Vor Zeichnung, Stufe 70 | pending, unsichtbar | Prüfer plant Strg+Leertaste |
| Nach Zeichnung, Stufe 71 | sichtbar, 330 × 260 | Frist ist inzwischen abgelaufen |
| Nach Verarbeitung der Taste | verborgen | Der geplante Umschalter blendet sie aus |

Auswahl 648, Übernahmebereitschaft, Cursor 0, leerer Quelltext und beide
Scrollpositionen 0/0 bleiben gleich. Damit kein Viewportdrift in dieser
Gegenprobe; die frühere Vermutung wird dafür nicht bestätigt. Windows C erreicht
reproduzierbar denselben Fehler an Stufe 71: **0/1**, `run-06uwoald`.
Physim und Linux wurden in dieser negativen Phase nicht ausgeführt.

Die Korrektur ist die zusätzliche begrenzte Zustandsbedingung
`completion_info.mode != PS_COMPLETION_INFO_PENDING` vor dem bedingten
Umschaltbefehl. Danach wird das wirkliche Zeichnungsergebnis geprüft. Kein fester
Schlaf, kein wiederholtes blindes Umschalten. Gezielte Variante meldet sowohl
die tatsächlich injizierte Frist als auch den beobachteten Pending-Zustand.

## Abgeschlossene lokale Ausführung

[Originalkommandos, Quellen-/Binärhashes, Marker, Zustandsfolgen und Grenzen](auto-info-deadline-2026-11-05.json).

| Phase | Windows Release/MSVC | Linux Debug/GCC ASan/UBSan |
| --- | --- | --- |
| Korrektur | 3/3, `run-wrqzdvpk` | 3/3, `run-s2dwpvv4` |
| Abschluss mit bereinigter Registrierung | 3/3, `run-5wx3me0m` | 3/3, `run-s96zafnf` |

Jeweils normaler Automatikablauf, gezielte Fristvariante und Kurzinfo-Bedienfolge;
je zwei echte App-Prozesse für C/Physim pro Gruppe, insgesamt sechs je Plattform
und Phase. Alte Enter-/Tab-/Escape-/Undo-/IME-/Präfix-/Fokus-/Viewport-/Scroll-
Prüfungen bleiben erhalten. Kurzinfo-Gruppe deckt weiterhin die drei Editor-/
Theme-Kontexte samt echten Zeichnungsbeobachtungen ab. Linux-GUI wie bisher mit
`detect_leaks=0`, daraus keinen neuen Leaknachweis ableiten.

Die App-SHAs beider positiver Phasen sind je Plattform gleich. Nach der ersten
Registrierung wurde die ursprüngliche kompakte JSON-/Unicode-Darstellung
wiederhergestellt: nur ein neuer Eintrag, alle dekodierten übrigen Nutzdaten
nachweislich gleich. Mit diesen endgültigen Eingaben wurden die drei Gruppen
nochmals tatsächlich ausgeführt. Alle lokalen Handles beendet; Quell-/Laufzeit-
und referenzierte Archivbytes gelesen, Anschluss `auto-info-deadline-final`.

Registrierungs-/Harnessprüfung tatsächlich Exit 0, Verträge für win32/Linux/
Darwin gelesen. Ihre absichtlichen Mock-Build-/Exit-/Timeoutfehler sind keine
Produktfehler oder Mac-Abnahme. Eine anfängliche Setup-Verwechslung des JSON-
Schlüssels scheiterte vor dem Schreiben; die daraus vorbereitete erste Kopie
wurde nie ausgeführt. Kein Erfolg oder Produktfehler daraus abgeleitet.

Kein neues Bild erzeugt; Diagnose besteht aus Zustandsübergängen. Keine
Produktionsrenderer geändert und keine geschlossenen manuellen/API-/F1-/
Protokoll-/Settings-Matrizen erneut als neue Abnahme ausgegeben. Keine pauschale
Objekt-, Abhängigkeits- oder VM-Bereinigung. Nur D:, kein Zugriff auf F:.

## Weitere Abnahme

Die gehostete Editorgruppe enthält nun zusätzlich die Fristvariante: sieben
Gruppen je Mac-Runner. Lauf **`41935266700`** auf `0c525422` inzwischen tatsächlich
abgeschlossen: **ARM und Intel je 7/7**. Beide Archive/Digests und alle Original-
einträge gelesen; [gesonderter Abschluss](auto-info-deadline-macos-2026-11-05.json).
Keine Abnahme der nachfolgenden lokalen Kandidaten daraus ableiten.
Der Nutzer-Mac bleibt ausgeschaltet;
kein Zugriff oder Erreichbarkeitstest. Der historische ARM-Vorgang bleibt ohne
zusätzliche damalige Telemetrie begrenzt erklärbar.

Das ist ein korrigierter Prüfer, keine vollständige D-/UI-Abnahme. Native lokale
Namen/Quellstellen-Karte, Einfügen/Undo/leere Argumentauslöser, echte C-Scopes,
separate Aufrufhilfe und physische/assistive/DPI-/Monitor-/Vollbildabnahme bleiben
offen. Der [geprüfte Physim-Parserbaustein](language-scopes-2026-11-05.md) ist
weiterhin noch nicht an die Vorschlagsliste angeschlossen.
