# Barrierefreiheit

Die vollständige Tastatur- und Screenreader-Bedienung ist noch in Arbeit.
Die vorhandene Tastaturführung für Hauptmenüs, Einstellungen, Projekterstellung
und beide Dokumentationswege bleibt verfügbar. F10 öffnet die Hauptmenüs;
Pfeiltasten, Tab/Shift+Tab und Enter bedienen die jeweils dokumentierten Bereiche.
UI-Schriftgrößen von 16, 18, 20 und 22 Pixeln sind unabhängig von der Codegröße.

## Nativer macOS-Einstieg

Die macOS-App veröffentlicht sichtbare einfache Schaltflächen und Beschriftungen
nun über AppKit als native Accessibility-Elemente. Beschriftungen besitzen
UTF-8-Namen und die Rollen Schaltfläche oder statischer Text. Die sichtbaren
Rahmen folgen dem Fenster und der aktuellen Darstellung. Aktivierte Buttons
können eine native Press-Aktion entgegennehmen; deaktivierte, entfernte oder
veraltete Elemente lösen keine Aktion aus. Eine angenommene Aktion wird genau
einmal im nächsten UI-Frame an den passenden Button übergeben.

Dieser Einstieg deckt noch keine vollständige VoiceOver-Bedienung ab.
Textfelder, Editoren, Menüs, Auswahlfelder, Regler, Szenen, Diagramme und
programmatische Fokusführung benötigen weitere semantische Anbindungen.
Die bestehende Sichtbarkeits-/Deaktivierungslogik wird berücksichtigt, sodass
dieser Einstieg keine Aktivierung gesperrter oder momentan schreibgeschützter
UI-Bereiche ermöglicht. Eine vollständige praktische VoiceOver-Abnahme steht
noch aus. Linux/AT-SPI und Windows/UI Automation sind noch nicht angebunden.
Die portable Modellprüfung allein beweist keinen Screenreader-Zugang dort.

## Modell und Prüfumfang

Das private App-Modell veröffentlicht kopierte Frame-Snapshots. Kennungen
bleiben bei Größen-/Positionsänderungen stabil und werden nach Verschwinden
eines Elements nicht wiederverwendet. Doppelte Beschriftungen werden pro
Nuklear-Fenster und Rolle unterschieden. Es gibt höchstens 256 sichtbare
Elemente pro Snapshot und 1023 Byte pro Beschriftung; längere Texte werden an
UTF-8-Skalargrenzen gekürzt. Nichtendliche Rahmen, leere/ungültige Texte und
überzählige Elemente werden nicht veröffentlicht. Diese Grenzen sind keine
Abnahme vollständiger Dokumente oder Editoren.

Das Modell ist unter einem SDL-Mutex serialisiert; native Elemente werden vor
Freigabe des Modells ungültig. Zurückbehaltene native Referenzen können danach
keine Aktionen mehr auslösen. Die macOS-Anbindung benutzt typisierte Objective-C-
Laufzeitaufrufe aus C17 und systemeigene AppKit/CoreGraphics-Frameworks.
[Apples Anleitung für eigene Controls](https://developer.apple.com/library/archive/documentation/Accessibility/Conceptual/AccessibilityMacOSX/ImplementingAccessibilityforCustomControls.html)
beschreibt die native Elementhierarchie, Rollen, Rahmen und Änderungsmitteilungen.

`tests/test_accessibility.c` prüft veröffentlichte Snapshots, stabile/veraltete
Kennungen, doppelte Namen, deaktivierte Aktionen, UTF-8 und Kapazität.
`tests/test_accessibility_native.c` prüft tatsächliche AppKit-Rollen, Labels,
Rahmen, Aktionen und zurückbehaltene Referenzen.
`tests/test_accessibility_ui.c` prüft den Weg von gezeichneten Nuklear-Controls
über native Aktionen zurück zur tatsächlichen Button-Auswertung. Diese
Prüfungen ersetzen keine Abnahme mit einem laufenden Screenreader.
