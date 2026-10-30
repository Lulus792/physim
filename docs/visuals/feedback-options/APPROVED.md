# Verbindliches Rückmeldungsdesign A · Leiser Hinweis

**Vom Nutzer am 30. Oktober 2026 als Teil des fertigen Designs freigegeben.**
Die Auswahlphase für D05 ist abgeschlossen. A im bestehenden nativen Auftrag
mit umsetzen; keine erneute Variantenentscheidung vorschalten. B/C sind nur
Vergleichshistorie. [ADR 019](../../decisions/019-selected-feedback-a.md).

[Festgelegte Referenz](index.html?approved=1&scenario=burst&theme=dark) ·
[einzelner Erfolg](index.html?approved=1&scenario=saved) ·
[Kopierfehler](index.html?approved=1&scenario=copy) ·
[Pfadfehler](index.html?approved=1&scenario=path) ·
[veraltet](index.html?approved=1&scenario=stale) ·
[historischer Vergleich](index.html?approved=0&variant=a).
Standardaufruf und `approved=1` erzwingen A auch bei alten B/C-Parametern.

## Gewählte Anordnung

1. Kurze Erfolgskarten rechts unten innerhalb des zugehörigen Fensters, oberhalb
   der Statuszeile. Höchstens zwei sichtbar, neueste zuerst. Titel, konkreter
   Datei-/Laufbezug, kurzer Text, „Ansehen“ und Ausblenden × wie in A.
2. „Rückmeldungen“ mit Anzahl öffnet einen kleinen Verlauf rechts unten;
   alternativ der dezente Rückmeldungsbereich der bestehenden Statuszeile.
   Während der Verlauf offen ist, keine zusätzlichen Erfolgskarten zeigen.
3. Verlauf: Kopf, Schließen ×, Zeit und Herkunft, Symbol/Titel, Wiederholungszähler,
   Kontexttext und passende Navigation. Keine immer offene Seitenleiste C und
   kein hervorgehobenes Statusband B übernehmen.
4. Lokale Fehler/Warnungen unmittelbar an betroffener Eingabe/Aktion bzw. über
   veraltetem Ergebnis. Status und Verlauf verweisen kurz darauf; keine zweite
   vollständige Meldungsbox über dem Arbeitsinhalt.

Die vorhandene Hauptnavigation, Bereichsaufteilung und Resizing-Verträge bleiben
erhalten. Die hier vereinfachte Werkbank stellt nur den Bezug der Rückmeldungen
dar. Karten dürfen keine Hauptaktionen oder die Statuszeile dauerhaft verdecken;
bei wenig Platz nur die neueste Karte sichtbar halten und weitere Ereignisse
im Verlauf erreichbar lassen. Die Begrenzung auf zwei ist eine Obergrenze,
kein Anlass zum Abschneiden bei großer Schrift.

## Visuelle Werte

`feedback.css` ist die genaue Referenz; Maße sind logische Pixel, nach DPI und
UI-Schrift anzupassen. Karten: 360 px breit, 21 px vom rechten Fensterrand,
52 px über der unteren Kante, 10 px Abstand, Radius 9 px, Innenabstand 15 px,
1-px-Kontur, ruhiger Schatten. Kleines Fenster: 12 px Rand, höchstens 340 px
bzw. verfügbare Breite minus 24 px, Innenabstand 12 px. Kein horizontaler
Zwangsscroll, lange Pfade/Wörter umbrechen.

Verlauf: 365 px breit, rechts 20 px, unten 48 px, Radius 9 px, maximal 490 px
hoch und scrollbar. Bei höchstens 540 px Referenzbreite links/rechts 10 px,
Breite flexibel, maximal 460 px hoch. Nativ immer an tatsächlich verfügbare
Clientfläche, Statushöhe und Textskalierung anpassen.

| Rolle | Dunkel | Hell |
| --- | --- | --- |
| Arbeitsfläche | `#16171b` | `#e9ebef` |
| Panel | `#1e1f23` | `#f6f7f9` |
| Karte/Kopf | `#25262b` | `#edeff3` |
| Verlauf/Eingabe | `#191a1e` | `#ffffff` |
| Kontur | `#3b3d45` | `#bec3ce` |
| Text | `#edeef2` | `#20242c` |
| Sekundärtext | `#a6a9b1` | `#555e6c` |
| Akzent | `#8abaff` | `#17569e` |
| Erfolg | `#8bcca5` | `#286240` |
| Warnung / Hintergrund | `#ebc279` / `#322d24` | `#805509` / `#faf2df` |
| Fehler / Hintergrund | `#ffaaa2` / `#35262a` | `#a52925` / `#fbeef0` |

