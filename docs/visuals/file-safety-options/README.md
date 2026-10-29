# Speichern, Dateikonflikte und Wiederherstellung – B2 freigegeben

**29. Oktober 2026 · B2 ausdrücklich für das fertige Design freigegeben.**
Die bisherige Einstufung als getrennter, noch nicht übernehmbarer Entwurf ist
aufgehoben. [Verbindliche Spezifikation](APPROVED.md) und
[ADR 017](../../decisions/017-selected-file-safety-b2.md) gelten für die native
Fortsetzung. Bestehende Bereichs-, Export-, Fenster- und A2-Freigaben bleiben erhalten.

Der Nutzer hat zunächst B bevorzugt, dann B2 mit gemeinsamer Änderungsansicht
vergleichen lassen und anschließend **B2 ausdrücklich angenommen**. A/B/C bleiben
Vergleichsmaterial. Die Browserdaten sind weiterhin nur Beispieldaten;
die Designfreigabe bestätigt keine fertige native Funktion oder Plattformabnahme.

[Freigegebene Referenz öffnen](index.html?approved=1&topic=save) ·
[A](index.html?topic=save&variant=a) · [B](index.html?topic=save&variant=b) ·
[B2 · Git-Diff](index.html?topic=save&variant=b2) ·
[C](index.html?topic=save&variant=c)

Lokal direkt per `index.html` oder über den bestehenden Vorschau-Server unter
`http://127.0.0.1:8791/file-safety-options/`. Keine Bibliotheksinstallation,
externen Schriften, Dienste oder echten Dateioperationen in der Vorschau.
Zeitstempel, Pfade und Inhalte sind festgelegte Beispieldaten.

## Freigegebenes B2 und die Vergleichshistorie

| Richtung | Speichern | Dateikonflikt | Wiederherstellung |
| --- | --- | --- | --- |
| **A · Kompakt** | Dateiliste, klare Abschlussaktionen; Inhalte ausklappbar | Warnung und Wahl; Versionsvergleich bei Bedarf | Gemeinsame Sicherung mit kurzer Entscheidung; Inhalt bei Bedarf |
| **B · Vergleichen** | Dateinavigation links, Entwurf und gespeicherter Inhalt rechts | Entwurf und externer Stand direkt nebeneinander | Sicherungsinhalt links, Sicherung und gespeicherte Fassung rechts |
| **B2 · Git-Diff**, final freigegeben | Ein Dateivergleich von gespeichertem Stand zu Editorentwurf | Aktuelle externe Fassung und Entwurf in einer gemeinsamen Änderungsansicht | Aktuelle Datei und automatische Sicherung in einer gemeinsamen Änderungsansicht |
| **C · Schritt für Schritt** | Bestand prüfen → Entscheidung und Wirkung | Erst Fassungen betrachten, danach Umgang wählen | Erst Sicherung prüfen, danach bewusst laden oder verwerfen |

Farben, A2-Markierung und Datengrundlage sind absichtlich gemeinsam. Die
Unterschiede betreffen Informationsmenge, Anordnung und Ablauf. Die drei Themen
verwenden jetzt verbindlich B2; eine neue getrennte Variantenwahl ist nicht nötig.

### Ausgewählte Darstellung B2

- Dieselbe Dateinavigation und dieselben Entscheidungen wie B; rechts **eine**
  gemeinsame Datei mit Kontextzeilen, entfernten Zeilen mit Minus/Rot und neuen
  Zeilen mit Plus/Grün. Alte und neue Zeilennummern stehen links. Zeichen ergänzen
  die Farben; hoher Kontrast erhält die Unterscheidung ohne Farberkennung.
- Zähler und Markierungen werden tatsächlich aus den beiden jeweiligen
  Beispieltexten berechnet. Auswahl einer anderen Datei oder eine neue externe
  Revision aktualisiert den Vergleich. Keine fest markierte dritte Zeile.
- Die Legende benennt die Richtung: **− gespeicherte/aktuelle Datei**, **+ Entwurf**
  beziehungsweise **+ automatische Sicherung**. „Neu“ ist die Vergleichsseite,
  keine Behauptung eines späteren Zeitpunkts der Sicherung.
- Fehlt die Datei, gibt es nur hinzugefügte Zeilen und den Hinweis „Datei fehlt“.
  Fehlt eine lesbare Sicherung, wird kein Vergleich mit erfundenem Inhalt gezeigt.
- Die gemeinsame Ansicht ist **keine zusammengeführte oder gespeicherte Datei**.
  Es gibt keine Git-Anbindung, keinen Merge, keinen Patch-Export und keine
  Übernahme einzelner Änderungen. Die vorhandenen Entscheidungen bleiben erhalten.
