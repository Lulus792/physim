# Verbindliches Design B2: Speichern, Konflikte und Wiederherstellung

**Nutzerfreigabe: 29. Oktober 2026.** B2 ist Teil des fertigen Designs und gehört
zum bestehenden nativen Umsetzungsauftrag. Die bisherige Übernahmesperre ist
aufgehoben; A/B/C sind nur Vergleichsmaterial. [ADR 017](../../decisions/017-selected-file-safety-b2.md).

Freigegebene Referenz: [Speichern](index.html?approved=1&topic=save) ·
[Dateikonflikt](index.html?approved=1&topic=conflict) ·
[Wiederherstellung](index.html?approved=1&topic=recovery).
Die sechs [B2-Referenzbilder](review/b2/VALIDATION.md) bleiben maßgeblich für die
unverändert ausgewählte Clientgestaltung. Ihre damalige Kennzeichnung als
Entwurf dokumentiert die Historie, keine weiterhin offene Auswahl.

## Anordnung und gemeinsame Darstellung

- **Dateinavigation links**, Inhalts-/Entscheidungsbereich rechts, gemeinsame
  Kopf- und Fußzeile. Keine zwei nebeneinander liegenden Dateifassungen mehr.
  Die Dateiauswahl wechselt die Vorschau; sie wählt keine einzelnen Bestandteile
  einer gemeinsamen Projektsicherung zur Wiederherstellung ab.
- Rechts Dateiname und Art, ein gemeinsamer Diff-Block, danach gegebenenfalls
  die Konflikt-/Wiederherstellungsentscheidung. Warnungen stehen davor. Aktionen
  und konkrete Folgen stehen in der Fußzeile wie in den B2-Bildern.
- Referenz bei 1440 Pixeln: Dialog-Clientbreite bis 1120 CSS-Pixel,
  Dateinavigation 240 Pixel, rechts 21 Pixel Innenabstand; bei mittleren Fenstern
  205 Pixel Navigation. Im schmalen Fenster Navigation über dem Inhalt.
  Gemeinsame native Schrift-/UI-Skalierung beachten, keine festen Bildpixel kopieren.
- Gemeinsame Werkbankflächen und **Markierung A2** verwenden. Keine neue globale
  B2-Palette für andere Bereiche. Vergleichen ist keine Textbearbeitung; der
  lesbare Originalinhalt darf nicht durch Bedienelemente verdeckt werden.

## Gemeinsamer Dateivergleich wie bei Git

1. Eine fortlaufende Ansicht mit **entfernten Zeilen (−), hinzugefügten Zeilen (+)
   und unverändertem Kontext**. Zusammengehörige entfernte Zeilen vor den neuen
   Zeilen anzeigen. Keine fest markierte Beispielzeile und keine zweite Spalte.
2. Gutter: alte Zeilennummer, neue Zeilennummer, Änderungszeichen, Text. Bei
   entfernten Zeilen bleibt die neue Nummer leer, bei neuen die alte. Kontext
   erhält beide Nummern. Zeilen zählen ab 1; Umbruch erzeugt keine zusätzliche
   Quellzeilennummer. Leere Zeilen, Tabs, Unicode und Zeilenenden korrekt erhalten.
3. Oben Dateiname und echte Plus-/Minus-Zähler, darunter die benannten Seiten
   und eine Bereichszeile. Zähler betreffen geänderte Zeilen, nicht Zeichen oder
   Dateien. Identische Texte als unverändert darstellen.
4. **Richtung:** Speichern = Datei → Editorentwurf; Konflikt = aktuelle externe
   Datei → Editorentwurf; Wiederherstellung = aktuelle Datei → Sicherung.
   Die Plus-Seite ist keine Aussage über einen späteren Zeitstempel. Richtung
   ausdrücklich benennen. Fehlende Datei = leerer Ausgangstext mit Hinweis;
   fehlende/unlesbare Sicherung = kein erfundener Vergleich.
5. Dateiauswahl, Inhalte und Revision bestimmen den Vergleich. Bei neuer externer
   Revision aktualisieren und eine alte Entscheidung verwerfen. Die Ansicht ist
   keine gespeicherte, zusammengeführte oder automatisch übernommene Datei.
