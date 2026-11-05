# Änderungen dokumentieren und übergeben

Diese Anleitung richtet sich an die Projektentwicklung. Benutzeranleitungen
beginnen im [Handbuch](guide.md); Buildbefehle stehen zentral in der
[Buildanleitung](build.md). Implementiertes Verhalten am aktuellen Code prüfen,
bevor es als verfügbare Funktion beschrieben wird.

## Wo welche Information steht

| Frage | Verbindlicher Einstieg |
| --- | --- |
| Was ist jetzt geprüft, offen und als Nächstes zu tun? | [Kurzer Arbeitsstand](weiterarbeit.md) |
| Welche Gestaltung und Arbeitsregeln gelten? | [AGENTS.md](../AGENTS.md), dort verlinkte ADRs und Freigaben |
| Welcher konkrete Quellstand wurde wie geprüft? | [Prüfindex](native-verification-index.md), danach der verlinkte Originalbericht |
| Wie wird gebaut und gestartet? | [Buildanleitung](build.md) |
| Welche generierten Daten dürfen wo liegen und wann weg? | [Speicherpflege](development-storage.md) |

Aktuellen Status an einer Stelle pflegen. Der kurze Arbeitsstand ist kein
fortlaufendes Chatprotokoll: ungefähr eine bis zwei Bildschirmseiten, bevorzugt
unter 100 Zeilen. Letzten geprüften Stand, tatsächlich aktive Laufkennungen,
offene Befunde, nächste Handlung und Schutzregeln nennen. Abgeschlossene Details
im Prüfindex verlinken, statt bei jedem Schritt einen weiteren Vollbericht anzuhängen.

## Nach einer Änderung

1. `git status --short` und relevante Diffs lesen. Eigene Änderung von fremder
   Arbeit unterscheiden; den Quellstand mit `git rev-parse HEAD` festhalten.
2. Den Prüfanlass nennen: geänderte Funktion, neuer Fehler oder bislang offene
   Dimension. Passende Prüfungen wählen. Kontextwechsel allein ist kein Anlass,
   abgeschlossene F1-, Protokoll-, Settings- oder Plattformprüfungen zu wiederholen.
3. Ausführen und Ergebnis aus tatsächlichen Logs/Dateien ermitteln. Konfiguration,
   gestarteter Prozess und grüner fremder Quellstand sind keine eigene Abnahme.
   Einen laufenden Job über dieselbe Kennung weiter beobachten; Beobachtungstimeout
   ist kein Beweis für einen beendeten Job.
4. Ergebnis und Grenzen kurz dokumentieren, dann den Arbeitsstand ersetzen.
   Geprüfte eigene Dateien gezielt committen und gemäß AGENTS in beide Repositories
   pushen. Ein Bericht nennt den tatsächlich geprüften Commit und gegebenenfalls
   die geprüften lokalen Änderungen; spätere Änderungen sind davon getrennt.

Ein Build aus dem Arbeitsverzeichnis enthält dessen lokale Änderungen.
Ein bestandener isolierter Stand bestätigt andere uncommittete Änderungen nicht.
Diese Unterscheidung auch bei Release-Anleitungen und Übergaben beibehalten.

## Nachweise ohne unnötige Bildserien

[Markdown-Vorlage](templates/native-validation.md) und
[JSON-Vorlage](templates/native-validation.json) verwenden. Im kurzen Text genügen
Ergebnis, Umfang, Grenzen und Verweise. Wiederholte Zustandsfolgen, Quell-/Binär-
Hashes, Befehle, Soll/Ist und Exitcodes stehen einmal im strukturierten Bericht.
Gemeinsame Laufzeitdaten referenzieren; unveränderte Programme nicht pro Testfall
erneut archivieren.

Vor jeder Aufnahme die konkrete visuelle Frage nennen. Für reine Parser-,
Speicher-, CLI- oder Zustandsprüfungen sind keine Screenshots erforderlich.
Bei UI-Arbeit gezielte Zustände aufnehmen und visuell prüfen. Textmesswerte oder
Bildverzicht ersetzen keine Prüfung von Layout, Glyphen, Material und Gesamteindruck.

Im Bericht unterscheiden:

- **Geplant:** noch kein Ausführungsnachweis.
- **Bestanden:** genau benannter Quellstand, Plattform und tatsächlich geprüftes
  Verhalten; beispielsweise fünf Datenprüfungen sind keine komplette GUI-Abnahme.
- **Offen:** konkreter Rest, nächster Prüfschritt und benötigte Umgebung.
- **Historisch:** Originalbericht gilt für seinen damaligen Stand. Spätere
  Korrekturen und Nachläufe als eigene Belege verlinken, Originale nicht umschreiben.

## Pflege beim Abschluss

Aufbewahrung gleich mit entscheiden: Referenz/offener Fehler/eingefrorener Beleg,
aktuell benötigter Build oder ersetztes Zwischenprodukt. Erst nach Prüfung von
Pfaden, laufender Verwendung und Referenzen entfernen. Bei gleichen Bildern
mindestens eine verifizierte Referenz behalten und Zuordnung/Hashes quittieren.
Ein erfolgreicher Test allein belegt keine visuelle Abnahme und macht seine
Dateien nicht automatisch entbehrlich.

Bereinigungen nennen geprüfte Pfade, Grund, erhaltene Bestände und tatsächlich
entfernte Dateien/Bytes. Freien Platz auf D: und freien Platz innerhalb einer VM
getrennt angeben. Keine pauschale Löschung nach Alter, Dateiendung oder Ordnername;
keine erneuten Builds bloß zur Bestätigung einer Dokumentationskorrektur.
Die örtlichen D:-Regeln und geschützten Bestände stehen in der Speicherpflege.