- Der kleine Zeilenvergleich ist auf die Vorschau begrenzt. Bei mehr als 100.000
  Vergleichszellen zeigt er ausdrücklich einen vollständigen Ersatz statt eines
  feinen Abgleichs; native Großdatei-/Performanceverträge sind separat zu entwerfen.

[B2-Prüfung und Referenzbilder](review/b2/VALIDATION.md).

Die große Arbeitsfläche ist eine **eingebettete Dialogvorschau**, damit die
Variantenumschaltung erreichbar bleibt. **Als Popup testen** öffnet dieselbe
Gestaltung als echtes HTML-Modal mit Tastaturfokus und Escape. Der native
Physim-Dialogrenderer ist dadurch nicht implementiert oder abgenommen.

Die HTML-Modalsperre ist nur eine Prüfhülle dieser Browsergalerie. Für die spätere
native Umsetzung gilt bereits [ADR 015](../../decisions/015-independent-task-windows.md):
eigene OS-Aufgabenfenster, während Hauptfenster und herausgelöste Bereiche
bedienbar bleiben. Die Freigabe B2 entscheidet Inhalt und Ablauf, nicht über
eine neue globale Modalsperre. Nur den betroffenen Speichern-/Wechselauftrag
anhalten, fremde Eingaben erhalten, den Dateistand vor einer Änderung erneut
prüfen und Fokus nicht aus einem inzwischen anderen aktiven Fenster stehlen.

## Ausprobierbare Zustände

| Thema | Szenarien | Tatsächlich im Browser durchspielbar |
| --- | --- | --- |
| Speichern | Ungespeichert / Schreibzugriff verweigert / teilweise gespeichert | Drei virtuelle Dateien nacheinander speichern; Wechsel erst nach Erfolg; Fehler hält an; Wiederholen ohne freigegebenen Beispielzugriff schlägt erneut fehl; Verwerfen erfordert die angezeigte Bestätigung |
| Dateikonflikt | Extern geändert / gelöscht / schreibgeschützt / während Entscheidung erneut geändert | Kopie mit neuem Namen im Beispiel vormerken; Dateistand nach Bestätigung laden; Entwurf weiter bearbeiten; bei neuer Revision erneut prüfen |
| Wiederherstellung | Sicherung vorhanden / Original inzwischen geändert / Sicherung unlesbar / keine Sicherung | Entwurf zunächst nur laden, später gesondert speichern; Sicherung bewusst verwerfen oder erhalten; beschädigte Inhalte nie laden |
| Sicherungsumfang | Projektquellen zusammen / zusätzliches Dokument | Zwei Projektquellen als Einheit, unabhängiger Dokumententwurf als eigener Umfang |

Variantenwechsel erhält den aktuellen Stand, die Entscheidung und eingegebene
Kopienamen. Zurück im geführten Ablauf erhält die Eingaben. Themen-/Szenario-
oder Sicherungsumfangwechsel startet den jeweiligen Beispielzustand neu.
„Diesen Zustand erneut ansehen“ setzt ebenfalls nur diese Beispieldaten zurück.

### Zusammenhängender Ablauf

1. **Wiederherstellung → Sicherung laden:** Gesicherte Texte sind im virtuellen
   Editor, bleiben aber ungespeichert. Kein Build oder Lauf startet.
2. **Speichern prüfen:** Nur die tatsächlich wiederhergestellten Quellen kommen
   in den Speicherablauf: beide Projektquellen oder genau das zusätzliche Dokument.
3. Bei zwischenzeitlich extern geändertem Original wird Speichern blockiert.
   **Dateikonflikt prüfen** übernimmt die betroffene Quelle und erhält alle
   übrigen Entwürfe.
4. Nach bewusstem Laden einer externen Fassung kann der Speicherablauf für die
   übrigen Quellen fortgesetzt werden. Eine Entscheidung über `main.c` ist keine
   Freigabe für eine ebenfalls extern geänderte `analysis.phys`.

## Gemeinsame Sicherheits- und Interaktionsregeln

- Entwürfe, gespeicherte Inhalte und automatische Sicherung klar benennen.
  **Sicherung laden ist kein Speichern.** A2-Flächen liegen hinter lesbarer Schrift.
- Dateistände werden nacheinander gespeichert. Bei Teilfehlern keine gemeinsame
  Dateisystemtransaktion behaupten: bereits gespeicherte Dateien bleiben gespeichert,
  weitere Entwürfe offen und der angefragte Projektwechsel angehalten.
- Abbrechen verhindert den angefragten Vorgang. Ein bereits abgeschlossener
  Speicherschritt wird nicht rückgängig gemacht. Spätere Timerantworten eines
  abgebrochenen Beispielablaufs dürfen keinen neuen Erfolg melden.
