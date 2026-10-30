# Verbindliches Fortschrittsdesign B mit Pop-out

**Vom Nutzer am 30. Oktober 2026 ausgewählt: „B, aber mit der Möglichkeit,
das rauszupoppen als eigenes Fenster“.** B · Aufgabenleiste mit der beschriebenen
Fensterergänzung gehört zum fertigen Design. D06 im bestehenden nativen Auftrag
mit umsetzen, keine erneute Variantenentscheidung vorschalten.
[ADR 020](../../decisions/020-selected-progress-b-popout.md).

[Festgelegte Referenz](index.html?approved=1&task=series&scenario=running&theme=dark) ·
[unbekannter Fortschritt](index.html?approved=1&task=analysis) ·
[Abbruchanforderung](index.html?approved=1&task=series&scenario=canceling) ·
[Exportabschluss](index.html?approved=1&task=export&scenario=commit) ·
[historische A/B/C-Auswahl](index.html?approved=0&variant=b).
Standardaufruf und `approved=1` zeigen fest B, auch bei alten A/C-Parametern.

## Gewählte Anordnung

- Standardmäßig kompakte Aufgabenleiste B unten über der Statuszeile: Titel,
  aktueller Status/Tätigkeit, Fortschrittsanzeige, **Aufklappen** und
  einem **Icon-only-Pop-out-Button** (Pfeil aus einem Fenster). Keine dauerhaft offene Karte A oder standardmäßig
  ausgelagerte Ansicht C.
- Aufklappen reserviert Platz für dieselbe Aufgabenkarte mit Quelle/Ziel,
  Phasen, Fortschritt, Erklärung, Details, Abbrechen bzw. Ergebnis/erneut versuchen
  und Ausblenden. Zuklappen ändert nur die Sichtbarkeit.
- Während des Pop-outs bleibt die Leiste als kompakter Status mit
  Icon **Fenster anzeigen** und beschriftetem **Zurückholen** erhalten. Die große Karte wird dort
  nicht zusätzlich gezeichnet. Keine zweite Aufgabe oder doppelten Aktionen.
- Das Pop-out enthält dieselbe Karte, Status, Details und Abbruch-/Ergebnisaktionen,
  plus **Zur Leiste**. Ein erneuter Aufruf aktiviert dieses eine Fenster.

## Pop-out, Rückholen und Zustand

1. **In eigenem Fenster:** eigene echte native OS-Clientfläche nach ADR 015.
   Hauptfenster und andere Bereiche bleiben bedienbar. Dieselbe Auftragsidentität,
   Quelle/Revision, Versuchsnummer, Zähler, Phase, Fehler und Detailauswahl.
2. Die Anzeige hat keine eigene Worker-Kopie. Abbrechen im Fenster fordert den
   Abbruch genau dieses Auftrags an; beide Ansichten zeigen denselben Zustand.
   Die Leiste bleibt bis zur Bestätigung ehrlich „Abbruch angefordert“.
3. **Zur Leiste / Zurückholen:** eigenes Fenster schließen und B aufgeklappt
   wieder anzeigen. Aufgabe läuft unverändert weiter; kein Neustart, kein
   Zurücksetzen von Zählern oder Versuchen. Bewusstes Rückholen fokussiert den
   Pop-out-Zugang in der Leiste.
4. **Fenster-Schließen und Escape im Pop-out:** ebenfalls Anzeige zurückholen,
   nicht abbrechen. Die zentrale Aufgabe lebt unabhängig von ihrer Anzeige.
   Ein bloßes Schließen darf keinen Abbruchbefehl oder zweiten Versuch auslösen.
5. Wiederholtes Öffnen erzeugt kein zusätzliches Fenster pro Aufgabe. Bei
   mehreren echten Aufgaben muss die Zugehörigkeit eindeutig bleiben; die
   Ein-Aufgaben-Demo ist keine Vorgabe für globale Singleton-Jobs.
6. Fensterverschieben/-größe, Theme und Textskalierung nativ korrekt behandeln.
   Hauptfenster-/Quellwechsel dürfen keine alten Antworten auf neue Aufgaben
   anwenden. Auftrag, Fenster und Datenlebenszeit voneinander trennen.
7. Schließen der Quellansicht ist nativ anhand bestehender Shutdown-/Ownership-
   Verträge zu behandeln. Kein stiller Datenverlust oder falscher Erfolgszustand.
   Die reine Browserdemo verliert beim Neuladen ihren Seitenspeicher und zeigt
   im Pop-up einen getrennten Hinweis ohne aktive Befehle. Das ist kein
   Produkt-Lifecycle für laufende native Aufgaben.

