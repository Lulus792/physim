# Kopierfertiger Auftrag: die gemeinsame Physim-Oberfläche nativ umsetzen

**Wiedereinstieg:** Zuerst [den kurzen Arbeitsstand](../../weiterarbeit.md) lesen.
Er nennt abgeschlossene Prüfungen und den nächsten konkreten Umsetzungsschritt.
Eine Kontextkompaktierung ist kein Anlass, erledigte Prüfungen neu zu starten.

**Neuer Plattformauftrag vom 30.10.:** [ADR 024](../../decisions/024-native-macos-menubar.md):
die vorhandenen Menüs unter macOS in die echte Systemmenüleiste verlegen.
Die übrige App-Gestaltung erhalten; gemeinsame Aktionen und Verfügbarkeit
weiterverwenden. Adapter und gezielte Intel-/Apple-Silicon-Prüfungen sind
vorhanden; die gesamte GUI-, manuelle Vollbild- und VoiceOver-Abnahme bleibt
offen. Aktuelle abgeschlossene und laufende Prüfumfänge stehen im Arbeitsstand.

**Nutzerwahl vom 30. Oktober 2026 – Vervollständigung D mit Ergänzungen:**
[ADR 023](../../decisions/023-selected-completion-d.md),
[verbindliche Spezifikation](../completion-typing-options/APPROVED.md) und
[festgelegte Referenz](../completion-typing-options/index.html?approved=1&scenario=arguments)
lesen. **D ist gewählt**, keine neue D/E/F-Wahl. Standardmäßig fünf sichtbare
Zeilen, nicht nur fünf Ergebnisse; restlicher Bestand scrollbar und per Pfeil
erreichbar. Horizontale Dicke der Scrollleiste klein/ruhig. Anzahl später in
Einstellungen A / Editor einstellbar, Standard 5, bestehende Preferences-
Transaktion verwenden. In Funktionsargumenten nach `(`/`,` sowie Präfixen
zugängliche Variablen und Funktionsparameter anbieten, lokale Namen priorisieren,
Scope/Revision/Rechte berücksichtigen. Kein fremder/geschlossener/nachträglicher
Name; keine erfundenen Laufzeitwerte. Browser-C-Lexer ist nur begrenzte Studie,
nativ tatsächliche sprachabhängige Symbol-/Scope-Daten verwenden. Code in
Kurzinfo und Editor aus demselben aktiven Syntaxschema zeichnen. **Abschließende
Freigabe samt neutralem Kurzinfo-Hintergrund; keine offene Farbwahl mehr.**
Die letzte Nutzerpräzisierung ersetzt die alte Palette: Codefarben an VS Code/
Visual Studio 2022 orientieren, **Funktionen gelb** (dunkel #dcdcaa, hell #795e26).
Gemeinsame Rollen code_function, code_variable und code_control im nativen
Token-/Syntaxsystem ergänzen; Funktionen, Typen und Variablen tatsächlich
unterscheiden, nicht alle ps_-Namen als Typ färben. Alle Codekontexte inklusive
Completion-Namen/-Icons, Kurzinfo und Parameterhilfe verwenden dieselben Rollen.
[Genaue Palette und Kontrastvertrag](../completion-typing-options/APPROVED.md#ein-syntaxschema-für-editor-und-kurzinfo),
[aktuelle Prüfung](../completion-typing-options/review/COLOR-VALIDATION.md).
API-Karte, F1, Suchinsel, A2 und Auto-Anzeige erhalten.

**Präzisierung desselben Auftrags – erwartete Funktionsparameter:** Zusätzlich
zur Variablen-Completion eine separate Aufrufsignatur wie in VS Code und Visual
Studio 2022 anzeigen: Namen, Typen, Anzahl und hervorgehobener aktiver Parameter.
Automatisch beim Aufruf/Argumentwechsel, manuell Strg+Umschalt+Leertaste; nach
Übernahme eines Vorschlags sichtbar lassen. Verschachtelte Aufrufe und Kommata
in Strings/Kommentaren/Indizes korrekt abgrenzen; keine unbekannten Signaturen
oder falsche letzte Parameter erfinden. Tatsächliche sprachabhängige Signaturdaten
verwenden, Browser-Katalogfixture ist kein vollständiger Parser. Die Liste bleibt
normalerweise unten, mit Abstand zur Signatur. **Zusätzliches Oben-Beispiel wurde
vom Nutzer zurückgenommen.** [Vertrag und Referenzen](../completion-typing-options/APPROVED.md#parameterhilfe-erwartete-argumente-sichtbar-machen),
[zweites Argument](../completion-typing-options/index.html?approved=1&scenario=signature-second)
und [Prüfbericht](../completion-typing-options/review/SIGNATURE-VALIDATION.md) lesen.

**Aktualisierter Auftrag vom 30. Oktober 2026 – automatische Vorschläge D09:**
Der Nutzer will **automatisches Anzeigen während des Tippens**. Bisherige Optik
A/B/C wurde ausdrücklich verworfen. [ADR 022](../../decisions/022-automatic-completion.md)
und [neue Entwürfe D/E/F](../completion-typing-options/README.md) beachten:
D Editor, E Neovim, F IDE; flache Zeilen und separate Dokumentation.
**Funktion automatisch anzeigen ist beauftragt; D ist inzwischen mit Ergänzungen gewählt.**
A/B/C nicht erneut vorlegen oder übernehmen. Manuelles Strg+Leertaste erhalten.
Getestete Entwurfswerte: zwei Zeichen, 140 ms Eingabepause, Physim-Memberpunkt
ohne Blattpräfix, 280 ms Zusatzinfo. Automatisch keine Übernahmeauswahl;
Pfeile/Ctrl+N/P wählen, dann Enter/Tab übernehmen, Klick direkt. Sonst Enter
Zeilenumbruch/Tab Einrückung; Umschalt+Tab verlässt das Feld. Escape unterdrückt
für den laufenden Bezeichner; manuelles Öffnen überstimmt. Keine automatische
Einfügung, Vorschau-Textänderung oder Übernahme durch Satzzeichen.
IME, Paste, Undo, Kommentare/Strings, Nur lesen, Cursor-/Versions-/Dokumentwechsel
und ausstehende Timer beachten. Vorhandenen Katalog, Sprach-/Memberregeln,
Undo/Dirty/Build-Regeln und Umschalt+F1 an denselben echten Quellen weiterverwenden.
Auto-Option in vorhandene Einstellungen-A-Editor-Kategorie einpassen, kein
zweites Preferences-Modell. Kein neuer LSP/KI-/Provider-Worker. API-Karte,
F1 H und Suchinsel bleiben unverändert. Referenz-Ladefall nur Zustandsentwurf.
Browsernachweise bestätigen keine native oder echte Screenreader-/IME-Abnahme.
D10 vor neuer Gestaltung am bereits vorhandenen Explorer-/API-Bestand prüfen.

**Bestandskorrektur vom 30. Oktober 2026 – Editor-Suche D08:**
Die vorhandene Suchinsel samt aufklappbarem Ersetzen ist bereits gestaltet und
nativ vorhanden. Der Nutzer hat auf diese Doppelung hingewiesen. **Bestehende
Suchinsel weiterführen, keine erneute Variantenwahl vorschalten.** Die
[ergänzende A/B/C-Studie](../search-options/README.md) ist nicht freigegeben und
ersetzt das bestehende Design nicht. Ihre neuen Anordnungen, Trefferliste und
„Alle ersetzen“-Erweiterung sind kein neuer nativer Implementierungsauftrag.
Vorhandene aktuelle-Datei-Suche, Unicode-/Undo-/Dirty-Regeln, A2 und alle folgenden
Freigaben erhalten. Verbleibende Detailprüfungen sind keine neue Layoutentscheidung;
projektweite Suche bleibt separat. D09 hat inzwischen getrennte Popup-Detailentwürfe.

**Verbindliche Auswahl vom 30. Oktober 2026 – Kontextmenüs A / D07:**
Der Nutzer hat **A · Kompakt** gewählt. Die vorherige Übernahmesperre ist
aufgehoben; im bestehenden nativen Auftrag mit umsetzen, keine neue Variantenwahl.
[ADR 021](../../decisions/021-selected-context-menu-a.md),
[genaue Spezifikation](../context-menu-options/APPROVED.md),
[Befehlsmatrix für neun Ziele](../context-menu-options/README.md#vollständige-befehlsmatrix-dieser-studie)
und [festgelegte Referenz](../context-menu-options/index.html?approved=1&context=text)
lesen. Kompakte Liste, Breite 260 logische Einheiten, Trennlinien, Tastaturhinweise,
Untermenüs und Checkboxzustände; gesperrte Befehle nennen ihren Grund. Keine
B-Zielköpfe/Gruppenbeschriftung oder C-Erklärung je Befehl. B/C bleiben Historie.
Text, Tab, Datei, Ordner, Meldung, Diagramm, Szenenobjekt, Mehrfachauswahl und
freie Fläche haben ihre festgelegten Befehle. A2 bleibt unverändert, Linksauswahl
öffnet keine Aktionsleiste. Öffnen über Rechtsklick/Menütaste/Umschalt+F10;
Rechtsklick innerhalb der Auswahl erhält die Menge, anderes Element wird zum Ziel.
Fokus/Untermenü-Rückkehr, rechte Kamera-Ziehbewegung und stabile Ziel-/Versions-
identitäten erhalten, ungültige Menüs verwerfen und vor Ausführung erneut prüfen.
Echte native Funktionen/Clipboard/Undo verwenden; den angeklickten Tab statt
einer inzwischen anderen aktiven Datei speichern/schließen. Ungespeicherte
Inhalte gehen durch B2; Export erhält denselben Diagrammstand, Projekt B ruft
Explorer A auf. Fehler aus realem Ergebnis über Rückmeldungen A/Protokoll C.
Keine erfundenen Datei-/Objektfunktionen oder Fixture-Erfolgstexte übernehmen.
Alle bisherigen Freigaben und die laufende native Arbeit bleiben erhalten.
Browserreferenz ist keine native oder Screenreader-/Plattformabnahme.
D08 führt die vorhandene Suchinsel fort; D09 hat getrennte Popup-Detailentwürfe.

**Verbindliche Auswahl vom 30. Oktober 2026 – Fortschritt B mit Pop-out / D06:**
Der Nutzer hat **B · Aufgabenleiste mit Herauslösen als eigenes Fenster** gewählt.
Die vorherige Übernahmesperre ist aufgehoben; nativ mit umsetzen, keine neue
Variantenentscheidung. [ADR 020](../../decisions/020-selected-progress-b-popout.md),
[genaue Spezifikation](../progress-options/APPROVED.md) und
[festgelegte Referenz](../progress-options/index.html?approved=1&task=series)
lesen. Kompakte B-Leiste bleibt Standard, Details aufklappbar; der ausdrücklich
gewünschte Icon-only-Pop-out-Button öffnet ein echtes eigenes OS-Fenster nach ADR 015.
Tooltip/zugänglicher Name statt Platz verbrauchendem Text; auch „Fenster anzeigen“
als Icon, „Zurückholen“ bleibt beschriftet. Dieselbe Aufgabe/Versuchsnummer,
Zähler, Details, Fehler und Abbruchanforderung; keine zweite Worker-Kopie.
Erneuter Aufruf aktiviert das vorhandene Fenster. Zurückholen, Fenster-Schließen
und Escape im Pop-out bringen die Anzeige in B zurück, ohne Abbruch/Neustart.
Hauptfenster bleibt bedienbar. Bekannte/unbekannte Mengen, bestätigter Abbruch,
Teilresultate, Erfolg-Abbruch-Rennen und ehrliche Adapterfähigkeiten erhalten.
Browser-Pop-up ist nur Referenz, keine native Physim- oder Plattformabnahme.
A/C bleiben Historie. Rückmeldungen A, Läufe A und alle folgenden Freigaben
sowie laufende native Arbeit erhalten; D07 ist inzwischen A, vorhandene Suche bleibt.

**Verbindliche Freigabe vom 30. Oktober 2026 – Rückmeldungen A / D05:**
Der Nutzer hat **A · Leiser Hinweis ins fertige Design übernommen**. Die
vorherige Übernahmesperre ist aufgehoben; nativ mit umsetzen, keine neue
Variantenwahl. [ADR 019](../../decisions/019-selected-feedback-a.md),
[genaue Spezifikation](../feedback-options/APPROVED.md) und
[festgelegte Referenz](../feedback-options/index.html?approved=1&scenario=burst)
lesen. Höchstens zwei kurze Erfolgskarten rechts unten, kleiner erreichbarer
Verlauf; lokale Pfad-/Kopierfehler und dauerhafte Veraltet-Kennzeichnung gehören
dazu. Wiederholungen sinnvoll gruppieren, keine doppelten vollständigen Fehler-
meldungen; echte Datei-/Lauf-/Fenster-/Versionsidentitäten und Protokoll C als
Ereignisquelle verwenden. Schließen behebt nichts, Erfolg erst nach echter
Rückgabe; Daten und Eingaben erhalten. Optionales Ausblenden mit Fokus-/Hover-
Pause wie spezifiziert, keine zusätzliche Einstellung aus dem Prüfschalter
ableiten. Lokale Meldungen in vorhandenen ADR-015-Aufgabenfenstern anzeigen.
B/C bleiben Vergleichshistorie; Browserbelege bestätigen keine fertige native
Umsetzung. D06/D07 sind inzwischen ebenfalls gewählt. Alle folgenden
Freigaben und die laufende native Arbeit bleiben erhalten.

**Verbindliche Freigabe vom 29. Oktober 2026 – Einstellungen A:** Der Nutzer
hat **A · Kategorien ins fertige Design übernommen**. Die vorherige
Übernahmesperre ist aufgehoben; im bestehenden nativen Auftrag mit umsetzen,
keine erneute Variantenwahl. [ADR 018](../../decisions/018-selected-settings-a.md),
[genaue Spezifikation](../settings-options/APPROVED.md) und
[festgelegte Referenz](../settings-options/index.html?approved=1&group=appearance)
lesen. Links fünf Kategorien, mittig Optionen, rechts Entwurfsvorschau; Suche,
Änderungsprüfung, Übernehmen/Abbrechen/Standardwerte, Schreib-/Lade-/Schriftfehler
und eigene Anzeigeeinheitenaufgabe gehören dazu. Nativ tatsächliche OS-Fenster
nach ADR 015 bauen, Hauptfenster bedienbar lassen; Preferences, unabhängige
Workspace-Geometrie und Einheiten getrennt halten. Kein Zurücksetzen paralleler
Änderungen durch einen alten Entwurf. Demo-Ausgangswerte sind keine neuen Defaults,
Galerie-/Prüfknöpfe und Erfolgsseite keine Produktseiten. B/C sind Vergleichshistorie.
Browserbelege bestätigen keine fertige native Umsetzung. Alle folgenden Freigaben
bleiben erhalten; D04 ist gestalterisch entschieden.

**Verbindliche Freigabe vom 29. Oktober 2026 – Dateisicherheit B2:** Der Nutzer
hat **B2 für Speichern, Dateikonflikte und Wiederherstellung ins fertige Design
übernommen**. Die vorherige Übernahmesperre ist aufgehoben. Im bestehenden
nativen Auftrag mit umsetzen, keine erneute Auswahl vorschalten.
[ADR 017](../../decisions/017-selected-file-safety-b2.md),
[genaue B2-Spezifikation](../file-safety-options/APPROVED.md) und
[festgelegte Referenz](../file-safety-options/index.html?approved=1&topic=save)
lesen: links Dateinavigation, rechts **eine gemeinsame Git-Diff-Ansicht** mit
Minus-/Pluszeilen, Kontext, Alt-/Neu-Zeilennummern und echten Änderungszählern.
Speichern/Teilfehler/Abbruch, Konfliktentscheidungen, echte Kopie über Explorer A
und Wiederherstellung mit späterem bewusstem Speichern gehören zum Auftrag.
Ein erneuter externer Stand braucht erneute Prüfung; keine automatische Zusammenführung.
Die [UI-Designliste](UI-DESIGN-BACKLOG.md) führt D01–D03 jetzt als gestalterisch
entschieden; native Umsetzung und Abnahme bleiben offen. Die übrigen Freigaben
und der folgende Vertrag eigenständiger Aufgabenfenster bleiben erhalten.

**Ergänzung vom 29. Oktober 2026 – eigenständige Aufgabenfenster:** Der Nutzer
verlangt tatsächliche Fenster für Dialoge wie Projektanlage und Grafikexport,
während Hauptfenster und herausgelöste Bereiche bedienbar bleiben. [ADR 015](../../decisions/015-independent-task-windows.md)
und der [native Teilnachweis](TASK-WINDOWS-NATIVE.md) gelten zusätzlich. Grafikexport
und Projektanlage B sowie Picker A sind umgestellt und auf Windows/Linux in den
genannten Teilumfängen geprüft. A besitzt getrennte Fenster pro Auftrag; Hauptfenster
und herausgelöste Bereiche bleiben bedienbar. Weitere Aufgabenfenster und der
vollständige Vorlagenabgleich bleiben offen. Die bestehenden Gestaltungen
und die Freigaben unten bleiben erhalten.

**Ergänzung vom 29. Oktober 2026 – Auswahl/Markierung:** Der Nutzer wünscht
lesbare Markierungen und legt fest: **Linksauswahl öffnet keine automatischen
Aktionsleisten; Auswahlaktionen erscheinen erst per Rechtsklick**, zusätzlich
über Menütaste/Umschalt+F10. **A2 · Dezenter Schleier ist jetzt ausdrücklich als
Teil des fertigen Designs freigegeben und ersetzt die frühere Grundlage A.**
[ADR 016](../../decisions/016-selected-marking-a2.md),
[verbindliche A2-Spezifikation](../selection-options/APPROVED.md) und
[festgelegte Referenz](../selection-options/index.html?approved=1&context=code)
lesen und im bestehenden nativen Auftrag mit umsetzen. Farb-/Deckkraftwerte,
Text hinterlegende Zeichenreihenfolge, Zeilen-/Objektmarkierungen, Rechtsklick,
Mehrfachauswahl und Tastaturmenü daraus übernehmen. Keine erneute Variantenwahl.
Die [UI-Designliste](UI-DESIGN-BACKLOG.md) trennt offene Detailentwürfe,
Abgleich bereits freigegebener Teile und spätere Optionen; sie ist keine
zusätzliche Funktionsfreigabe. Die folgende Bereichsauswahl bleibt verbindlich.

**Verbindliche Auswahl vom 25. Oktober 2026:** Der Nutzer hat entschieden:
**Leeransicht bestätigt; Protokoll C; Szene & Werte A; Auswertung A;
Läufe/Serien A; Neues Projekt B; Datei-Explorer A.** Die Auswahlphase ist beendet;
die vorherige Übernahmesperre gilt für diese Teile nicht mehr. Mit der bestehenden
nativen Arbeit fortfahren und diese konkrete Kombination umsetzen.
[ADR 011](../../decisions/011-selected-workspace-additions.md),
[APPROVED.md](../design-options/APPROVED.md) und die
[gemeinsame ausgewählte Referenz](../design-options/index.html?approved=1&topic=empty)
sind maßgeblich. Keine neue Variantenentscheidung vorschalten.

Unteraufrufe behalten ihre eigene Auswahl: **Protokoll C → Projektdialog B →
Explorer A**. Die [vollständige Leeransicht](../design-options/index.html?approved=1&topic=empty&standalone=1)
zeigt alle drei Bereiche ohne Projekt mit passenden deaktivierten Aktionen;
kein Demoexperiment automatisch laden. Designfreigabe und native Abnahme trennen.

Der [native Anschluss der gemeinsamen Protokoll-Datenquellen](PROTOCOL-SOURCES-NATIVE.md)
ist inzwischen geprüft. Ein nativer Teilstand von Navigator C ist jetzt mit
Quellen und Tests versioniert. Der aktuelle
[Diagnose- und Integrationsnachweis](PROTOCOL-DIAGNOSTICS-NATIVE.md) prüft echte
Compiler-/Laufmeldungen, explizite Quellsprünge und herausgelöste Bereiche auch
ohne fremde lokale Änderungen. Die früheren
[Teilprüfungen](PROTOCOL-C-NATIVE.md) und der
[Fokusnachlauf](PROTOCOL-FOCUS-NATIVE.md) behalten ihren datierten Geltungsbereich.
Der folgende [Originaltext-Nachweis](PROTOCOL-ORIGINAL-NATIVE.md) ergänzt den
nativen weichen Umbruch langer Zeilen, unveränderte Auswahl/Kopie und den
Layoutwechsel ohne verlorenen äußeren Scrolloffset. Der folgende
[Nachlauf zur natürlichen Originaltexthöhe](PROTOCOL-ORIGINAL-FLOW-NATIVE.md)
verlagert das Scrollen in den Detailbereich und schließt die Klickübernahme
durch ausgeclippte Feldteile. C ist weiterhin nicht vollständig abgenommen.
Der [Detail- und Aktionsabgleich](PROTOCOL-ACTIONS-NATIVE.md) übernimmt 20 px
Innenabstand und inhaltsbreite Aktionen mit automatischem Zeilenwechsel.
Der [Nachlauf zum durchgehenden Originaltext](PROTOCOL-CONTINUOUS-ORIGINAL-NATIVE.md)
entfernt anschließend die 12-KiB-Abschnitte und prüft vollständige native
64-KiB-Werte, Auswahl und Kopie mit unabhängigen UIA-/AT-SPI-Clients.
Der [Detailtypografie-Abgleich](PROTOCOL-DETAIL-TYPE-NATIVE.md) übernimmt danach
Schriftrollen, Zeilenhöhen, Badge-, Eigenschafts- und Blockabstände mit je sechs
nativen Windows-/Linux-Zuständen. Zusätzliche Dateninhalte der Browserdemo,
weitere C-Zustände und die vollständige Gestaltung bleiben offen.
Der [native Projektdialog B](PROJECT-WIZARD-B-NATIVE.md) ergänzt den echten
Dreischritt, acht zugängliche Vorlagenkarten und überprüfbare Projektanlage.
Windows und Linux bestehen je 68 vollständige Projektabläufe sowie die
Dialog-, Fehler- und nativen Accessibility-Prüfungen. Der eingebettete
Explorer A ist inzwischen als nativer Teilstand eingebunden; die vollständige
Oberflächenabnahme bleibt offen.
Die [geprüfte Dateisystemgrundlage für Explorer A](EXPLORER-A-NATIVE.md)
ergänzt danach echte Ordnerlisten, Metadaten, Filter, Abbruch und exklusive
Ordneranlage. Windows und Linux bestehen die Modellprüfungen samt langen Pfaden;
die native Junction-Gegenprobe bestätigt die korrigierte Zielprüfung. Der folgende
IO-Arbeiter prüft asynchrone Aufträge, Textvorschau und veraltete Ergebnisse mit
einer negativen Versionsgegenprobe. Controller und A-Oberfläche übernehmen jetzt
Orte/Liste/Vorschau, echte Textvorschau und den Unteraufruf aus B; Windows/Linux
prüfen Widgets und unabhängige lange Pfadwerte. Offen bleiben insbesondere
Messlauf-/Bildvorschau, C-API-Kontext, große native Listen, vollständige
bereichsübergreifende Aufgaben und die im Einbindungsnachweis genannte
anfängliche UIA-Zeitüberschreitung. Die folgende
[freie Exploreransicht](EXPLORER-INLINE-NATIVE.md) ist nun in den Arbeitsbereich
eingebettet. Sie erhält ihren Zustand über Bereichswechsel, Projekt B und dessen
unabhängigen Picker. Der folgende [Vorschau-/Fokusabgleich](EXPLORER-PREVIEW-NATIVE.md)
übernimmt natürliche Höhe, Umbruch und vollständig lesbare Orte-Buttons. Er
schließt die pendelnde Fokusnachführung einer übergroßen Dateiliste und veraltete
Vorschauziele außerhalb des sichtbaren Ausschnitts. Der Nutzerauftrag für
[schmale Einfügemarken](TEXT-CARET-NATIVE.md) ist im gemeinsamen Textfeldrenderer
umgesetzt; zehn Cursor-/Editorfälle je Plattform und die gemeinsamen sieben
App-Gruppen je Plattform bestehen. Eine vollständige Designabnahme folgt daraus nicht.
Der folgende
[Nachlauf für schnelle Zeigerereignisse](PROTOCOL-POINTER-NATIVE.md) erhält
vollständige Klicks innerhalb eines Frames und den ursprünglichen Druckpunkt;
Linux prüft den schnellen Suche-/Trenner-Fokuswechsel mit anschließendem
40-Pixel-Schritt. Der spätere Originaltext-Nachlauf bestätigt den sichtbaren
Windows-Mausfokus und die 40-Pixel-Ziehung. Am 29. Oktober hat der Nutzer die
manuelle Prüfung wieder aufgenommen: Der [physische Tastaturnachweis](PROTOCOL-PHYSICAL-KEYS-NATIVE.md)
belegt sechs echte Umschalt-Pfeil-Schritte von jeweils 40 Pixeln an der fokussierten
Protokollhöhe. Andere Trenner und praktische Plattformprüfungen bleiben offen.
Ebenfalls am 29. Oktober hat der Nutzer für den Protokollbereich ausdrücklich
dieselben Farben wie für die übrige Physim-Oberfläche gewünscht. Diese Anpassung
der gemeinsamen Werkbank-Farbrollen hat Vorrang vor der eigenen Farbpalette
der älteren Navigator-C-Referenz; die Anordnung C bleibt ausgewählt. Der
[native Farbabgleich](PROTOCOL-COLORS-NATIVE.md) belegt die gemeinsamen Flächen
in beiden Themes und hohem Kontrast auf Windows und Linux. Die zusätzliche
F1-UIA-Scrollprüfung ist im folgenden
[Reihenfolgenachlauf](../f1-layouts/UIA-SCROLL-ORDER.md) geschlossen: Der Test
wartet nun auf den vollständigen externen Edit-/Fokusabschluss, bevor er Tab
erzeugt. Die Gegenprobe mit altem Programm bleibt gezielt negativ.
macOS, Screenreader und vollständige
Designgleichheit bleiben eigene offene Nachweise.
Der [Gliederungs-Hovernachlauf](OUTLINE-HOVER-NATIVE.md) schließt die
Tooltip-Überlagerung aus unsichtbaren Deklarationszeilen über dem Protokoll.
Native Gegenprobe, drei Hoverzustände und die bestehenden C-/Physim-
Gliederungsabläufe bestehen auf Windows und Linux.

Das [Inhaltsinventar](CONTENT-INVENTORY.md) bleibt mitzuprüfen: Kräfte, Texte,
Objekttypen/Hierarchie, Parameter, Messkanäle, Modellmeldungen und variable
Analyseausgaben. Bestehende native Funktionen übernehmen. Keine Newtonwerte aus
beliebigen Pfeilgeometrien erfinden. Für das Protokoll ersetzt C die Anordnung
des Entwurfs vom 24. Oktober; dessen gemeinsamer Verhaltensvertrag bleibt erhalten.

**Verbindliche Nutzerfreigabe vom 20. Oktober 2026:** Der Grafikexport aus
[EXPORT-EXAMPLE.md](EXPORT-EXAMPLE.md) gehört jetzt zum fertigen Design und ist
nativ mit umzusetzen. Er ist kein optionaler zusätzlicher Entwurf mehr.
[ADR 010](../../decisions/010-analysis-graphic-export.md) hält die Entscheidung
und die native Abnahme fest. Diese Ergänzung gilt auch beim Fortsetzen des
bereits laufenden nativen Referenzabgleichs.

**Jüngste Nutzerbefunde vom 21. Oktober:** Die
[Originalaufnahmen und ihre offenen Korrekturen](USER-REVIEW-2026-10-21.md)
zeigen weiterhin sichtbare Standard-/Altbedienelemente. Zuerst diese gemeinsamen
Komponenten und die übrigen sichtbaren Abweichungen angleichen. Ein fertiger
Export-Rasterbaustein ist kein Ersatz für den sichtbaren Dialog oder eine fertige
Gesamtoberfläche.

Der [native Export-Umsetzungsstand](GRAPHIC-EXPORT-NATIVE.md) enthält inzwischen
die geprüfte gemeinsame Datenbasis für Berichtsdiagramme und bis zu acht
Messläufe sowie [Zeichenbeschreibung und SVG](GRAPHIC-FIGURE-NATIVE.md).
Der [native Rasteradapter](GRAPHIC-RASTER-NATIVE.md) ergänzt reale Schriftmetriken,
PNG in allen vier Größen und eine eigene RGB-Vorschautextur. Der folgende
[native Berichtsdialog](GRAPHIC-DIALOG-NATIVE.md) ersetzt die alte breite
Berichtsleiste und ergänzt echte Vorschau, Optionen, Speicheraufträge und native
Bildkopie. Der folgende [Materialabgleich](GRAPHIC-MATERIAL-NATIVE.md) ergänzt Blur
und skalierte Felder. [Schatten, Referenzmaße und kleine Umbrüche](GRAPHIC-SHADOW-NATIVE.md)
sind anschließend nativ ergänzt. Der folgende [sichtbare Messlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md)
bindet bis zu acht Läufe an Kurve, Referenz, Legende, vollständige Statistik und
denselben Exportdialog. Die vollständige Design-/OS-/Assistive-/DPI-Abnahme
bleibt zusammenhängend umzusetzen.
Die bisherigen Daten-/Geometrieprüfungen ersetzen diese native Abnahme nicht.

Aktueller nativer Folgeauftrag vom 20. Oktober: zuerst die gesamte Gestaltung
einschließlich F1 direkt gegen die gewählten Referenzen angleichen. Der
[native Abgleich](NATIVE-FIDELITY.md) nennt den aktuellen Zwischenstand und die
noch erforderlichen Schritte. Bestehende Umsetzung übernehmen; die folgende
ursprüngliche Übergabe bleibt Anforderungsgrundlage, nicht Fertigstellungsnachweis.

Stand: 20. Oktober 2026. Die Browserreferenz verbindet die drei gewählten Designs
einschließlich gemeinsamem Resizing. Dieser Text ist zur Übergabe in einen
anderen Chat bestimmt. Die Vorschauarbeit selbst ist keine native Umsetzung.

---

Arbeite im bestehenden Projekt `D:\Physim` die native Physim-Oberfläche zu der
zusammenhängenden Oberfläche aus
`docs/visuals/unified-workspace/index.html` um. Implementiere die drei Bereiche
**Entwickeln**, **Simulieren** und **Auswerten** im vorhandenen C-/SDL-/Nuklear-/
OpenGL-Aufbau bis zur geprüften Fertigstellung. Die Browserreferenz ist festgelegt;
erstelle keine weiteren Designvarianten und keinen WebView-Ersatz.

Lies zuerst `AGENTS.md` und `D:/Skills/skills/AGENTS.md`. Prüfe `git status`, den
aktuellen lokalen Code und die vorhandenen Builds. Der lokale Projektstand ist
die Grundlage. Bei Erstellung der Übergabe liefen separate native Arbeiten;
Dateien wie `app/laboratory_*.inc`, `app/model_edit*.inc` und `app/model_values.*`
waren lokal vorhanden und teilweise noch uncommittet. Übernimm brauchbare bereits
vorhandene Umsetzung; überschreibe oder verwerfe keine fremden Änderungen. Die
Browserprüfung ist keine Abnahme dieser nativen Zwischenstände.

Lies anschließend:

- `docs/visuals/unified-workspace/README.md`
- `docs/visuals/unified-workspace/RESIZING.md`
- `docs/visuals/unified-workspace/IMPLEMENTATION.md`
- `docs/decisions/010-analysis-graphic-export.md`
- `docs/visuals/unified-workspace/EXPORT-EXAMPLE.md` als verbindliche Exportreferenz
- `docs/design-direction.md` und `docs/workspace.md`
- `docs/visuals/develop-designs/DESIGN.md` und `FIDELITY.md`
- `docs/visuals/simulation-designs/IMPLEMENTATION.md` und `MODEL-EDITING.md`
- `docs/visuals/analysis-workspace/IMPLEMENTATION.md` und `docs/reports.md`

Öffne die gemeinsame HTML-Datei lokal im Browser oder stelle `docs/visuals` mit
einem lokalen HTTP-Server bereit und öffne `/unified-workspace/`. Die sechs
`develop-*`, `simulate-*` und `analyze-*`-PNG-Dateien in diesem Ordner sind echte
Referenzaufnahmen. Die älteren Einzelvorlagen enthalten zusätzliche Zustände.
Die verbindliche aktuelle Ergänzung zu Resizing steht ausschließlich in
`unified-workspace/RESIZING.md`; ältere abweichende Home-/Shift-Regeln sind ersetzt.

Erhalte die kompakte globale Menüzeile und drei gleich breite Arbeitsbereich-Tabs.
Ein Projekt, Dokumente, Buildstatus, Laufdaten und Berichtsherkunft verbinden die
Bereiche. Der native Start bleibt die bestehende leere Arbeitsumgebung mit
„Ordner öffnen“; das automatisch offene Pendelbeispiel gehört zur Browserdemo.

**Entwickeln:** Die gewählte Werkbank Klarglas Kühl mit Panelköpfen, kompakten
Dateitabs, Workspace-Pfadlinien und schwebender Suchinsel erhalten bzw. angleichen.
Suche/Ersetzen dürfen den Code nicht verschieben. Der Kontextschalter bleibt
äußerst rechts. Echte Dokumente, Undo/Redo, Dirty, Save/Recovery, Build, Diagnosen,
Completion, API und F1 bleiben funktionsfähig.

**Simulieren:** Messlabor mit Wertefeldern B. Ohne persönliche abweichende
Dockanordnung initial Szenenbaum zu, Inspektor offen, Kurven zu. Große echte Szene,
Laufsteuerung und Tempo oben, Messwerte und optionale Kurven darunter, Zeitleiste
unten. Szenenbaum, Inspektor und Kurven sind unabhängig; Auswahl, Kamera und
betrachtete Zeit bleiben erhalten. Alle bestehenden Inspektorfunktionen,
Laufparameter/Validierung, Limits, Ereignisse und Aufzeichnung zugänglich halten.
Modellwerte verwenden einen expliziten typisierten Bindungsvertrag. Vormerken,
Verwerfen und ggf. Stoppen & neu kompilieren führen über den echten Buildweg;
erst ein neuer Lauf verwendet neue Artefakte. Keine pauschalen Regex-Ersetzungen
aus der Browserdemo in native C-/Physim-Quellen übernehmen.

**Auswerten:** Links durchsuchbare echte Messläufe und Mehrfachauswahl mit
eindeutiger Referenz, Mitte Messdaten/Analyseergebnis, rechts Kanal/Einheiten,
ausdrücklich benannter aktiver Lauf und Herkunft. Analysecode öffnet den vorhandenen
Editor. Analyse starten und Export bleiben echte vorhandene Prozesse. Implementiere
einen begrenzten echten Mehrlaufvergleich mit unabhängigen Zeitvektoren, passenden
Kanälen/Dimensionen, Statusmasken, Lücken und vollständiger Statistik je Lauf.
Bewahre die Auswahlreihenfolge und acht Analyseeingaben. Kein Paaren nach Sampleindex.
Berichte behalten ihre ursprüngliche Auswahl, Einheiten und Herkunft auch nach
neuer Laufwahl; alte Workerabschlüsse dürfen aktuelle Auswahl nicht überschreiben.
Rohdaten-/Ausschnittexporte, CSV/SVG/PNG und PNG-Größenwahl bleiben erhalten.

**Freigegebener Grafikexport:** Setze den Dialog aus `EXPORT-EXAMPLE.md` nativ um:
beschrifteter Button **Grafik exportieren…** neben den Zoomaktionen, links die
saubere Vorschau, rechts Format, Umfang, Darstellung und PNG-Größe, unten
**Bild kopieren** sowie **PNG/SVG speichern…**. Standard: PNG, 2400 × 1700 Pixel,
gesamtes Diagramm und Hell auf Weiß. Weitere PNG-Größen: 1200 × 850, 3600 × 2550,
4800 × 3400. SVG ist skalierbar; PNG-Größenwahl dort deaktivieren. Sichtbarer
Ausschnitt verwendet die tatsächlichen Achsengrenzen; die zweite Umfangsauswahl
ist ohne Ausschnitt deaktiviert. **Wie in der Auswertung** übernimmt die beim
Öffnen geltende Darstellung. Titel, Achsen, Einheiten, Legende, Referenz und alle
gewählten Vergleichskurven gehören ins Bild. Ausgabe und Vorschau halten denselben
beim Öffnen festgelegten Datenstand und bleiben unabhängig von Panelmaßen.
Echte native Bildkopie, Dateidialog, Abbruch und Fehlerfälle implementieren; CSV
als separaten Datenexport erhalten. Derselbe Ablauf gilt an einzelnen echten
Berichtsdiagrammen. Mit den beiden freigegebenen Exportaufnahmen und ADR 010
abgleichen; keine Browserformel oder WebView in die native Anwendung übernehmen.

**Ein gemeinsames Resizing:** Verwende für alle drei Bereiche dieselbe native
Trennerinteraktion auf Grundlage der bestehenden Werkbank/Docking-Implementierung.
Ruhige Grenze, bei Hover/Fokus eine durchgehende dünne Akzentlinie, großzügige
Trefffläche, korrekter Cursor, Maus-Capture, sichtbarer Fokus und native Separator-
Semantik. Pfeile bewegen die Grenze um 10 logische Pixel, Umschalt um 40.
Home/End setzen Minimum/Maximum, Enter/Doppelklick die jeweilige Standardgröße.
Escape, Eingabeabbruch oder Fokusverlust während Ziehen stellen den Ausgangszustand
wieder her. Rechts/unten wird der Bereich bei Bewegung nach links/oben größer.

Entwickeln: Workspace, Codekontext, Protokoll. Simulieren: Szenenbaum, Inspektor,
Szene/Messbereich, Zeitleiste und Verhältnis der beiden Kurven. Auswerten: Laufliste,
Details und Diagrammhöhe gegenüber Statistik. Die Gesamtvorschauhöhe der einzelnen
Simulation ist Präsentationsrahmen und kein zusätzlicher nativer Trenner.
Speichere Wunschgrößen und Grenzen passend zum bestehenden Docking, ohne sie bei
Fensterresize oder Seitenwechsel still zu überschreiben. Die Größen müssen echte
Renderrechtecke verändern. Kein Code-, Modell-, Zeit-, Kamera-, Daten- oder
Statistikwechsel durch Resizing. Beachte UI-/Textskalierung und kleine Fenster.

Die Vorschau führt weder C noch Analysecode aus, verwendet eine Kleinwinkelnäherung
und hält höchstens acht Demoläufe nur in der Sitzung. Diese Grenzen und das
automatische Pausieren beim Bereichswechsel sind kein neuer Vertrag für den
nativen Runner, die gespeicherte Laufbibliothek oder echte Analysen. Erhalte
bestehende native Abläufe, Abbruch, Initialisierung, HELLO/Timeouts, Reset in neue
Laufdateien, Analyseprojekte ohne Eingaben und historische Wiedergabe.

Prüfe die fertige native Umsetzung anhand von `IMPLEMENTATION.md`: echte
Windows-Aufgaben und Screenshots, tatsächliche Datenherkunft, alle acht
Simulation-/vier Analyse-Seitenkombinationen, Maus und Tastatur an jedem Trenner,
Abbruch/Reset, Größenwechsel, Hell/Dunkel, hoher Kontrast, große Schrift und
Accessibility. Führe Linux-Builds/Tests bei verfügbarer Umgebung in der gemeinsamen
Distribution `Alpine-Dev` aus. Linux/macOS nur nach tatsächlich ausgeführten
Prüfungen als geprüft melden; Browserergebnisse oder vorhandene CI-YAML genügen
nicht. Dokumentiere offene Plattform-/Screenreader-/DPI-Nachweise ehrlich.

Prüfe zusätzlich die native Exportabnahme aus ADR 010: alle vier PNG-Größen,
SVG, vollständige und gezoomte/verschobene Mehrlaufgrafiken, Vorschau-/Dateigleichheit,
festgehaltene Eingaben bei späteren Datenwechseln, echte Zwischenablage, Fokuskehr,
Speicher-/Renderfehler und von Panelresize/DPI unabhängige Ausgabemaße.

Halte native Änderungen, ausgeführte Tests, echte Aufnahmen und begründete
Abweichungen im gemeinsamen Designordner fest. Committe abgeschlossene geprüfte
Änderungen und pushe nach `origin` und `upstream` ohne Force-Push entsprechend
AGENTS.md. Räume nur eigene nachweislich entbehrliche Ausgaben auf; Quellen,
Nutzdaten, Referenzen, aktive Arbeiten und den gemeinsamen WSL-Datenträger erhalten.
