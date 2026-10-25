# Prüfung der getrennten Designgalerie

**25. Oktober 2026 · Windows · Edge 156.0.4314.42 · Playwright, headless.**
Historischer Prüfstand der getrennten Entwürfe: Browserprüfung bestanden,
keine native Abnahme. Die **anschließende direkte Nutzerfreigabe** der konkreten
Kombination ist in [APPROVED.md](../APPROVED.md) festgehalten.

## Tatsächlich ausgeführt

| Prüfung | Ergebnis |
| --- | --- |
| `node tools/check_design_options.cjs` | Exitcode 0; 84 Layoutzustände, acht fachliche Prüfgruppen, keine JavaScript-Fehler |
| `node --check` für `data.js`, `options.js`, `dialogs.js` und den neuen Prüfer | Exitcode 0 |
| Zusätzlicher realer Doppelklick nach großer API-/Codevorschau | A/B/C bei 1440 und 500 px bestanden; Ordner `runs` wird tatsächlich innerhalb der Beispieldaten geöffnet |
| Zusätzlicher Themewechsel in der Leeransicht | Galerie- und Fensterschalter wechseln gemeinsam Hell/Dunkel; [Ausgabe](theme-controls.txt) |
| Abschließender Bildabgleich: Bibliothekskopf in Auswertung B | Kopf überspannt beide Kartenspalten; 1440/500 px in Hell/Dunkel separat bestanden; [Ausgabe](report-header.txt) |
| Git-Whitespace-Prüfung der eigenen Änderungen | Bestanden |

Maschinenlesbare Hauptprüfung: [validation.json](validation.json).
[Lokale Quellbytes als SHA-256](source-hashes.json) dokumentieren das überprüfte
Paket einschließlich der danach separat geprüften Theme-Beschriftung und der
Zeilenspanne des Bibliothekskopfs in Auswertung B.

