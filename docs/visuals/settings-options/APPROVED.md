# Verbindliches Einstellungsdesign A · Kategorien

**Freigegeben vom Nutzer am 29. Oktober 2026.** A gehört zum fertigen Design.
Die Auswahlphase ist abgeschlossen; diese Spezifikation im bestehenden nativen
Auftrag umsetzen. B/C sind ausschließlich Vergleichshistorie. Keine erneute
Variantenentscheidung vorschalten. [ADR 018](../../decisions/018-selected-settings-a.md).

[Festgelegte Referenz](index.html?approved=1&group=appearance&theme=dark) ·
[nur Fensterinhalt](index.html?approved=1&group=appearance&standalone=1) ·
[historischer Vergleich](index.html?approved=0&variant=a) ·
[Inhalts- und Quelleninventar](README.md).
`approved=1` und der Standardaufruf erzwingen A, auch bei `variant=b/c`.

## Aufbau und visuelle Referenz

- Eigenes Aufgabenfenster „Einstellungen“, Untertitel „Persönlich · gilt für
  diese Anwendung“, Änderungsanzahl und Schließen oben. Suche direkt darunter.
- Links die Kategorien in fester Reihenfolge: **Darstellung**, **Editor &
  Sicherung**, **Simulation**, **Arbeitsfläche**, **Anzeigeeinheiten**.
- Mitte: Titel, kurze Erklärung und die Optionen der aktiven Kategorie, durch
  ruhige horizontale Linien getrennt. Beschriftung/Hilfe links, Eingabe rechts;
  drei Theme-Kacheln nebeneinander. Keine Übersichtskarten aus B oder
  Einzelwert-Navigation aus C übernehmen.
- Rechts: Live-Vorschau mit Entwurfstheme, Schrift, schematischer Szene,
  Panelverhältnissen und Protokoll; darunter geänderte Werte. Hinweis
  „noch nicht übernommen“ unterscheidet Entwurf und aktiven Stand.
- Fußleiste: **Standardwerte**, Änderungsstatus, **Abbrechen**,
  **Änderungen prüfen**, **Übernehmen**. Kein wirksamer Entwurf: Übernehmen und
  Prüfen deaktiviert; Ausnahme bewusstes Speichern nach unlesbarer Konfiguration.

Maßgebliche Dateien: `settings.css` (Tokens/Layout), `data.js` (Felder),
`settings.js` (A und Zustände). Browsermaße sind logische Referenzmaße,
keine physischen Pixelvorgaben unabhängig von DPI/Schrift. Bei breiter Ansicht:
Kategorie 190 px, Optionen flexibel, Vorschau 290 px; Inhaltsabstand 24 px,
Feldabstand 22 px, Zeilenabstand 17 px, Bedienelemente mindestens 34 px hoch.
Fensterrahmenradius 11 px, Eingaben 5 px, Buttons 6 px.

| Rolle | Dunkel | Hell |
| --- | --- | --- |
| Arbeitsfläche | `#16171b` | `#e9ebef` |
| Panel | `#1e1f23` | `#f6f7f9` |
| Kopf/Fuß | `#25262b` | `#edeff3` |
| Eingabe | `#191a1e` | `#ffffff` |
| Kontur | `#3b3d45` | `#bec3ce` |
| Text | `#edeef2` | `#20242c` |
| Sekundärtext | `#a6a9b1` | `#555e6c` |
| Akzent | `#8abaff` | `#17569e` |
| Primäraktion / Text | `#1965ce` / Weiß | `#1556ac` / Weiß |
| Kategorieauswahl | `rgba(170,184,202,.15)` | `rgba(90,108,130,.11)` |
| Auswahlkante | `#abb9ca` | `#65788d` |

Kategorieauswahl erhält eine 3-px-Kante links; Text bleibt lesbar. Text-/Objekt-
Markierung folgt weiterhin [A2](../selection-options/APPROVED.md). Hoher Kontrast:
Schwarz/Weiß, gelber Akzent `#fff44f`, dunkler Text auf Primäraktion.
Weitere Fehler-, Hinweis- und Vorschaufarben genau aus `settings.css` übernehmen.

