# Barrierefreiheit

Die vollständige Tastatur- und Screenreader-Bedienung ist noch in Arbeit.
Die vorhandene Tastaturführung für Hauptmenüs, Einstellungen, Projekterstellung
und beide Dokumentationswege bleibt verfügbar. F10 öffnet die Hauptmenüs;
Pfeiltasten, Tab/Shift+Tab und Enter bedienen die jeweils dokumentierten Bereiche.
UI-Schriftgrößen von 16, 18, 20 und 22 Pixeln sind unabhängig von der Codegröße.

## Benannte Regler

Die sechs Regler der [räumlichen Pendelerklärung](spatial-explanations.md)
und die Kamera- und Laufpositionsregler veröffentlichen einen stabilen Namen, aktuellen Wert, Minimum, Maximum und
Schrittweite. Namen bleiben beim Ändern der Werte gleich. Die Grenzen und Werte
sind endlich; unzulässige Werte werden abgewiesen. Eine native Änderung wird erst
beim nächsten Besuch desselben sichtbaren, aktiven Controls übernommen. Entfernte,
veraltete, verdeckte und deaktivierte Ziele können keine Werte verändern.

macOS verwendet die Rolle `AXSlider`, numerische Werte und Grenzen sowie
Erhöhen, Verringern und Wertsetzen. Die tatsächliche AppKit-Gegenprobe prüft
diese Aktionen und zurückbehaltene Referenzen. Nativ fokussierte Regler lassen
sich außerdem mit Pfeiltasten verändern. Grundlage ist Apples
[Slider-Protokoll](https://developer.apple.com/documentation/appkit/nsaccessibilityslider?language=objc).

Linux ergänzt die Rolle Slider und das standardisierte
[AT-SPI Value-Interface](https://docs.gtk.org/atspi2/iface.Value.html) mit
`MinimumValue`, `MaximumValue`, `MinimumIncrement`, `CurrentValue` und einer
Wertänderungsmitteilung. Der unabhängige PyAT-SPI-Prüfer kontrolliert echte
Wertlieferung, zwei Fenster sowie falsche und deaktivierte Anfragen. Die
Ausführung dieser Erweiterung besteht unter Linux mit Clang und GCC jeweils
10/10 native und 19/19 GUI-Fälle einschließlich des unabhängigen AT-SPI-Prüfers
([ausgeführter Lauf](https://github.com/PhysicSimulator/physim/actions/runs/38239448663)).
Der separate gezeichnete UI-Prüfer läuft derzeit nur unter macOS.

Das portable Modell besteht die neuen Range-Grenzfälle unter ASan/UBSan.
Die native macOS-/UI-Gegenprobe und die beiden Dokumentationsgrößen bestehen
vier tatsächlich ausgeführte Fälle. Andere numerische Eingabefelder, virtuelle
Listen und praktische VoiceOver-/Orca-Abnahme bleiben eigene Aufgaben.

## Nativer macOS-Einstieg

Die macOS-App veröffentlicht sichtbare einfache Schaltflächen, Checkboxen und Beschriftungen
über AppKit als native Accessibility-Elemente. Beschriftungen besitzen
UTF-8-Namen und die Rollen Schaltfläche, Checkbox oder statischer Text. Die sichtbaren
Rahmen folgen dem Fenster und der aktuellen Darstellung. Aktivierte Buttons
können eine native Press-Aktion entgegennehmen; deaktivierte, entfernte oder
veraltete Elemente lösen keine Aktion aus. Eine angenommene Aktion wird genau
einmal im nächsten UI-Frame an den passenden Control übergeben.

Dieser Einstieg deckt noch keine vollständige VoiceOver-Bedienung ab.
F1 ergänzt jetzt ein eigenes navigierbares statisches Textelement mit seinem
vollständigen Lesetext. AppKit liefert Wert, Zeichenanzahl, UTF-16-Textbereiche,
logische Zeilenbereiche und sichtbare Glyphenrahmen. Eine native Anforderung
für den sichtbaren Bereich wird im normalen UI-Frame zum passenden Dokument
übernommen. Verborgene Dokumente bleiben lesbar, liefern aber keine sichtbare
Geometrie und nehmen keine Scrollanforderungen an. Entfernen und Wiedererscheinen
erzeugen eine neue Dokumentidentität; alte Referenzen bleiben ungültig. Das
Dokument besitzt keine editierbare Auswahl und kann keine Control-Aktion
auslösen, auch wenn seine lokale Nummer einem Control entspricht.
Die ausgeführten ARM64-/Intel-Prüfungen und tatsächlichen F1-Aufnahmen stehen
in der [F1-Abnahmeentscheidung](decisions/004-f1-handbook.md).

Die beiden F1-Suchfelder veröffentlichen jetzt die Rolle `AXTextField`, ihren
vollständigen Wert, UTF-16-Auswahlbereiche und die ausgewählten Zeichen. Ein
zusammengefallener Bereich beschreibt den Caret. AppKit liefert außerdem
Einzelzeilenbereiche, sichtbare Zeichenbereiche und tatsächliche Zeichenrahmen.
Die einfachen und die als `NSValue` gelieferten Auswahlbereiche beschreiben
denselben Zustand. Wertsetzen, Auswahl und Bereichsanforderungen werden durch
den normalen UI-Frame verarbeitet. Ersetzen einer Auswahl liefert Wert und
Caret gemeinsam; eine inzwischen geänderte Textfassung wird nicht überschrieben.
Ungültige Surrogatgrenzen, eingebettete Nullzeichen, mehrzeilige Eingaben und
Kapazitätsüberschreitungen werden verworfen. Verborgene und deaktivierte Felder
nehmen keine Schreibaufträge an; entfernte oder zerstörte Referenzen bleiben
ungültig. Die jeweils ausgeführten Prüfungen stehen in der F1-Abnahmeentscheidung.

Interne F1-Links sind außerdem mit ihren UTF-16-Bereichen im nativen Lesetext
verbunden. AppKit liefert die zugehörigen Linkobjekte, ihren vollständigen
Beschriftungstext, das relative Dokumentziel und sichtbare Glyphenrahmen. Die
Linkliste folgt der Textreihenfolge und enthält auch Ziele außerhalb des
aktuellen Ausschnitts. Native Fokussierung macht den Textbereich sichtbar;
Aktivierung läuft durch die normale F1-Navigation. Ein ungültiges Ziel zeigt
die Fehlermeldung in der Hilfe. Verborgene und deaktivierte Links nehmen keine
Aktionen an; nach einem Seitenwechsel bleiben gehaltene alte Linkreferenzen
ungültig. Das Kopieren externer Quellenadressen bleibt der dokumentierte Ablauf.

Weitere Textfelder, Editoren, Menüs, Dropdowns, weitere Regler, Szenen, Diagramme und
Fokusführung für weitere Widgettypen benötigen zusätzliche semantische Anbindungen.
Die bestehende Sichtbarkeits-/Deaktivierungslogik wird berücksichtigt, sodass
dieser Einstieg keine Aktivierung gesperrter oder momentan schreibgeschützter
UI-Bereiche ermöglicht. Eine vollständige praktische VoiceOver-Abnahme steht
noch aus. Windows besitzt die im folgenden Abschnitt beschriebene
UI-Automation-Anbindung. Linux besitzt den unten beschriebenen AT-SPI-Einstieg.
Die portable Modellprüfung allein beweist keinen praktischen Screenreader-Zugang.

## Windows UI Automation und F1

Windows veröffentlicht benannte einfache Controls, Auswahlgruppen und Regler
über UI Automation. F1 stellt zusätzlich seinen vollständigen Lesetext über
TextPattern bereit. Native Textbereiche unterstützen Unicode-Suche, sichtbare
Zeichenrahmen und ScrollIntoView. Die beiden Suchfelder veröffentlichen Wert,
Textauswahl und Caret; native Eingaben werden im tatsächlichen Feld verarbeitet.
Interne Links besitzen Textbereiche, Fokus und Aktivierung. Externe Quellenadressen
werden weiterhin kopiert, wie in der Hilfe beschrieben.

Unabhängige Clientprozesse prüfen Lesen, Feldbedienung und Linkaktivierung an der
laufenden App. Beim Schließen oder Minimieren bleiben gehaltene Inhalte lesbar;
verborgene Elemente veröffentlichen keine sichtbaren Zeichenrahmen, keinen
aktiven Feldfokus und nehmen keine Bedienaufträge an. Wiederöffnen und
Wiederherstellen erhalten Inhalt und Elementidentitäten. Die jeweiligen
Revisions-, Lauf- und Bildbelege stehen in der F1-Abnahmeentscheidung im Repository.

Dies bestätigt noch keine vollständige Narrator-/NVDA-Bedienung. Native
Tabellensemantik, weitere Editoren und Widgettypen sowie die vollständige
DPI-/Monitorwechselmatrix bleiben offen. Ein Suchfeld kann Unicode unverändert
speichern und nativ liefern, obwohl die Schrift ein Ersatzzeichen zeichnet;
Die offline gebündelten Schriftquellen decken inzwischen alle druckbaren Zeichen
der registrierten Hilfeseiten ab, einschließlich des Globus. Die reale App-Probe
prüft 16/32 Pixel auch mit umgangenen Systemschriften. Weitere Schriftsysteme,
komplexe Textformung und der vollständige optische Plattformabgleich bleiben offen.

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

F1 veröffentlicht zusätzlich ein eigenes Textdokument mit vollständigem
Lesetext über das AT-SPI-Text-Interface. Zeichenpositionen zählen Unicode-Zeichen
und werden von UTF-8-Bytepositionen getrennt. Der Client kann den gesamten Text,
Teilbereiche, Zeichen und logische Wort-/Zeilenbereiche lesen, sichtbare
Glyphenrahmen abfragen und eine Textstelle zum Lesen sichtbar machen.
Das Dokument besitzt keinen editierbaren Caret und keine Textauswahl.
Verborgene Fenster behalten ihren Lesetext, liefern aber keine sichtbare
Textgeometrie und nehmen keine Scrollanforderungen an. Ein entferntes Dokument
erhält bei erneutem Erscheinen eine neue Objektkennung; alte Referenzen bleiben
ungültig. Inhalts- und Namensänderungen erzeugen native Mitteilungen.

Der unabhängige Client liest in der tatsächlichen F1-App die vollständige
Quellseite mit 498.605 Bytes und 493.143 Unicode-Zeichen und erreicht das sichtbare
Dokumentende über eine native Scrollanforderung. Die erweiterte Fensterprobe
prüft Unicode-Bereiche, Zeichenrahmen, Änderungsmitteilungen, Verbergen,
Entfernen und neue Dokumentidentitäten. Die aktuellen Ausführungsbelege stehen
in der [F1-Abnahmeentscheidung](decisions/004-f1-handbook.md).

Die beiden benannten F1-Suchfelder veröffentlichen die Rolle `entry` sowie
`Text` und `EditableText`. Native Clients können ihren Wert lesen und setzen,
Unicode-Auswahl und Caret verändern sowie Text einfügen, löschen, kopieren,
ausschneiden und einfügen. Zeichenpositionen zählen Unicode-Skalarwerte;
die Längenangabe von `InsertText` zählt dagegen UTF-8-Bytes. Ein abgeschnittener
Mehrbyte-Codepunkt wird abgewiesen. Die Zwischenablage wird im UI-Thread
verarbeitet; danach werden Besitzer, Inhaltsversion und Bedienbarkeit erneut
geprüft. Veraltete, deaktivierte und verborgene Ziele übernehmen keine Änderung.
Der unabhängige F1-Client bestätigt die tatsächlichen Suchwerte, eine markierte
Unicode-Auswahl, deren Ersetzung durch Tastatureingabe und den Tab-Ausstieg.
Eine zusätzliche Zwei-Fenster-Probe prüft Zwischenablage und ungültige Anfragen.

Interne F1-Links sind dem Lesetext über `Hypertext` zugeordnet. Der Client
kann die Links in Lesereihenfolge, ihre Unicode-Textbereiche, Zieladressen und
zugehörigen Link-Controls abfragen. Ein nativ fokussierter Link außerhalb des
sichtbaren Ausschnitts wird zum Lesen sichtbar gemacht. Seine Aktion folgt
demselben Navigationsweg wie Maus und Tastatur. Alte Linkobjekte werden nach
einem Inhaltswechsel ungültig. Quellenadressen bleiben die dokumentierte
Kopieraktion und gehören nicht zu dieser internen Linknavigation.

Es gibt höchstens acht native Fenster pro Prozess und die bestehenden 256
Controls/1023 Label-Byte je Modell. Der zusätzliche Dokumentknoten verwendet
den separaten vollständigen Lesetext mit dessen dokumentierten Größenlimits.
Weitere assistive Eingabefelder/Editoren, Tabellensemantik,
visuelle statt logischer Zeilengrenzen, sprachabhängige Textsegmentierung,
begrenzte Bereichsabfragen und Scrollen zu einer bestimmten Bildschirmposition
bleiben offen. Bildschirmkoordinaten werden nur
geliefert, wenn SDL die Fensterposition kennt; anderenfalls folgt ein
`NotSupported`-Fehler statt einer erfundenen Position. Der bisher ausgeführte
Clientnachweis verwendet X11/Xvfb unter Debian; die neue Textprüfung wurde
zusätzlich unter Alpine 3.24.2/WSL2 mit SDL 3.2.30 tatsächlich ausgeführt.
Wayland und praktische Orca-
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

Dieser Dienst deckt einfache Controls ab. Vollständiger Text-/Editorfokus,
Menüs, Dropdowns, virtuelle Listen, allgemeines Scroll-to-Reveal, Wayland und
praktische Screenreader-Abnahme bleiben offen. Die oben beschriebene
Windows-F1-Anbindung ergänzt diesen Umfang. Eine vollständige
Barrierefreiheitsabnahme wird dadurch nicht behauptet.
