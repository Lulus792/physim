# Barrierefreiheit

Die vollständige Tastatur- und Screenreader-Bedienung ist noch in Arbeit.
Die vorhandene Tastaturführung für Hauptmenüs, Einstellungen, Projekterstellung
und beide Dokumentationswege bleibt verfügbar. F10 öffnet die Hauptmenüs;
Pfeiltasten, Tab/Shift+Tab und Enter bedienen die jeweils dokumentierten Bereiche.
UI-Schriftgrößen von 16, 18, 20 und 22 Pixeln sind unabhängig von der Codegröße.

## Nativer macOS-Einstieg

Die macOS-App veröffentlicht sichtbare einfache Schaltflächen, Checkboxen und Beschriftungen
über AppKit als native Accessibility-Elemente. Beschriftungen besitzen
UTF-8-Namen und die Rollen Schaltfläche, Checkbox oder statischer Text. Die sichtbaren
Rahmen folgen dem Fenster und der aktuellen Darstellung. Aktivierte Buttons
können eine native Press-Aktion entgegennehmen; deaktivierte, entfernte oder
veraltete Elemente lösen keine Aktion aus. Eine angenommene Aktion wird genau
einmal im nächsten UI-Frame an den passenden Control übergeben.

Dieser Einstieg deckt noch keine vollständige VoiceOver-Bedienung ab.
Textfelder, Editoren, Menüs, Dropdowns, Regler, Szenen, Diagramme und
Fokusführung für weitere Widgettypen benötigen zusätzliche semantische Anbindungen.
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
sichtbare Texte, einfache Buttons und Checkboxen verwenden denselben geprüften Frame-
Snapshot wie macOS. Rollen, UTF-8-Namen, Zustände, Index-/Elternbeziehungen,
Fenster-/Bildschirmrahmen und Hit-Tests werden über die Standardinterfaces
`Accessible`, `Application`, `Component` und bei Buttons/Checkboxen `Action` angeboten.
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
Elemente/1023 Label-Byte je Modell. Fokus für weitere Widgettypen, komplexe Widgets und vollständige
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


## Checkboxen und Zustände

Beschriftete Checkboxen in Einstellungen, Szenenbaum, Darstellung und Batch-
Steuerung veröffentlichen nun ihren aktuellen booleschen Wert. Auch die optisch
unbeschriftete Lauf-Auswahl der Bibliothek besitzt den jeweiligen Laufnamen als
nativen Namen. Sichtbare Darstellung und Klickverhalten bleiben erhalten.

