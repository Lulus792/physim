# Prüfung der freigegebenen Kombination

25. Oktober 2026 · Windows · Microsoft Edge 156.0.4314.42 · Playwright headless.

- `node tools/check_approved_design.cjs`: **Exitcode 0**, 28 Kombinationen aus
  sieben Themen, zwei Themes und 1440/500 px; keine JavaScript-Fehler.
- `node tools/check_design_options.cjs`: **Exitcode 0**, alle 84 bestehenden
  Vergleichs-/Leeransichtslayouts und acht Interaktionsgruppen weiterhin bestanden.
- `node --check` für die geänderten JavaScript-Dateien und Git-Whitespace-Prüfung
  bestanden. Die Auswahl bleibt auch bei widersprechendem `variant`-URL-Parameter
  fest; URL/Reload behalten den Freigabemodus.

Maschinenlesbar: [Auswahlprüfung](validation.json),
[Vergleichsregression](comparison-regression.json),
[geprüfte lokale Quellbytes](source-hashes.json).

Die reale Browseraufgabe prüft **Protokoll C / Szene A / Leeransicht →
Projektdialog B → Datei-/Ordner-Explorer A**, Ordnerwahl, Zurück ohne Eingabeverlust,
Abschluss der Projektvorschau und Rückkehr zum unveränderten Elternlayout.
Die historische Vergleichsgalerie kann die eingefrorene Nutzerentscheidung nicht
überschreiben. Die vollständige Leeransicht bleibt ohne geladenes Demoexperiment.

Sichtbar und visuell geprüft:
[Protokoll C](approved-protocol-c.png),
[Projektdialog B aus Protokoll C](approved-project-b.png),
[darin Explorer A](approved-picker-a.png).

Ein anfänglich nicht sichtbarer Menüeintrag in der Prüfaufgabe wurde durch den
tatsächlich sichtbaren Leeransichts-Auslöser ersetzt. Kein nativer Quellcode wurde
geändert. Native Dateioperationen, Plattformabnahme und Screenreader bleiben
Aufgabe der weiteren Umsetzung; dieser Nachweis bestätigt die Browserreferenz
und die dokumentierte Auswahl, keine bereits fertige native Anwendung.
