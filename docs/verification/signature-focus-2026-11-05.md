# Texteingabe nach OS-Fokuswechsel im Zusatzfenster

Ein herausgelöster Editor konnte nach der Rückkehr den echten SDL-Tastaturfokus
haben und seine Parameterhilfe zeigen, während der Texteingabekanal ausgeschaltet
blieb. Der Windows-Gegenlauf `run-qd34pas1` zeigt genau diesen Zustand:
`focus=child`, `SDL-text=0`, `UI-owner=panel-file-tabs`, `completion-owner=0`.
Die Hauptfenstergruppe bestand im selben Lauf.

Das Schließen der Vorschlagsliste bei Fokusverlust löschte ihren gemeinsam
gespeicherten Tastaturbesitzer. Das Zusatzfenster merkt sich jetzt seinen zuvor
tatsächlich aktiven Quelleditor und stellt dessen normalen Fokusvertrag erst bei
echtem OS-Fokusgewinn wieder her. Dafür muss er weiterhin aktives Ziel sein;
die Suche darf die Eingabe nicht besitzen. Rückholen und erneutes Herauslösen
verwerfen den gespeicherten Besitzer. Quelle, Auswahl und Anordnung werden
nicht umgebaut; der SDL-Kanal wird nicht unabhängig vom Editor erzwungen.

Der Abschluss besteht unter **Windows Release2/2 (`run-8aio6i_b`)** und
**Linux Debug mit ASan/UBSan2/2 (`run-dx4glpkf`)**. Die beiden GUI-Gruppen
prüfen Hauptfenster und herausgelösten Workspace. C und Physim durchlaufen jeweils
Modell-, Analyse- und Dateieditor: offene Vorschlagsliste, tatsächlicher Wechsel
zum Hauptfenster und zurück, anschließend ein separates Helperprozess-Fenster
mit echter OS-Fokusbestätigung über eine Pipe und erneute Rückkehr. Quelle,
Cursor, tatsächliche Auswahlanker und Editor-Scrollposition bleiben erhalten;
die Hilfe pausiert ohne Fokus, Liste und Textkanal haben die erwarteten Zustände.
Anschließende SDL-Texteingabe und Löschen funktionieren. Eine zusätzliche
OS-Fensterrückkehr erhält eine echte, nicht leere Textauswahl. Die bestehenden
Scrollgriff-/Rückholprüfungen bleiben enthalten.

Die ersten Versuche verbesserten auch die Probe: tatsächliche Zeichendaten statt
eines internen Berechtigungsflags messen; Completion an ihrer gültigen Position
öffnen; nach der Peer-Fokusbestätigung auf neue Zeichenbefehle warten; nach
bewusstem Deselektieren den Vergleichscursor aktualisieren. Diese Versuche sind
im [strukturierten Bericht](signature-focus-2026-11-05.json) getrennt erhalten.
Die dortigen Phasen entsprechen den archivierten Snapshots
`signature-focus-<phase>` unter `build/validation/explorer-inline-2026-10-29`;
`focused` verwendet unverändert die Quelle `signature-focus-accepted`.

Ein Windows-Versuch derselben Abschlussbinärdatei erhielt bereits beim Start
keinen OS-Fokus. Der reguläre Nachlauf bestand mit identischem Programmhash.
Computer Use wurde vorbereitet; die versuchte Aktivierung traf ein bereits
beendetes Testfenster und war keine ausgeführte Fokusaktion. Daraus wird kein
physischer Eingabenachweis abgeleitet. Linux verwendet einen eigenen
Xvfb-/Openbox-Desktop, damit WSLg- und Windows-Fenster nicht konkurrieren.

Alle vier geänderten Quell-/Testdateien wurden vor dem Abschlussbuild bytegleich
gegen Root, Isolation und das Manifest mit303 Eingaben geprüft. Alle acht finalen
Zusatzfensterbilder sind vollständig bytegleich zu bereits geöffneten und
erhaltenen Referenzen; [nur ihre neuen Kopien wurden entfernt](signature-focus-images-cleanup-2026-11-05.json).
Neue Vollbildserien für nichtvisuelle Fokuszustände sind nicht erforderlich.

Der Peer ist ein eigener Prozess mit eigenem OS-Fenster, ausgeführt über einen
speziellen Testeinstieg derselben Binärdatei. Die Texteingaben bleiben geskriptete
SDL-Ereignisse. Physische beziehungsweise OS-injizierte Tasten, Rückkehr aus
Suchfeldern/anderen Controls, Screenreader-Aufgaben und die vollständige
Gestaltungsabnahme bleiben eigene Prüfdimensionen. Die neue Fokusfolge ist auf
Mac noch nicht abgenommen; der ausgeschaltete Nutzer-Mac wurde nicht angesprochen.