macOS meldet `AXCheckBox` mit einem `NSNumber`-Wert 0/1 und `AXValueChanged` bei
Wertänderungen. `accessibilityPerformPress` schaltet den aktuellen Wert einmal
um; direkte Fremdschreibzugriffe auf `accessibilityValue` sind gesperrt.
[Apples Checkbox-Vertrag](https://developer.apple.com/documentation/appkit/nsaccessibilitycheckbox)
trennt Wert und native Aktivierung. Linux meldet `check box`, `checkable` und
gegebenenfalls `checked`. Die Aktion heißt maschinenlesbar `toggle`, lokalisiert
`Umschalten`; `StateChanged:checked` aktualisiert Clients in beiden Richtungen.
[AT-SPI-Zustände](https://gnome.pages.gitlab.gnome.org/at-spi2-core/libatspi/enum.StateType.html)
und [Aktionen](https://gnome.pages.gitlab.gnome.org/at-spi2-core/devel-docs/doc-org.a11y.atspi.Action.html)
definieren die verwendeten Protokollwerte und Rückgaben.

Eine angenommene Aktion verändert den Wert erst beim normalen Besuch des noch
lebenden Controls. Der veröffentlichte Snapshot berücksichtigt den umgeschalteten
Wert. Deaktivierte, verborgene, entfernte oder nachträglich gesperrte Ziele
verwerfen Aktionen. Späte Aktionen zwischen Zeichnen und Veröffentlichung bleiben
für den nächsten Besuch erhalten. Eine Checkbox bleibt bei Wertänderungen unter
derselben Kennung erreichbar. Zwei Fenster besitzen unabhängige Werte. Es gibt
weiterhin höchstens eine ausstehende Aktion pro Modell, keine Tri-State-Checkbox
und den unten beschriebenen Fokusdienst für einfache Controls. Der ältere
Checkbox-Prüfer aktiviert zuerst den Einstellungsbereich; der neue Fokusprüfer
benutzt dafür die native Fokusanforderung.

Die portable Modellprüfung provoziert späte, doppelte und gesperrte Aktionen.
AppKit-Prüfungen lesen tatsächliche Rollen und Zahlenwerte, prüfen Aktivierung,
Readonly-Selektoren und zurückbehaltene ungültige Referenzen. Der gerenderte UI-
Prüfer schaltet sowohl eine beschriftete als auch eine optisch unbeschriftete
Checkbox über die native Schnittstelle. Ein unabhängiger PyAT-SPI-Client prüft
zwei tatsächlich gezeichnete Fenster, zwei Wertwechsel, Cache und Zustandsereignisse
sowie deaktivierte, verborgene und entfernte Checkboxen.
`tests/test_accessibility_checkbox_app.py` führt außerdem die tatsächliche App aus:
Native Aktionen verändern den Einstellungsentwurf zweimal und erhalten die
bereits angewandten Darstellungsflags. Speichern und Neustart werden durch den
bestehenden Tastaturprüfer abgedeckt. Der SDK-Prüfer besitzt dafür `--accessibility-only`;
Linux verwendet einen eigenen Accessibility-Bus und einen unabhängigen Client.
Diese Nachweise ersetzen keine praktische VoiceOver-/Orca-Abnahme und schließen
den gesamten Barrierefreiheitsblock des Projektplans nicht ab.


## Benannte Optionsgruppen

Darstellung sowie Schriftgrößen der Oberfläche und des Code-Editors besitzen
jetzt native Optionsgruppen mit höchstens einem sichtbaren ausgewählten Wert. Die Gruppen
heißen wie ihre sichtbaren Überschriften. Gleichlautende Optionen wie `16 px`
bleiben anhand ihres Elternobjekts unterscheidbar. Gruppen und Optionen besitzen
stabile Kennungen, echte Eltern-/Kindbeziehungen und Geschwisterindizes.

macOS meldet `AXRadioGroup` und `AXRadioButton`. Die Optionen liefern `NSNumber`
0/1; die Gruppe liefert ihre Kinder und ausgewählten Kinder. Änderungen melden
`AXValueChanged` und `AXSelectedChildrenChanged`. Die nativen Rahmen der Kinder
beziehen sich auf die Gruppe. [Apples Radio-Button-Vertrag](https://developer.apple.com/documentation/appkit/nsaccessibilityradiobutton)
verlangt den booleschen Auswahlwert und Wertänderungsmitteilungen. Linux meldet
`grouping` und `radio button`, `checkable` und gegebenenfalls `checked`.
[Die AT-SPI-Rollen](https://gnome.pages.gitlab.gnome.org/at-spi2-core/libatspi/enum.Role.html)
unterscheiden benannte Gruppen von ihren auswählbaren Kindern. Cache, Eltern,
Kindabfragen, Geschwisterindizes, Zustandsereignisse und Rahmen in Fenster-/Eltern-
koordinaten folgen dieser Hierarchie. Die Aktion heißt `select` beziehungsweise
`Auswählen`; eine Auswahlgruppe selbst besitzt keine Aktion.

Auswahl aktiviert eine Option und deaktiviert die übrigen derselben Gruppe.
Erneutes Aktivieren der ausgewählten Option wählt sie weiter aus. Eine native
Auswahl innerhalb des Frames überschreibt später übermittelte alte Auswahlflags;
eine ausdrücklich danach erfolgte Pointerauswahl kann sie wieder ablösen.
Dadurch besitzen veröffentlichter Snapshot und der tatsächliche UI-Wert dieselbe
Auswahl. Separate Gruppen und Fenster beeinflussen einander nicht. Gesperrte,
verborgene und veraltete Optionen folgen denselben Aktionsregeln wie Checkboxen.

`ps_ui_option_label` ist eine private UI-Bindung mit explizitem Gruppennamen.
Wie andere Controls werden nur sichtbare Optionen veröffentlicht. Liegt die
aktuelle Auswahl außerhalb des sichtbaren Ausschnitts, enthält die veröffentlichte
Gruppe keinen ausgewählten sichtbaren Wert. Die Gruppenknoten zählen zum bestehenden Budget von 256 Elementen pro Modell;
fehlender Platz für eine neue Gruppe und ihre erste Option veröffentlicht keine
leere Gruppe. Die portable Prüfung deckt Gruppenidentität, Indizes, exklusive
und idempotente Auswahl, gemischte Pointer-/Native-Ereignisse, UTF-8 und Grenzen ab.
AppKit-Prüfungen lesen Rollen, Eltern, Kinder, ausgewählte Kinder, echte Rahmen
und Werte. Der unabhängige AT-SPI-Prüfer verwendet zwei gezeichnete Fenster mit
je zwei gleichlautenden Optionsgruppen und prüft beide Auswahlrichtungen.

Der tatsächliche App-Prüfer führt zusätzlich sechs native Auswahlen aus:
Oberfläche 22→16 px, Darstellung Hell→Dunkel und Code-Editor 20→16 px. Er prüft
Änderungen am Entwurf und erhält die angewandte Konfiguration. Der SDK-Prüfer
führt denselben Ablauf mit dem verschobenen App-Paket aus. Dropdowns, Listen,
Text-/Editorinterfaces, Fokusführung für weitere Widgettypen und praktische VoiceOver-/
Orca-Abnahme bleiben gesondert offen.


## Fokus und Tastatur für einfache Controls

Sichtbare aktivierte Buttons, Checkboxen und Optionen können jetzt nativen
Tastaturfokus erhalten. macOS bietet `isAccessibilityFocused` und den erlaubten
Setter `setAccessibilityFocused:`; Fokuswechsel werden mit
`AXFocusedUIElementChanged` gemeldet. Linux bietet `Component.GrabFocus`, die
Zustände `focusable`/`focused` und Fokuszustandsereignisse. Die AT-SPI-Anforderung
wartet höchstens eine Sekunde auf den tatsächlich veröffentlichten UI-Fokus und
meldet andernfalls Misserfolg; eine offene Anforderung wird dann verworfen.
Fokusanforderungen an Texte, Gruppen, deaktivierte, verborgene, verdeckte und
veraltete Ziele werden abgewiesen. Die gemeinsame UI-Verarbeitung validiert das
Ziel beim Zeichnen und den fertigen Fensterstapel vor Veröffentlichung erneut.

Der Fokusdienst aktiviert das besitzende Nuklear-Fenster und hebt das SDL-Fenster
an. Die Fokuszustände gelten nur, wenn dieses Fenster tatsächlich Tastatureingaben
erhält. Ein sichtbarer Rahmen markiert das einfache Control. Inaktive Dock-
Bereiche veröffentlichen ihre sonst bedienbaren Controls; Maus-Routing und echte
Sperren bleiben getrennt. Ein Popup oder ein höheres überlappendes UI-Fenster
verhindert eine native Aktivierung. Ein neues, erst später im Frame gezeichnetes
Fenster wird bei der abschließenden Prüfung ebenfalls berücksichtigt.

Enter oder Leertaste aktiviert das fokussierte Control einmal; Key-Repeats
lösen dabei keine wiederholte Aktivierung aus. Tab/Shift+Tab besucht die aktuell
sichtbaren fokussierbaren Controls. Eine Optionsgruppe besitzt einen Tab-Stopp
beim ausgewählten sichtbaren Wert, andernfalls bei ihrer ersten sichtbaren
Option. Pfeiltasten wechseln innerhalb der fokussierten Optionsgruppe Auswahl
und Fokus. Escape gibt den nativen Fokus frei und läuft durch die vorhandene
App-Behandlung weiter. Eine Pointerbetätigung, Fokusverlust des SDL-Fensters,
Entfernen oder Sperren des Controls beendet diesen Fokus. Modifizierte globale
App-Tastenkürzel bleiben verfügbar; einfache Texteingabe landet nicht im
Code-Editor unter einem fokussierten Button.

Die portable Prüfung deckt aufgeschobene, eindeutige und verworfene Anfragen,
Tab-/Optionsnavigation und tatsächliche Tastatureigentümerschaft ab. Die
AppKit-/UI-Prüfung aktiviert ein gezeichnetes Control per Fokus-Setter und
Leertaste, prüft Key-Repeat und ein später gezeichnetes verdeckendes Fenster.
Ein unabhängiger AT-SPI-Client prüft GrabFocus, veröffentlichte Fokuszustände und
Tastaturaktivierung an tatsächlich gezeichneten Fenstern. Der zusätzliche
App-/SDK-Prüfer fokussiert `Standardwerte` im zuvor inaktiven Einstellungsbereich,
aktiviert es mit Leertaste und wechselt mit Tab/Pfeiltaste zur Darstellung.
Entwürfe ändern sich, die angewandte Konfiguration bleibt erhalten. Ein
Fokus-Screenshot gehört zu den lokalen Testausgaben.

Dieser Dienst deckt einfache Controls ab. Text-/Editorfokus, Menüs, Dropdowns,
virtuelle Listen, Scroll-to-Reveal, Windows/UIA, Wayland und praktische
VoiceOver-/Orca-Abnahme bleiben offen. Eine vollständige Barrierefreiheitsabnahme
wird dadurch nicht behauptet.
