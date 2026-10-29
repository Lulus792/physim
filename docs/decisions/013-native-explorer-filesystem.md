# ADR 013: Dateisystemmodell für den nativen Explorer A

Status: angenommen für die schrittweise Umsetzung, 28. Oktober 2026.
Die Gestaltung ist durch [ADR 011](011-selected-workspace-additions.md) bereits
freigegeben; dies ist keine neue Variantenentscheidung.

## Bestehender Vertrag und Alternativen

Die Workspace-Seitenleiste verwendet `ps_workspace_tree`: einen begrenzten,
aufklappbaren Baum mit mehreren Wurzeln. Der Systemdialog meldet seine Auswahl
über `workspace_dialog_event` an die vorhandenen Öffnen-/Importfunktionen.
Explorer A benötigt dagegen eine Ordnerliste mit Typ, Größe, Änderungsdatum,
Such-/Sortierzustand und fester Vorschau. Die Auswahl allein darf weder den
geöffneten Workspace noch einen Experiment- oder Analyseeditor ersetzen.

Eine Wiederverwendung des Baums würde seine rekursive Sicht, Identität und
Erweiterungszustände mit der unabhängigen Dialognavigation koppeln. Zusätzliche
Metadaten-/Filteranforderungen würden auch die bestehende Seitenleiste ändern.
Eine unabhängige flache Momentaufnahme hält diese Verträge getrennt und lässt
sich ohne Renderer gegen echte Dateien prüfen. Die bestehenden SDL-/Pfadadapter
bleiben die gemeinsame Betriebssystemgrundlage. Gewählt ist die zweite Struktur.

## Zuständigkeiten

- `file_browser.h/.c` besitzt Dateisystemeinträge, begrenzte Auflistung,
  Pfadauflösung, Filter, Sortierung, Typprüfung und exklusives Anlegen eines
  einzelnen Unterordners. Es zeichnet keine Widgets und öffnet keine Nutzerdaten.
- `file_browser_worker.h/.c` besitzt einen laufenden und den jüngsten wartenden
  IO-Auftrag. Eigene Pfad-/Namenskopien, atomare Abbruch-/Endsignale und ein
  Versionsvergleich schützen die Übernahme. Ordnerlesen, begrenzte Textvorschau,
  erneute Auswahlprüfung und exklusive Anlage laufen außerhalb der Zeichnung.
  Abbruch wartet nicht auf einen laufenden OS-Aufruf; endgültiges Zerstören
  verbindet den Arbeiter vor der Speicherfreigabe. Eine bereits ausgeführte
  Ordneranlage wird durch Abbruch nicht rückgängig gemacht.
- Der Controller in `file_browser_ui.inc` besitzt Verlauf, Auswahl und
  den Bezug der Vorschau zum aktuellen Ort. Ein gescheiterter Ortswechsel darf
  die übertragene alte Liste nicht als Inhalt des neuen Orts ausgeben.
  Messlauf-/Bildvorschau bleiben zusätzlich zur UTF-8-Textvorschau offen.
- Die native A-Oberfläche zeichnet Orte, Werkzeugleiste/Pfad, Liste und Vorschau
  samt Fußzeile. Dieselbe Oberfläche dient der freien Datei-/Ordnernavigation,
  Messlaufauswahl, Serienfortsetzung und Ordnerwahl innerhalb von Projektdialog B.
- Bestätigte Ergebnisse gehen an die vorhandenen Workspace-/Importfunktionen.
  Deren Datei-, Format-, Projekt- und Runnerprüfungen bleiben maßgeblich.

## Fehler, Grenzen und Migrationsfolge

