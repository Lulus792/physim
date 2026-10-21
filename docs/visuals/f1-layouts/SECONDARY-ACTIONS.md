# F1 H: Artikelansicht und Aktionen bei Bedarf

Fortsetzung des direkten Designabgleichs vom 21. Oktober 2026 auf Grundlage
von `2fc5740`. Die gewählten H-Bilder und ihre HTML-/CSS-Quellen bleiben
unverändert. [Gesamtauftrag](../unified-workspace/NATIVE-FIDELITY.md).

## Native Umsetzung

Die Standardansicht besitzt keine zusätzliche Artikelwerkzeugzeile und keine
drei gefüllten Zoomknöpfe in der Statuszeile mehr. Die Herkunftszeile nennt den
tatsächlichen Sprachlernweg, die Themengruppe und den aktuellen Artikel. Die
vorhandene Leseschriftgröße bleibt als ruhiger Statuswert sichtbar.

Rechtsklick im Artikel, **Umschalt+F10**, die Kontextmenütaste oder die
Schriftgrößenangabe in der Statuszeile öffnen die Handbuchaktionen. Dort bleiben
Zurück, Vorwärts, Start des gewählten Lernwegs, Arbeitsbereich, lokale Suche und
alle drei Leseschriftaktionen erreichbar. Nicht verfügbare Verlaufsschritte und
Zoomgrenzen sind deaktiviert. Pfeile, Pos1/Ende und Tab wechseln die verfügbaren
Einträge; Enter/Leertaste aktiviert, Escape kehrt zum vorherigen Fokus zurück.
Ein Klick außerhalb schließt das Menü und aktiviert kein darunterliegendes Ziel.
Die vorhandenen Kürzel für Verlauf und Zoom funktionieren weiterhin direkt.

**Strg/Command+F** zeigt die Artikelsuche als eigenes natives Overlay. Das Feld
enthält vorherigen/nächsten Treffer und eine Schließaktion. Es verändert weder
Artikelrechteck noch Panelbreiten. Der neue Suchtext wird vor Artikelumbruch,
Treffersprung und Veröffentlichung der sichtbaren Leseboxen übernommen. Die
globale Suche im H-Kopf bleibt unabhängig davon erreichbar. F6 führt weiterhin
zwischen Kopf, Themen, Artikel und Kontext; die Suchzeichen bleiben erhalten.
Escape schließt das F1-Fenster entsprechend dem bisherigen Handbuchvertrag.

Menü und Suchoverlay werden nur bei Bedarf als Nuklear-Oberflächen im eigenen
SDL-Dokumentationsfenster gebaut. Ihre Bediennamen, deaktivierten Zustände und
Fokusziele gehen durch den vorhandenen nativen Accessibility-Adapter. Die
Ergänzung ist kein Nachweis einer neuen externen Screenreader-Abnahme.

## Gefundene Fehler und Korrekturen

Die erste zusätzliche Menüprüfung las ihre Maße vor dem ersten gezeichneten
Menüframe. Der Diagnoselauf belegte unveränderte Artikelmaße und unveränderten
Scrollversatz bei noch null Menümaßen. Die Prüfung wartet jetzt auf den
veröffentlichten Menüfokus und den echten Hostfokus; die Geometrie- und
Scrollanforderungen bleiben unverändert.

Die anschließend erreichte Mausprüfung reproduzierte einen echten Absturz:
Eine neue Schriftgröße konnte aus einem gerade gezeichneten Menü heraus einen
Fontatlas ergänzen, während Nuklear-Schriftstile auf dem Stack lagen. Der
Fontpfad setzt diesen Stack zurück. Mausbefehle werden deshalb als einzelne
Aktion vorgemerkt und vor dem nächsten Eingabe-/Zeichenframe ausgeführt. Vor
der Ausführung wird die aktuelle Verfügbarkeit erneut geprüft. Das Menü nimmt
den öffnenden Mausdruck samt Freigabe nicht zugleich als Eintragsauswahl an.
Die Regression bedient eine noch nicht geladene Größe ausdrücklich per Maus.

