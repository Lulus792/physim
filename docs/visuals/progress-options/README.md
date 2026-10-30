# D06: Fortschritt und Abbruch · getrennte Designvorschauen

Stand: **30. Oktober 2026 · Auswahl offen.** Auf „mit dem nächsten weiter“
folgt D06 der [UI-Liste](../unified-workspace/UI-DESIGN-BACKLOG.md). A/B/C dieser
Runde sind noch nicht für das fertige Design ausgewählt. Alle bisherigen
Freigaben bleiben erhalten, einschließlich Rückmeldungen A und Läufe/Serien A.

[Vorschau öffnen](index.html?variant=a&task=series&scenario=running&theme=dark) ·
[A · Am Arbeitsort](index.html?variant=a&task=analysis) ·
[B · Aufgabenleiste](index.html?variant=b&task=series) ·
[C · Eigenes Fenster](index.html?variant=c&task=export).
Lokaler Server: `http://127.0.0.1:8791/progress-options/`.
Auch als lokale HTML-Datei nutzbar; keine externen Fonts oder Netzwerkzugriffe.

## Gestaltungsrichtungen

| Variante | Platzierung | Verhalten |
| --- | --- | --- |
| **A · Am Arbeitsort** | Aufgabenkarte direkt über dem Inhalt | Bezug, Tätigkeit, Fortschritt, Phasen und Abbruch unmittelbar beim Ergebnis. |
| **B · Aufgabenleiste** | Kompakte Zeile unten, über der Statuszeile | Aufgabenstatus bleibt sichtbar; Aufklappen zeigt dieselben Details und Aktionen. Reserviert Platz, kein blockierendes Overlay. |
| **C · Eigenes Fenster** | Begleitfenster neben der Werkbank | Aufgabe getrennt ansehen, Hauptbereich weiter bedienen. Die Browserseite zeigt nur zwei Clientflächen; kein tatsächliches OS-Fenster. |

Alle Richtungen verwenden denselben Beispielauftrag. Wechsel erhält Zustand,
Abbruchanforderung, Fortschritt, Versuch und Notizen. Details und Aufgabenanzeige
dürfen ausgeblendet werden; über Status oder „Aufgabe anzeigen“ sind sie wieder
erreichbar. Ausblenden/Escape bricht nicht ab. Variante C ist ein vorgeschlagenes
zusätzliches Fortschritts-Begleitfenster, kein Ersatz für die bereits verlangten
echten Projekt-/Export-Aufgabenfenster nach ADR 015.

## Sechs Aufgaben und ehrliche Fortschrittswerte

| Aufgabe | Beispiel | Anzeige |
| --- | --- | --- |
| Laufserie | Serie 12A, zwölf Läufe | Anfangs 4/12 beendet = gerundet 33%; Fortschritt zählt Abschlüsse, keine Zeit. Teilfehler getrennt. |
| Experiment bauen | gespeicherte Quelle `pendel.c` | Compiler-/Phasentätigkeit; Gesamtumfang unbekannt, keine Prozent- oder Restzeitangabe. |
| Projekt laden | Projektdateien eines Beispielordners | Unbestimmte Tätigkeit; bisheriges Projekt bleibt im Entwurfsablauf bis zum erfolgreichen Übernehmen erhalten. |
| Datei-/Laufvorschau | `runs/lauf-04.physim` | Unbestimmter Lesezustand; Auswahl/Original erhalten. Keine vollständige Vorschau vortäuschen. |
| Auswertung | Lauf 04 mit `auswertung.c` | Unbestimmte Analyse-/Berichtsphase; bisheriger Bericht bleibt als vorheriger Stand verfügbar. |
| Grafik speichern | PNG, 2400 × 1700 px | Vorbereitung und anschließender Abschluss ohne sichere Abbruchmöglichkeit als getrennte Beispielphasen. |

Phasenpunkte zeigen Reihenfolge, nicht gleich große Zeitanteile. Die Serie ist
das einzige Beispiel mit bekannter Gesamtzahl. Ein Anteil von 100% bedeutet
alle Laufstatus erfasst, nicht zwingend alle Ergebnisse gültig: im Zustand
„Mit Teilfehlern beendet“ stehen 10 gültige und 2 fehlerhafte Läufe.
Unbekannter Fortschritt hat keinen `aria-valuenow`; Bewegung ist entbehrlich
und bei Reduced Motion abgeschaltet.

## Zustände und Interaktionen

