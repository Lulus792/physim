# Browserprüfung · 4. November 2026

Prüfkommando: `node tools/check_compute_device_options.cjs`.
Windows/Edge: bestanden. 90 Layoutkombinationen (A/B/C, fünf Zustände,
drei Themes, 1440/390 px) ohne horizontalen Seitenüberlauf.
Kopplung mit Codebestätigung, Entkoppeln, Rechnerauswahl erst nach Kopplung,
Testabschluss in allen Varianten, Testfehler, Abbruch, Offline-Sperre,
Escape und Fokusrückkehr geprüft.

Erster Lauf: Testselektor für den Fehlertext war mehrdeutig (Option/Aufgabentitel).
Auf Aufgabenbereich begrenzt; anschließender vollständiger Lauf bestanden.
A/B/C-Dunkelansichten und Kopplungsdialog visuell gesichtet: passende Hierarchie,
lesbare Werte, keine sichtbaren Überschneidungen. Bilder sind gezielte Referenzen.
DPI-/Textgrößen-, echte Screenreader- und native Plattformprüfungen bleiben offen.
Beispielwerte sind keine Messungen. sourceCommit in validation.json bezeichnet
die Basis vor diesen Änderungen; geprüft wurden die neuen lokalen Quellen.