6. Keine Git-Anbindung, Patch-Ausführung, automatische Zusammenführung,
   Einzelhunk-Übernahme oder neuer Merge-Editor durch diese Freigabe voraussetzen.

### Genaue Farb- und Textrollen

| Rolle | Dunkel | Hell |
| --- | --- | --- |
| Hinzugefügte Zeile, Hintergrund | `rgba(104,177,131,0.12)` | `rgba(54,135,81,0.08)` |
| Entfernte Zeile, Hintergrund | `rgba(214,92,92,0.12)` | `rgba(181,69,62,0.08)` |
| Plus / Hinzugefügt-Zähler / Legende | `#a9d6b5` | `#286240` |
| Minus / Entfernt-Zähler / Legende | `#ffb2ac` | `#a12e2a` |
| Text der Datei | Gemeinsame Textfarbe, volle Deckkraft | Gemeinsame Textfarbe, volle Deckkraft |
| Textauswahl und ausgewählte Datei | A2 gemäß [dessen Spezifikation](../selection-options/APPROVED.md) | A2 gemäß dessen Spezifikation |

Referenz-Code: 12 CSS-Pixel, Zeilenhöhe 1,95; Gutterbreiten 2,7 / 2,7 / 1,8 em.
Im schmalen Browserbeispiel 11 Pixel und 2,2 / 2,2 / 1,6 em. Native Schriftrollen
und eingestellte Codeschriftgröße respektieren. Lange Zeilen lesbar umbrechen;
Datum, Pfad und Dateiname müssen aus echten Daten kommen. System-/Hochkontrast
verwendet sichtbare Plus-/Minuszeichen und unterscheidbare Konturen zusätzlich
zur Farbe. Kein Durchstreichen, kein Abschwächen des gesamten Textes.

## Verbindliche Abläufe und Fehler

### Speichern, Schließen und Projektwechsel

Betroffene ungespeicherte Quellen anzeigen, den angefragten Vorgang benennen und
**Speichern**, **Nicht speichern…** sowie **Abbrechen** wie in B2 anbieten. Beim
Speichern erst nach Erfolg aller erforderlichen Dateien fortsetzen. Bei reinem
Speicherauftrag keine Projektwechsel-Behauptung. Für Datei schließen und App
beenden dieselben Zustandsregeln mit zutreffenden Aktionen verwenden.

Dateien werden einzeln gespeichert, keine gemeinsame Dateisystemtransaktion
behaupten. Bei Teilfehlern gespeicherte Dateien gespeichert lassen, übrige
Entwürfe erhalten, Fehler mit Dateibezug zeigen und den angefragten Vorgang
anhalten. Erneuter Versuch prüft echte Rechte/Dateistände; kein im Beispiel
umschaltbarer Schreibzugriff in der produktiven Oberfläche.

Nicht speichern / Verwerfen zeigt vorher die betroffenen Entwürfe und konkrete
Folge. Zurück erhält den Stand. Abbrechen verhindert den angefragten Vorgang,
macht bereits abgeschlossene Speicherschritte aber nicht rückgängig. Vorhandene
Backupregeln beibehalten; keine falsche Erfolgsmeldung aus einer verspäteten Antwort.

### Dateikonflikte

**Entwurf als Kopie speichern…**, **aktuellen Dateistand laden** oder **Entwurf
weiter bearbeiten** anbieten. Laden ersetzt den ungespeicherten Editorentwurf
erst nach der angezeigten Bestätigung. Weiter bearbeiten lässt den Konflikt
offen; normales Speichern darf die externe Fassung weiterhin nicht überschreiben.
Eine gelöschte Datei kann nicht als vorhandene Fassung geladen werden.

Der Browsertext „als Kopie vormerken“ beschreibt nur seine technische Grenze.
Nativ echte Zielwahl über **Explorer A**, Namen, Existenz, Pfad und Rechte prüfen,
Datei schreiben und Fehler/Abbruch wahrheitsgemäß anzeigen. Original und Entwurf
bis zum Erfolg erhalten. Ein Erfolg ohne erzeugte Datei erfüllt den Auftrag nicht.