| Zustand | Gezeigtes Verhalten |
| --- | --- |
| Noch nicht gestartet | Kein aktiver Fortschritt; „Im Beispiel starten“. |
| Wartet auf Start | Kein erfundener Fortschritt. Entfernen aus Warteschlange bestätigt sofort, dass noch keine Ausführung begann. |
| Wird ausgeführt | Tätigkeit und Bezug, echte Beispielzählung nur bei Serie; Abbrechen anfordern. |
| Abbruch angefordert | Deaktivierte Wiederholungsaktion und Erklärung: noch nicht beendet, Bestätigung abwarten. Erhaltene Ergebnisse ausdrücklich nennen. |
| Abgebrochen | Erst nach dem Bestätigungs-Schritt. Fertige Laufdateien bleiben; neuer Versuch ist eigene Aktion. Keine Erfolgskarte. |
| Abgeschlossen | Ergebniszugang und vorhandenes Rückmeldungsdesign A; Ausblenden des Hinweises verändert das Ergebnis nicht. |
| Fehlgeschlagen | Lokale Erklärung und Befund in Details, vorheriger Inhalt erhalten; neuer Versuch statt stiller Wiederholung. |
| Mit Teilfehlern beendet | Nur Serie; vorhandene Ergebnisse und getrennte Fehlerzahl, kein pauschales „alles erfolgreich“. |
| Ausgabe abschließen | Nur Export; Abbruch deaktiviert und begründet. Erfolg erst nach bestätigtem Abschluss. |

Der Beispielzustand lässt sich direkt auswählen. „Nächster Beispielschritt“
treibt ausschließlich den Seitenspeicher voran: Serie zählt zwei weitere
Abschlüsse; unbekannte Aufgaben wechseln Phasen ohne Prozentwert. Abbruch
anklicken, anschließend einen Schritt auslösen: erst jetzt „Abgebrochen“.
„Erfolg vor Abbruchbestätigung“ demonstriert, dass ein bestätigter Erfolg nicht
nachträglich als Abbruch umetikettiert wird.

Neuer Versuch erhält bisherige Ergebnisse/Notizen, bekommt eine neue
Versuchsnummer und startet seinen eigenen Zähler. Der Prüfschalter für ein
verspätetes altes Ergebnis zeigt ausschließlich dessen Verwerfen an; er
simuliert **keine echte Worker-/Generationssicherheit**. Das muss nativ geprüft
werden. Er zeigt keine alten Daten als Ergebnis des neuen Versuchs.

Tastatur: Details/Aktionen per Tab, Aktivieren wie üblich, sichtbarer Fokus.
Wird der Abbruchbutton deaktiviert, wandert der Fokus zu Details. Escape aus
der Aufgabenanzeige blendet sie aus und fokussiert den Wiederöffnungszugang.
Neue Statusmeldungen holen keinen Fokus. Die Notizen sind durchgehend
bearbeitbar; keine modale Sperre. Erfolgshinweise entsprechen D05 A, keine
neue Rückmeldungsvariante oder zusätzliche Bestätigung.

## Abgleich mit dem aktuellen nativen Bestand

Gelesene Quellen sind die lokalen Dateien einschließlich laufender Änderungen:

- [Serien-UI](../../../app/batch_ui.inc) und
  [Serienaktionen](../../../app/batch_actions.inc): vorhandene Zähler
  `batch_completed`/`batch_active`, Abbruchflag `batch_cancel`, `ps_batch_run`.
  Die gewählte Läufe-A-Gestaltung, Start-/Limitregeln und archivierte
  Fortsetzung bleiben erhalten; diese Studie ersetzt sie nicht.
- [Auswertung](../../../app/analysis_ui.inc): bestehender Prozessabbruch mit
  `ps_process_kill`, `protocol_job_cancel` und `ps_process_close`.
- [Browserworker](../../../app/file_browser_worker.h): nicht blockierendes
  Verwerfen alter Anfragen/Ergebnisse, Versionen; aktive OS-Aufrufe können
  weiterlaufen. Bereits ausgeführtes `mkdir` wird durch Abbruch nicht rückgängig.
- [Vergleichsworker](../../../app/analysis_comparison.h): aktuelle Anforderung,
  veröffentlichte/verworfen gezählte Revisionen, eigenes Request-/Result-Ownership.
- [Grafikexport](../../../app/graphic_export_ui.inc): vorhandene Poll-Zustände
  `PS_GRAPHIC_SAVE_WAITING/WRITING/OK/CANCELLED`, sichere Ergebnisrückmeldung.