## Exakte Gestaltung

`progress.css`/`progress.js` liefern die ausgewählte Referenz; Maße sind logische
Pixel und müssen mit DPI/UI-Schrift und verfügbarer Clientfläche skalieren.
Leiste: 12/17 px Innenabstand, 15 px Abstand, 1-px-Kontur oben; Titel und
Sekundärtext, kompakter 130-px-Fortschritt, Bedienelemente mindestens 34 px hoch.
Pop-out/erneutes Anzeigen nur als 34 × 34 px Icon-Button, 17-px-SVG in aktueller
Textfarbe; Tooltip „In eigenem Fenster öffnen“ bzw. „Aufgabenfenster anzeigen“,
zugänglicher Name zusätzlich ausdrücklich gesetzt. Zurückholen bleibt beschriftet.
Diese kompakte Fassung folgt dem anschließenden ausdrücklichen Nutzerwunsch.
Bei schmaler/großer Darstellung umbrechen statt abschneiden.

Aufgeklappte Details: Abstand 0/17/15 px, Aufgabenkarte mit Kopfabstand 17/18 px,
Textkörper 4/18/16 px, Aktionszeile 12/18 px; weitere Details in ruhiger
konturierter Fläche. Fensterinhalt verwendet dieselbe Karte, keine neue
Formensprache. Browser-Fenstergröße 480 × 650 dient als kleine Referenz,
keine starre native Mindestgröße. Bei 360 px und 200% Text bleiben Aktionen
durch Umbruch/Scrollen erreichbar. OS-Titelleiste ist native Fensterdekoration.

| Rolle | Dunkel | Hell |
| --- | --- | --- |
| Arbeitsfläche | `#16171b` | `#e9ebef` |
| Panel | `#1e1f23` | `#f6f7f9` |
| Kopf/Status | `#25262b` | `#edeff3` |
| Aufgabenfläche | `#191a1e` | `#ffffff` |
| Kontur | `#3b3d45` | `#bec3ce` |
| Text | `#edeef2` | `#20242c` |
| Sekundärtext | `#a6a9b1` | `#555e6c` |
| Fortschritt/Akzent | `#8abaff` | `#17569e` |
| Erfolg | `#8bcca5` | `#286240` |
| Warnung / Hintergrund | `#ebc279` / `#322d24` | `#805509` / `#faf2df` |
| Fehler / Hintergrund | `#ffaaa2` / `#35262a` | `#a52925` / `#fbeef0` |

Hoher Kontrast: Schwarz/Weiß, gelber Akzent `#fff44f`, sichtbare Konturen.
Symbole, Text und Zustandsnamen ergänzen Farbe. Reduced Motion stoppt die
unbestimmte Balkenbewegung; Tätigkeit und Status bleiben lesbar. A2 für Auswahl,
Rückmeldungen A für Abschlussmeldungen erhalten.

## Unveränderte Aufgabenzustände

| Zustand | Verbindliches Verhalten |
| --- | --- |
| Noch nicht gestartet / Warteschlange | Kein erfundener Fortschritt. Entfernen vor tatsächlichem Start ist vom laufenden Abbruch zu unterscheiden. |
| Läuft mit bekannter Gesamtmenge | Echte Zähler, etwa abgeschlossene Läufe / geplante Läufe. Prozent ist Mengenanteil, keine Zeitprognose. |
| Läuft ohne bekannte Gesamtmenge | Tätigkeit/Phase statt Prozent oder erfundener Restzeit; kein numerischer ARIA-Istwert. |
| Abbruch angefordert | Wiederholten Befehl sperren, Bestätigung der Aufgabe bzw. bestätigte Unwirksamkeit der alten Anfrage abwarten. Nicht vorschnell „abgebrochen“. |
| Abgebrochen | Vorhandene Laufdateien/Teilresultate und bisherige Inhalte erhalten. Neuer Versuch separat. |
| Erfolg | Erst nach echter Rückgabe und verfügbarem passendem Ergebnis. Ergebniszugang an richtige Aufgabe binden. |
| Fehler | Lokale Erklärung/Befund, ursprüngliche Eingaben und bisherige Ergebnisse erhalten. Kein optimistischer Erfolg. |
| Mit Teilfehlern beendet | Gültige und fehlerhafte Laufstatus getrennt; 100% abgeschlossene Status bedeutet nicht 100% gültige Resultate. |
| Nicht sicher unterbrechbarer Abschluss | Abbruch deaktivieren und erklären, Ergebnis abwarten. Nur echte Adapterfähigkeiten anbieten. |