Dateifassung vor jeder Änderung/Ladeentscheidung erneut prüfen. Wurde sie während
der Entscheidung erneut geändert, den neuen Vergleich zeigen und eine neue
Entscheidung verlangen. Kein stilles Überschreiben oder automatischer Merge.

### Autosave und Wiederherstellung

Projekt-Autosave enthält Experiment und Analyse als zusammengehörige Sicherung;
beide Texte gemeinsam laden. Weitere Dokumententwürfe separat behandeln. Die
linke Dateiauswahl dient nur der Inhaltsvorschau. Sicherungsherkunft und Zeit
tatsächlich ermitteln; keine automatisch entdeckten unbekannten Pfade behaupten.

**Sicherung laden übernimmt zunächst nur Editorinhalte.** Kein Speichern,
Build oder Lauf dadurch. Bewusstes späteres Speichern ist ein eigener Schritt
mit den richtigen betroffenen Dateien. Wurde eine Quelle inzwischen extern
geändert, in die B2-Konfliktprüfung führen und übrige Entwürfe erhalten. Eine
Entscheidung über eine Quelle autorisiert keine Änderung einer anderen.

Gespeicherte Dateien verwenden verwirft die Sicherung erst nach Bestätigung;
Später entscheiden erhält sie. Unlesbare Sicherungen nicht laden, überschreiben
oder still löschen. Mit gespeicherten Dateien weiterarbeiten erhält die
beschädigte Sicherung und zeigt den tatsächlichen pausierten Autosave-Zustand.
Ohne Sicherung keine Wiederherstellung oder ungespeicherten Inhalte vortäuschen.

## Native Einbindung, Fenster und Abnahme

Die bestehenden Datei-, Autosave-, Backup-, Rechte-, UTF-8- und Textverträge
weiterverwenden. Keine WebView-/JavaScript-Übernahme in Physim. Der auf 100.000
Vergleichszellen begrenzte Browseralgorithmus ist keine neue native Dateigröße.
Für große native Texte Ressourcenbudget, Reaktionsfähigkeit und eine klar
benannte gröbere Darstellung sicherstellen; Originale nicht still abschneiden.

**Eigene OS-Aufgabenfenster nach [ADR 015](../../decisions/015-independent-task-windows.md)**.
Das Hauptfenster und andere Bereiche bleiben bedienbar; nur den betroffenen
Vorgang anhalten. Auftraggeber, Projekt-/Datei-Identität und Anfragegeneration
festhalten. Geänderte/entfernte Ziele verwerfen oder erneut prüfen, niemals
späte Antworten auf ein neues Projekt anwenden. Kopien-Picker gehört seinem
Aufgabenfenster. Die HTML-Modalsperre ist nur eine Browser-Prüfhülle.

Tastaturzugang, verständliche Namen/Zustände, Menü-/A2-Konventionen und sinnvolle
Fokus-Rückgabe erhalten. Escape aus einer Bestätigung geht zurück; nach Abschluss
schließt Escape nur das Aufgabenfenster, ohne das Ergebnis rückgängig zu machen.
Keine Fokus-Rückgabe in ein fremdes inzwischen aktives Fenster erzwingen.

Native Abnahme: alle gezeigten Normal-/Leer-/Fehler-/Abbruchzustände mit echten
Dateien, verschiedene Sprachen und Namen, korrekte Diff-Zeilen/Zähler, Wieder-
herstellungsumfang, Teilfehler, reale Kopie, weitere externe Revision, veraltete
Antworten, Weiterarbeit im Hauptfenster, Hell/Dunkel/hoher Kontrast, kleine Fenster,
große Schrift und tatsächlich ausgeführte Plattformprüfungen dokumentieren.
Vergleichskarten, Szenariowahl, feste Beispieldaten und Demo-Erfolgsformulierungen
gehören nicht in die produktive App.

Die bisherigen [176 Browser-Layouts und Diff-Prüfungen](review/b2/VALIDATION.md)
belegen den ausgewählten Entwurf. Die Freigabe bestätigt keine fertige native
Umsetzung. [Gesamtauftrag für den anderen Chat](../unified-workspace/NEXT-CHAT.md).