Bei knapper Breite folgt die Vorschau unter den Optionen, während Kategorien
links bleiben; bei sehr schmaler Ansicht Kategorien oben umbrechen und Inhalt
untereinander scrollen. Referenz-Breakpoints 1200/1000/700 px; für native
Fenster nach tatsächlich verfügbarem Platz und UI-Schrift anpassen. Suche,
Fehler und Fußaktionen erreichbar halten, keine Inhalte abschneiden. Große
Schrift, Tastaturfokus, High Contrast und reduzierte Effekte berücksichtigen.

## Vollständige Inhalte und Werte

| Kategorie | Optionen |
| --- | --- |
| Darstellung | Dunkel/Hell/Hoher Kontrast; UI-Schrift 16/18/20/22 logische px |
| Editor & Sicherung | Codeschrift 12/14/16/18/20/22 px; Autosave 10/30/60/120 Sekunden |
| Simulation | Vektoren, Flugbahnen, Punkte, Beschriftungen, Raster/Achsen, orthografische Ansicht |
| Arbeitsfläche | Protokoll geöffnet; Seitenleistenbreite 140–480, Inspektorbreite 180–480, Protokollhöhe 126–500 px, ganzzahlig |
| Anzeigeeinheiten | Aktuelle Einheiten und Einstieg „Anzeigeeinheit bearbeiten…“ in eigene Aufgabe |

Die Größenfelder bearbeiten gespeicherte Ausgangswerte für **Entwickeln**.
Nicht alle Workspaces auf diese Werte setzen. Laufende Sitzungsgrößen,
herausgelöste Fenster, [gemeinsames Resizing](../unified-workspace/RESIZING.md)
und feste Paneltopologie erhalten. Explizite Größenänderung hier betrifft
diesen Entwurf; ein reiner Theme-/Schriftentwurf darf parallel veränderte
Panelgrößen nicht aus einer veralteten Kopie zurückschreiben.

Defaults aus `PS_PREFERENCES_DEFAULT`: Theme dunkel; UI 16, Code 12, Sicherung
30 s; Seite 218, Inspektor 255, Protokoll 340 px; Sichtbarkeitsmaske 31
(Vektoren/Bahnen/Punkte/Beschriftungen/Raster an, Ortho/Protokoll aus).
Der normale Demo-Ausgangsstand UI 18/Code 14 usw. ist **kein neuer Produktdefault**.
Reale Werte aus dem vorhandenen Preferences-Bestand laden und erhalten.

## Verbindliche Bedienung und Zustände

| Auslöser | Ergebnis |
| --- | --- |
| Kategorie wechseln / suchen | Entwurf bleibt erhalten; Suche über alle Kategorien, passende Felder mit aktueller Eingabe. Kein Treffer mit Rücksetzen der Suche. |
| Wert ändern | Nur Entwurf/Miniatur/Änderungsstatus aktualisieren; kein unmittelbares Schreiben oder globaler Themewechsel. |
| Änderungen prüfen | Tabelle Einstellung/Aktuell/Entwurf, vollständige Änderungen; Zurück erhält Werte und Fokus. Unabhängige Einheiten-/Layoutänderungen nicht einmischen. |
| Übernehmen | Alle Werte validieren, Ressourcen vorbereiten und vorhandenen sicheren Speicherablauf verwenden; erst bei Erfolg aktivieren und Fenster schließen. |
| Schreib-/Schriftfehler | Inline verständlich melden; aktive Werte/bisherige Datei und Entwurf erhalten; kein Erfolg anzeigen, erneut versuchen ermöglichen. |
| Ungültige Größe | Bereich erklären, Eingabe erhalten, kein Speichern; Korrektur ermöglichen. |
| Abbrechen / Fenster schließen | Allgemeinen Entwurf verwerfen, zuletzt angewendete Werte behalten; unabhängige Geometrie und Einheiten erhalten. |
| Standardwerte | Nur Entwurf ändern. Erst Übernehmen speichert; keine Fenstergeometrie oder feste Panelplätze zurücksetzen. |
| Unlesbare Konfiguration | Mit Defaults und Warnung öffnen; Original bis bewusstem Übernehmen erhalten. Abbrechen schreibt nichts. |
| Erneut öffnen | Tatsächlich angewendeten Stand und aktuelle unabhängige Geometrie lesen, keinen alten verworfenen Entwurf. |