Erfolg ✓, Warnung △, Fehler ⊗ ergänzen Text; keine Bedeutung nur durch Farbe.
Lokale Boxen erhalten eine 3-px-Kante links, Radius 6 px und 14/16 px Abstand.
Hoher Kontrast: Schwarz/Weiß mit gelbem Akzent `#fff44f`, sichtbare Konturen,
bezeichnete Zustände; native Forced-Colors-/Kontrastvorgaben respektieren.
Bedienelemente mindestens 34 logische px hoch, sichtbarer Fokus, Text nicht
unter Markierungen verbergen. Auswahlfarben bleiben gemäß A2.

## Meldungen und Aktionen

| Zustand | Verbindliches Verhalten |
| --- | --- |
| Gespeichert | Erst nach tatsächlich erfolgreichem Speichern kurze Karte; Dateibezug und Zeitpunkt. Keine OK-Bestätigung. Spätere Änderungen am Dokument bleiben als ungespeichert erkennbar. |
| Hintergrundaktion beendet | Abschluss nur für tatsächlich verfügbares, passendes Ergebnis. Lauf-/Aufgabenbezug und „Ansehen“/„Ergebnis ansehen“ navigieren zu genau diesem Ergebnis. |
| Kopie fehlgeschlagen | Direkt im vorhandenen Export bei der Aktion: verständlicher Grund, „Erneut versuchen“, bestehendes PNG/SVG-Speichern als Alternative. Grafik, festgehaltener Datenstand und Exportoptionen erhalten. Kein zusätzlicher Fehler-Toast. |
| Projektpfad ungültig | Erklärung direkt unter dem Pfad, zugänglicher Fehlerzustand und Eingabe erhalten. Korrigieren/Prüfen, ohne vorzeitig ein Projekt anzulegen. |
| Ergebnis veraltet | Warnung über betroffenem Diagramm mit Erklärung, auf welchen älteren Stand sich die Daten beziehen. Erst explizite, erfolgreiche Neuberechnung für passende Revision erledigt sie. Fehlgeschlagene/abgebrochene Arbeit lässt sie stehen. |
| Wiederholt | Dieselbe offene Ursache am selben Ziel/Versionsstand in einem Eintrag mit Zähler und aktualisiertem Zeitpunkt zusammenfassen. Andere Ursache, anderes Ziel oder neue Operation nicht zusammenwerfen. |
| Mehrere Ursachen | Einzelne Ereignisse im Verlauf, höchstens zwei kurze Karten. Lokale Erklärungen bleiben unabhängig von Karten sichtbar. Anzahl bezeichnet Einträge/Ursachen, der Wiederholungszähler die zugehörigen Vorkommen. |
| Leer | „Keine Rückmeldungen“ und ruhiger Leerzustand im geöffneten Verlauf. Kein künstlicher Aufmerksamkeitszustand. |

Verwerfen, Ersetzen, Konflikte und Wiederherstellung bleiben im freigegebenen
B2-Ablauf. Reine Erfolge/Informationen brauchen keinen zusätzlichen Dialog.
Diese Runde gestaltet keine neuen Fortschrittsdialoge oder Kontextmenüs.

## Lebensdauer, Verlauf und Fokus

- × blendet eine Karte aus, löscht kein Ereignis und behebt keine Ursache.
  Verlauf schließen ändert weder Fehler noch Veraltet-Zustand.
- Der gewählte Referenzstart hält kurze Karten sichtbar, bis sie ausgeblendet
  werden. Optional gezeigt: nach sechs aktiven Sekunden nur kurze Erfolge
  ausblenden. Fokus/Mouseover auf Karten oder verborgene Seite pausieren;
  Ereignis bleibt im Verlauf. Lokale Fehler/Warnungen laufen nie automatisch ab.
  Die Demo-Checkbox ist Prüfhülle; daraus keine neue Einstellung in A ableiten.