Der vollständige Fensterablauf fand danach eine weitere Ursache für einen
ausbleibenden schnellen Start-Klick. Position und Hoverzustand passten, die
Menüfläche trug jedoch noch Nuklears Nur-Lesen-Flag: `flags=4129`,
`pressed=0`, bei bereits beendeter Öffnungssperre. `nk_window_set_focus`
hebt das Fenster an, entfernt dieses alte Flag aber nicht. Die ausdrücklich
aktivierten Menü-/Suchfenster werden jetzt vor ihrem Aufbau von diesem Flag
befreit. Die entsprechende positive Spur lautet `flags=33`, `pressed=1` an
derselben Position. Andere Dokumentfenster werden damit nicht aktiviert.
Nach Escape gibt die Regression zusätzlich ein echtes Suchzeichen ein und
löscht es wieder; ein scheinbar fokussiertes, aber nicht editierbares Feld
reicht zum Bestehen nicht aus.

Die Mathematikprobe fand außerdem eine Reihenfolgeabhängigkeit: Wenn der
Such-Editor erst nach dem Artikel gebaut wird, ist sein Text schon übernommen,
aber die sichtbare Lesebox gehört noch zur vorherigen Suche. Der Editor wird
jetzt zuerst verarbeitet und sein Fenster anschließend über dem Artikel gehalten.
Die vorhandene Prüfung fordert weiterhin das tatsächliche sichtbare Formelziel.

## Tatsächlich ausgeführte Prüfungen

[Prüfdaten, Quell- und Binärhashes, Befehle und Bildherkunft](secondary-actions-2026-10-21/validation.json)
halten die Läufe getrennt fest. Windows verwendet MSVC/AddressSanitizer;
Linux läuft nativ in `Alpine-Dev` mit ASan/UBSan, Xvfb und einem eigenen
Openbox-Prozess. Die Quellstände 62 und 63 unterscheiden sich ausschließlich
in `documentation_h_tests.inc`: Die neue Eingabeprobe setzt ihren Cursor vor
dem Menü ausdrücklich mit Ende ans Textende. Die Anwendungslogik ist gleich.
Ein zuvor falsch angenommenes Cursorziel wird damit nicht als Appfehler ausgegeben.

| Umgebung / Lauf | Ergebnis und Aussagegrenze |
| --- | --- |
| Windows, `run-slm3wx7e` (62) | Fensterablauf, Tastatur, Verlauf und Detailtext bestanden: 4/6. Seiten- und H-Prüfung scheitern bei fehlendem echten Hostfokus (`keyboard=0`). |
| Windows, `run-kf4sbcb8` (63) | H erneut nicht bestanden: Zeitlimit an Stufe 21, Hostfokus weiterhin null. Die 56 zuvor aufgenommenen H-Bilder sind vorhanden; die nachfolgende Menü-/Fokusprüfung ist damit nicht bestanden. |
| Linux, `run-xqh_u9p6` (62) | 7/8 bestanden: Fenster, Tastatur, Verlauf, Mathematik, Detailtext, Inline-Formeln und Seiten. H erreicht die neue Eingabeprobe, deren ursprüngliche Ende-Annahme falsch war. |
| Linux, `run-xtmu62o7` (63) | H vollständig bestanden, einschließlich Menü, Suchoverlay, Cursor-/Auswahlkehr, echter Texteingabe, Mauszoom und Rechtsklick. Zusammen mit den sieben Prüfgruppen aus 62 sind alle acht Gruppen an unveränderter Anwendungslogik positiv geprüft. |

Frühere positive Läufe `run-u7b2cw_m` (Windows, 5/5) und `run-gzs3jasv`
(Linux, 7/7) gehören zu Stand 57. Die Daten behalten sie als historische
Nachweise bei; sie ersetzen keine fehlgeschlagene aktuelle Windows-Prüfung.
Auch die vorherigen Abstürze und Zeitlimits sind mit ihren tatsächlichen
Ausgaben erfasst. Die Ursache der ausbleibenden Windows-Aktivierung ist
mit diesen Befunden noch nicht abschließend erklärt.

