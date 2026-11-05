# Suchfokus nach einem Anwendungswechsel

Im herausgelösten Editor blieb die Eingabe im Suchfeld nach einem Wechsel zu
einem anderen Programm erhalten. Eine per Tab fokussierte Suchschaltfläche
verlor dagegen ihren logischen Fokus. Die neue Probe verwirft diesen alten
Zustand sowohl unter Windows als auch unter Linux exakt nach der Rückkehr
vom separaten Prozess (Stufe 50).

Der SDL-Adapter behandelt jetzt den Verlust des OS-Fensterfokus getrennt von
einem Mausklick: Beim Anwendungswechsel wird die Tastaturhoheit sofort entzogen,
der logische Bedienfokus aber erhalten. Die nächste Veröffentlichung zeichnet
den Fokus erst bei tatsächlich aktivem Fenster; verschwundene oder deaktivierte
Ziele werden weiterhin verworfen. Ein Mausklick verwirft wie bisher das alte
Ziel. Layout, Farben, Suchinsel und Quelltextverhalten wurden nicht geändert.

Die erweiterte native Probe verwendet denselben separaten Peerprozess wie die
vorhandene Editorprüfung. Dieser bestätigt tatsächlichen OS-Fokus. Danach prüft
sie den Suchfeldfokus samt weiterer Eingabe und den Schaltflächenfokus samt
ausbleibender Texteingabe in Suche oder Quelle. Für C und Ph werden Experiment,
Analyse und zusätzliche Datei geprüft: **12 Sequenzen je Plattform**. Quellbytes,
Cursor, Auswahlanker und Scrollwerte müssen dabei erhalten bleiben.

Windows Release und Alpine Linux Debug mit ASan/UBSan bestehen jeweils:

- **4/4 Datenprüfungen:** Accessibility-Fokus, Suspendierung, Texteingabe und Optionen.
- **3/3 UI-Gruppen:** erweiterte Zusatzfensterprobe, Einstellungen und Werkbank.

Die Datenprüfungen verwenden unter Linux aktive Leak-Prüfung; die GUI-Prozesse
schließen externe Grafikallokationen wie bisher davon aus. Vor dem Build waren
alle drei geänderten Dateien ausdrücklich im 303-Dateien-Manifest enthalten und
zwischen Projekt, Isolation und Manifest bytegleich. Beide Zustände und die
Runtimearchive sind eingefroren. Acht repräsentative Bilder wurden gegen
erhaltene, bereits geöffnete Referenzen gelesen und sind vollständig bytegleich.
Es wurden keine zusätzlichen Bildzustände für die neue Fokusprobe eingeführt.

Der [strukturierte Bericht](search-focus-2026-11-05.json) enthält Originalbefehle,
Exitcodes, Programme, Ergebnis-/Logpfade und Prüfsummen. Die geskripteten SDL-
Text-/Tastenereignisse ersetzen keine physische Tastatur- oder Screenreader-
Abnahme. Der Test richtet den Quelltextfokus erst nach beiden Suchprüfungen für
die vorhandene Fortsetzung wieder ein; das ist kein Nachweis der vollständigen
Tastaturnavigation von der Suche zurück in den Quelltext. Neue macOS-Abnahme
und vollständige UI-Abnahme bleiben offen; der private Nutzer-Mac bleibt aus.
