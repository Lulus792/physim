# Kandidaten-Kurzinfo: Dokumentbesitz und Inhalt

Quellcommit **`25dd37dca639005f9640fe49fa774b03d595feec`** in beiden Repositories.
[Tatsächlicher Git-Abgleich und neuer offener CI-Stand](completion-help-commit-2026-11-04.json).

Die neutrale D-Karte zeigt die echte Kurzbeschreibung des gewählten Katalogeintrags,
seine Signatur in den gemeinsamen Syntaxrollen, Art und Referenzquelle. Alle Zeilen
der vorhandenen zweispaltigen Argumenttabelle bleiben erhalten. Deren tatsächliche
Spaltenüberschrift bleibt sichtbar: „Bedeutung im Beispiel“ bezeichnet weiterhin
Beispieltexte, keine erfundenen allgemeinen Parameterregeln.

Ein eigener Cache besitzt eine geparste Referenzseite pro Vorschlagsliste, unabhängig
von der Inspektor-Karte. Ein anderer Eintrag derselben Seite braucht keinen erneuten
Dateizugriff. Auswahlwechsel binden Namen und Abschnitt neu; ein Fehler zeigt keine
Beschreibung der vorherigen Auswahl. Beim Schließen der Liste wird der Cache
freigegeben. Die vorübergehend ausgeblendete Kurzinfo darf ihn weiterverwenden.
Allgemeine Sprachhandbuch-Fallbacks erhalten keine erfundene typspezifische Beschreibung.

Fehlende Dokumentation bleibt lokal erklärt; Name/Signatur bleiben nutzbar,
der Referenzknopf ist gesperrt. Ein neuer Versuch nach dem Schließen lädt erneut.
Die Karte bleibt höchstens 330 × 260 logische Einheiten groß, kann bei Platzmangel
schmaler werden und scrollt vertikal mit 6-Einheiten-Leiste. Kurze Karten erhalten
keine unnötige Scrollleiste. Mausinteraktionen erreichen keinen darunterliegenden
Editor/Inspektor. Nach Übernahme/Rückkehr bleibt der Editor Tastaturbesitzer bis
zum bewussten Fokuswechsel; Undo wurde unverändert geprüft.

## Ausgeführter Nachweis

Windows Release/MSVC und Alpine-Dev Linux Debug/GCC mit ASan/UBSan unter Xvfb.
166 eigene Eingaben, 1.118 Compiler-/Testdateien. Fremdes AX-/Orca-/Workflow-/Status-
WIP ausgeschlossen. [Originalkommandos, Ergebnisse, Hashes und Archivbeziehungen](completion-help-2026-11-04.json).

| Abgrenzung | Windows | Linux |
| --- | --- | --- |
| Cache-/Grenzfallstand `completion-help-routing`: Unit `completion_help`, `completion`, `api_reference` | 3/3 `run-bfibuvnu` | 3/3 `run-9wl5kmg1` |
| Fokuskorrektur `completion-help-focus`: manuelle/automatische Completion und API-Zugang | 3/3 `run-fk1dm48r` | 3/3 `run-llec6gt7` |
| Abschließende Geometrie/Rendererfixture `completion-help-bottom`: manuelle Completion und API-Zugang | 2/2 `run-l1ud0_3b` | 2/2 `run-xbzg1kzk` |

Danach wurden nur UI-Inhalt/Geometrie und ihre gezielten Prüfer geändert; die
Cache-Unit-/Automatik-Komponenten bleiben per Quellhash abgegrenzt. Keinen neuen
vollständigen 3-Unit-/3-GUI-Lauf mit der letzten Binärdatei behaupten.

Manuelle Completion führt C und Physim in Modell, Analyse und weiterem Dokument
aus: dunkel/Code 12, hell/UI und Code 22, Kontrast/Code 12. Tatsächliche Zeichenbefehle
bestätigen Beschreibung, Rollen und Semibold-Präfix beim selben Kandidaten/Editor/
Theme über das Scrollen hinweg. Referenzklick, Quelldaten, Übernahme und Undo bleiben
geprüft. Der reale C-Referenzfall zeigt beide vorhandenen Argumentzeilen; eine
**separate ausdrücklich synthetische Rendererfixture** weist auch den dritten
gezeichneten Eintrag nach. Keine drei Zeilen im echten Referenzbestand behaupten.
Inspektordokument und dessen Scrollversatz sowie Editor-Scrollversatz bleiben erhalten.
Fehlende Referenz, gesperrter Klick und erneutes Laden nach Schließen tatsächlich geprüft.

Zusätzlich besteht die aktuelle Cache-Unit direkt unter ASan/UBSan mit
`detect_leaks=1`: eigene Dokumente, Seitenwiederverwendung, vollständige Tabelle,
Fehlercache, keine alten Abschnitte/Pfade, Guide-Fallback, Größenlimit und ungültige
Anfragen. Normaler C17-Compileraufruf mit `-Wall -Wextra`; kein Warnings-clean-Versprechen.
Ein erster `-Werror`-Hilfsaufruf scheiterte an einer unveränderten Warnung in
`documentation.c:411`; Originalbefund und tatsächlicher Folgeaufruf sind erhalten.