- Extern geänderte Dateien nicht still überschreiben oder automatisch mergen.
  Eine beim Prüfen erneut geänderte Fassung braucht eine neue Entscheidung.
- Verwerfen oder Ersetzen eines Entwurfs zeigen die konkrete Folge vor der
  Bestätigung; Zurück erhält die Daten. Escape aus dieser Bestätigung geht zurück.
- Das echte Test-Popup hält Tab im Dialog. Escape/Abbrechen erhält offene Arbeit
  und gibt den Fokus an den auslösenden Button zurück. Auswahlcontrols behalten
  ihren Tastaturfokus bei Inhaltswechseln.
- Unlesbare Sicherungen bleiben erhalten. Der angebotene Weiterarbeitsweg lädt
  nur gespeicherte Quellen, hält Autosave für die Quelle pausiert und löscht nichts.
- Kein Touch-/Hover-Aktionsmenü neu einführen. Die bestehenden Regeln zu A2 und
  Rechtsklick gelten weiter; diese Entwürfe betreffen Entscheidungsdialoge.

## Bezug zum tatsächlichen Physim-Stand

Geprüfte Grundlagen: [Autosave](../../autosave.md),
[weitere Dokumente / Speichern](../../workspace.md#weitere-textdateien-bearbeiten),
`app/file_tabs_ui.inc`, `app/main.c`, `app/project_manager_ui.inc` am lokalen Stand.
Vor Übernahme den dann aktuellen Code erneut abgleichen; parallele native Arbeit
ist keine abgeschlossene Gestaltungsauswahl dieser Entwürfe.

| Belegter Vertrag | Entwurfsfolge |
| --- | --- |
| Projekt-Autosave enthält Experiment und Analyse gemeinsam; weitere Dokumente besitzen eigene Entwürfe | Keine unabhängigen Wiederherstellungscheckboxen für die zwei Projektquellen; Dokumentumfang separat wählbar |
| Autosave verändert Quelldateien nicht, Wiederherstellung lädt Editorinhalte | Laden und Speichern als zwei ausdrücklich getrennte Schritte |
| Native Quellen werden einzeln gespeichert, vorhandener Stand als `.bak` gesichert | Einzelstatus und Teilfehler statt pauschalem „alles zurückgerollt“ |
| Externe Änderungen blockieren das Speichern; kein automatischer Merge | Versionsbezug sichtbar, Entscheidung erneut prüfen, keine stille Überschreibaktion |
| Beschädigte Sicherung wird nicht automatisch geladen oder überschrieben | Unlesbare Sicherung erhalten; mit gespeicherten Dateien weiterarbeiten |

**Browsergrenze gegenüber dem nun beauftragten nativen Umfang:**
Die Nachfrage vor dem Projektwechsel, die gemeinsame Änderungsansicht und die
Konfliktentscheidungen sind mit B2 angenommen. Die Aktion **Entwurf als Kopie
vormerken** existiert in der Demo nur im Arbeitsspeicher. Es wird keine Datei
geschrieben, heruntergeladen oder in Explorer A angelegt.
Eine native Kopienfunktion braucht echte Zielwahl über den vorhandenen Explorer A,
Namens-/Rechte-/Existenzprüfung sowie Fehler- und Abbruchbehandlung. Es wird kein
Diff-/Merge-Editor und kein Durchsuchen unbekannter Sicherungspfade implementiert.

Das bisherige automatische Speichern bei normalem Projektwechsel/Beenden ist
belegt. Die Nachfrage bei betroffenen ungespeicherten Quellen ist jetzt mit B2
angenommen; vorhandene Autosave-/Backup- und Speicherschutzregeln dabei erhalten.

## Verbindliche native Fortsetzung

**B2 gemäß [APPROVED.md](APPROVED.md) übernehmen**, im vorhandenen
nativen Dialog-/Datei-/Autosave-Vertrag umsetzen. Datenverlustfragen, Dateirechte,
weitere externe Änderungen während eines Dialogs und echter Teilfehler brauchen
native Prüfungen. Ein Browsererfolg belegt weder reale Dateioperationen noch
Windows-/Linux-/macOS-Funktionsgleichheit.

Die Runde bearbeitet D01–D03 der [UI-Designliste](../unified-workspace/UI-DESIGN-BACKLOG.md).
Prüfer: [check_file_safety_options.cjs](../../../tools/check_file_safety_options.cjs).
Ausgeführte Nachweise und Referenzbilder: [review/VALIDATION.md](review/VALIDATION.md).
Die [Prüfung der verbindlichen B2-Referenz](review/approved/VALIDATION.md) ergänzt
den historischen Vergleichsstand nach der Nutzerfreigabe.