Reproduktion benötigt Playwright, lokal auf dieser Maschine über:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_design_options.cjs
```

Der Prüfer öffnet lokale `file:`-URLs. Ein laufender Server ist dafür nicht nötig.
Die lokale Galerie im Browser kann zusätzlich unter Port 8791 angeboten werden.

## Umfang

- **72 Variantenlayouts:** sechs Themen × drei strukturelle Richtungen × Hell/Dunkel
  × 1440/500 px, Höhe 1100 px. Auch alle drei tatsächlichen Projekt-Popups.
- **12 Leeransichten:** Entwickeln/Simulieren/Auswerten × Hell/Dunkel × 1440/500 px,
  Höhe 1000 px. Vollständiges Fenster mit erhaltenem Standalone-Link, keine
  Modell-/Szeneninitialisierung und deaktivierte nicht verfügbare Aktionen.
- **Protokoll:** Filter und Suche kombiniert, Quellbeispiel, Originaltext,
  gefilterter echter TXT-Download, fehlende Zwischenablage, 68 Meldungen als Flutfall.
- **Simulation:** Kraftauswahl mit expliziten Beispielmaßstäben, Beschriftungen,
  Parametergrenzen, Verlet mit Widerstand, adaptive Grenzen, elf Kanäle und
  nicht mehr vorhandene Auswahl.
- **Auswertung/Läufe:** festgehaltener Bericht gegen neue Laufauswahl, fehlender
  Kanal, wiederhergestellte Daten, zwei Diagramme, Serien-Teilfehler, Abbruch,
  lange Aufzeichnung und Start-/Antwort-/Limitfälle.
- **Explorer:** Code- und API-Vorschau, echter Doppelklick im Beispielbaum,
  Ordnernavigation, Suche, versteckte Einträge, nur im Speicher angelegter Ordner,
  wiederhergestellter Lauf und Zugriffsfehler.
- **Projekt-Popup:** Duplikatname, verschachtelte Ordnerauswahl mit Escape und
  Zielwahl, getrennte Sprachen, Zurück im geführten Ablauf ohne Eingabeverlust,
  Erfolg ohne Dateioperation, Variantentausch mit erhaltenen Eingaben,
  Analyseprojekt, Tab-Begrenzung im Dialog und nicht beschreibbarer Ort.
- **Zusatzdarstellung:** 760×740 mit 200 % Root-Schrift im Projektdialog und
  erreichbarer Hauptaktion; Forced Colors/Reduced Motion mit Tastaturfokus;
  zusätzliche 2×-Browser-Pixeldichte der vollständigen Leeransicht.

Die native Betriebssystem-Zwischenablage wird in der Prüfung nicht beschrieben;
der verweigerte Kopierzugriff ist nur auf der Testseite ersetzt. Dateiaktionen
der Galerie arbeiten ausschließlich an einem JavaScript-Datensatz.

## Sichtbare Referenzen

Die Dateinamen `*-a/b/c.png` stehen für Werkbank/Fokus/Navigator. Screenshots der
Kandidaten zeigen jeweils den Entwurf selbst, nicht die gesamte Galerieseite.

| Thema | A | B | C |
| --- | --- | --- | --- |
| Protokoll | [A](protocol-a.png) | [B](protocol-b.png) | [C](protocol-c.png) |
| Szene | [A](simulation-a.png) | [B](simulation-b.png) | [C](simulation-c.png) |
| Auswertung | [A](analysis-a.png) | [B](analysis-b.png) | [C](analysis-c.png) |
| Läufe/Serien | [A](runs-a.png) | [B](runs-b.png) | [C](runs-c.png) |
| Projekt-Popup | [A](project-a.png) | [B](project-b.png) | [C](project-c.png) |
| Explorer | [A](files-a.png) | [B](files-b.png) | [C](files-c.png) |

Vollständige Leeransicht:
[Entwickeln dunkel](empty-develop-dark.png) /
[hell](empty-develop-light.png),
[Simulieren dunkel](empty-simulate-dark.png) /
[hell](empty-simulate-light.png),
[Auswerten dunkel](empty-analyze-dark.png) /
[hell](empty-analyze-light.png).
[Große Schrift im Projektdialog](project-large-text.png).

Visuelle Durchsicht: Referenzen der drei Popupstrukturen, vollständige
Leeransichten, Protokollliste/-karten/-herkunft, Kräfte und Objektkontext,
Berichts-/Studienstruktur und Datei-/Kachel-/Spaltennavigation geprüft.
Der Dialog bei großer Schrift bleibt scrollbar; Aktionen werden nicht abgeschnitten.

Korrigierte Befunde: Der Namenswechsel löst beim Verlassen des Textfelds kein
Formular-Neurendern mehr aus, das den anschließenden Submit-Klick verlieren würde.
Die Dateivorschau hat eine stabile Höhe und scrollt intern; so verschiebt eine
umfangreiche Vorschau nicht das Doppelklickziel. Tab bleibt innerhalb des aktuell
obersten Dialogs. Die große Kraftbeschriftung bricht im SVG in lesbare Zeilen um.

## Isolation und Prüfgrenzen

Die bisherige gemeinsame HTML-Oberfläche und der Grafikexport wurden nicht
geändert. Im vorhandenen Übergabepaket stehen nur Auswahlhinweise. Die neue
Galerie lädt keine Bridge und keinen Iframe der bestehenden Vorschau; keine
Netzwerk-/Dateisystem-/Runner-Schnittstelle und kein gespeicherter App-Zustand.
Die parallel laufenden nativen Änderungen sind nicht Bestandteil dieser Prüfung.

Keine ausgeführte native Windows-/Linux-/macOS-Abnahme, kein realer Screenreader,
kein physischer DPI-/Monitorwechsel und kein großer nativer Meldungs-/Datenstrom.
Ein SVG-Szenenbild ist kein neuer Physik- oder 3D-Renderer. Die Leeransicht folgt
der aktuellen gemeinsamen Designsprache, ohne native Pixelgleichheit zu behaupten.
Browsererfolg entscheidet weder über eine Variante noch über deren Übernahme.

Speicherpflege: 29 eigene rohe Browserbilder nach der Prüfung entfernt
(2.056.360 Byte). Die 25 benötigten Design-/Leeransichtsreferenzen und die
Prüfberichte bleiben versioniert. Keine fremden/nativen Ausgaben oder offenen
Fehlernachweise gelöscht, kein Buildverzeichnis pauschal geleert.