Suchfeld beim Öffnen fokussieren; Tab/Shift+Tab logisch durch sichtbare
Kategorien, Eingaben und Aktionen, ohne Fokusverlust bei Aktualisierungen.
Pfeiltasten/Leertaste für native Auswahlfelder; sichtbarer Fokus mit Mit-Scrollen.
Escape verlässt zuerst die Änderungsprüfung, danach verwirft es den Entwurf.
In der Einheitenaufgabe schließt Escape nur diese Aufgabe. Bestehende
Einstellungsbefehle und plattformspezifische Tastenkürzel erhalten.
Screenreader brauchen Namen, Gruppen, Werte und verständliche Statusmeldungen.

## Eigene Anzeigeeinheitenaufgabe

Die gezeigte Aufgabe gehört zum ausgewählten Ablauf: Kanal, Vorgabe,
Symbol, Faktor „SI pro Anzeigeeinheit“, direkte Umrechnungsvorschau,
**SI wiederherstellen**, **Schließen**, **Einheit anwenden**.
Die Fixture-Kanäle Position/Geschwindigkeit sind Beispiele; nativ echte
Messkanäle und vorhandene Kataloge verwenden. Keine erfundenen Kanäle einbauen.

Einheitenentwürfe je Kanal behalten, wenn der Kanal gewechselt wird. Vorgabe
oder SI-Wiederherstellen ändern zunächst den Einheitenentwurf; erst „Einheit
anwenden“ übernimmt ihn. Schließen verwirft noch nicht angewendete Änderungen
dieser Aufgabe. Allgemeines Abbrechen lässt separat angewendete Einheiten und
eine noch offene Einheitenaufgabe einschließlich ihres Entwurfs bestehen.

Symbol höchstens 63 UTF-8-Bytes, nicht leer, keine äußeren Leerzeichen oder
Steuerzeichen; Faktor positiv und endlich. Über-/Unterlauf bei realen Daten
prüfen, keine Offseteinheiten ergänzen. Rohwerte, Modell und CSV bleiben
unverändert; Berichte verwenden ihre vorhandenen Analyse-Einheiten. Vollständige
native Datenvalidierung beibehalten, nicht auf zwei Vorschauwerte reduzieren.

## Native Umsetzung, Abgrenzung und Abnahme

Einstellungen und Anzeigeeinheiten nach [ADR 015](../../decisions/015-independent-task-windows.md)
als eigene tatsächliche OS-Aufgabenfenster umsetzen. Hauptfenster und andere
Fenster bleiben bedienbar. Unabhängige Änderungen korrekt zusammenführen;
Preferences, Workspace-/Fenstergeometrie, benannte Layouts und Einheitenkatalog
nicht in eine gemeinsame Transaktion mischen.

Die HTML-Dateien sind Referenz für die bestehende native UI, keine Vorgabe
einer WebView. Galerieheader, Variantenwahl, Szenarioauswahl, „Nur Fensterinhalt“,
Paralleländerungs-/Fehlerfreigabeknöpfe und Erfolgsseite mit „erneut öffnen“ sind
Prüfhülle, keine zusätzlichen Produktseiten. Bei Erfolg das echte Aufgabenfenster
schließen; keine Demo-Erfolgsseite bauen. Keine Miniatur-Simulation berechnen;
sie veranschaulicht Darstellungsänderungen. Reale Autosave-/Schrift-/Preferences-
Funktionen erhalten, laufende native Änderungen berücksichtigen.

Abnahme nach Implementierung: Kategorien und genaue Werte, Vorschau versus
aktive Anwendung, vollständiger Vergleich, Abbruch/Defaults, wiederholte Fehler
und sichere Wiederholung, parallele Geometrie/Einheiten, echte Aufgabenfenster,
kleine Fenster/große Schrift, Themes, Tastatur/Screenreader und echte
Windows-/Linux-/macOS-Prüfungen. Fehlende Plattformprüfung offen benennen.
[Browserbelege](review/VALIDATION.md) und [Freigabeprüfung](review/APPROVAL-VALIDATION.md)
bestätigen ausschließlich die Referenz, keine fertige native Umsetzung.

Alle bisherigen Freigaben bleiben gültig: A2, B2, Grafikexport, Bereichsauswahl,
Projekt B, Explorer A. Der [Gesamtauftrag](../unified-workspace/NEXT-CHAT.md)
ist der Einstieg für den umsetzenden Chat.
