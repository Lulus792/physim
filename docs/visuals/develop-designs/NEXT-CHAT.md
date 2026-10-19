# Kopierfertiger Umsetzungsauftrag

Stand: 19. Oktober 2026. Diesen Text im nächsten Chat mit dem Projekt
`D:\Physim` verwenden. Das Design ist entschieden; die native Umsetzung steht aus.

---

Setze in D:\Physim das finale Entwickeln-Design aus
docs/visuals/develop-designs nativ um. Gewählt ist **Werkbank mit Klarglas Kühl**,
16 px Außenrundung, Panelköpfen, kompakten Dateitabs und Workspace-Pfadlinien.
Erstelle keine weiteren Designvarianten. Beginne nach der Bestandsprüfung mit
der Umsetzung und führe sie bis zur geprüften Fertigstellung durch.

Lies zuerst AGENTS.md und D:/Skills/skills/AGENTS.md. Lies danach:

- docs/visuals/develop-designs/DESIGN.md — verbindliche Gestaltung und Verhalten.
- docs/visuals/develop-designs/IMPLEMENTATION.md — native Ausgangspunkte,
  Unterschiede zur Demo und Abnahme.
- docs/visuals/develop-designs/README.md — Vorschau, Bilder und Designprüfung.

Die lokale HTML-Vorschau liegt unter
D:/Physim/docs/visuals/develop-designs/index.html. Die Bildreferenzen heißen
glass-cool-dark.png und glass-cool-light.png; glass-overview.png zeigt das
Suchdetail in beiden Erscheinungen. Übernimm das Produktdesign in die bestehende
C17-/SDL3-/Nuklear-/OpenGL-Anwendung. Die äußeren Präsentationsüberschriften,
Demo-Zustandswahl und Prüfoptionen gehören nicht in die App.

Besonders wichtig:

- Entferne den großen Abschnitt über dem Dateitext. Die Codefläche beginnt
  direkt unter den kompakten Dateitabs, ohne Projektbanner, zusätzliche Pfadzeile
  oder eigene Suchleiste im Dokumentfluss. Zeilennummern und Text beginnen mit
  nur 6 logischen px Innenabstand.
- Suche schwebt rechts oben über dem Code: 4 px nach unten, 8 px nach rechts,
  maximal 540 px breit und bei engem Editor passend begrenzt. Öffnen, Schließen
  und Aufklappen der Ersetzung verändern Editorrechteck und bisherigen Scrollstand
  nicht. Erst die bewusste Treffernavigation darf zum Treffer scrollen.
- Verwende die gewählte kühle, durchscheinende Glasfläche mit 2 px Hintergrundblur,
  16 px Außenradius, 9 px Innenradius und den Farben aus glass.css/DESIGN.md.
  Eingaben und Iconflächen haben stärkere Deckung. Nutze die eigenen Suchicons
  aus search-resize.js: Lupe, Aufklappchevron, gemeinsames Vor-/Zurück-Feld,
  Schließenkreuz und Ersetzen-Symbol mit Text. Keine Schriftzeichen als Icons.
- Der Kontextschalter bleibt beim Zu- und Aufklappen **an exakt derselben Stelle
  ganz rechts** in der oberen Panelzeile, immer rechts von Suche und Speichern.
  Verankere ihn an der gemeinsamen Arbeitsfläche; baue keine zweite Position
  links der Aktionen für den geschlossenen Zustand. Linker Workspace-Schalter
  sitzt offen im Panelkopf und geschlossen links der Dateitabs.
- Workspace, Editor/Protokoll und rechter Kontext sind über Trennlinien
  größenverstellbar. Verwende das vorhandene Docking einschließlich gespeicherter
  Größen, ausgeblendeter Panels, schwebender Fenster und Tabgruppen. Ergänze
  Tastaturbedienung, Fokus und Abbruch beim Resizen. Freie Panelaufteilung ist
  nicht das Ändern der Breite einzelner Dateireiter.
- Erhalte die Pfadlinien, Einrückung und Ebene der Unterordner; assets, runs und
  build sind Beispielnamen auf derselben Ebene. Zeige echte Workspace-Daten.
  Profil und tatsächlicher Buildstatus sitzen unten, Suche/Speichern an den
  Dateitabs. Die übrigen App-Funktionen bleiben erreichbar.

Die Suchinsel gilt für Experimentcode, Analysecode und zusätzliche Textdokumente.
Verbinde sie mit dem realen aktiven Editor: vorwärts/rückwärts mit Umbruch,
Trefferzahl, einzelnes Ersetzen, UTF-8, Undo/Redo, Dirty-/Save-/Recovery-Verhalten.
Ctrl/Command+F öffnet und fokussiert die Suche; Enter und Umschalt+Enter navigieren;
Escape schließt sie und gibt den Fokus an das Suchicon zurück. Sie ist nichtmodal;
Maus und Scrollen außerhalb der Insel bleiben im Editor nutzbar. Sicherheits- und
Wiederherstellungsdialoge behalten Vorrang.

Die Demo simuliert Dateien und Builds und zeigt illustrative Kontextdaten.
Erhalte echte Projekt-/Sprachunterstützung, Dateitabs samt Umordnen/Schließen,
Dateidialoge, Speichern, Build/Abbruch, Diagnosesprung, Completion und API-Referenz.
Keinen WebView-Ersatz, keine Pendel-Hardcodierung und keine neuen Scheinaktionen
einbauen. Für Kontextinformationen vorhandene reale Daten verwenden, fehlende
Informationen als leeren Zustand behandeln.

Der native Renderer besitzt noch keinen Suchinsel-Backdrop-Blur. Implementiere
den Effekt im vorhandenen Renderer mit korrekter Zeichenreihenfolge, Clipping,
Skalierung und Ressourcenfreigabe. Bei reduziertem Transparenzbedarf, hohem
Kontrast oder fehlender Unterstützung gilt der deckende Fallback. Ein pauschal
deckender Ersatz ist keine vollständige Umsetzung des gewählten Normalzustands.

Prüfe anhand von IMPLEMENTATION.md mit sinnvollen bestehenden und ergänzten
nativen Regressionen sowie echten Aufgaben in der App. Vergleiche native Bilder
mit beiden Referenzen. Teste Seitenkombinationen, Resizen, enges Fenster,
Hell/Dunkel, große Schrift, Tastatur, Fokus, Systemtextskalierung und Accessibility.
Browserprüfungen sind keine native Abnahme. Linux/macOS nur nach tatsächlich dort
ausgeführten Builds/Tests als geprüft melden; offene Nachweise konkret benennen.

Arbeite vom aktuellen lokalen Stand aus und bewahre fremde Änderungen. Die in
IMPLEMENTATION.md genannte Revision dient nur der Orientierung, nicht zum
Zurücksetzen. Dokumentiere Umsetzung, echte Screenshots, Prüfergebnisse und
begründete Abweichungen im Designordner. Committe geprüfte Änderungen und pushe
laut AGENTS.md ohne Force-Push nach origin und upstream.
