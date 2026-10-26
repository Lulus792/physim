# Native Prüfung: <Bereich und Datum>

Auftrag: <konkretes Verhalten und freigegebene Vorlage>.
Geprüfter Stand: <Commit und Hashliste der zusätzlichen lokalen Änderungen>.
Umgebung: <Betriebssystem, Compiler/SDK, Buildvariante, Skalierung und Theme>.

## Reproduzieren

```text
<exakte Befehle mit Ausgabe- und Temporärpfaden unter D:/Physim/build>
```

Binärhash: `<SHA-256>`. Vollständige lokale Logs: `<D:/Physim/build/validation/...>`.

## Ergebnisse

| Kriterium | Aktion und Soll | Beobachtetes Ist / Messwerte | Ergebnis |
| --- | --- | --- | --- |
| <Verhalten> | <Eingabe → erwarteter Zustand> | <Zustand, Logzeile oder JSON-Pfad> | <bestanden / fehlgeschlagen / ungeprüft> |

Geometrie, Farben, Fokuskennungen und Zustandswechsel als kompakte Messwerte
festhalten. Ein erfolgreicher synthetischer Tastaturtest belegt allein keinen
physischen Windows-Fokus und keine Screenreader-Nutzung.

## Gezielte visuelle Belege

<Nur die erforderlichen Ansichten verlinken; zu jedem Bild den eigenen
Prüfzweck nennen. Für zusätzliche gleichartige Eingabeschritte genügen die
Zustandswerte. Gegenproben und Fehlerbilder bis zur Befundklärung erhalten.>

## Grenzen und nächste Schritte

<Offene Befunde mit Reproduktion, Ursache soweit belegt, Abweichung von der
Vorlage und nächstem prüfbarem Schritt. Ausgeführtes von Geplantem trennen.>

Aufbewahrung: <ausgewählte versionierte Nachweise, rohe Fehlerbelege auf D:,
nach Abnahme entbehrliche Rohbilder>. Bestehende eingefrorene Manifeste und
Referenzen unverändert lassen. Nachfolgende Quelländerungen benötigen einen
neuen Nachweis; alte Ergebnisse behalten ihren ursprünglichen Geltungsbereich.

| Datei oder Gruppe | Eigenständiger Prüfzweck | Entscheidung / Voraussetzung für Entfernung |
| --- | --- | --- |
| <Bild, Zustandsfolge oder Laufzeitarchiv> | <welches Kriterium wird damit belegt?> | <erhalten / nach Abnahme entbehrlich / entfernt mit Quittung> |

Für eine reine Speicher-, Text- oder CLI-Prüfung ohne visuelle Aussage sind
keine Bildschirmbilder nötig. Bei identischem Laufzeitstand ein vorhandenes
hashgeprüftes Archiv referenzieren; zusätzliche Bilder und Archive nur mit
eigenem Zweck erzeugen.

## Kompakte Übergabe

Einen aktuellen Einstiegspunkt pflegen, der den letzten gültigen Bericht und
die offenen Befunde verlinkt. Neue Ergebnisse als eigenen Nachlauf anlegen;
eingefrorene Berichte nicht überschreiben. Ein noch ungeprüfter lokaler Patch
gehört nicht zum zuletzt bestandenen Quellstand.

Für längere Bedienfolgen genügt eine kleine JSON-Datei mit Quellstand,
Laufzeitreferenz und Fällen. Pro Fall Aktion, Soll, Ist, Ergebnis und den
passenden Log-/Zustandspfad nennen. Gemeinsame Umgebungs- und Binärdaten einmal
auf Laufebene festhalten. Bei identischem Programm auf denselben geprüften
Laufzeitstand verweisen. Bilder nur ergänzen, wenn sie eine zusätzliche
visuelle Aussage belegen; dafür einen Zweck und eine Aufbewahrungsentscheidung
eintragen. Ein Bildverzicht ist kein Nachweis für ungeprüfte visuelle Kriterien.

Die [JSON-Vorlage](native-validation.json) enthält dafür ein kleines Grundgerüst.
Platzhalter ersetzen, unbenötigte Felder entfernen und den Status erst nach
Ausführung ändern. `null` beim Exitcode bedeutet ungeprüft. Ein kompakter
Markdown-Bericht nennt Ergebnis, Grenzen und den Link zur JSON-Datei; dieselben
Zustandsfolgen dort nicht nochmals vollständig abschreiben.
