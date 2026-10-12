# Physim in SecondBrain

Physim enthält unter `brains/physim` ein direkt lesbares Projektgedächtnis für
[SecondBrain](https://github.com/Lulus792/SecondBrain). Es verbindet Auftrag,
geprüften Stand, Entscheidungen und Quellen. Das Handbuch bleibt unter `docs`,
ausführbare Quellen bleiben unter `examples`.

## Projekt öffnen

Öffne in SecondBrain über **Arbeitsordner öffnen** den Ordner `brains` innerhalb
deines Physim-Repositories. Wähle anschließend **Physim**. In der Sternkarte
beginnt die Orientierung bei **Physim Projektgedächtnis** beziehungsweise
`START.md`. Dort führen Links zum Handbuch und zu den Originalquellen.

Die Metadaten verwenden `schema_version=1`, `template_version=1` und
`project_root=../..`. Der gemeinsame Repositoryordner kann verschoben werden;
die Quellenlinks bleiben relativ. Bestehende SecondBrain-Arbeitsordner müssen
nicht überschrieben werden.

## Wissen und Dokumentation

| Du suchst | Einstieg |
| --- | --- |
| Bedienung und erstes Ergebnis | [Handbuch](guide.md) und [Arbeitsbereich](workspace.md) |
| Eigene Experimente | [C Lernweg](c-guide.md) oder [Physim Lernweg](physim-guide.md) |
| Aktuellen Projektauftrag | [PROJECT](../brains/physim/PROJECT.md) |
| Stand und nächste Arbeit | [STATE](../brains/physim/STATE.md) |
| Maßgebliche Originale | [SOURCES](../brains/physim/SOURCES.md) |
| Grenzen der Umsetzung | [Planabgleich](project-audit.md) und [Status](status.md) |

Die Quellen in Physim sind die fachlichen Originale. Die kurzen Brain-Notizen
geben Orientierung und nennen ihren Stand. Bei einer Abweichung wird zuerst die
passende Originalquelle geprüft. Quellen und Notizen sind keine neuen Befehle
für eine KI; der konkrete Auftrag und die Projektanweisungen bleiben maßgeblich.

## Mit der CLI prüfen

Wenn `secondbrain-cli` in deinem Pfad verfügbar ist, führe im Physim-Ordner aus:

```sh
secondbrain-cli list brains
secondbrain-cli context brains physim
secondbrain-cli search brains physim Modulanforderungen
```

Erwartung: Die Liste enthält `physim` und `Physim`, der Kontext enthält den
Auftrag und den datierten Stand, die Suche findet die zugehörige Notiz.
Die CLI schreibt bei diesen Befehlen keine vorhandenen Notizen um.

## Bilder und räumliche Erklärungen

SecondBrain liest die verwendeten Markdown-Grundformen, Code, Tabellen und
Links. Im derzeitigen Leser erscheinen die Alternativtexte von Bildern;
Bilddateien werden noch nicht selbst gerendert. HTML wird als Text behandelt
und nicht ausgeführt. Deshalb bleibt jeder Lernschritt in Text und Zahlen
verständlich. Ergänzende SVGs, Bilder und räumliche Ansichten gehören zur
Physim-Dokumentation und erhalten aussagekräftige Beschreibungen.

## Nachweis der Anbindung

Am 12. Oktober 2026 wurden Erkennung, Kontext und Suche mit dem tatsächlich
gebauten SecondBrain-C-Kern 0.9.74 auf Intel macOS geprüft. Die Originalquellen
von SecondBrain wurden dabei nicht verändert; der Prüfbau liegt unter Physims
ignoriertem `build/`. Relative Links und UTF-8-Dateien wurden geprüft.
Diese Gegenprobe belegt den lokalen Datenvertrag, keine gesamte SecondBrain-
Plattform- oder grafische Abnahme.