Die Linux-H-Probe erhält das Artikelrechteck `262,140,556,608` und den
Scrollversatz 4 beim Öffnen des Menüs. Nach Escape bleiben Cursor 6 und
Auswahl `0,0` unverändert. Ein echtes Eingabeereignis erzeugt `PendelZ`;
Rücktaste stellt den vorherigen Text wieder her. Nichtleere UTF-8-Inhalte
im Experiment- und Analyseeditor bleiben mit 61 beziehungsweise 57 Bytes
bytegenau erhalten. Der Statuswert hat einen zugänglichen Namen mit seiner
sichtbaren Größe; der veröffentlichte native Fokus wird ausdrücklich geprüft.

Die vorhandene Kontrollpixelprüfung wurde zusätzlich auf alle 56 tatsächlich
aufgenommenen H-Zustände beider Plattformen angewendet und besteht. Das ist
eine getrennte Bildprüfung und macht den fehlgeschlagenen Windows-Ablauf
nicht erfolgreich. Der neue direkte Vergleich mit den Originalen besteht
jeweils elf begrenzte Größen-/Flächenprüfungen in Hell und Dunkel auf beiden
Plattformen. Beide Originale bestehen ihre Selbstprobe. Das vorherige native
Bild aus Stand 52 verfehlt neun Flächenprüfungen und wird korrekt abgewiesen.

Die aktuelle Linux-Mathematikprüfung erfasst 185 Seiten, 1082 Links und
183 Zeichen in allen 42 Schriftvarianten einschließlich gebündelter Fonts.
Alle 48 Inline-Fälle bestehen einschließlich Größen- und Themewechseln.
Die mitgebauten separaten lokalen Accessibility-Änderungen sind unverändert
fingerprintet und nicht Teil dieses Patches; dies ist kein Clean-Checkout-Nachweis.

## Native Aufnahmen

Die 16 PNGs sind verlustfreie RGB-Konvertierungen der nativen Test-BMPs;
Quellbild-, PNG- und Pixelhashes stehen in den Prüfdaten. Sie wurden nicht
zugeschnitten oder skaliert. Direkt angesehen wurden unter anderem:

- Standardansicht: [Linux dunkel](secondary-actions-2026-10-21/linux-h-dark.png),
  [Linux hell](secondary-actions-2026-10-21/linux-h-light.png),
  [Windows dunkel](secondary-actions-2026-10-21/windows-h-dark.png).
- Bei Bedarf: [Tastaturmenü](secondary-actions-2026-10-21/linux-menu-keyboard.png),
  [Artikelsuche](secondary-actions-2026-10-21/linux-find-overlay.png),
  [Rechtsklick](secondary-actions-2026-10-21/linux-menu-context.png).
- Grenzen: [Mindestfenster](secondary-actions-2026-10-21/linux-h-minimum.png),
  [große Schrift](secondary-actions-2026-10-21/linux-h-large.png),
  [Formelsatz bei 32 px](secondary-actions-2026-10-21/linux-math-32.png).

## Grenzen des Abgleichs

Dies ist ein Teil des vollständigen H-Auftrags. Die Referenz enthält eine
gezeichnete Titelleiste, native Aufnahmen enthalten den Clientbereich. Der
begrenzte Pixelvergleich berücksichtigt diese verschiedenen Ursprünge und
prüft ausschließlich ausdrücklich genannte freie Flächen im Artikelkopf und
in der Statuszeile. Er verändert, skaliert und maskiert keine Eingabebilder.
Er ist kein vollständiger Bild-, Schrift- oder Interaktionsscore.

Der direkte Vergleich zeigt weiterhin unterschiedliche Schriftmetriken und
Gewichte, Artikelabstände sowie weitere Navigations- und Kontextdetails.
Die bereits für die Werkbank geprüfte Unterscheidung von CSS-em-Größe und
Nuklear-Schrifthöhe muss auch für die F1-Rollen fachlich abgeglichen werden;
die unterschiedlichen Größen gelten nicht als technische Unmöglichkeit.
Echte Inhalte, zusätzliche Abschnitte, korrekte Modellwerte und Fachgrafiken
bleiben erhalten. Aktuelle macOS-, physische Maus-/DPI- und externe
Screenreader-Nachweise bleiben offen.
