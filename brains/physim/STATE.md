# Physim Stand

Stand: 12. Oktober 2026. Physim ist in Entwicklung, keine freigegebene 1.0.

Der [Planabgleich](../../docs/project-audit.md) führt 127 implementierte,
14 konkret unvollständige und 390 noch nicht einzeln geprüfte Blöcke.
Die maschinenlesbaren Originaltexte stehen in
[project audit](../../docs/project-audit.json).

## Belegt

Kameraebenen, gespeicherte Projektanzeige, vollständige Parameterdefinitionen,
Modulanforderungen und die individuelle Szenensichtbarkeit besitzen lokale
Prüfungen und verschobene SDK-Gegenproben. Die
[Plattformnachweise](../../docs/platform-validation.md) nennen die jeweiligen
Revisionen, echten Testläufe und Grenzen.

## Laufend

Der eingefrorene Gesamtlauf für c60cced ist erfolgreich: 724 native Fälle und
714 SDK-Befehle. Die spätere Sichtbarkeit und neue Dokumentation gehören nicht
zu diesem Stand und besitzen eigene Gegenproben.

Die Dokumentation enthält jetzt [User Guide](../../docs/user-guide.md),
[Examples](../../docs/examples.md), Vorhersage-/Vergleichsübungen in zehn Tutorials
und native Diagramme. Die [3D-Erklärung](../../docs/spatial-explanations.md)
erscheint mit Reglern direkt in der F1-Hilfe. Der Browserentwurf wurde entfernt.
Aktuelle Einzelprüfungen stehen in den Plattformnachweisen.

Der Dokumentationsstand 9c93312 besteht inzwischen auch die tatsächlichen
Linux-Sanitizergruppen mit GCC und Clang. Sein vollständiger eingefrorener
macOS-/SDK-Lauf besteht 724 native Fälle und 718 SDK-Befehle. Spätere native Accessibility-Regler besitzen
eigene portable und macOS-Gegenproben. Ihre Linux-Value-Anbindung besteht
inzwischen mit Clang und GCC die tatsächliche AT-SPI-Gegenprobe; je Compiler
bestehen 10 native und 19 GUI-Fälle.

Abschnittssprünge und Beispielquellen sind jetzt in der nativen Hilfe erreichbar. Der native
Parser besteht 185 Links aus dem Repository und 186 aus dem verschobenen SDK.
Alle 180 indexierten Seiten und ihre internen Links sind geprüft.
Drei kopflose Prüfungen und fünf aktuelle native GUI-Fälle bestehen. Die
sichtbare Zielüberschrift ist bei 16/22 Pixeln geprüft; der große Planabgleich
ist lesbar und kopierbar. Die aktuelle Linux-Ankerprüfung bleibt gesondert offen.

## Nächster Einstieg

Prüfe zuerst den tatsächlichen Laufstatus und den aktuellen Git-Stand.
Bei späteren Änderungen ist diese datierte Zusammenfassung anhand der
Originalquellen abzugleichen.