## Erkannte Fehler und Gegenprobe

- Erste Builds scheiterten vor nativen Tests an der fehlenden Vorwärtsdeklaration
  von `documentation_line`. Deklaration ergänzt; beide Folge-Unitgruppen bestanden.
- Langer Inhalt blieb zunächst unerreichbar: Nuklear benötigt die aktive Wurzel für
  Gruppen-Scrollen; außerdem konnte ein darunterliegender Dockbereich das Mausrad
  vorher verbrauchen. Erst temporäre Kartenroute plus Abschirmung vor dem Zeichnen
  der Dockbereiche beheben beide Teile. Die Prüfungen blieben streng.
- Nach wirklichem Kartenscrollen erreichte Ctrl+Z den Editor nicht mehr. Der
  ursprüngliche Text blieb nach Undo verändert; keine Prüfung gelockert. Explizite
  Tastaturrückkehr und Besitz bis zum nächsten Fokuswechsel schließen diesen Fall.
- Bildprüfung fand eine unnötige Leiste an kurzen Karten. Horizontaler Reservestreifen
  entfällt, Padding wird mitgemessen. Die Folgeprüfung fand ganzzahlige Scrolloffsets
  gegenüber gebrochenen Schriftmetriken: Footer 0,1/0,2 Einheiten außerhalb. Zwei
  Einheiten Schlussabstand erhalten den vollständigen Knopf samt strenger Sichtprüfung.
- Isolierter Windows-Gegenfall `run-cgtbhjhm` **0/1**: ausschließlich die Glyphen
  der Beschreibung unterdrückt, Katalog, Dokument, Signatur und Messung unverändert.
  Tatsächliche Ausgabe `summary=0`, `own-document=1`, `reference-visible=1`, alle
  Syntaxmerkmale 1; die C-App wird an Stufe 5 abgelehnt. Physim/Linux hierfür nicht
  ausgeführt. Mutant-Quellen und ursprüngliche Bytes archiviert, Projektwurzel unverändert.

Nach der Gegenprobe wurden Quellstand und **sämtliche 239 Windows-/129 Linux-
Laufzeitdateien exakt aus dem bestandenen Stand wiederhergestellt**. Alle Hashes
und referenzierten ZIP-Einträge gelesen; `completion-help-restored` ist der neue
eingefrorene Anschluss. Das ist kein erfundener weiterer Testlauf mit identischen Bytes.
Alle lokalen Build-/Test-/Archivhandles beendet. Vier aktuelle native Bilder
tatsächlich angesehen: Windows dunkel, hell/groß und Fehler/Kontrast, Linux Fehler/Kontrast.

27 ersetzte erfolgreiche Completion-/API-BMPs aus `completion-help-focus` entfernt:
**86.317.326 Bytes**. Pfade, Schutz, Hashes, Ersatzbilder und laufende Nutzung vorher
geprüft. Aktuelle Ansichten, Fehler, Gegenprobe, eine ursprüngliche visuelle
Vergleichsaufnahme, Quellen, Archive, Daten, Abhängigkeiten und VM erhalten.
Objektordner sind Define-/Sprachvarianten, keine automatisch veralteten Quellstände;
keine pauschale Cache-Löschung. Nur D:, kein F:-Zugriff.

## Weiter offen

Keine vollständige D-/UI-Abnahme: verzögerte Kurzinfo nach Auswahl, versteckte
Trefferflächen, Tastatur-Scrollen und Assistive-Bedienung weiter prüfen. Echte lokale
Scopes, Kurzinfos mit Quellstellen statt erfundener API-Einträge und die separate
Aufrufsignatur bleiben offen. Ebenso übrige Codekontexte, physische IME/Eingabe,
DPI/Monitor/Vollbild und aktuelle macOS-Completion-Abnahme.

Der Nutzer-Mac bleibt aus und wurde nicht kontaktiert. Gehosteter **vorheriger**
Syntaxstand `f97344fb`: Einstellungen Intel/ARM je **16/16**, F1 ARM **6/6**, F1
Intel **5/6**. Alle vier ZIPs/Digests/Originalergebnisse gelesen. Intel
`documentation_h` erreicht bei 60,026 s an Stufe 39 das Zeitlimit; Ursache offen,
keinen Produkt- oder Harnessfehler allein daraus ableiten.
[Getrennter Mac-Nachweis](syntax-roles-macos-2026-11-04.json). Er enthält diese neue
Kandidaten-Kurzinfo nicht. Neue F1-Untersuchung braucht diesen konkreten Befund,
keine Wiederholung geschlossener Matrizen allein wegen Kontextwechsels.