- Bei neuer Meldung Fokus nicht verschieben. Ein höflicher, deduplizierter
  Accessibility-Status kündigt sie an. Keine wiederholten Ansagen aus Status,
  lokaler Box, Karte und Verlauf zugleich.
- „Rückmeldungen“/Statuszeile öffnen den Verlauf. Schließen/Escape gibt Fokus
  an den Rückmeldungszugang zurück. Beim Folgen eines Eintrags Quelle sichtbar
  machen und sinnvoll fokussieren. Tab/Shift+Tab und Aktivieren vollständig
  unterstützen, kein globaler modaler Fokusfang.
- Fenster-/Bereichswechsel erhalten Ereignisbezug. Alte Karte ist kein Beweis,
  dass die inzwischen aktive Datei ebenfalls gespeichert oder aktuell ist.

## Echte Daten und native Integration

Den vorhandenen Ereignisbestand von **Protokoll C** verwenden. Karten und kurzer
Verlauf sind weitere Ansichten desselben Ereignisses, keine zweite unabhängige
technische Fehlerhistorie. Originalmeldungen und Protokollfilter erhalten.
Für Gruppierung und Aktionen mindestens Operation/Ursache, Projekt/Dokument/
Lauf/Quelle, Ziel-Fenster und relevante Revision eindeutig verbinden.
Die Demo gruppiert mangels mehrerer Ziele nur nach Typ; diese Vereinfachung
nicht in den nativen Code übernehmen.

Aktionen gegen die aktuelle Existenz/Gültigkeit des festgehaltenen Ziels prüfen.
Bei geschlossenem/entferntem Ziel verständlich reagieren; nicht ein anderes
Projekt oder neue Revision stillschweigend verwenden. Navigation verändert
keine Datei, rechnet nicht automatisch neu und bestätigt keine Konfliktwahl.
Ein veraltetes Ergebnis behält seine Daten; kein automatisches Ersetzen.

Schreib-/Zwischenablageerfolg aus echtem Rückgabewert ableiten. Wiederholter
Fehler bleibt Fehler; kein optimistisches Erfolgsbild. Speicheralternative
nutzt nativ den vorhandenen Export mit demselben festgehaltenen Stand, keine
unabhängige Demo-Datei. Pfadprüfung verwendet das reale Dateisystem und dessen
Rechte; die feste Demo-Pfadliste ist ausdrücklich kein Produktverhalten.

Lokale Fehler im zugehörigen echten Projekt-/Explorer-/Exportfenster nach
[ADR 015](../../decisions/015-independent-task-windows.md) anzeigen und
Hauptfenster bedienbar lassen. Ein kurzer Hinweis ist kein neues OS-Dialogfenster.
Die HTML-Referenz verlangt keine WebView. Galerie, vereinfachte Kontextwerkbank,
Beispielcode/-zahlen, Szenariowahl, Fehlerfreigabe-Checkbox, „Im Beispiel“-Aktionen
und Links zu separaten Referenzseiten gehören nicht ins Produkt.

## Übergabe und Abnahme

Am aktuellen lokalen nativen Stand weiterarbeiten; laufende fremde Änderungen
erhalten. [README mit Quellen](README.md), [historischer Browsernachweis](review/VALIDATION.md)
und [aktuelle Freigabeprüfung](review/APPROVAL-VALIDATION.md) lesen. Historische
offene Auswahl ist durch diese Freigabe aufgehoben, native Funktion nicht bewiesen.

Nativ prüfen: echte Erfolgs-/Fehlerrückgaben, Eingabe-/Exportstand erhalten,
korrekte Ereignisgruppierung und Protokollidentität, fehlendes Ziel, mehrere
Fenster, veraltete Revisionen, Schließen/Wiederöffnen/Fokus, optionaler Timer
mit Pausen, schmale Fenster/große Schrift, Hell/Dunkel/Hoher Kontrast,
Tastatur/Screenreader. Windows/Linux/macOS nur mit tatsächlich ausgeführten
Prüfungen als abgenommen bezeichnen. Fehlende Plattformbelege offen führen.

Alle bisherigen Freigaben bleiben gültig. [Gesamtauftrag](../unified-workspace/NEXT-CHAT.md)
und [UI-Liste](../unified-workspace/UI-DESIGN-BACKLOG.md) führen D05 als entschieden.
D06 „Fortschritt und Abbruch“ und D07 bleiben eigene offene Entwurfsrunden.