Die Browserzustände beschreiben ein **Designziel**, nicht bereits vorhandene
gleichartige Abbruchfähigkeit aller Adapter. Insbesondere projektweites
Hintergrundladen und der dargestellte Vorbereitungsabbruch benötigen vor
nativer Umsetzung einen Funktionsabgleich. Kann eine Operation nicht sicher
abbrechen, die Aktion ehrlich deaktivieren/erklären; keine Fähigkeit erfinden.
„Ausgabe abschließen“ ist ein vorgeschlagener UI-Zustand, keine Behauptung über
einen neuen nativen Commit-API-Zustand. Dateiwahlabbruch, laufendes Schreiben
und technische Fertigstellung bleiben unterschiedliche Vorgänge.

## Grenzen und Übergabe

**A/B/C noch nicht ins fertige Design übernehmen.** Nach Nutzerwahl Variante,
Zustände, genaue Werte aus `progress.css` und Referenzen festschreiben und in den
[Gesamtauftrag](../unified-workspace/NEXT-CHAT.md) aufnehmen.

- Native Fortschrittsmeldungen aus echten Zählern/Status ableiten; keine
  Demo-Schritte, künstlichen Verzögerungen, erfundenen Prozente oder Restzeiten.
- Auftrag mit Projekt-/Datei-/Lauf-/Fenster-/Versionsbezug festhalten. Alte
  Antwort darf neuen Versuch oder Auswahl nicht überschreiben.
- Abbruchanforderung von bestätigter Beendigung bzw. bestätigter Unwirksamkeit
  der alten Anfrage trennen. Eine verworfene I/O-Anfrage muss nicht die UI
  blockieren, bis der OS-Aufruf physisch zurückkehrt. Ressourcen korrekt besitzen;
  keine blockierenden Joins im UI, keine rückwirkende Rücknahme bereits wirksamer
  Datei-/Ordneroperationen versprechen.
- Rennen Erfolg/Abbruch/Fehler anhand autoritativen Endzustands auflösen;
  Fehler und fertige Teilresultate nicht ausblenden oder als Erfolg umetikettieren.
- Bisherige Projekte/Ergebnisse und ungespeicherte Inhalte erhalten. Neue
  Projektübernahme, Wiederholung und Dateischreiben müssen vorhandene B2- und
  Rechte-/Versionsprüfungen durchlaufen. Bestehende Serie fortsetzen ist nicht
  dasselbe wie alle Läufe erneut zu starten.
- Nur Aktionen sperren, die mit dieser Arbeit tatsächlich kollidieren.
  Gültige bisherige Ergebnisse bleiben bedienbar und als vorheriger Stand
  gekennzeichnet. Modell-/Analyseversionen und vorhandene Limits respektieren.
- [Rückmeldungen A](../feedback-options/APPROVED.md), Protokoll C und
  [Grafikexport](../unified-workspace/EXPORT-EXAMPLE.md) erhalten. Abschlusskarte
  nutzt dasselbe Ereignis, keine zweite unabhängige Erfolgsmeldung.
- [ADR 015](../../decisions/015-independent-task-windows.md) und tatsächliche
  Fenster-/Auftraggeberbindung erhalten. C wäre bei Wahl nativ ein echtes
  zusätzliches Begleitfenster; die Browser-Clientflächen sind kein Beleg dafür.

Galerie, Zustandswahl, Schritt-/Renn-/Altantwort-Prüfschalter und Notizfeld sind
Prüfhülle. Keine echte Physik, Compiler, Worker, Projekte, Dateien oder Exporte
werden gestartet. Beispielcode, Laufnamen, Bericht und Zähler sind synthetisch.
Die reduzierte Kontextwerkbank ist kein Ersatz für die bereits freigegebenen
Bereiche. Neu laden setzt den Zustand zurück; keine Speicherung in Local Storage.

D07 „Vollständige Kontextmenüs“ bleibt die nächste neue offene Entwurfsrunde.
Diese Vorschau enthält keine neuen Kontextmenüs.

## Nachweise

[Prüfbericht](review/VALIDATION.md), [Maschinenbericht](review/validation.json).
`node tools/check_progress_options.cjs` mit Playwright und unter Windows Edge;
alternativ `PHYSIM_BROWSER`. Ausgaben unter `build/progress-options-review`.
Browserprüfung ist keine native Windows-/Linux-/macOS- oder Screenreaderabnahme.
