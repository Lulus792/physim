# Native Messkanal- und Einheitenkarten

Fortsetzung der [übrigen Inspektorinhalte](INSPECTOR-TAIL-NATIVE.md). Maßgeblich
sind der ausgewählte Stil B aus [MODEL-EDITING.md](../simulation-designs/MODEL-EDITING.md),
die tatsächliche Kaskade aus `inspector.css` / `inspector-variants.css` und die
Messkanalgruppe im ursprünglichen HTML. Die Referenzen bleiben unverändert.

## Umsetzung

„Messkanal & Einheit“ trägt jetzt den Referenzzusatz „Live“. Kanal und
Anzeigeeinheit stehen jeweils in einer eigenen B-Karte: 8 px Rundung, 1 px
Kontur, 11-px-Beschriftung, 5 px Abstand und 13-px-Auswahltext im mindestens
32 px hohen Feld. Der Auswahlbereich hat innerhalb der Karte keine zusätzliche
gefüllte Schaltfläche. Lange Namen umbrechen bei schmaler Breite und größerer
Schrift. Darunter steht der Messwert einschließlich Einheit in 24-px-Schrift,
gefolgt vom erklärenden Hinweis der Vorlage.

Kanalwahl und gebräuchliche Einheiten sind unmittelbar per Maus oder Tastatur
wählbar. Zentimeter pro Sekunde ergänzen die vorhandenen Vorgaben. Die Auswahl
verwendet dieselben Dimensions-/Winkelregeln, Überlaufprüfungen und persönliche
Einheitenablage wie die native Auswertung. Die ursprüngliche Einheit entfernt
den persönlichen Eintrag. „Benutzerdefiniert …“ im Menü erhält den bestehenden
Editor einschließlich Abbrechen und ausdrücklicher Wiederherstellung einer
beschädigten Ablage. Ein nicht schreibbarer Katalog wird nicht überschrieben.

Der Wert folgt `display_values`, also auch der betrachteten Zeit. Vorhandene
Messmasken, fehlende Messungen und nicht darstellbare Werte behalten ihre
ausdrücklichen Zustände. Ohne Kanäle erscheinen zwei gesperrte Auswahlfelder
und „Noch keine Messwerte“. Messdateien und Modellquellen werden durch die
Darstellungswahl nicht verändert.

## Tatsächlich ausgeführte Prüfung

Windows/MSVC Release und Alpine-Dev/GCC mit ASan/UBSan bestehen jeweils
`inspector_channel` und die vorhandene Gruppe `inspector_design`, mit System-
und gebündelten Schriften. Die neue Gruppe enthält insgesamt 60 native
OpenGL-Aufnahmen: sechs Theme-/Größenvarianten, ursprüngliche Einheit,
Zentimeter, Zeitrückblick und tatsächlicher Leerzustand. Die zusätzliche
Inspektorprüfung umfasst 205 Aufnahmen und prüft unter anderem die angeglichene
Überschrift. Die Bildleser prüfen echte Zeichenkommandos, Rechtecke und Pixel.

Interne SDL-Ereignisse wählen den echten Pendelkanal `position.x`, ändern die
Anzeige in Zentimeter, navigieren auf der nativen Zeitleiste zurück, kehren zu
Live zurück, öffnen/abbrechen die benutzerdefinierte Auswahl und stellen die
ursprüngliche Einheit wieder her. Quell- und Laufdatei-CRC bleiben gleich.
Die gespeicherten Zahlen werden unabhängig mit dem protokollierten SI-Wert und
dem tatsächlichen Umrechnungsfaktor verglichen.

Die bestehenden C-/Physim-Einheitenabläufe bestehen auf beiden Plattformen mit
Quelle 2. Dazu gehören Wiederöffnen, beschädigte Ablage, ausdrückliches
Zurücksetzen und unveränderte Modell-/Laufdateien. Die abschließenden Bild- und
Inspektorprüfungen verwenden Quelle 3. Die Runner-/Katalogprüfung besteht auf
beiden Plattformen. Exakte Befehle, Binärhashes, Quellstände und die gescheiterten
Testvorbereitungen stehen in den [eingefrorenen Belegen](inspector-channel-2026-10-24/README.md).

## Unterschiede und Grenzen

Namen, Kanalanzahl und Werte stammen aus dem wirklichen Modell. Deshalb zeigt
die Pendelprüfung `position.x` und ihren berechneten Wert statt der illustrativen
Namen und Zahlen aus dem HTML. Benutzerdefinierte Einheiten bleiben als
zusätzlicher Menüeintrag erreichbar, weil die native Anwendung beliebige
Modellkanäle unterstützen muss.

Systemschriften und deren Rasterung unterscheiden sich zwischen Windows und
Linux; die geprüften Schriftgrößen und Layoutmaße werden daraus nicht als
unvermeidlich abweichend erklärt. Die Bildbelege ersetzen keinen vollständigen
Browser-Pixelvergleich. macOS, praktische Screenreader-Aufgaben und physische
DPI-/Monitorwechsel bleiben offen. Diese Teilprüfung ist keine vollständige
Abnahme von Entwickeln, Simulieren, Auswerten oder F1.

Die aktuelle und die beiden verworfenen Prüfserien bleiben mit ihren jeweiligen
Bildern erhalten. Keine dieser Dateien wird als ersetzter erfolgreicher
Rohbestand gelöscht; zusätzliche Buildkopien wurden nicht angelegt.
