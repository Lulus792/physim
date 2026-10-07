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
noch aus. Windows/UI Automation ist noch nicht angebunden. Linux besitzt jetzt den
unten beschriebenen AT-SPI-Einstieg.
Die portable Modellprüfung allein beweist keinen praktischen Screenreader-Zugang.

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


## Linux über AT-SPI

Die App registriert einen gemeinsamen Anwendungsroot am Accessibility-Bus.
Hauptfenster und Handbuch sind getrennte Fenster mit eigenen Objektpfaden;
sichtbare Texte und einfache Buttons verwenden denselben geprüften Frame-
Snapshot wie macOS. Rollen, UTF-8-Namen, Zustände, Index-/Elternbeziehungen,
Fenster-/Bildschirmrahmen und Hit-Tests werden über die Standardinterfaces
`Accessible`, `Application`, `Component` und bei Buttons `Action` angeboten.
Ein Bulk-Cache und Ereignisse für hinzugefügte/entfernte Controls, Zustände und
Rahmen halten Clientansichten aktuell. Eine fremde oder veraltete Kennung,
ein verborgenes Fenster oder ein deaktivierter Button aktiviert nichts.

Der Busdispatcher läuft in einem eigenen Thread. Er greift unter den
festgelegten Server-/Modellmutexen auf kopierte Werte zu; Aktionen werden
weiterhin im normalen UI-Frame angenommen. Eine erst nach dem Zeichnen
eingetroffene Aktion bleibt für den nächsten Besuch eines weiterhin sichtbaren,
aktivierten Controls erhalten; entfernte oder gesperrte Ziele verwerfen sie. Beim Schließen eines Fensters
werden seine Objekte entfernt. Die letzte Fensterfreigabe beendet Dispatcher
und Busverbindung. Ohne passenden Accessibility-Bus bleibt die normale GUI
verfügbar. `NO_AT_BRIDGE=1` deaktiviert diese Desktop-Anbindung ausdrücklich.

Es gibt höchstens acht native Fenster pro Prozess und die bestehenden 256
Elemente/1023 Label-Byte je Modell. Fokus, komplexe Widgets und vollständige
Text-/Editorinterfaces bleiben offen. Bildschirmkoordinaten werden nur
geliefert, wenn SDL die Fensterposition kennt; anderenfalls folgt ein
`NotSupported`-Fehler statt einer erfundenen Position. Der bisher ausgeführte
Clientnachweis verwendet X11/Xvfb unter Debian. Wayland und praktische Orca-
Bedienung sind dadurch noch nicht abgenommen. Ein unterbrochener Bus wird
nicht automatisch neu verbunden; ein Neustart stellt die Anbindung wieder her.

[Die AT-SPI-Registry](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Socket.html),
[Accessible](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Accessible.html),
[Component](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Component.html) und
[Cache](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Cache.html)
sind die primären Protokollquellen. `tests/test_accessibility_atspi.py` verwendet
den unabhängigen PyAT-SPI-Client und explizite D-Bus-Gegenproben an zwei
tatsächlich gezeichneten Fenstern. Er prüft getrennte gleichnamige Buttons,
native UI-Aktionen, Zustands-/Cacheänderungen, verborgene Fenster, unzulässige
RPC-Signaturen und schreibgeschützte Properties.
