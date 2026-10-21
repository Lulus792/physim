Stand: Der native F1-Artikel wird weiter mit der gewählten H-Vorlage abgeglichen.
Der [aktuelle SVG-Schriftabgleich](SVG-LABELS.md) ergänzt echte kursive
Serifenglyphen für das Kontextmodell und einen vollständig offline nutzbaren
Font. Sanitizer-Gegenfälle und erfolgreiche native Prüfungen sind dort getrennt
belegt; außerhalb der Modellgrafik bleiben 112 native Bildpaare pixelidentisch.

Der vorhergehende [Navigations-/Kontextabgleich](NAV-CONTEXT.md) korrigiert die
Seitenleisten nach CSS, einschließlich der Panelkante, Gruppen- und
Abschnittsmaße, Modellwerte und Beispielkopf. Die Reiter verwenden intrinsische
Schriftbreiten und eine volle 2-px-Unterstreichung. Prüfungen und offene
Schrift-/Raster-/Laufzeitbefunde sind dort getrennt dokumentiert.

Der vorhergehende [Scrollabgleich](SCROLL.md) führt Herkunft und Artikel in einem
Viewport zusammen, stellt die volle Navigationsbreite her und dokumentiert
native Eingabe-/Umbruchprüfungen samt fehlgeschlagenen Vorläufen.

Der [Paarabstands-Abgleich](KERNING.md) korrigiert die Windows-Titelbreite über
OpenType-Paarwerte. Frühere Teilstände bleiben in [Artikelzeilen](ARTICLE-FLOW.md),
[Schriftrollen](TYPOGRAPHY.md) und [Artikelaktionen](SECONDARY-ACTIONS.md)
nachvollziehbar; ihre damaligen offenen Punkte beschreiben den jeweiligen
historischen Stand. Maßgeblich für den Folgeauftrag sind der aktuelle
[visuelle Abgleich](H-FIDELITY.md) und der
[Gesamtoberflächenstand](../unified-workspace/NATIVE-FIDELITY.md).

Weiter offen sind unter anderem Linux-Schrift/Glyphenrasterung, Bildunterschrift,
horizontale Codevorschau und weitere Kontextdetails sowie die vollständige H-Abnahme.
Browser-PNGs oder bestandene Funktionsgruppen ersetzen diese Abnahme nicht.
Die lokale HTML-Datei blieb im Browserwerkzeug gesperrt; daraus wird keine
neue interaktive Browserprüfung abgeleitet. macOS und vollständige externe
Assistive-Nachweise sind weiterhin nicht belegt.

Frühere Fokusarbeiten: [Lesefokus](READING-FOCUS.md),
[Paneelnavigation](PANE-NAVIGATION.md), [Windows-Fokus](WINDOWS-FOCUS-STATE.md),
[Texteingabe](PLAIN-LETTER-INPUT.md). Historische Erstabnahme: [NATIVE-H.md](NATIVE-H.md).
Der folgende ursprüngliche Umsetzungsauftrag bleibt als Abnahmegrundlage erhalten.

---

Setze in D:\Physim das ausgewählte F1-Layout/Design H nativ in der bestehenden
Dokumentationsoberfläche um.

Lies zuerst die Projektanweisungen in AGENTS.md und D:/Skills/skills/AGENTS.md,
dann docs/decisions/007-f1-layout-h.md und
docs/visuals/f1-layouts/DESIGN-H.md. Die verbindlichen visuellen Vorlagen sind
docs/visuals/f1-layouts/h-light.png und h-dark.png. Die vier unabhängigen
Seitenzustände zeigt h-final-overview.png. index.html in diesem Ordner ist die
klickbare Referenz und funktioniert lokal ohne Build.

H ist bereits vom Nutzer ausgewählt. Links stehen Themen und direkt sichtbare
Abschnittssprünge wie in E/G. Rechts steht der Modellbereich aus Entwurf C mit
den Tabs Modell und Beispiel. Die gesamte linke und die gesamte rechte Seite
werden ausschließlich mit ihrem kleinen eigenen Icon an den äußeren Enden der
Kopfleiste ein-/ausgeklappt. Keine langen sichtbaren Umschaltbeschriftungen,
keine verschachtelten Aufklappmenüs und keine weitere Designauswahl nötig.

Besonders wichtig: Der linke Schalter verändert nur die linke Seite, der rechte
nur die rechte. Der jeweils andere Bereich behält seine korrekte Position und
Breite. Alle vier Kombinationen müssen nutzbar sein; der Artikel darf niemals
auf null Breite geraten. Im Mockup wurde genau dieser Fehler durch explizite
Zuordnung zu den drei Spalten behoben. Übertrage die geometrischen Eigenschaften
auf die nativen Rechtecke und prüfe echte Sichtbarkeit sowie Breite/Position,
nicht nur boolesche Zustände. Erhalte Lesestelle, Scrollpositionen und rechten Tab.

Die Wahl C/Physim erfolgt einmal global im Dokumentkopf für den gesamten
Dokumentationskontext: Lernweg, Artikel/Anleitung, Suche und Beispielcode.
Gemeinsame Physikerklärungen bleiben ein Inhalt. Keine Sprachwahl in einzelnen
Code- oder Kontextbereichen. Die Sprache und beide Seitenzustände bleiben bei
Themewechseln erhalten. Suche darf links Treffer öffnen, ohne rechts zu ändern.

Übernimm die helle und dunkle Variante mit den vorhandenen semantischen
Farbrollen aus app/design_tokens.h. Erhalte auch den bestehenden hohen Kontrast,
Schrift-/DPI-Einstellungen und die vollständigen Handbuchinhalte. Der HTML-
Beispielbestand ist nur ein Designbeispiel und darf nicht die echte Themenliste
oder die vorhandene Suche ersetzen. Fehlender Modellkontext erhält einen
korrekten Leerzustand; kein statisches Pendel auf fachfremden Seiten.

Arbeite im vorhandenen C-/SDL-/Nuklear-Renderer, insbesondere
app/documentation_window.inc, app/documentation_ui.inc, app/main.c sowie den
bestehenden Design- und Dokumentationsadaptern. Baue keinen WebView-Ersatz.
Erhalte Formelsatz, Inline-Formeln, Tabellen, Links, Medien, Verlauf, Codekopieren,
Fokusführung, native Accessibility sowie die Trennung von Hilfe und Simulation.

Führe die Abnahme aus DESIGN-H.md mit passenden vorhandenen Tests und sinnvollen
Regressionen für die neuen Zustände aus. Prüfe echte Fensterbilder, den
Mindestfensterfall, große Schrift und zugängliche Bedienung. Behaupte Linux- oder
macOS-Gleichwertigkeit nur nach tatsächlich ausgeführten Builds/Tests.

Das bisher fertige Ergebnis ist ein geprüfter Designprototyp mit Übergabe; die
native App ist noch nicht auf H umgestellt. Beginne jetzt die native Umsetzung
auf Basis des aktuellen lokalen Projektstands und arbeite bis zur geprüften
Fertigstellung. Committe die abgeschlossene Arbeit und pushe wie in AGENTS.md
autorisiert ohne Force-Push nach origin und upstream.