Auflistungen besitzen ihre Pfade selbst. Nur vollständige oder ausdrücklich als
begrenzt/beschädigt markierte Ergebnisse ersetzen einen alten Snapshot. IO-,
Speicher-, Eingabe- und Abbruchfehler erhalten ihn; die Oberfläche muss einen
gescheiterten Ortswechsel eindeutig anzeigen und darf alte Inhalte nicht als
erfolgreich neu eingelesen ausgeben. Eine Bestätigung prüft den aktuellen Objekttyp
erneut, statt veraltete Metadaten als Öffnungsberechtigung zu verwenden.

Das vorhandene 4096-Byte-Pfadlimit bleibt vorerst bestehen. Unter Windows werden
normale Laufwerks-/UNC-Pfade intern für den Betriebssystemzugriff in die
erweiterte Form überführt; die Oberfläche erhält lesbare normale Pfade. Wurzeln
bleiben beim Aufwärtswechsel erhalten.
Auf POSIX werden tatsächliche Dateisystemziele mit den nativen Symlink-/`..`-
Regeln aufgelöst. Die reine Textkürzung der Breadcrumb-Hilfen darf nicht als
Ersatz dafür dienen. Neue Unterordner beziehen sich auf dieses tatsächlich
aufgelöste Elternverzeichnis. Unter Windows werden Trennzeichen und Punktsegmente
vor dem Überführen in die erweiterte Form aufgelöst.

Maximal 8192 Einträge und kooperativer
Abbruch begrenzen die Auflistung; ein bereits laufender Kernelaufruf lässt sich
damit nicht abbrechen. Die Zahl ausgelassener Einträge ist beim frühen Stopp nur
eine Untergrenze und darf nicht als vollständige Gesamtzahl dargestellt werden.

Neue Ordner werden mit einem exklusiven Betriebssystemaufruf angelegt. Bestehende
Dateien, Verzeichnisse oder Links werden nicht als erfolgreiche Neuanlage
ausgegeben. Bei Fehler bleibt der Rückgabepfad erhalten. Es gibt keine Probe-
Schreibzugriffe beim bloßen Anzeigen einer möglichen Speicherposition.
Die Bestätigung unter Windows folgt dem Ziel von Reparse Points über einen
geöffneten Metadaten-Handle. Eine Junction mit verschwundenem Ziel darf trotz
noch lesbarer Linkattribute nicht als vorhandener Zielordner bestätigt werden.

Die Pfad-/Exklusivverträge beruhen auf den vorhandenen SDL-3.2.30-Adaptern und
den [Win32-Verzeichnisaufrufen](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createdirectoryw).
Die [erweiterte Laufwerks-/UNC-Schreibweise](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)
wird intern verwendet, ohne die Systemkonfiguration für lange Pfade zu ändern.
Der [SDL-Adapter dieser konkreten Version](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/filesystem/windows/SDL_sysfsops.c)
verwendet für Metadaten `GetFileAttributesExW`; die zusätzliche Zielprüfung wird
durch eine reale Junction-Gegenprobe unabhängig von den bloßen Linkattributen geprüft.

Die erste Modellsortierung nutzt SDL-Unicode-Casefolding mit stabiler
Byte-Reihenfolge bei Gleichheit. Das ist keine Behauptung identischer deutscher
Browserkollation; der endgültige native Abgleich muss diesen Unterschied
entweder schließen oder konkret ausweisen. Modellprüfungen ersetzen keine
visuelle, Tastatur-, Screenreader- oder Plattformabnahme der A-Oberfläche.

Modell und Fehlerpfade sind geprüft; Controller und native Oberfläche sind seit
29. Oktober produktiv angebunden. Die alten Systemdialogadapter dienen noch ihren
ausdrücklich benannten Tests. Die [native Umsetzungsnotiz](../visuals/unified-workspace/EXPLORER-A-NATIVE.md)
trennt die tatsächlichen Teilprüfungen von offenen Gestaltungs- und Plattformaufgaben.
Insbesondere bleiben Bild-/Messlaufvorschau, freie eingebettete Exploreransicht,
große native Listen und vollständige bereichsübergreifende Aufgaben offen.
