# Native B-Wertefelder – 24. Oktober 2026

Der [Umsetzungsbericht](../RUN-FIELDS-NATIVE.md) beschreibt die Karten,
Eingabeverträge und verbleibende Gesamtaufgaben. Dies ist eine automatisierte
native Teilabnahme, keine vollständige Abnahme der Oberfläche.

## Ergebnisse und Herkunft

Die Feldprüfung besteht unter Windows/MSVC Release und Alpine-Dev/GCC Debug
mit ASan/UBSan: jeweils 50 Aufnahmen pro Schriftquelle, also 200 Feldaufnahmen.
Sechs Darstellungen umfassen Hell/Dunkel, Kontrast, UI 16/18/20/22 sowie schmale
Breiten. Geprüft werden echte Eingaben, sichtbare Zeichen, Kartenmaße, leere
Haltepunkte, ungültige Einstellungen, Lauf-Sperre und unveränderte Quell-/Laufdaten.
Der ursprüngliche Inspektortest besteht zusätzlich unter Windows mit beiden
Schriftquellen, vollständigen Titelwörtern, Scrollen und Tastatur-/Mausbedienung.
Seine 102 Aufnahmen ergeben zusammen 302 native Bildhashes.

`validation.json.gz` ordnet die Läufe zu. `linux-validation.json.gz` und
`windows-validation.json.gz` enthalten Feldrechtecke, tatsächliche Textkommandos
und die Bedienergebnisse. `windows-headings-validation.json.gz` enthält die
zusätzliche Kopfprüfung. Einzelne PNGs illustrieren die Ergebnisse;
`capture-hashes.json.gz` erfasst alle 302 Originalaufnahmen.

Die fünf Bindungsgruppen bestehen: Projekteinstellungen, adaptive Läufe,
Haltepunkte, Parameterformulare und Parameterstudien mit Einheiten. Die ersten
vier wurden mit SDK-Quelle 6 geprüft; der abschließende Studienlauf mit Quelle 7.
Quelle 7 korrigiert gegenüber 6 nur die Hover-Farbrolle des Standardknopfs und
das veraltete Physim-Suchmuster der Studienfixture. Die vier bereits bestandenen
Bindungsgruppen wurden dafür nicht erneut ausgeführt. Native Feld- und
Windows-Kopfprüfungen sowie Parameterkatalog-/Projektdateitests wurden mit
SDK-Quelle 7 ausgeführt. Die Testrunnerprüfung besteht separat auf beiden Systemen.
`regressions-binaries.json` und `study-binaries.json` belegen unveränderte SDKs
während der schreibgeschützten Nachläufe.

## Gegenfälle und verworfene Prüfstände

- `negative0.json.gz` und `negative-old-parameters.png`: alte generische
  Parameterfelder; Öffnen der Einstellungen bricht an `layout->row.tree_depth`
  ab. Der zugehörige Lab-Abschnitt hatte keinen Nuklear-Baum geöffnet.
- `partial1` / `partial2`: der neue Eingabetest beobachtet vor dem UI-Zeichnen
  und damit einen Frame zu früh. Die direkte Laufzeitdiagnose zeigt aktives
  Editieren und korrekte Auswahl; eine zusätzliche Verarbeitungsphase behebt
  den Test. `partial-long-number.png` hält außerdem die noch ungekürzte Zahl fest.
- `partial3`: die gesperrte Parameteraufnahme erfolgt nach dem Umschalten zur
  Einstellungsgruppe. Die unabhängige Pixelkontrolle erkennt die falsche
  Zuordnung. Der nächste Stand wechselt erst nach der Aufnahme.
- `partial4`: bei schmaler Breite liegt das erste Einstellungsfeld teilweise
  unter dem sichtbaren Bereich. Der Prüfer verlangt vollständige Sichtbarkeit;
  die finale Fixture erreicht sie vor der Aufnahme über echte Scrollereignisse.
- `prior4-parameter_form_workflow`: „5e+01“ statt „50“ zeigt, dass die erste
  exakt zurücklesbare Zahl noch nicht die kürzere Schreibweise sein muss.
  Der finale Formatter vergleicht die Längen gültiger Kandidaten.
- `prior4-parameter_units_workflow`: veralteter Workspace-Knopf statt aktuellem
  Menüweg. `prior6-study`: C einschließlich Wiederöffnen besteht; die
  Physim-Vorbereitung sucht noch eine Literalzahl statt `lengthDefault`.
  Der finale Studienlauf aktualisiert beide tatsächlichen Bedien-/Quellwege
  und behält die Metadaten-, SI-, Endzeit-, Diagramm- und Laufdatenprüfungen.

Die fehlgeschlagenen Gesamtfälle bleiben fehlgeschlagen archiviert. Ein
erfolgreicher interner App-Marker ersetzt keine bestandene Bildprüfung.

## Reproduzierbarkeit und Grenzen

`run-fields-source0` bis `7` enthalten LF-normalisierte Dateihashes und
unveränderte Font-/Fremdhashes. Die passenden `run-fields-delta`-Dateien enthalten
die öffentlichen Überschreibungen gegenüber `d46f4435e9f209d75dd28ab7e8ff463bfd66c7a7`.
Die finale Map umfasst 272 Dateien und 18 Fontdateien. Drei fremde Dateien
bleiben im lokalen SDK-Kompiliergraphen; alle zwölf fremden Änderungen wurden
unverändert erhalten und nicht in diesen Commit aufgenommen. Dies ist kein
Clean-Checkout-Nachweis. `manifest.json` sichert die 66 eingefrorenen Belegdateien.

SDL-Ereignisse, tatsächlicher Fensterfokus und OpenGL-Aufnahmen belegen die
automatisierten Abläufe. Praktische Screenreader-Aufgaben, macOS, physische
Monitor-/DPI-Wechsel und vollständige Browser-Pixelgleichheit bleiben offen.
Die Gesamtoberfläche einschließlich F1 ist weiterhin nicht vollständig abgenommen.