Bei Erfolg vor Abbruchbestätigung entscheidet der bestätigte Endzustand; kein
nachträgliches Umetikettieren. Alte Antworten dürfen neue Versuche/Revisionen
nicht überschreiben. Phasenpunkte sind Reihenfolge, keine gleich gewichteten
Zeitanteile. Auf-/Zuklappen, Auslagern, Rückholen und Escape sind Anzeigeaktionen.

Sechs gezeigte Aufgaben gehören zum Abgleich: Bauen, Projektladen, Datei-/
Laufvorschau, Analyse, Grafikexport und Serie. Die bisherigen Läufe-A-Start-/
Limit-/Fortsetzungsregeln bleiben. Projekt- und Exportzustände sind an echte
Adapter und B2-/Rechte-/Versionsregeln anzubinden; die Demo beweist nicht,
dass jeder Adapter bereits asynchron abbrechen kann. Keine blockierenden
UI-Joins, kein rückwirkendes Undo bereits wirksamer Datei-/Ordneroperationen
versprechen. Bestehende Daten dürfen nicht still ersetzt werden.

## Bedienung und Fensterkommunikation

Alle Leisten-, Fenster-, Detail- und Abbruchaktionen per Tastatur, sichtbar
fokussierbar und mit zugänglichem Namen/Rolle/Zustand. Bei deaktiviertem
Abbruchbutton Fokus auf Details erhalten. Neue Fortschrittswerte holen keinen
Fokus und erzeugen keine pausenlosen Screenreaderansagen. Fokus nur bei
bewusstem Öffnen/Rückholen bzw. Ergebnisnavigation sinnvoll führen.

Rückmeldungen A/Protokoll C verwenden denselben Auftrags-/Ereignisbestand.
Ein Abschluss ist ein Ereignis, keine doppelte Erfolgskarte aus Hauptfenster
und Pop-out. Die Referenz zeigt die Erfolgskarte einmal im Hauptbereich;
im Pop-out bleibt der Endzustand mit Ergebnisaktion erreichbar.

## Browserreferenz und native Abnahme

Die Vorschau öffnet ein echtes Browser-Pop-up über `window.open`; Browser/App
können es als Tab behandeln oder blockieren. Bei Blockade bleibt die Aufgabe
in B und eine Erklärung ermöglicht erneuten Aufruf. Das ist kein Ersatz für
das native eigene Physim-OS-Fenster nach [ADR 015](../../decisions/015-independent-task-windows.md).
Keine WebView oder Browserfensterverwaltung in die native Anwendung übernehmen.

In der Demo besitzt die Quellseite den Fixture-Zustand. Das Pop-up sendet
benannte Befehle und empfängt aktualisierten Zustand; Nachrichten werden anhand
Quellfenster und Sitzung zugeordnet. Nativ vorhandenes Thread-/Request-Ownership
und echte Jobidentitäten verwenden, nicht diese Demo-Brücke. Neue Seite ohne
Auftraggeber darf keine verwaisten Befehle ausführen.

[README/Quellen](README.md), [historische Prüfung](review/VALIDATION.md) und
[aktuelle Pop-out-Prüfung](review/APPROVAL-VALIDATION.md) lesen. Galerie,
Beispielzustände/-schritte, Rennen-/Altantwort-Prüfschalter, Notizfeld und
synthetischer Bericht sind Prüfhülle. Keine echten Prozesse/Dateien werden
durch diese Referenz gestartet.

Nativ abnehmen: ein Auftrag/ein Pop-out, wiederholtes Öffnen, synchroner Zustand
und Abbruch, Rückholen und OS-Schließen ohne Abbruch, unabhängige Hauptarbeit,
späte Antworten und Quellwechsel, sichere Ergebnisnavigation, Fenster-/DPI-/
Schriftwechsel, kleine Fenster, Themes, Tastatur/Screenreader und echte
Windows-/Linux-/macOS-Läufe. Browserbelege bestätigen keine native Abnahme.

Alle bisherigen Freigaben bleiben erhalten. [Gesamtauftrag](../unified-workspace/NEXT-CHAT.md)
und [UI-Liste](../unified-workspace/UI-DESIGN-BACKLOG.md) führen D06 als entschieden.
D07 „Vollständige Kontextmenüs“ bleibt die nächste neue Entwurfsrunde.
