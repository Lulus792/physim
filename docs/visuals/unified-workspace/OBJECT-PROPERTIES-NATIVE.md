# Auswahlkopf und Objekteigenschaften im Messlabor

Arbeitsstand vom 23. Oktober 2026, Grundlage
`a23ed541612e89900798a9ea2edf1fee60c6df34`. Dies ist ein weiterer Teilabgleich
des verbindlichen Gesamtauftrags, keine vollständige UI- oder Plattformabnahme.

Maßgeblich sind das ausgewählte [B · Wertefelder](../simulation-designs/MODEL-EDITING.md),
die endgültige Kaskade in `simulation.css`, `inspector.css` und
`inspector-variants.css` sowie die gemeinsame Palette aus `bridge.js`.
Die gemeinsame Vorschau setzt `--panel` und `--window` auf denselben Farbwert;
die stärkere Flächentrennung der älteren einzelnen Inspektorstudie ist damit
kein zusätzliches natives Farbziel. Geometrie und Schriftrollen bleiben B.

## Umsetzung

Oberhalb des Objektabschnitts steht bei vorhandener Auswahl ein eigener Kopf:
34 px großes Zeichen auf einer abgerundeten Auswahlfläche, 10 px Abstand zum
Text, 14-px-Titel und 11-px-Metadaten mit Typ und stabiler ID. Unbenannte Objekte
verwenden ihren tatsächlichen Typ. Namen von Gruppen und Beschriftungen
stammen aus dem tatsächlichen Szenentext. Lange Namen und Metadaten umbrechen;
die Kopfhöhe wächst mit dem Inhalt. Passt ein Titelwort neben dem Zeichen nicht,
steht der Text darunter mit voller Breite, statt unnötig innerhalb dieses Wortes
umzubrechen. Der Kopf bleibt im Scrollinhalt erhalten, wenn die Gruppe
„Objekt“ zugeklappt ist; er ist kein zusätzlicher feststehender Kopf.

Die read-only Eigenschaften verwenden B-Karten mit 7 px Radius,
10/11 px Innenabstand, 10-px-Bezeichnung, 5 px Abstand und 19-px-Wert.
Zwischen den Karten liegen 8 px. Bei ausreichender Breite stehen zwei Karten
nebeneinander; schmale Flächen und große Schrift wechseln auf eine Spalte.
Zahlen und Einheiten dürfen zusätzliche Zeilen benötigen, statt kleiner
gerechnet oder abgeschnitten zu werden. Einheiten verwenden die sekundäre
Textfarbe. Hochkontrast ergänzt eine vollständig innen liegende Kontur.

Die Karten zeigen vorhandene Szenenwerte: Radius nur bei Kugeln, Weltposition
bei geometrischen Einträgen und Beschriftungen sowie tatsächliche Skalierung
bei Koordinatenrahmen. Die bisherige lokale Koordinateninformation bleibt
zusätzlich erhalten. Organisationsgruppen erhalten keine erfundenen
Weltkoordinaten. Zahlen bleiben bei vier signifikanten Stellen und verwenden
das Dezimalkomma; die zugrunde liegenden SI-Werte werden nicht geändert.
Welt- und Lokalkoordinaten erhalten getrennte 10-px-Bezeichnungen und
14-px-Werte mit sechs signifikanten Stellen und Dezimalkomma. Lange Vektoren
umbrechen. Die normale Aktion „Auswahl ausblenden“ verwendet 11-px-Schrift,
8-px-Innenabstand und 10 px Abstand nach oben. Die sekundäre Aktion
„Alle Objekte einblenden“ verwendet 10-px-Schrift, 7-px-Innenabstand,
4 px Abstand nach oben und eine transparente Ruhefläche. Beide umbrechen
bei Bedarf, veröffentlichen native Buttonaktionen und behalten sichtbaren Fokus.

## Tatsächliche Daten statt Demobezeichnungen

