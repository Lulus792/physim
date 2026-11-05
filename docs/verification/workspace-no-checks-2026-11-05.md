# Workspace ohne Häkchen

Der Nutzer hat am 5. November 2026 die vollständige Entfernung der Häkchen
aus dem Workspace-Dateibaum verlangt. Der native Renderer zeichnet dort keine
Häkchen oder Checkboxrahmen mehr. Auch die reservierte Spalte entfällt:
Dateisymbole, Namen, Hierarchielinien und Ordnerpfeile rücken entsprechend nach links.
Die aktive Datei behält ihre vorhandene Zeilenhervorhebung und den Seitenakzent;
der Tastaturfokus bleibt getrennt erkennbar.

Die Präzisierung steht in AGENTS, [ADR016](../decisions/016-selected-marking-a2.md)
und der [A2-Spezifikation](../visuals/selection-options/APPROVED.md). Die frühere
allgemeine Listenregel darf die Häkchen hier nicht wieder einführen. Historische
Referenzbilder bleiben unverändert. Andere Checkbox-Bedienelemente waren nicht
Gegenstand dieser Änderung.

Geprüft werden die vorhandenen nativen Abläufe `workspace_tree_workflow` und
`project_tree_workflow`: verschachtelte Ordner, zusätzliche Wurzeln, Dateiöffnung,
Modell-/Analysezuordnung, Assetbearbeitung, Ein-/Ausklappen, Messlauföffnung und
Erhaltung bei ungültigen Laufdateien. Dafür wurde kein neuer Test geschrieben,
der lediglich das Entfernen eines Zeichenbefehls nachbildet. Je Plattform zwei
gezielte Ansichten prüfen die tatsächliche Darstellung ohne Kästchen und ohne
übrig gebliebene Checkboxspalte.

Ausgeführte Kommandos, Programme, Quellstand und Ergebnisse stehen im
[strukturierten Nachweis](workspace-no-checks-2026-11-05.json).
Die vorherigen [Scrollgriff-/Eingabekorrekturen](signature-thumb-2026-11-05.md)
bleiben ein eigener geprüfter Stand; sie werden nicht allein wegen dieser
schmalen Zeichnungsänderung nochmals vollständig getestet. Eine vollständige
Plattform-, DPI-, Theme- oder Screenreader-Abnahme folgt daraus nicht.

Windows Release und Linux Debug mit ASan/UBSan bestehen jeweils **2/2** dieser
bestehenden Abläufe. Je zwei gezielte Bilder wurden tatsächlich geöffnet;
beide zeigen den Dateibaum ohne Häkchen und mit intakter Auswahlfläche.