Das native `ps_object` hat keine allgemeine Massenbindung und für geometrische
Objekte bislang keinen zugesicherten Anzeigenamen. Deshalb wird eine Kugel
ohne Namen als „Kugel · ID …“ identifiziert. Die Bezeichnungen und Beispielwerte
„Pendelkörper“, „Masse 1,00 kg“ oder „Radius 0,08 m“ dürfen nicht blind aus dem
HTML übernommen werden: Das tatsächliche Beispielmodell hat einen anderen
Radius und ordnet die Modellmasse nicht über einen allgemeinen Metadatenvertrag
einem beliebigen Kugelobjekt zu. Eine weitergehende fachliche Bindung oder ein
benanntes Objektmodell bleibt umsetzbare Folgearbeit; es wird keine technische
Unmöglichkeit behauptet. Die native Auswahl bleibt an ihre echte Identität gebunden.

## Prüfumfang und verbleibende Arbeit

Stand 1 besteht drei vorhandene Linux-Gruppen. Sein tatsächlicher neuer
Systemschriftprozess endet nach 30 Aufnahmen erfolgreich; die Python-Gruppe
scheitert anschließend am zu breiten Datensatzpräfix, das auch den Erfolgsmarker
erfasst. Der enger begrenzte Parser besteht den lokalen Nachlauf auf denselben
30 Aufnahmen. Der Fehlerlauf bleibt vollständig erhalten.

Stand 2 besteht unter Alpine-Dev mit GCC, ASan/UBSan und Xvfb/Openbox **4/4**:
Szenensichtbarkeit mit Schrift 16/22, vollständiger Messlaborablauf und die neue
Objektprüfung. Letztere besteht System- und mitgelieferte Schriften mit insgesamt
72 Bilddatensätzen. Der MSVC-Release-Build unter Windows ist erfolgreich;
ein aktueller Windows-Objekt-GUI-Lauf wurde nicht ausgeführt.

[Läufe, Quell-/Fontgraphen und Bilder](object-properties-2026-10-23/validation.json)
und [unveränderte native Bildbelege](object-properties-2026-10-23/README.md)
halten diesen Umfang fest. Die Gegenprobe besteht mit dem realen alten Bild
ohne Auswahlzeichen, dem unnötigen Wortumbruch und den folgenden tatsächlichen
Auswahl-/Wert-/Aktionen. Die aktuellen schmalen Bilder zeigen das vollständige
Wort „Stangenkraft“ unter dem Zeichen anstelle des früheren „Stangenkr / aft“.

Der neue Prüfweg kompiliert das tatsächliche C-Pendelmodell, startet den Runner
und pausiert ihn. Die native Tastaturauswahl wechselt zu Kugel ID 3, Gruppe
ID 101 und Beschriftung ID 11. Je sechs Theme-/Schriftzustände werden mit
Systemschriften und mitgelieferten Schriften betrachtet. Kopf, Zeichen,
Kartenraster, tatsächlich gezeichnete Zahlen/Einheiten und per Mausrad
erreichbare letzte Eigenschaft werden aus nativen Bildern und Eingaben geprüft.
Die zusätzliche Zeigerprobe klappt die Kugelgruppe zu und auf, blendet die
Auswahl über ihre Aktion aus, blendet alle Objekte wieder ein und wählt dieselbe
stabile ID erneut per Tastatur. Quelle, Szenensnapshot, pausierte Zeit und aufgezeichnete Messdatei müssen
unverändert bleiben. Koordinatenrahmen sind in dieser Pendelprobe nicht enthalten;
ihre neuen Karten benötigen einen eigenen Laufnachweis.

Der vollständige Szenenbaum und weitere Inspektorgruppen sind weiterhin
abzugleichen. Der vorangehende Windows-Nachtest der Kopfkorrektur besteht
anschließend über den separaten unveränderten Prüfstand unter
`F:/Physim/Pruefstaende/Inspektor-5`. Er gilt für Inspektor-Stand 5 und ist kein
Windows-Nachweis der späteren Objektänderungen. macOS, praktische Screenreader-
Aufgaben und physische DPI-/Monitorwechsel sind ebenfalls nicht nachgewiesen.
