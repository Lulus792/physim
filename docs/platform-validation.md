# Plattformprüfung

Stand: 8. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

Die [historischen Plattformnachweise](platform-validation-history.md) enthalten
die früheren lokalen/CI-/Paketprüfungen bis zum ersten nativen macOS-
Accessibility-Baum. Ihr vollständiger Text und die Belegpfade bleiben erhalten.
Die folgenden Abschnitte dokumentieren die neueren tatsächlich ausgeführten
Prüfungen. Beide Seiten sind im Offline-Handbuch erreichbar.

## Linux-AT-SPI und asynchrone UI-Aktionen (PP-0710)

Ausgangspunkt ist `b1d678cb6221eeb41775857c7508407542a09e53`. Linux besitzt
jetzt eine libdbus-Anbindung für die vorhandenen einfachen Buttons und Texte.
Ein unabhängiger PyAT-SPI-Client entdeckt zwei tatsächlich gerenderte
SDL-/Nuklear-Fenster über die Registry und prüft UTF-8, Rollen, Eltern,
Fenster-/Bildschirmrahmen, Hit-Tests, Bulk-Cache, native Buttonaktionen und
Zustands-/Entfernungsereignisse. Ungültige RPC-Signaturen, Zahlenüberläufe in
Objektpfaden und schreibgeschützte Properties werden ausdrücklich geprüft.
Die App bleibt ohne Bus sowie mit `NO_AT_BRIDGE=1` nutzbar.

Unter Debian 12/X11, Clang 14 Debug mit AddressSanitizer und
UndefinedBehaviorSanitizer besteht diese Prüfung nach den unten beschriebenen
Korrekturen **1/1** in `build/atspi-asan-linux/test-results/run-4sg6kclt`
(`build/atspi-race-asan-linux.log`). `ASAN_OPTIONS=detect_leaks=0` betrifft
SDL/Mesa-Leaks; Address-/UndefinedBehavior-Prüfungen bleiben aktiv. Die
isolierte D-Bus-Sitzung verwendet temporäre GSettings und einen ausdrücklich
zugeordneten Accessibility-Bus, damit X11-Sitzungsdaten keine andere
Testsitzung auswählen.

Die erste Instrumentierung belegte einen C17-Grenzfehler in der vendorten
Nuklear-Struktur: `nk_draw_text` schrieb über `char string[2]` hinaus, obwohl
der Befehlsbuffer bereits dynamisch genug Speicher reservierte. Ein echtes
flexibles Array `char string[]` beschreibt jetzt diese Speicherung korrekt.
Die neue Gegenprobe prüft eine vollständige 1500-Byte-UTF-8-Nutzlast samt NUL.
Ein weiterer Lauf belegte eine echte Timing-Lücke: Nach dem Zeichnen
angenommene Aktionen wurden beim Veröffentlichen gelöscht. Sie bleiben nun
für den nächsten Besuch eines weiterhin vorhandenen, aktivierten Controls
erhalten; verschwundene/deaktivierte Ziele verwerfen sie. Eine deterministische
Modellprüfung deckt diesen Übergang und die einmalige Zustellung ab.

Die Linux-ASan-/UBSan-Prüfungen für Modell, Textbefehle, UI-Geometrie und
Clipboard bestehen **4/4** in `run-c8w74e68`; das frühere Textpuffer-Ergebnis
`run-vrhrjna7` lag noch vor der Aktionskorrektur. Auf Intel-macOS bestehen die
vier Release-Prüfungen in `run-zv47b3bg`, die zwei tatsächlichen AppKit-/UI-
Prüfungen nach der Textpufferkorrektur in `run-colyd17p`. Der zusätzlich mit
Apple Clang/C17/Werror gebaute Modelltest mit Aktionskorrektur besteht in
`build/atspi-model-mac.log`. Der macOS-Sanitizer-Aufruf wurde wegen fehlendem
`ld64.lld` vor dem Build abgewiesen und zählt nicht als ausgeführte Prüfung.

Die frühen Fehlversuche bleiben erhalten: falscher Statusobjektpfad
`run-vc5gg8bf`; Busadress-/Cacheprobleme `run-wofa__h8`, `run-zqx281r6`;
Clientbehandlung veralteter Interfaces `run-8j42i9ay`; lokale Python-
Signaturprüfung `run-lbuqeen1`; ungetrennte X11-Buswahl `run-fpyigr37`;
Nuklear-Grenzfehler `run-30hri577`/`run-hxak4vac`; verlorene späte Aktion
`run-e_nqfms8`. Die beiden letzten waren Implementierungsfehler und wurden
behoben, ohne Sanitizer auszuschalten oder Wartezeiten zu verlängern.
Die Release-Gegenprobe für geprüfte Objektpfade vor der zusätzlichen
Fallback-/Aktionskorrektur bestand in `run-6wyxse59`.

Dieser Nachweis gilt für Debian/X11 und Intel-macOS. Vollständige Orca-/
VoiceOver-Bedienung, Wayland, Fokus, Textfelder und weitere Widgets sowie
Windows/UI Automation bleiben offen. PP-0710 bleibt unvollständig; der gesamte
Projektplan wird damit nicht abgenommen.


Die zusätzliche externe Gegenprobe für Elternkoordinaten belegte anschließend
noch einen Hit-Test-Fehler: Das Elternobjekt eines Fensters verwendet
Bildschirmkoordinaten, das Elternobjekt seiner Controls Fensterkoordinaten.
Die Child-Gegenprobe wird jetzt im Koordinatenraum des Aufrufers ausgeführt.
`build/atspi-parent-before-2.log` hält die fehlgeschlagene echte Clientassertion
fest; der erste Harness-Aufruf (`atspi-parent-before.log`) scheiterte bereits
am fehlenden Legacy-PyAT-SPI-Namen für den standardisierten Koordinatentyp 2.
Die komplette externe Prüfung inklusive Fenster-, Bildschirm- und Eltern-Hit-
Tests besteht danach unter ASan/UBSan **1/1** in `run-fk9dyxub`
(`build/atspi-parent-final-asan-linux.log`).


Der vollständige Intel-macOS-Release-Fensterlauf besteht **79/79** in
`run-ogg1ybol` (`build/atspi-all-gui-mac.log`), einschließlich aller 68
Projektmanager-Kombinationen mit 16/22-Pixel-Schrift. Sein App-Binary wurde vor
der zusätzlichen späten Aktionskorrektur gebaut; diese wird separat durch den
finalen Modell-/Native-/SDK-Nachweis geprüft. Der Nuklear-Textpuffer war bereits
korrigiert. Dadurch wird der breite GUI-Lauf nicht als neuer vollständiger
Core- oder plattformübergreifender Screenreader-Nachweis umgedeutet.

Für freien Linux-Prüfspeicher wurde ausschließlich der abgeschlossene ältere
`Linear systems SDK ä linux lk2mfuc5`-Prüflauf archiviert. Das Archiv
`build/atspi-linear-archived-proofs.tar.gz` enthält **14617 Dateien/Verweise**,
ist **313895774 Byte** groß und besitzt SHA-256
`883b00c02b36bb6941e467e025387a9e19dcb9d9096271d3ae0529bced278986`.
Jeder Inhalt wurde gegen das Originalinventar geprüft, die Originale vor dem
Entfernen nochmals vollständig geprüft, und die spätere Linux-Archivkopie
hat denselben Hash. Archiv und Inventar liegen auf Mac und Linux. Wiederherstellung
entpackt die archivierten `build/`-Pfade; der vorherige SDK-Nachweis bleibt damit
nachprüfbar. Receipts: `build/atspi-linear-archived-proof-inventory.json`,
`atspi-linear-archive-verified.json`, `atspi-linear-archive-removed.json` und
`atspi-linear-archive-linux-copy-verified.json`. Der erste gleichzeitige SCP-
Transfer wurde vor Beginn mit einem SSH-Bannerfehler abgewiesen; der serielle
Transfer und seine Hashprüfung waren erfolgreich.


Am **8. Oktober 2026** bestehen die finalen AppKit-/UI-Prüfungen mit der
späten Aktionskorrektur **2/2** in `run-k5qmbdrk`
(`build/atspi-final-native-mac.log`). Das frisch gebaute und nach
`AT-SPI SDK ä mac 0er2rk56/Relocated SDK ä` verschobene SDK besteht seine
Manifest-/Bytegleichheitsprüfung für 450 SDK-Dateien, das unabhängige
Verification-Kit mit 95 Dateien sowie echte
Menü-, Einstellungs- und Handbuch-GUI-Abläufe. Receipt:
`build/atspi-sdk-mac-PASSED.json`. Die unveränderten UI-/Build-/Guide-Eingaben
sind in `build/atspi-source-freeze.json` festgehalten; diese fortgeschriebene
Prüfchronik ist bewusst nicht Teil des Freeze. Die Paketprüfung ist gezielt
und ersetzt keinen neuen vollständigen Core- oder Screenreader-Lauf.


Der vollständige Debian/GCC-Release-Fensterlauf besteht **78/78** in
`run-38r48_zv` (`build/atspi-all-gui-linux.log`), einschließlich aller 68
Projektmanager-Kombinationen und des finalen externen AT-SPI-Clients. Das
App-Binary dieses breiten Laufs enthält die Aktions-/Textpufferkorrekturen,
wurde aber vor der zusätzlichen Eltern-Hit-Test-Korrektur gebaut. Die letzte
native Fixture wurde anschließend mit der korrigierten Implementierung gebaut
und geprüft; die ASan-/UBSan-Gegenprobe `run-fk9dyxub` deckt denselben finalen
Hit-Test-Code ab. Ein aktuelles Linux-SDK wird danach aus diesem Code neu gebaut.


Das am 8. Oktober frisch gebaute Debian/GCC-Release-SDK wurde nach
`AT-SPI SDK ä linux chlu_b3p/Relocated SDK ä` verschoben. Alle **450 SDK-Dateien**
und **95 Kit-Dateien** stimmen mit ihren Manifesten überein; der App-Binary
ist bytegleich mit dem finalen frischen Build. `ldd` löst `libdbus-1.so.3`
auf und zeigt keine fehlende Bibliothek. Menü, Einstellungen und Handbuch
bestehen danach als tatsächliche GUI-Abläufe aus dem verschobenen Paket.
Alle sechs aufgezeichneten Schritte (Install, Kit, Linkage, drei GUI-Abläufe)
bestehen; Receipt: `build/atspi-sdk-linux-PASSED.json`, zusammen mit dem
macOS-Receipt und den 21 unveränderten Dateien in
`build/atspi-source-freeze.json`. Dies ergänzt die native externe Clientprüfung,
behauptet aber keine praktische Screenreader- oder vollständige Core-Abnahme.


## Skalare Suche, exakte Toleranzen und Modulblock-Closures (PP-0361)

Ausgangspunkt am 8. Oktober 2026 ist
`c07e4b832d504d85581284cd74f453f4bcf1ceb2`. Die Abbruchbedingung von Bisection
und Golden Section wird jetzt als exakter Vergleich der binären Werte behandelt.
87 vorbereitete Gegenbeispiele belegten falsche Konvergenz bei subnormalen
Intervallen. Eine einfache Differenzkorrektur schloss diese, ließ aber 1007
relative Grenzfälle in der separaten Probe offen. Die finale Implementierung
entscheidet klar entfernte Fälle über eine konservative Fehlerumhüllung und
Rundungsgrenzen über einen festen Ganzzahlakkumulator, ohne Heap oder breiteren
Gleitkommatyp. Das betrifft die Abbruchentscheidung, nicht die Präzision der
Callback-Ergebnisse.

Die öffentliche Gegenprobe `tests/test_scalar_range_oracle.py` prüft **7787 C**-
und **243 Physim**-Berichte mit unabhängigen Fraction-Vergleichen. Sie prüft
anfängliche sowie finale Abbruchentscheidungen, Callback-Bereiche/Zähler und
bekannte Lösungen. Der ältere Code scheitert auch an dieser öffentlichen
Gegenprobe (`build/scalar-original-oracle-failure.txt`). Zusätzlich bestanden
1152 vorbereitete C-Gegenproben an den größten endlichen Intervallgrenzen.

Ein weiterer belegter Grenzfehler war vorzeitige Stagnation: Bei
`[DBL_TRUE_MIN, 4*DBL_TRUE_MIN]` vertauschten getrennt gerundete Produkte die
Golden-Section-Punkte. Der ältere Code lieferte `PS_LIMIT` nach null Iterationen,
obwohl zwei Innenpunkte vorhanden waren. Skalierte Mischwerte liefern nun
geordnet die beiden Innenpunkte und nach einer Iteration `PS_OK` mit erfüllter
Toleranz. Auch der bisezierende Mittelpunkt wird in kleinen/engen Intervallen
stabil berechnet; gewöhnliche breite Intervalle behalten ihre normale Rechnung.
Die neue öffentliche C-Gegenprobe enthält beide konkreten Sample-Grenzfälle.

Die erste Physim-Gegenprobe belegte außerdem einen echten Compilerfehler:
Schleifen-/Blockwerte auf Modulebene wurden wie echte Modulglobals behandelt,
aber als C-Lokalvariablen ausgegeben. Closures referenzierten so undeclared
Identifikatoren. Der Checker unterscheidet jetzt direkte Modulvariablen von
Blocklocals und übernimmt letztere als besitzende Captures. Zurückbehaltene
Schleifenwerte, veränderte lokale Entwürfe, echte lebende Modulglobals,
verschachtelte String-Captures, lokale Funktionen mit Array-Captures und
optionale Bindungen werden nativ ausgeführt. Der frühere Harness-Aufruf mit
`Optional.none()` war ein Syntaxfehler der Probe; die Sprache verwendet `nil`.

Die gezielte Linux-Clang-Debug-ASan-/UBSan-Prüfung vor der zusätzlichen
Sample-Korrektur besteht **5/5** in `run-7ggfzxaa`
(`build/scalar-final-initial-asan-linux.log`). Die frühere fünffache Prüfung
`run-v71uny34` hatte die verstärkte anfängliche Entscheidungsgegenprobe noch
nicht enthalten. Unter Intel-macOS/Apple Clang bestehen vor der Sample-Korrektur
**5/5** in `run-lzvgypq1`; danach bestehen **19/19** relevante Numerik-, Scalar-,
Closure-, ODE- und Referenzfälle in `run-h1nrywai`
(`build/scalar-sampling-final-mac.log`).

Der breite Intel-macOS-Release-Lauf besteht **640/641** in `run-b17mylss`
(`build/scalar-full-mac.log`). Sein einziger Fehler war die veraltete erzeugte
API-Referenz nach der Header-Vertragsänderung. `tools/generate-reference.py`
aktualisierte nur `docs/reference/numerics.md`; alle 30 Referenzdateien bestehen
anschließend `--check`, und die Referenzgegenprobe besteht im gezielten finalen
Lauf. Dieser breite Lauf lag noch vor der zusätzlichen Sample-Korrektur; die
finale 19er-Prüfung deckt diese gezielt ab. Die Fehlerbelege `run-ho7tfwlu`
(falsche erwartete Iterationslage der neuen C-Probe), `run-823gb3qm`
(Harness-Syntax) und `run-b5ug0q9u` (echte Capture-Lücke) bleiben erhalten.

Das finale Intel-macOS-SDK `Scalar search SDK ä mac nn54utn3` besteht seinen
Manifestnachweis für **451 SDK-Dateien** und das unabhängige Kit mit **100 Dateien**.
Der aus dem Kit entpackte Prüfer verwendet `--scalar-only`, verschiebt das Paket
und baut den Core ausschließlich aus dessen Quellen neu. Öffentliche C-Probe,
Fraction-/C-/Physim-Berichte und Modulblock-Closures bestehen mit installiertem
und frisch gebautem Core. Receipt: `build/scalar-sdk-mac-PASSED.json`. Dieser
gezielte Paketnachweis ist keine Wiederholung der vollständigen früheren
SDK-Domänen- oder GUI-Abnahme.

Für die Linux-Prüfung wurden nur die abgeschlossenen älteren SDK-Prüfläufe
`ODE range SDK ä linux lpk4sq92` und `Series numeric SDK ä linux p5iekc7w`
archiviert. `build/scalar-old-archived-proofs.tar.gz` enthält **28513 Dateien/Verweise**,
**594890472 Byte**, SHA-256
`7cdd96fd1c440adfd32b066f8263792f74d28004bdce6aef8de6dc50742323be`.
Inhalte wurden gegen das Inventar geprüft, Originale vor dem Entfernen nochmals
geprüft und die Linux-Archivkopie gegen denselben Hash geprüft. Archiv und
Inventar liegen auf Mac und Linux; Entpacken stellt die ursprünglichen `build/`-
Pfade wieder her. Receipts: `build/scalar-old-archived-proof-inventory.json`,
`scalar-old-archive-verified.json`, `scalar-old-archive-removed.json` und
`scalar-old-archive-linux-copy-verified.json`.

PP-0361 besitzt damit einen begrenzten Implementierungsnachweis für Bisection
und Golden Section; globale Optimierung, weitere Mathematikforderungen und
die gesamte Plattform-/Produktabnahme bleiben offen. Die 531 Originalplanblöcke
bleiben vollständig erhalten: 27 implementiert, fünf unvollständig, 499 ungeprüft.


Der breite Debian/GCC-Release-Lauf besteht **641/641** in `run-t4s0ldly`
(`build/scalar-full-linux.log`). Die erzeugte Referenz war dort bereits
aktualisiert. Dieser Lauf lag vor der zusätzlichen Sample-Korrektur.
Mit dem finalen stabilen Mischwert bestehen danach die **19/19** relevanten
Fälle sowohl unter GCC/Release in `run-h51lh8ze` als auch unter
Clang/Debug/ASan/UBSan in `run-z0mxt__a`
(`build/scalar-sampling-final-release-linux.log`,
`build/scalar-sampling-final-asan-linux.log`). Es wurden keine Wartezeiten
verlängert und keine Sanitizer-Prüfungen unterdrückt; `detect_leaks=0` ist wie
zuvor für SDL/Mesa gesetzt, Address-/UndefinedBehavior bleiben aktiv.


Der eng begrenzte Apple-Clang-O2-Zeitvergleich von 20000 flachen Golden-Section-
Suchen auf `[-10,10]` mit absoluter Toleranz `1e-10` behält 53 Iterationen/
55 Auswertungen. Drei ältere Läufe brauchen 0,00519–0,00534 Sekunden, drei finale
0,01284–0,01531 Sekunden. Der exakte Vertrag und stabile Randpunkte verursachen
hier etwa Faktor 2,5 im Median; das sind in dieser Probe rund 0,4 Mikrosekunden
zusätzliche Zeit pro Suche. `build/scalar-benchmark-final.json` bewahrt die Werte.
Dies ist kein allgemeines Leistungsversprechen für andere Callbacks/Intervalle;
Grenzfälle verwenden zusätzliche exakte Arbeit.


Das finale Debian/GCC-SDK `Scalar search SDK ä linux pe_dtue1` besteht ebenfalls
seine **451 SDK-Dateien** und das unabhängige Kit mit **100 Dateien**. Der aus
dem Kit entpackte Prüfer verschiebt es und führt `--scalar-only` mit dem
installierten sowie allein aus Paketquellen neu gebauten Core aus. Alle drei
äußeren Schritte (Install, Kit, Verify) bestehen; die sechs jeweiligen
C-/Closure-/Orakel-Gegenproben bestehen. Receipt:
`build/scalar-sdk-linux-PASSED.json`. Die beiden Plattform-Receipts passen zu
allen 20 Dateien in `build/scalar-source-freeze.json`; die fortgeschriebene
Prüfchronik ist davon bewusst ausgenommen. Der gezielte SDK-Nachweis ersetzt
keinen neuen vollständigen SDK-Domänen-/GUI-Lauf und keine aktuelle Windows-
oder Apple-Silicon-Abnahme. Der Gesamtplan bleibt unvollständig.


## Transformationsbereiche und Mathematikzuordnung (PP-0357)

Ausgangspunkt am 8. Oktober 2026 ist
`e33094d920708b85991842f6cedfdaf90429172a`. Die lokalen API-/Quellen-/Testprüfungen
ordnen Vec2/3/4, Mat3/4, Quaternionen und Transformationen konkret zu. Die
bestehenden 2000er-Inversions-, Rodrigues-, Slerp- und Senkrechtstellungsprüfungen
bleiben aktiv. Der gesamte Plan und die übrigen Mathematikblöcke bleiben offen.

Drei direkte C-Gegenbeispiele lieferten zuvor `PS_NUMERIC` bei endlichen
Ergebnissen: `DBL_MAX*2-DBL_MAX`, der projektive Quotient
`(DBL_MAX*2)/DBL_MAX` und die normierte Richtung der invers-transponierten
Skalierung `diag(DBL_TRUE_MIN,1,1)`. Ein weiterer Fall verlor den kleinsten
Rest zwischen `DBL_MAX` und `-DBL_MAX`. Die Zeilensummen behalten nun exakte
binäre Produkte/Summen. Projektive Punkte dividieren den exakten Zähler durch
die exakte homogene Koordinate; Ausgabe wird einmal nearest-even gerundet.
Ein eigens provozierter halber kleinster Zahlenwert plus positiver Rest belegte
auch doppelte Rundung; die finale Konvertierung erhält diesen Rest korrekt.

Normalen lösen das transponierte System nach Zeilenskalierung der ursprünglichen
Matrix direkt mit unabhängigen Lösungsexponenten. Eine überlaufende vollständige
Inverse ist dafür nicht erforderlich. Der interne lineare Solver wurde ohne
Änderung seiner Elimination/Pivotregeln in ein privates gemeinsames Header
überführt; die öffentliche Lösung konvertiert weiterhin atomar nach Double und
meldet echte nichtdarstellbare Ausgaben. Normalen bleiben durch die dokumentierte
Double-Koeffizienten-/Pivot-/Konditionsgenauigkeit begrenzt.

Die dauerhafte unabhängige Gegenprobe umfasst **1288 C**- und **100 Physim**-
Fälle. Fraction bestimmt die exakt gerundeten Punkt-/Richtungswerte, rationale
Elimination und 100-stellige Decimal-Normierung bestimmen Normalen. Enthalten
sind allgemeine projektive Matrizen, Auslöschung, subnormale Halbwege, negative
homogene Koordinaten, starke Skalierungsunterschiede/Scherungen und atomare
Fehler. Zusätzliche vorbereitete C-Prüfungen bestanden für 3000 gemischte und
3000 allgemeine projektive Fälle sowie 1770 Rundungsgrenzen.

Vor der abschließenden Beschleunigung bestehen auf Intel-macOS **10/10** relevante
Mathematik-/Transformations-/Linear-/Scalar-/Referenzfälle in `run-dq6ziv03`
(`build/transform-final-targeted-mac.log`) und unter Linux/Clang/Debug/ASan/UBSan
**10/10** in `run-j3sb1yjj` (`build/transform-final-asan-linux.log`). Die früheren
sechsteiligen Nachweise `run-zwsfs74t` und `run-_oxj56zk` liegen vor den finalen
Fast-Path-/Dokumentationsänderungen. Der erste vierteilige Regressionstest
`run-crffb4gf` lag noch vor der exakten Zeilen-/Quotientenrechnung.

Die ersten vier tatsächlichen Intel-macOS-Grafikprüfungen bestehen **4/4** in
`run-m8k_1_vt` (`build/transform-gui-mac.log`): Grafik/Picking, UI-Renderer,
Hierarchie und Szenenframes. Die Mikroprüfung zeigte dennoch erhebliche
Mehrarbeit der ausschließlich ganzzahligen affinen Rechnung. Der finale Pfad
verwendet gewöhnliche fehlerfreie Produkt-/Summenentwicklungen und fällt nur
bei kleinen Produkten oder überlaufenden Entwicklungsstufen auf Ganzzahlen
zurück. Alle 15000 zusätzlich vorbereiteten gewöhnlichen C-Summen stimmen
weiterhin mit rationalen Referenzen überein. Diese Grafikprüfung lag vor der
zusätzlichen Beschleunigung; deren finale Numerik-/Grafiknachweise folgen separat.

Für 10000 affine Punkttransformationen mit Scherung 0,2 und Translation 10
braucht die ältere Apple-Clang-O2-Probe rund 0,000294 Sekunden. Die reine
Ganzzahlfassung brauchte 0,034–0,036 Sekunden, die finale Entwicklung
0,000868–0,000917 Sekunden. Das sind in dieser bewusst kleinen Probe rund
0,06 Mikrosekunden zusätzliche Zeit pro Punkt; keine allgemeine Garantie für
andere Matrizen/Callsites. Werte: `build/transform-benchmark-proof.json`.


Die finale beschleunigte Intel-macOS-Numerikprüfung besteht **10/10** in
`run-ysdfd81_` (`build/transform-expansion-final-mac.log`), ihre vier tatsächlichen
Grafik-/Szenenprüfungen bestehen **4/4** in `run-ng4ovmww`
(`build/transform-expansion-gui-mac.log`). Unter Linux/Clang/Debug/ASan/UBSan
bestehen dieselben zehn finalen Numerikfälle in `run-19keu_1u`
(`build/transform-expansion-asan-linux.log`). Die Linux-Numerik-/Grafikläufe vor
der Beschleunigung bestehen **10/10** in `run-2n32hh01` und **4/4** in
`run-rh_pczdj`; sie werden nicht als finale Fast-Path-Prüfung ausgegeben.

Das finale Intel-macOS-SDK `Transform SDK ä mac 822s18g_` besteht seine
Manifest-/Bytehash-Prüfung und das unabhängig entpackte Verification-Kit.
Der gezielte `--transform-only`-Prüfer verschiebt das Paket und prüft die
1288 C-/100 Physim-Gegenproben sowie atomare C-Fehler mit installiertem und
allein aus Paketquellen neu gebautem Core. Receipt:
`build/transform-sdk-mac-PASSED.json`. Kein Repository-Implementierungscode
wird zum Neubau herangezogen; der gezielte Modus ist keine vollständige
SDK-Domänen- oder Grafikabnahme.


Die finale beschleunigte Debian/GCC-Release-Numerik besteht **10/10** in
`run-3_5kpyom` (`build/transform-expansion-release-linux.log`); ihre tatsächlichen
Grafik-/Picking-/Hierarchie-/Szenenprüfungen bestehen **4/4** in `run-o36d9nbr`
(`build/transform-expansion-gui-linux.log`). Damit liegen für den finalen Pfad
gezielte Release-Numerik-/Grafiknachweise auf beiden Plattformen und die
Linux-ASan-/UBSan-Gegenprobe vor. Keine vollständige neue Core-/Windows-/
Apple-Silicon-Abnahme wird daraus abgeleitet.


Das finale Debian/GCC-SDK `Transform SDK ä linux r2044mtc` besteht den gleichen
gezielten Paketnachweis: **453 SDK-Dateien**, **104 unabhängige Kit-Dateien**,
verschobene Paketkopie sowie öffentliche C-/Physim-Gegenproben mit installiertem
und allein aus Paketquellen neu gebautem Core. Alle drei äußeren Schritte
(Install, Kit, Verify) und die vier jeweiligen C-/Orakel-Ausführungen bestehen.
Receipts: `build/transform-sdk-linux-PASSED.json` und
`build/transform-sdk-mac-PASSED.json`. Beide passen zu den 19 unveränderten Dateien
in `build/transform-source-freeze.json`; diese fortgeschriebene Prüfchronik ist
bewusst ausgenommen. Die Quellen und Originaltexte aller 531 Planblöcke bleiben
erhalten. PP-0357 ist begrenzt implementiert; der Gesamtplan bleibt offen.

## Opake Run-Streams am 8. Oktober 2026

`run_stream.h` ergänzt einen explizit allozierten opaken Store mit jeweils acht
Reader-/Writer-Slots und Besitzer-/Slot-/Generationsprüfung. Schließen invalidiert
alle Handlekopien; eine erschöpfte Generation wird stillgelegt. Fehler erhalten
Reader-Ausgaben, bekannte Fehlerläufe lassen sich ohne Erfolgsfooter abbrechen.
Experiment-Runner und Run-Importvalidierung verwenden die Schnittstelle tatsächlich.
Die alte `data.h`-API und weitere öffentliche Ressourcenzustände bleiben bestehen;
PP-0352 ist deshalb weiterhin unvollständig. [Verträge](run-streams.md).

Intel macOS 14.6.1/Apple Clang 16 mit SDL 3.2.30 besteht die 20 ausgewählten
Release-Integrationsfälle einschließlich C-/Physim-Runner, Run-Import, Snapshots,
Core-Katalog und nativem Projektbuild unter
`build/contact-world-language-release-mac/test-results/run-drpye0_5`.
Die zusätzliche Generationsgrenze und öffentliche API bestehen zusammen mit der
Referenzprüfung unter `run-q_t_abw5` (drei Fälle). Der abschließende Test der
formatierten Grenzfixture und des unabhängigen Prüfkits besteht unter
`run-3qh4h8f7` (zwei Fälle). Der gemeinsame Katalog umfasst jetzt 28 Core-Module.

Debian 12/GCC 12.2 besteht die entsprechende Release-Auswahl einschließlich der
Generationsgrenze mit 21/21 Fällen unter
`build/contact-world-language-release-linux/test-results/run-1dszgzhw`.
Clang 14 mit ASan/UBSan besteht 20/20 ausgewählte API-, Import-, Snapshot-,
Runner-, Katalog- und Referenzprüfungen unter
`build/atspi-asan-linux/test-results/run-v95w1uph`; der längere native Projektbuild
ist dort nicht Teil der Auswahl. `ASAN_OPTIONS=detect_leaks=0` deaktiviert nur
Leak-Prüfung, nicht Address-/UndefinedBehavior-Prüfungen. macOS-Sanitizer sind
weiterhin wegen des fehlenden `ld64.lld` nicht nachgewiesen.

Analyseprojekte mit Run-Import, gespeicherter Run-Lernpfad und Handbuch-Tastatur-
führung bestehen jeweils 3/3 auf macOS unter `run-isqgpkfk` und auf Linux/X11
unter `run-obxfnhkn` in den jeweiligen Release-Testverzeichnissen. Der erste
Linux-GUI-Aufruf ohne `DISPLAY` scheitert mit „No available video device“ unter
`run-uoizmvbz` (0/3); er wird nicht als bestanden gewertet. Der nachfolgende
Aufruf verwendet den tatsächlich laufenden Xvfb-/Openbox-Desktop mit `DISPLAY=:99`.

Die zusätzlich ausgeführte Selbstprüfung des nativen Test-Runners deckt zwei
bestehende Katalogannahmen auf: macOS-Accessibility-Unitfälle besitzen keinen
Integrationseintrag, der Linux-AT-SPI-Fall verwendet einen eigenen Client statt
des Workflow-Wrappers. Die Zuordnung ist korrigiert; sämtliche erwarteten Fehler,
Timeouts, Start-/Buildfehler und vollständigen Ergebnisberichte bestehen danach
auf beiden Plattformen (`build/run-stream-test-runner-final-mac.log` und
`build/run-stream-test-runner.log` auf Linux). Die Prüfkiterstellung bestätigt
105 unabhängige Eingaben mit SHA-256, Paketgrenzen und exklusiver Veröffentlichung.

Das macOS-SDK enthält 457 manifestierte Dateien einschließlich öffentlichem
Stream-Header und Stream-Implementierung. Die fokussierte `--stream-only`-Prüfung
verschiebt es nach `build/run-stream-sdk-checks/Native SDK ä j0z8_tve` und baut
Core ausschließlich aus dessen Quellen neu. Derselbe öffentliche API-Test besteht
gegen installierte und neu gebaute Bibliothek. Ownership, alte/fremde/gefälschte
Handles, Slotgrenzen, Allocatorfehler, atomare Reads, Snapshots, Release und
recoverable Abort sind enthalten. Dieser Nachweis ersetzt keine vollständige
SDK-Domänenabnahme; direkte Low-Level-Physim-Bindungen sind nicht ergänzt.

Das Linux-SDK besteht denselben Test mit 457 manifestierten Dateien unter
`build/run-stream-sdk-checks/Native SDK ä 8zn5r21_`. GCC warnt dabei in der
Legacy-Kompatibilitätsgegenprobe wegen eines Zweierarrays am alten Parameter
`double values[PS_MAX_CHANNELS]`. Die Testfixture verwendet abschließend ein
Array in der dort deklarierten Größe; die Kapazitäts-/Fehlerprüfungen der neuen
API bleiben bestehen. Der endgültige öffentliche Test besteht unter macOS in
`run-728edbkz` und unter Linux-ASan/UBSan in `run-g800s406` (je 1/1).
Beide verschobenen SDKs mit installierter und aus Paketquellen gebauter Bibliothek
bestehen danach erneut: macOS `Native SDK ä b9b9zvd3`, Linux `Native SDK ä kfyip904`.
Das endgültige unabhängige Kit besitzt weiterhin 105 exakt manifestierte Dateien
und besteht den Paketgrenzentest auf beiden Plattformen.

Die geänderten Quellen sind zwischen macOS und Linux per SHA-256 abgeglichen
(`build/run-stream-source-freeze.json`). Laufdaten, SDK-Kopien und Prüfberichte
bleiben ignorierte Nachweise im Build-Verzeichnis. Aktuelle Windows-, Apple-
Silicon-, Wayland- und praktische Screenreader-Nachweise entstehen durch diese
fokussierten Läufe nicht.

## Exakte Toleranzvergleiche am 8. Oktober 2026

`ps_close` und Physim `isClose` entscheiden die symmetrische absolute/relative
Bedingung für exakte binäre Eingabewerte. Ein konservatives Fehlerintervall
beschleunigt eindeutige Fälle; die vorhandenen begrenzten Integer-Hilfen erhalten
kritische Differenzen, Produkte und Summen ohne Heapallokation. Beide Toleranzen
null verlangen exakte Gleichheit; nichtendliche Werte/Toleranzen und negative
Toleranzen ergeben false. [Einheiten und Grenzvertrag](math.md).

Die unabhängige Fraction-Gegenprobe findet beim bisherigen, im Run-Stream-SDK
aufbewahrten Core 1751 abweichende Grenzentscheidungen in 9176 Fällen
(`build/close-baseline-final.log`). Darunter sind 1 gegen den negativen kleinsten
Subnormalwert bei absoluter Toleranz 1 und entgegengesetzte `DBL_MAX`-Werte
knapp unter der kombinierten Toleranzgrenze. Die endgültige Probe prüft jede
C-Eingabe in beiden Vergleichsrichtungen; die Sprachfixture umfasst 263 aus
binären Mantissen/Exponenten exakt rekonstruierbare Eingaben. Referenzen werden
mit Python Fraction unabhängig von der Produktionsimplementierung berechnet.

Intel macOS 14.6.1/Apple Clang 16 besteht sechs ausgewählte Release-Fälle unter
`build/contact-world-language-release-mac/test-results/run-13xdmja8`:
Math, Sprachchecker, Referenzdokumentation sowie skalare Suche, Transformationen
und Zahlenvergleich mit unabhängigen Orakeln. Debian 12/Clang 14 besteht dieselbe
Auswahl unter ASan/UBSan in
`build/atspi-asan-linux/test-results/run-nuynpn3v` (6/6).
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UndefinedBehavior-Prüfungen aktiv;
macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` unbestätigt.

Das macOS-SDK besitzt 457 manifestierte Dateien. Die fokussierte Prüfung
`--comparison-only` verschiebt es nach
`build/close-sdk-checks/Native SDK ä 5gngu1fr`, prüft dessen Manifest und baut
Core ausschließlich aus Paketquellen neu. Der Paketcompiler übersetzt die
263 Physim-Eingaben. C-/Physim-Programme bestehen gegen installierte und neu
gebaute Bibliothek dieselbe rationale Gegenprobe. Das unabhängige Prüfkit
umfasst 108 Dateien und besteht Paketgrenzen, SHA-256, fehlende/dynamische
Eingaben und exklusives Veröffentlichen.

Debian 12/GCC 12.2 besteht dieselben sechs Release-Fälle unter
`build/contact-world-language-release-linux/test-results/run-mx8_srnb`.
Die großen bestehenden Such-/Transformations-Sprachfixtures benötigen längere
Optimierung; GCC meldet ein erschöpftes Variablen-Tracking-Budget und kompiliert
diese Dateien anschließend ohne dieses optionale Debug-Tracking. Der tatsächlich
abgeschlossene Gesamtlauf besteht inklusive der neuen Vergleichsfixture (6/6).

Das Linux-SDK enthält ebenfalls 457 manifestierte Dateien. Die verschobene
Prüfung unter `build/close-sdk-checks/Native SDK ä 5x7emncm` besteht dieselbe
C-/Physim-Gegenprobe gegen installierten und ausschließlich aus Paketquellen
neu gebauten Core. Alle 18 geänderten Dateien stimmen zwischen macOS und Linux
per SHA-256 überein (`build/close-source-freeze.json`). Die endgültige
Prüfkiterstellung mit 108 Dateien besteht auf beiden Plattformen.

PP-0358 ist mit diesen begrenzten Nachweisen als implementiert erfasst. Die
Originaltexte und Umfänge aller 531 Planblöcke bleiben erhalten; PP-0365 und die
vollständige Algorithmus-/Produkt-/Plattformabnahme bleiben offen. Die Nachweise
enthalten keine aktuelle Windows-, Apple-Silicon- oder macOS-Sanitizer-Abnahme.

## Kurvenauswertung am 8. Oktober 2026

Die unabhängige rationale Kurvenprobe findet in der bisherigen Core-Bibliothek
627 abweichende Fälle bei 956 Eingaben (`build/curve-baseline.log`). Besonders
kleine Tangenten zwischen benachbarten großen Kontrollpunkten gehen in gerundeten
De-Casteljau-Zwischenstufen verloren; bei Subnormalzahlen kann das Vorzeichen kippen.
Position, analytische Tangente und Unterteilungskontrollpunkte verwenden jetzt
exakte binäre Polynome und jeweils eine abschließende nearest-even-Rundung.
Begrenzte Integerarrays decken vier Double-Faktoren samt kleinen Koeffizienten
ab; Heapallokation und breitere Fließkommapräzision sind nicht erforderlich.
[Einheiten und Kurvenvertrag](math.md).

956 C-Fälle vergleichen Position, Tangente und alle acht Unterteilungskontrollpunkte
mit einer unabhängig implementierten rationalen De-Casteljau-Referenz. Die
77 Physim-Fälle prüfen denselben Core und kopierte `controlPoint(index)`-Werte;
negative/zu große Indizes werden abgefangen. Tangentenüberlauf erhält die gesamte
C-Ausgabe; die Unterteilung kann trotzdem gelingen. Bestehende Kurvenprüfungen
sichern Aliasierung, Parameterfehler und die analytische Kurve `(t,t²,t³)`.

Intel macOS 14.6.1/Apple Clang 16 besteht alle zehn ausgewählten Release-Fälle
unter `build/contact-world-language-release-mac/test-results/run-86mgjon4`.
Debian 12/Clang 14 besteht dieselben zehn Fälle mit ASan/UBSan unter
`build/atspi-asan-linux/test-results/run-dosv5bmb`. Die Auswahl enthält Math,
lineares Resampling, PCHIP, dessen Physim-Analyse, vier bestehende native Physim-
Bézierfälle, Referenzdokumentation und das neue rationale Orakel.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UndefinedBehavior-Prüfungen aktiv;
macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Die zusätzliche kleine C-Laufzeitprobe verwendet dieselbe vierpunktige Kurve
bei 10000 Parametern. Alte Auswertung: 0,000976–0,001257 CPU-Sekunden;
neue exakte Auswertung: 0,033960 in einem ersten und 0,042365–0,042924 Sekunden
in drei weiteren Läufen (`build/curve-benchmark-proof.json`). Das entspricht
rund 3,4–4,3 µs pro Auswertung; 33 Punkte rechnerisch etwa 0,11–0,14 ms.
Dies ist ein konkreter Genauigkeits-/Laufzeittradeoff eines einzelnen Kernels,
kein Nachweis allgemeiner Renderer- oder Simulationsleistung.

Zwei ältere abgeschlossene SDK-Prüfstände sind zur Platzgewinnung vollständig
archiviert: `AT-SPI SDK ä linux chlu_b3p` und `Accessibility SDK ä linux od5pk550`.
`build/curve-old-archived-proofs.tar.gz` enthält 948 geprüfte Dateien/Links,
63833110 Bytes, SHA-256
`0520c56ffe7aeca4a6caca7b0cfc44b42f9455cd325679a4580299654738ad61`.
Mac- und Linux-Kopie sind vollständig gegen das Inhaltsinventar geprüft;
Originale wurden vor Entfernen erneut gehasht. Inventar und Löschbeleg liegen
unter `build/curve-old-archived-proof-inventory.json` beziehungsweise
`build/curve-old-archive-removed.json`. Die jüngsten Scalar-/Transform-/Run-Stream-
und Vergleichsnachweise bleiben separat vorhanden.

Die endgültige Gegenprobe vergleicht Double-Bits einschließlich signierter Null
an kopierten Endpunkten. Der erste Fixture-Generator hatte negative Null als
positive Null ausgegeben; die Literale sind korrigiert. Der alte Core weicht in
der verschärften Prüfung bei 629 Fällen ab (`build/curve-baseline-bits.log`),
davon zwei zusätzliche Unterschiede der Nullvorzeichen gegenüber der ersten
rein numerischen Gegenprobe.

Der während dieser Testverschärfung bereits laufende erste GCC-Release-Aufruf
endet unter `run-p194e12o` mit 9/10: sein übersetztes Programm enthält noch die
alten Null-Literale und scheitert am neuen Bitvergleich. Dieser Lauf wird nicht
nachträglich als bestanden gewertet. Das sehr große ursprüngliche Main benötigt
zudem mehrere Minuten Variablen-Tracking/Optimierung. Die Fixture erzeugt nun
für jeden der unverändert 77 Fälle eine eigene Funktion und ruft alle in derselben
Reihenfolge auf; Referenzwerte und Abdeckung bleiben erhalten.

Die endgültige Fixture besteht auf macOS unter `run-jfdbgack` und auf Linux mit
ASan/UBSan unter `run-p6o3rk9h` (je 1/1 Orakel). Die gesamte endgültige
Linux-Release-Auswahl besteht mit 10/10 unter
`build/contact-world-language-release-linux/test-results/run-emecok__`.
Die jeweilige ältere Zehnerauswahl und die endgültige Bitprüfung sind getrennte
Nachweise; Änderungen an der Fixture werden nicht früheren Programmen zugerechnet.

Das macOS-SDK enthält 458 manifestierte Dateien einschließlich `curve_numeric.h`.
Die endgültige fokussierte Abnahme verschiebt es nach
`build/curve-sdk-checks/Native SDK ä cwuq2v11` und besteht mit installierter sowie
allein aus Paketquellen neu gebauter Bibliothek: 956 C- und 77 Physim-Fälle,
bitgleiche Positionen/Tangenten/Unterteilungskontrollpunkte, Fehlererhaltung,
kopierte Endpunkte und geprüfte Kontrollpunktindizes. Der ausgelieferte Compiler
übersetzt die endgültige Physim-Fixture selbst. Das unabhängige Kit umfasst
111 Dateien und besteht Paketgrenzen, SHA-256 und exklusives Veröffentlichen.

Das Linux-SDK enthält ebenfalls 458 manifestierte Dateien. Die endgültige
verschobene Abnahme unter `build/curve-sdk-checks/Native SDK ä 7mi0ajrh`
besteht dieselbe bitgenaue C-/Physim-Gegenprobe gegen installierten und aus
Paketquellen neu gebauten Core. Alle 20 geänderten Dateien stimmen zwischen
macOS und Linux per SHA-256 überein (`build/curve-source-freeze.json`).
Die Quellen, Tests, Referenzen und SDK-Hilfen werden gemeinsam versioniert;
SDK-Kopien, Archive und Laufdaten bleiben ignorierte Build-Nachweise.

PP-0359 ist mit den genannten Interpolations-/Kurvennachweisen implementiert.
Der gesamte Projektplan, die allgemeine Algorithmusabnahme PP-0365 und weitere
Plattformen bleiben offen. Die fokussierten Läufe beweisen keine aktuelle
Windows-/Apple-Silicon-, allgemeine GUI- oder macOS-Sanitizer-Abnahme.

## Einheitenkonvertierung am 8. Oktober 2026

Die bisherige Konvertierung verändert bei 220 von 2000 Identitätsfällen den
Eingabewert um ein Bit. Die breitere unabhängige Fraction-Gegenprobe findet
540 abweichende Fälle bei 8214 Eingaben (`build/unit-baseline.log`). Core und
Quantity konvertieren nun das exakte binäre Verhältnis mit einer abschließenden
Rundung; gleiche Skalen erhalten jedes Bit, einschließlich signierter Null.
Konvertierung in SI beziehungsweise aus SI verwendet eine einzelne
Multiplikation/Division, allgemeine Verhältnisse begrenzte exakte Hilfen.
[SI-Grenzen und Verträge](units.md).

8214 C-Fälle und 264 Physim-Fälle prüfen Identitäten, Exponentengrenzen, direkte
SI-Pfade, Dimensions-/Bereichsfehler, Aliasierung und unveränderte Fehlerausgaben.
Der separate Host-Trap-Test prüft sieben SI-Basisvektoren, benannte abgeleitete
Dimensionen, Formatierung und ursprüngliche Invalid-/Numeric-Codes samt
strukturierten Diagnosen und Quellposition. Unit-/Quantity-Wrapper bewahren jetzt
die tatsächlichen C-Codes, statt numerische Fehler als ungültige Eingaben zu melden.

Intel macOS 14.6.1/Apple Clang 16 besteht 15 ausgewählte Release-Fälle unter
`build/contact-world-language-release-mac/test-results/run-dgl3cubi`.
Die erweiterte Definitions-/Formatierungsgegenprobe besteht separat unter
`run-rdv_oss6` (1/1). Berichte, SI-Reihen, CSV-Export, Anzeigeeinheiten,
Units-/Parameter-Sprachprüfungen bestehen zusätzlich mit 11/11 unter `run-914vc28f`.
Der erste neue Orakelaufruf scheitert unter `run-6yegrvh9` mit 14/15, weil die
Fixture einen internen globalen Builtin-Namen statt der veröffentlichten
`Unit.convert`-Methode verwendet. Die Fixture und Anleitung sind korrigiert;
der fehlgeschlagene Lauf wird nicht als bestanden gewertet.

Debian 12/Clang 14 besteht dieselbe Fünfzehnerauswahl mit ASan/UBSan unter
`build/atspi-asan-linux/test-results/run-mut9n60w`.
Debian 12/GCC 12.2 besteht die erweiterte Release-Auswahl mit 23/23 unter
`build/contact-world-language-release-linux/test-results/run-qjnqrlge`.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UndefinedBehavior-Prüfungen aktiv.
macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Anzeigeeinheiten-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit
2/2 unter `run-8rzs0e8e`. Die neue SI-Anleitung und beide C-Referenzseiten sind
im Offline-Handbuch erreichbar; alle 31 generierten Referenzdokumente sind geprüft.
Die unabhängige Prüfkiterstellung umfasst 115 Dateien mit SHA-256, Grenzen und
exklusiver Veröffentlichung.

Die älteren abgeschlossenen SDK-Prüfstände `CI regression SDK ä linux h05lz9j2`
und `Scalar search SDK ä linux pe_dtue1` sind vollständig archiviert:
`build/units-old-archived-proofs.tar.gz`, 96393124 Bytes, 1592 Dateien/Links,
SHA-256 `0c837d63f9cf9cdceebeace9fe75288f0ab6d02559de3a77006f24971b0e8c93`.
Mac- und Linux-Kopie wurden gegen das Inhaltsinventar vollständig geprüft;
Originale vor dem Entfernen erneut gehasht. Inventar, Prüf- und Löschbelege
liegen unter `build/units-old-*`. Jüngere Transform-, Run-Stream-, Vergleichs-
und Kurvennachweise bleiben separat vorhanden.

Die beiden betroffenen GUI-Fälle bestehen auch unter Linux/X11 mit 2/2 unter
`build/contact-world-language-release-linux/test-results/run-d3qdbc80`.
Beide SDKs besitzen 459 manifestierte Dateien einschließlich der SI-Anleitung.
Die verschobene fokussierte Abnahme besteht auf macOS unter
`build/unit-sdk-checks/Native SDK ä 55xqlecr` und auf Linux unter
`build/unit-sdk-checks/Native SDK ä d5p64sin`. Der Paketcompiler übersetzt die
264 Physim-Fälle. Derselbe rationale Probe und Host-Trap-Test bestehen gegen
installierte und ausschließlich aus Paketquellen neu gebaute Bibliotheken.

Der erste Linux-SDK-Aufruf unter `Native SDK ä z9sd1tyk` scheitert beim Schreiben
der temporären Compiler-Assembly mit „No space left on device“
(`build/unit-sdk-verify-linux.log`). Er bleibt ein fehlgeschlagener Nachweis.
Weitere vollständig archivierte Prüfstände schaffen Platz für die anschließende
bestandene Abnahme:

- `Transform SDK ä linux r2044mtc`: `build/units-space-archived-proofs.tar.gz`,
  70562996 Bytes, 1139 Dateien/Links,
  SHA-256 `01093cd2336a9cba527ae52210c0b441a38c169173246dd9b9c1c0b971793480`.
- `run-stream-sdk-checks`, `close-sdk-checks`, `curve-sdk-checks` mit allen
  vier bestandenen SDK-Unterverzeichnissen: `build/units-recent-archived-proofs.tar.gz`,
  144444440 Bytes, 2305 Dateien/Links,
  SHA-256 `0a066cc47d494736a4b97c7ad1f944896c1e8e37042d16ba5998a0d4e82f617e`.

Jeder Bestand wurde vollständig auf macOS gegen sein Inhaltsinventar geprüft;
Originale vor Entfernen erneut gehasht. Weil zunächst selbst der Platz für eine
zweite Archivkopie fehlte, erfolgte das Entfernen nach der verifizierten Mac-
Sicherung. Danach wurden beide Archive zurück auf Linux kopiert und dort erneut
vollständig geprüft. Aktuelle Inventare und Lösch-/Prüfbelege liegen unter
`build/units-space-*` und `build/units-recent-*`; die früher genannten Pfade
sind daraus wiederherstellbar. Es wurden keine Quell-/Test-/Asset-Dateien entfernt.

Die 21 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/units-source-freeze.json`). PP-0369 und PP-0373 sind mit den genannten
Nachweisen implementiert; vollständige SI-/Metadatenabnahme aller Datenpfade,
Windows/Apple Silicon, Kalibrierung, affine Temperaturskalen und gesamte
Produktabnahme werden daraus nicht abgeleitet.

## Quantity-Werte an der SI-Kanalgrenze am 8. Oktober 2026

`ps_channel_sample_quantity` prüft Kontextprefix, Index, Schema und Dimensionen,
konvertiert die Eingabeskala nach SI und aktualisiert erst danach genau einen
Kanalwert. Jeder Fehler erhält den gesamten Kontext. Physim
`Channel.sampleQuantity` ergänzt Besitzer-/Samplingphasenprüfung und erhält die
ursprünglichen Invalid-/Numeric-/Version-Codes. Der rohe Sampler verlangt
weiterhin bereits in SI vorliegende Zahlen; deren Dimension kann er nicht erraten.
[SI-Grenze](units.md), [Messwert und Unsicherheit veröffentlichen](measurement.md).

Die echte C-/Physim-Runner-Gegenprobe liest einen Zentimetersensor samt
Standardunsicherheit und persistiert 1,25 m sowie 0,01 m. Ein unabhängiger Parser
prüft Run-Header, sämtliche CRCs, Schema, Zeilen, Footer und CSV-Einheiten/Werte.
Abgefangene Dimensions-/Bereichsfehler ändern keine zuvor gespeicherte Länge.
Der separate C-Test prüft ganze Kontexte, verkürzte Prefixe, malformed Schema,
Indexgrenzen, signierte Null, Sensor-/Unsicherheitsübergang, falsche Besitzer,
Samplingphase und strukturierte Diagnosecodes samt Quellposition.

Die Sensor-Wurfbeispiele verwenden den typisierten Weg für Messwert und
Unsicherheit tatsächlich. Der C-Wurf sammelt Werte in einem lokalen Kontext und
committet sie zusammen mit dem Modellzustand. Die Physim-Würfe erhalten separate
Statuskanäle und konvertieren gültige Szenenpositionen ausdrücklich in Meter.
Platzhalter werden durch den Quantity-Setter nicht zu gültigen Messungen.

Intel macOS 14.6.1/Apple Clang 16 besteht die endgültige native Auswahl mit 12/12
unter `build/contact-world-language-release-mac/test-results/run-ds5n4hbw`.
Debian 12/GCC 12.2 besteht dieselbe Auswahl mit 12/12 unter
`build/contact-world-language-release-linux/test-results/run-kn0htfu5`.
Clang 14/ASan/UBSan besteht 12/12 unter
`build/atspi-asan-linux/test-results/run-srg3k2uq`. Die Auswahl enthält C- und
Physim-Kanalläufe, Measurement, Sensor-Nativfälle, Sensor-Wurfparität und
Referenzdokumentation. `ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen
aktiv; macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Sensor-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-d6hx8l5h` und auf Linux/X11 mit 2/2 unter `run-g625xyws` in den jeweiligen
Release-Testverzeichnissen. Alle 31 generierten Referenzdokumente sind geprüft.
Die unabhängige Prüfkiterstellung enthält 116 genau manifestierte Eingaben und
besteht Paketgrenzen, SHA-256, fehlende/dynamische Pfade und exklusive Erstellung.

Der ältere vollständig bestandene SDK-Prüfstand
`build/ui-size-sdk-proof-linux/Native SDK ä 91etu6ft` ist vollständig archiviert:
`build/channel-space-archived-proofs.tar.gz`, 305565157 Bytes, 14467 Dateien/Links,
SHA-256 `2b1dc34c0a197032c2e5934ba7d5a71ecb79137c2ddf2c5faf07745e7d2850e4`.
Inhalt und Links wurden auf macOS vollständig gegen das Inventar geprüft;
Originale vor Entfernen erneut gehasht. Danach wurde das Archiv zurück auf Linux
kopiert und dort vollständig nachgeprüft. Inventar und Lösch-/Prüfbelege liegen
unter `build/channel-space-*`; der alte Prüfstand bleibt wiederherstellbar.
Dadurch stehen wieder rund zwei GiB für tatsächliche Linux-Builds zur Verfügung.

Das Linux-SDK besteht die erweiterte `--units-only`-Abnahme unter
`build/channel-quantity-sdk-checks/Native SDK ä 7jqcwary`: installierter und
allein aus Paketquellen neu gebauter Core, Quantity-Sampler, ursprüngliche
Diagnosecodes, exakte Konvertierung sowie echte C-/Physim-Runner und CSV-Export.
Der Paketcompiler übersetzt die unabhängigen Physim-Fixtures selbst.

Die erweiterte macOS-SDK-Abnahme besteht unter
`build/channel-quantity-sdk-checks/Native SDK ä x8hnbi_r` gegen installierten und
nur aus Paketquellen neu gebauten Core, einschließlich echter C-/Physim-
Sensorläufe und CSV-Gegenprobe. Beide SDKs besitzen weiterhin 459 manifestierte
Dateien; der neue öffentliche Sampler benötigt keinen ABI-Strukturumbau.
Alle 24 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/channel-quantity-source-freeze.json`).

Die umfassenden SI-/Metadatenanforderungen PP-0370/0371/0372/0375 bleiben für
alle Datenpfade einzeln offen. Der neue typisierte Übergang beweist die genannten
konkreten Pfade; er macht aus rohen Doubles keine dimensionsgeprüften Werte.
Weitere Plattformen, allgemeine Produkt-/Modellabnahme und macOS-Sanitizer sind
mit diesen fokussierten Nachweisen nicht abgenommen.

## Starrkörperträgheit und Energiebereich am 8. Oktober 2026

Die alten Formeln liefern in 253 von 1046 unabhängigen rationalen Fällen andere
Rundungen oder Bereichsentscheidungen (`build/body-range-baseline.log`). Konkret
scheitert eine Box mit Masse `1e-300` kg und Kanten `1e200` m am vorzeitig
überlaufenden Quadrat, obwohl ihre Trägheiten endlich sind. Bei einer Kugel mit
kleinster positiver Double-Masse und Radius `1e160` m rundet der alte Vorfaktor
zu null. Die neue positive Integerakkumulation rundet die rationalen
Kugel-/Boxformeln einmal. Identitätsenergie nutzt dieselbe Summation;
allgemeine Quaternionrotation bleibt eine skalierte Binary64-Näherung.
[Vertrag und Grenzen](mechanics.md).

Die unabhängige Prüfung enthält 1046 rationale C-Fälle, 48 Physim-Fälle und
260 gedrehte Körper gegen eine 120-stellige Decimal-Rotationsmatrix. Für die
allgemeine Rotation gilt in diesen Referenzen relative Toleranz `2e-14` und
absolute Toleranz von zwei kleinsten Subnormalwerten, keine Bitgleichheitszusage.
Körper und Energieausgaben bleiben bei Fehlern unverändert. Die erste Sprach-
Fixture wurde wegen einer verbotenen Mutation eines erfassten Werts abgewiesen
(`run-__uaslei`); korrigierte Funktionen verwenden eigene lokale Körperkopien.
Der korrigierte Mac-Vorlauf besteht 4/4 (`run-kwelpa5e`), der zusätzliche
Rotationslauf 1/1 (`run-rbobnntl`).

Die erweiterte native Auswahl besteht auf Intel macOS 14.6.1/Apple Clang 16 mit
26/26 unter `build/contact-world-language-release-mac/test-results/run-3v6rc0qy`
und auf Debian 12/GCC 12.2 mit 26/26 unter
`build/contact-world-language-release-linux/test-results/run-vwkp9oqa`.
Sie umfasst Mechanik, Box-/Paar-/Graphkontakte, Gelenke, Kontaktverwaltung,
Broad Phase, C-/Physim-Stoßparität, echte Stoß-/Boxläufe und Prüfkiterstellung.
Linux/Clang 14 besteht dieselbe Auswahl mit zusätzlicher Kugel-Sweep-Prüfung
unter ASan/UBSan mit 27/27 in
`build/atspi-asan-linux/test-results/run-k9oardvp`.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft.

Die tatsächlichen Physim-App-Workflows für Kugel- und Boxstöße bestehen auf
macOS mit 2/2 (`run-ab687ilq`) und Linux/X11 mit 2/2 (`run-h5p7p25i`) in den
jeweiligen Release-Testverzeichnissen. Alle 31 Referenzdokumente sind geprüft.
Die unabhängige Prüfkiterstellung besteht mit 119 genau manifestierten Eingaben,
SHA-256, Paketgrenzen, fehlenden/dynamischen Pfaden und exklusiver Erstellung.
Ein parallel zum GUI-Build gestarteter SDK-Build wurde durch die Build-Sperre
korrekt zurückgewiesen; erst nach Abschluss dieses Builds wurde er gestartet.

PP-0382 bleibt für umfassende Lage-/Winkelgeschwindigkeits-/Dynamikabnahme offen.
PP-0385 besitzt eine konkrete Lücke: Allgemeine konvexe Körper fehlen im
Narrow-Phase-Vertrag. Keine vollständige Mechanik-, Windows-, Apple-Silicon-
oder gesamte Produktabnahme wird aus diesen fokussierten Nachweisen abgeleitet.

Das macOS-SDK besteht `--body-only` unter
`build/body-sdk-checks/Native SDK ä jn4ccpv4`, das Linux-SDK unter
`build/body-sdk-checks/Native SDK ä 5nk_v3fm`. Beide enthalten 460 manifestierte
Dateien. Installierter und allein aus SDK-Quellen neu gebauter Core bestehen
jeweils rationale C-/Physim- und Decimal-Rotationsgegenproben. Der Paketcompiler
übersetzt die Sprach-Fixture selbst. Unter Linux läuft der Prüfer ausschließlich
aus `build/body-independent-kit`, ohne Implementierung oder Entwicklerheader im
Prüfkit. Die letzte Belegergänzung dieser Dokumentation erfolgte nach Paketierung;
Implementierung, öffentliche Header und Testeingaben blieben unverändert.
Die 16 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/body-source-freeze.json`); SDK-Belege liegen unter
`build/body-sdk-{mac,linux}-PASSED.json`.

## Konvexe Narrow Phase am 8. Oktober 2026

`ps_convex_mesh` ergänzt geschlossene, nach außen orientierte konvexe
Dreiecksnetze bis zu 64 Vertices und 128 Dreiecken. Paarprüfung benutzt Flächen-
und Kantenkreuzproduktachsen; minimale Trennverschiebung und Oberflächenzeugen
liefern einen repräsentativen gemeinsamen Kontakt. Kugel-/Ebenenkontakte und
konservative Hüllgrenzen ergänzen die API. `Body.withInertia` beziehungsweise
`ps_body_with_inertia` erlauben eigene positive Hauptträgheiten. Die C- und
Physim-Beispiele prüfen einen homogenen regulären Tetraeder; Masseneigenschaften
werden ausdrücklich angegeben, keine Punktwolkenhülle oder Trägheit erraten.
[Netzvertrag, Beispiele und Grenzen](mechanics.md).

Die unabhängige Welt-Vertex-Projektion prüft 1014 Box-/Tetraeder-/Oktaederpaare,
darunter vertauschte Partner. 24 getrennte Paare verlangen Kantenkreuzprodukt-
Trennachsen; Flächennormalen allein reichen dort nicht. Alle 230 erkannten
Kontakte besitzen unabhängig rekonstruierte Zeugen auf beiden ursprünglichen
Oberflächen. Separate analytische Fälle prüfen Kugelkontakte an Flächen/Kanten,
innere und grenzständige Kugelmittelpunkte, Enthaltensein, ein geschlossenes
64-Vertex-/124-Dreieck-Netz, SI-Skalen `1e-200` und `1e200`, offene/konkave/
falsch orientierte Netze, Index-/Kapazitätsgrenzen, Numeric-Fehleratomizität,
konservative AABBs und echte Impulsantwort mit dem bestehenden Solver.

Die ersten Physim-Fixtures benutzten einmal einen nur als Methode verfügbaren
Solvernamen und anschließend einen durch die statische Aabb-Methode verdeckten
Funktionsnamen. Diese Prüferfehler sind unter `run-hc5exynr` und `run-c974ua32`
erhalten; korrigierte Aufrufe verwenden `resolveSingle` und `Aabb.convex`.
Der erste Diagnoseprüfer dereferenzierte nach Entfernen des aktiven Traps noch
das trapabhängige Cleanup-Makro und scheiterte auf beiden Plattformen:
macOS `run-0b9x0n7w`, Linux `run-ndq11iik`, jeweils 22/23. Der Prüfer liest jetzt
den erhaltenen `trap.cleanup` direkt. Ein vorläufiger Feldnamen-Tippfehler wurde
vom Compiler zurückgewiesen (`build/convex-runtime-corrected-mac.log`).
Die fünf endgültigen Trapfälle prüfen Invalid-/Limit-/Numeric-Codes,
strukturierte Diagnosen, Quellpositionen und vollständig abgewickelte Cleanup-
Zustände. Der Debuggernachweis liegt unter `build/convex-runtime-assembly.log`.

Die endgültige native Auswahl besteht auf Intel macOS 14.6.1/Apple Clang 16 mit
23/23 unter `build/contact-world-language-release-mac/test-results/run-ilp0uvf2`
und Debian 12/GCC 12.2 mit 23/23 unter
`build/contact-world-language-release-linux/test-results/run-yelx7jkf`.
Linux/Clang 14 besteht 23/23 mit ASan/UBSan unter
`build/atspi-asan-linux/test-results/run-i3y8hd1q`.
Die Auswahl umfasst neue C-/Physim-Geometrie, Beispiele und Diagnosecodes,
bestehende Mechanik-, Box-/Kontakt-/Graph-/Constraint-, Broad-Phase-/Sweep-
Prüfungen, C-/Physim-Stoßparität, echte Stoßläufe, Energieorakel und Prüfkiterstellung.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft.

Stoß-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-lf9yfvyb` und Linux/X11 mit 2/2 unter `run-4llri2qu` in den jeweiligen
Release-Testverzeichnissen. Alle 31 Referenzdokumente sind geprüft. Das endgültige
unabhängige Prüfkit enthält 124 manifestierte Eingaben; Paketgrenzen, SHA-256,
fehlende/dynamische Pfade und exklusive Erstellung sind geprüft.

PP-0385 ist für die genannten diskreten Geometrieverträge implementiert.
Ein repräsentativer Kontakt ist kein Ruhemanifold; automatische konvexe
Kontaktweltverwaltung, konvexes CCD, aktuelle Windows-/Apple-Silicon- und gesamte
Produktabnahme bleiben gesondert offen. Das Gesamtziel ist nicht erreicht.

Die verschobene SDK-Abnahme `--convex-only` besteht auf macOS unter
`build/convex-sdk-checks/Native SDK ä ply2mr4r` und Linux unter
`build/convex-sdk-checks/Native SDK ä 6hvnic_q`. Beide SDKs enthalten 463
manifestierte Dateien. Installierter und ausschließlich aus Paketquellen neu
gebauter Core bestehen die unabhängigen Geometriegegenproben, Invalid-/Limit-/
Numeric-Diagnosecodes und C-/Physim-Tetraederbeispiele. Der Paketcompiler
übersetzt die Sprachquellen selbst. Die SDK-Sprachbeispiel-Binärdatei läuft
zusätzlich direkt. Unter Linux stammt der Prüfer ausschließlich aus
`build/convex-independent-kit`, ohne Entwicklerheader oder Implementierung
im Prüfkit. Die letzten Paketbelege wurden nach Paketierung ergänzt;
Implementierung, Header und Testeingaben blieben unverändert.
Die 30 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/convex-source-freeze.json`). SDK-Prüfbelege liegen unter
`build/convex-sdk-{mac,linux}-PASSED.json`.

## Lineare konvexe CCD am 8. Oktober 2026

`ps_sweep_convexes` schneidet Eintritts-/Austrittsintervalle sämtlicher
Flächen-/Kantenkreuzproduktachsen bei fester Orientierung. Der Ebenensweep
verwendet den ersten Vertexkontakt. Der Kugel–Netz-Sweep prüft Flächen,
Kantenzylinder und Vertexkugeln. Eine Bewegungshülle vereinigt gepufferte
Anfangs-/Endgrenzen für den gesamten linearen Weg. Displacements bleiben
explizit und unabhängig von gespeicherten Geschwindigkeiten; Fehler erhalten
Körper, Netze, Trefferwert und Flag. Neue Treffer haben Eindringtiefe null,
Anfangsüberlappungen behalten den diskreten Kontakt bei Anteil null.
[Verträge und vollständige Ereignisbeispiele](mechanics.md).

Die C-/Physim-Tetraederbeispiele führen ein kraftfreies elastisches Ereignis
gegen einen statischen Vertex tatsächlich aus: Kugel von `(10,1,1)` mit
`vx=-20` m/s, Kontakt bei Anteil `0.425`, Impulsantwort und anschließende Restzeit
bis `x=13` m mit 200 J kinetischer Energie. Der zentrale Impuls induziert hier
keine Rotation. Das ist ein isoliertes Ereignis, keine Mehrkörpersteuerung.

Die neue unabhängige Gegenprobe enthält 705 Fälle: 500 gedrehte
Box-/Tetraeder-/Oktaeder-Sweeps mit separaten Welt-Vertex-Zeitintervallen,
180 Kugelpfade mit unabhängiger Distanzminimierung und Eintrittsnullstelle
sowie 25 exakt rationale achsenparallele Zeitreferenzen. Darunter liegen
72 Polyeder- und 20 Kugeltreffer. Neue Kontaktpunkte werden auf ursprünglichen
Netzoberflächen zum Ereigniszeitpunkt geprüft, Kugelkontakte zusätzlich auf
der Kugeloberfläche. Anteilvergleich verwendet `2e-9` absolute/relative Toleranz,
Oberflächenzeugen `2e-8` absolute Toleranz in diesen Referenzfällen.

Separate analytische C-/Physim-Fälle prüfen Durchtunneln, beide bewegten Partner,
Streifkontakte, genau einen gemeinsamen Zeitanteil zweier Achsenintervalle,
Intervallende, Anfangsüberlappung auch bei Trennung, lange Wege `1e12` m,
Kugelkontakt an Flächen/Kanten/Vertices, Ebenen, Bewegungshüllen und atomare
Invalid-/Numeric-Fehler. Acht strukturierte Runtime-Traps erhalten die
Invalid-/Limit-/Numeric-Codes, Quellpositionen und vollständig abgewickelte
Cleanup-Zustände. Die erste Orakelversion prüfte auch bei Anfangsüberlappung
fälschlich einen nulltiefen Oberflächenkontakt und wurde unter `run-r09b_1an`
abgewiesen; sie folgt jetzt dem dokumentierten diskreten Überlappungsvertrag.

Intel macOS 14.6.1/Apple Clang 16 besteht 22/22 unter
`build/contact-world-language-release-mac/test-results/run-zfyxprb1`.
Debian 12/GCC 12.2 besteht 22/22 unter
`build/contact-world-language-release-linux/test-results/run-i29eq4e7`.
Clang 14/ASan/UBSan besteht 22/22 unter
`build/atspi-asan-linux/test-results/run-bpqptbnl`.
Die Auswahl enthält neue Sweeps, beide Ereignisbeispiele, Diagnosecodes,
bestehende Mechanik-/Box-/Paar-/Broad-Phase-/Kugel-Sweep-Prüfungen,
C-/Physim-Stoßparität, echte Stoßläufe, 1014 diskrete Geometriefälle,
Dokumentation und Prüfkiterstellung.

Die abschließend verstärkte Kugel-Netz-Zeugenprüfung besteht separat auf
macOS mit 1/1 (`run-z4o3irvx`), Linux/Release mit 1/1 (`run-5xowqt3r`) und
Linux/ASan/UBSan mit 1/1 (`run-un6j2n1m`) in den jeweiligen Testverzeichnissen.
Implementierung und andere Testeingaben blieben seit der Auswahl unverändert.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft. Alle 31 Referenzdokumente sind geprüft;
das unabhängige Prüfkit enthält jetzt 127 genau manifestierte Eingaben.

PP-0387 ist konkret unvollständig: Allgemeine rotierende und beschleunigte
Bahnen sowie Mehrkörper-Ereignissteuerung fehlen. Lineare CCD mit festen
Orientierungen ersetzt diese Anforderungen nicht. Keine allgemeine CCD-,
Windows-/Apple-Silicon- oder gesamte Produktabnahme wird daraus abgeleitet.

Stoß-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-imop6leh` und Linux/X11 mit 2/2 unter `run-4e5sxkkx` in den jeweiligen
Release-Testverzeichnissen. Die Prüfkiterstellung besteht mit 127 Eingaben,
Paketgrenzen, SHA-256, fehlenden/dynamischen Pfaden und exklusiver Erstellung.

Die abschließende Dokumentationsprüfung erkannte einen vorhandenen
Paketgrenzfehler: Durch die fortgeschriebenen Nachweise war diese Seite auf
266081 Bytes gewachsen und überschritt `PS_DOC_MAX_BYTES=256*1024`.
`generate-reference.py --check` wies sie deshalb zurück. Die ersten SDK-
Installationen unter `build/convex-sweep-sdk-{mac,linux}` waren zu diesem
Zeitpunkt gebaut, wurden aber nicht als bestandene Abnahmen ausgegeben.
Die neue SDK-Markdown-Größenprüfung weist das erste Mac-Paket zurück
(`build/convex-sweep-sdk-document-rejection.txt`).

207181 historische Bytes mit SHA-256
`331f4bbddba84bb535ce873161f77cc9720895c8c6dabc12d2e755cf6fe6dcb9`
sind unverändert in [Historische Plattformnachweise](platform-validation-history.md)
übernommen. Die neue Seite besitzt 207237 Bytes einschließlich Rücklink;
die aktuelle Seite liegt wieder deutlich unter der Grenze. Beide sind im
Offline-Handbuch registriert. Verweise aus README, SDK-README und Status
führen zur erhaltenen Historie. Der Abgleich liegt unter
`build/convex-sweep-document-split.json`.

Die Dokumentationsauswahl besteht nach der Aufteilung auf macOS mit 3/3
(`run-dsedhz69`) und Linux mit 3/3 (`run-hfyar3ak`). Die GUI-Nachprüfung nach
der zusätzlichen Offline-Seite besteht auf macOS mit 2/2 (`run-jp4qa776`)
und Linux mit 2/2 (`run-52x7vby5`). Der echte Markdown-Parser prüft inzwischen
zusätzlich beide ausgelieferten Plattformseiten samt Byte-/Blockgrenzen;
diese verstärkte macOS-Auswahl besteht mit 3/3 (`run-aohfjxdw`).

Die verstärkte Parserauswahl besteht auf Linux/Release mit 3/3
(`run-28476klb`) und auf Linux/ASan/UBSan mit 1/1 (`run-36ig_aus`).
Der historische Bericht lädt als 558 echte Markdownblöcke; die aktuelle Seite
besitzt deutlich weniger Blöcke. Beide liegen unter den tatsächlichen
Byte-/Blockgrenzen des ausgelieferten Parsers. Der Textnachtrag dieser Belege
ändert weder Parser noch historische Bytes.

Die korrigierten SDKs `build/convex-sweep-final-sdk-{mac,linux}` besitzen
jeweils 464 manifestierte Dateien. `--convex-only` besteht auf macOS unter
`build/convex-sweep-sdk-checks/Native SDK ä 0sc9q9f9` und Linux unter
`build/convex-sweep-sdk-checks/Native SDK ä rqagd4sc`. Installierter und nur
aus SDK-Quellen neu gebauter Core bestehen beide Geometrieorakel, lineare
Sweeps, Diagnosecodes und die tatsächlichen C-/Physim-Ereignisbeispiele.
Der Paketcompiler übersetzt die Sprachquellen selbst; das installierte
Sprachbeispiel läuft zusätzlich direkt. Unter Linux stammt der Prüfer nur aus
`build/convex-sweep-independent-kit`, ohne Implementierung oder Entwicklerheader
im Prüfkit. Der neue Größencheck prüft die manifestierten Markdownseiten in
allen fokussierten und vollständigen SDK-Modi.

Die 29 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/convex-sweep-source-freeze.json`). SDK-Belege liegen unter
`build/convex-sweep-sdk-{mac,linux}-PASSED.json`. Der letzte Dokumentationsnachtrag
erfolgte nach Paketierung; Programmcode, öffentliche Header und Testeingaben
blieben unverändert. Die historischen 207181 Bytes bleiben bytegleich erhalten.

## Konservative rotierende und quadratische CCD am 8. Oktober 2026

`ps_rigid_motion` erhält vollständige Weltachsenrotationen und quadratische
Translation. `ps_body_motion_pose` kopiert Lage und Orientierung auf dem
expliziten Pfad, ohne Kräfte oder gespeicherte Geschwindigkeiten zu integrieren.
Konvexe Paar-/Kugel-/Ebenen-Anfragen verwenden konservative Abstandsschritte,
Trennachsen und Zeugenrichtungen mit einer oberen Bewegungsschranke. Erfolg
innerhalb der expliziten Distanzhülle liefert einen Kontakt; ein freier Restweg
wird erst nach Ausschluss durch die Schranke gemeldet. `PS_LIMIT` erhält alle
Ausgaben und bedeutet einen offenen Suchfall. Eine nicht auflösbare
Koordinaten-/Rotationsgenauigkeit liefert `PS_NUMERIC` statt eines freien Wegs.
[Pfad, Hülle, Budgets und Grenzen](mechanics.md).

Die unabhängige analytische Gegenprobe enthält 234 Fälle mit 220 Erstkontakten:
60 Größenfamilien jeweils als rotierender Stab gegen Ebene, Kugel und Box;
40 quadratische Wege mit gleicher Anfangs-/Endhöhe und innerem Ebenenkontakt;
zwölf vollständig ausgeschlossene Pfade sowie Budget-/Invalid-Fälle.
Winkel reichen bis zu sechs vollständigen Drehungen. Referenzen lösen die
Sinus-/Kosinus-/Quadratikbedingungen des ersten Kontakts separat und vergleichen
mit der gewählten Distanzhülle, Kontaktpunkt und Normalen. Ein neuer Treffer darf
zwischen der analytischen Hüllgrenze und der idealen Berührung liegen.
Zusätzliche C-Fälle prüfen volle Drehungen mit identischen Endorientierungen,
Posen, konservative Radien/Translationsextrema, 1608 tatsächlich transformierte
Vertices im Bewegungs-AABB, fehlende Auflösung und atomare Budgetfehler.

Physim besitzt `RigidMotion` und `CcdSettings` als kopierbare Werte mit lesbaren,
geschützten Feldern. Strukturen, Arrays, optionale Werte, annotierte Funktionen,
Posen und alle Sweep-/Bounds-Aufrufe laufen tatsächlich. Negative Compilerfälle
verwerfen Feldmutation. 13 Host-Traps prüfen ursprüngliche Invalid-/Limit-/
Numeric-Codes, strukturierte Diagnosen, Quellpositionen und abgewickelte Cleanups.
Die C-/Physim-Tetraederbeispiele prüfen zusätzlich eine volle Drehung mit freien
Endlagen gegen eine Ebene und vergleichen den ersten Anteil mit einer unabhängigen
Winkelreferenz. Die bereits geprüfte lineare elastische Ereignis-/Restzeit bleibt
enthalten; eine automatisch gelöste rotierende Kontaktfolge wird nicht behauptet.

Die erste Sprachfixture scheiterte mit 1/2 unter `run-i6p2tusj`, weil die neuen
Werttypen noch nicht in allen Checker-/Emitter-Grenzen registriert waren.
Die Typgrenzen, Deskriptorerfassung und Größenabschätzung umfassen sie jetzt;
zugleich nutzt die Fixture die bestehende Quaternion-Rotationsmethode.
Die erste Orakelversion benutzte irrtümlich Code 7 für `PS_LIMIT`; die tatsächliche
Enumposition ist 9. Dieser Prüferfehler bleibt unter `run-lw5p8o5d` erhalten.
Die korrigierte analytische Auswahl besteht mit 3/3 (`run-iznldat5`), die
optionale Typprüfung mit 1/1 (`run-j4gkp9e_`).

Intel macOS 14.6.1/Apple Clang 16 besteht die endgültige Auswahl mit 16/16 unter
`build/contact-world-language-release-mac/test-results/run-l9g_yi27`.
Debian 12/GCC 12.2 besteht 16/16 unter
`build/contact-world-language-release-linux/test-results/run-03p9fphg`.
Clang 14/ASan/UBSan besteht 16/16 unter
`build/atspi-asan-linux/test-results/run-1w5wnt0d`.
Die Auswahl enthält neue C-/Physim-Pfade, Compiler-/Wertregressionen,
Diagnosecodes, Tetraederbeispiele, 1014 diskrete Geometriefälle,
705 lineare Sweep-Fälle, Referenzdokumentation und Prüfkiterstellung.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft. Die anschließende Aufteilung
mehrdeutiger einzeiliger Rückgaben ist rein formatierend; Paketprüfungen bauen
und prüfen diese endgültigen Header/Probes erneut.

Stoß-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-gg79ead_` und Linux/X11 mit 2/2 unter `run-r3zytq2y` in den jeweiligen
Release-Testverzeichnissen. Alle 31 Referenzdokumente sind geprüft. Das
unabhängige Prüfkit umfasst jetzt 131 genau manifestierte Eingaben.

Drei bereits vollständig bestandene ältere SDK-Prüfstände sind vollständig
archiviert: `parent-watch-sdk-proof-linux/Native SDK ä m7a3cycd`,
`collision-tutorial-sdk-proof-linux/Native SDK ä 4sht9ghs` und
`monte-carlo-tutorial-sdk-proof-linux/Native SDK ä 9_kllbwq`.
`build/motion-space-archived-proofs.tar.gz` besitzt 559487548 Bytes,
20873 Dateien/Links und SHA-256
`bf58a19bc9bef46775bb7ddaa18a1566421dba8e3d356a992b2460f523e8e4ba`.
Alle Inhalte/Links wurden auf macOS gegen das Inventar geprüft, Originale vor
Entfernen erneut vollständig gehasht. Danach wurde das Archiv zurück nach Linux
kopiert und dort vollständig geprüft. Belege liegen unter `build/motion-space-*`;
die ursprünglichen Pfade bleiben wiederherstellbar. Keine Quellen wurden entfernt.

PP-0387 bleibt unvollständig für allgemeine zeitabhängige Rotations-/Kraftpfade
und Mehrkörper-Ereignissteuerung. Der explizite Pfadvertrag, Distanzhülle und
Work-Budget ersetzen diese Anforderungen nicht. Aktuelle Windows-/Apple-Silicon-
und gesamte Produktabnahme bleiben gesondert offen.

Die abschließende Posenprüfung normiert auch bei Nullrotation gültige,
fast einheitliche Anfangsquaternionen, damit Pose und Bewegungshülle denselben
Rotationsvertrag verwenden. Der zusätzliche C-Fall prüft dies ausdrücklich.
Die drei betroffenen C-/Physim-/Analytikfälle bestehen anschließend auf macOS
mit 3/3 (`run-twvtgor0`), Linux/Release mit 3/3 (`run-0be1rdv_`) und
Linux/ASan/UBSan mit 3/3 (`run-ycufv4hc`). Andere Algorithmen und Tests blieben
gegenüber der erweiterten 16er-Auswahl unverändert.

Das verschobene macOS-SDK besteht unter
`build/motion-sweep-sdk-checks/Native SDK ä s4jc0vbz`, das Linux-SDK unter
`build/motion-sweep-sdk-checks/Native SDK ä ydssmug2`. Beide besitzen 464
manifestierte Dateien. Installierter und ausschließlich aus SDK-Quellen neu
gebauter Core bestehen die drei unabhängigen Geometrie-/Sweep-Orakel,
C-/Physim-Beispiele, typisierte Bewegungs-/Einstellungswerte und erhaltene
Invalid-/Limit-/Numeric-Diagnosen. Der Paketcompiler übersetzt die Quellen selbst;
das installierte Sprachbeispiel läuft zusätzlich direkt. Unter Linux stammt der
Prüfer allein aus `build/motion-sweep-independent-kit`, ohne Entwicklerheader
oder Implementierung im Prüfkit. Die endgültigen Header/Probes besitzen die
formatierend aufgeteilten Rückgaben; die Paketkompilierung prüft diesen Stand.

Die 29 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/motion-sweep-source-freeze.json`). SDK-Belege liegen unter
`build/motion-sweep-sdk-{mac,linux}-PASSED.json`. Dieser letzte Belegnachtrag
erfolgte nach Paketierung; Implementierung, öffentliche Header und Testeingaben
blieben unverändert. Vollständige Projektabnahme bleibt offen.


## Kontinuierlicher Mehrkörperschritt und erneute Review-Regressionen

Der externe Bericht vom 6. Oktober enthält sieben Befunde. Ihre bereits
committierten Korrekturen sind oben im historischen Nachweis zugeordnet; am
aktuellen Stand bestehen Core, Analysegrenzen, Autosave, Textdokumente und
Materialtutorial erneut auf Intel macOS mit 5/5 unter `run-j0hzzzhi`.
Debian/GCC besteht dieselben fünf Fälle zusammen mit sieben neuen CCD-/Paket-
Regressionen mit 12/12 unter `run-o3_2r_rt`. CR-006 verwendet weiterhin die
kontrollierte numerische Ablehnung des überlaufenden unskalierten Moments.
Diese Nachprüfung ändert die dokumentierten Grenzen zu ACLs/xattrs nicht.

`ps_ccd_step` integriert jetzt einen vollständigen Kraft-/Drehmoment-Kick und
ereignisgeteilten Drift für Kugeln, Boxen, statische Ebenen und konvexe Netze.
Nach jeder Kontaktgruppe werden Hüllen und Erstkontakte neu berechnet.
ID-geordnete Graph-Lösungen erfassen nahe gleichzeitige Kontakte; Boxen nutzen
Manifolds. C leiht Netze, Physim besitzt kopierbare Modelle und Ergebnis-Snapshots.
Der vollständige Schritt ist atomisch. Ereignisbudget, ungelöste Suche oder
stagnierende Nullzeitlösung erhalten alle Ausgaben. Das Verfahren erster Ordnung
besitzt einen expliziten Kontaktabstand und Residuen; es beweist keine allgemeine
Kraftpfad-/Gelenk-/Solverkonvergenz. [Vertrag und Beispiele](mechanics.md).

Ein unabhängiges rationales 1D-Orakel prüft 181 Folgen mit 470 elastischen
Ereignissen, Endlagen, Geschwindigkeiten, Impuls und Energie. C-/Physim-Runner
führen tatsächlich zwei Stöße innerhalb einer Sekunde aus; ein separater Parser
prüft elf Kanäle, vollständige Zeit, CRC und Abschluss. Weitere C-Fälle prüfen
simultane Kontaktgruppen, ruhende Kugel/Box unter Gravitation, einen rotierenden
Box-Ebenenstoß, Körper-/Modellreihenfolge und unveränderte Ausgaben bei Budget-,
ID-, Kraft- und Kapazitätsfehlern. Die Besitzprüfung provoziert Allocator- und
Referenzsättigungsfehler und behält unabhängige Ergebnis-Kopien.

Die erste Budgetfixture benutzte unbeabsichtigt vorzeichenlose Anfangspositionen;
`run-79f2_wl0` bleibt als fehlgeschlagener Prüflauf erhalten. Nach expliziter
Double-Konvertierung besteht der Fall. Der erste Referenzlauf `run-l39o9zto`
scheiterte 4/5 am fehlenden deutschen API-Zwecktext; dieser ist ergänzt.
Die Allocatorfixture benutzt nach `setjmp` statischen Zustand und eine volatile
Fehlernummer, damit `longjmp` keine unbestimmten automatischen Werte prüft.
Die erweiterte macOS-Auswahl besteht mit 9/9 unter `run-ieqk8gfq`, Debian/GCC
mit 9/9 unter `run-ug__cg2c`. Alle 31 Referenzdokumente sind geprüft;
das unabhängige Prüfkit umfasst 137 manifestierte Eingaben.

PP-0387 bleibt unvollständig für allgemeine zeitabhängige Kräfte/Drehungen;
der neue Controller schließt den begrenzten Mehrkörper-Ereignisablauf.
Aktuelle Windows-/Apple-Silicon- und vollständige Produktabnahme bleiben offen.

Beim abschließenden Bau aller Sprachbeispiele wurde eine neue Emitterregression
sichtbar: Der neue CCD-Drop-Zweig hatte den bestehenden Batch-Drop-Zweig ersetzt.
Dieser ist wiederhergestellt. Alle Sprachbeispiele und Module werden danach
nativ gebaut; die macOS-Auswahl besteht 10/10 (`run-mq3pp23c`), einschließlich
eines tatsächlichen Batch-Workflows. Die vier Batch-Wert-/Fehlerprogramme bestehen
zusätzlich unter `run-acybds0m`. Debian/GCC besteht 12/12 (`run-ix7kcrse`),
einschließlich dieser Batch-Programme, Checker, Werte, CCD und Paketprüfungen.
Linux/Clang/ASan/UBSan besteht die ursprüngliche 14er-Auswahl unter
`run-gpdahpp9` und die sieben betroffenen CCD-/Batch-/Checkerprüfungen mit dem
endgültigen Emitter/Header unter `run-o3zqb1db`. `detect_leaks=0` bleibt gesetzt;
explizite Allocatorbilanzen prüfen die neue Besitzverwaltung zusätzlich.

Die App besteht Autosave, Physim-Feder mit echtem Editor-Speicherpfad,
Stoß-Workflow und Handbuch-Tastaturführung mit 4/4 auf macOS (`run-tamfgs7m`)
und Linux/X11 (`run-7fevhvbf`). Der erste Linux-Fensteraufruf ohne `DISPLAY`
scheiterte 0/4 unter `run-q4ar2y_l`; der korrigierte Aufruf verwendet tatsächlich
Xvfb/Openbox auf `DISPLAY=:99`. Keine Plattformgleichheit wird aus CI abgeleitet.

Die verschobenen SDKs enthalten jeweils 469 manifestierte Dateien. Der endgültige
Prüfer besteht auf macOS unter
`build/ccd-step-final-sdk-checks/Native SDK ä rj46dc_d` und Linux unter
`build/ccd-step-final-sdk-checks/Native SDK ä ocdsm3to`. Beide prüfen installierten
und allein aus SDK-Quellen neu gebauten Core, alle drei bestehenden unabhängigen
Geometrie-/Sweep-Orakel, das neue rationale Ereignisorakel, C-/Physim-Werte,
Allocatorfehler und beide tatsächlichen CCD-Experimentmodule. Unter Linux stammt
der Prüfer allein aus `build/ccd-step-final-independent-kit` mit 137 Dateien;
Entwicklerheader oder Implementierung gehören nicht zum Prüfkit. Das zusätzliche
CCD-Beispiel ist kein neuer App-Projekttemplate; die bestehende achtteilige
Template-Selbsttestauswahl bleibt getrennt. Die Paketprüfung behauptet keine
vollständige Mechanik-/Produktabnahme.

Belege liegen unter `build/ccd-step-sdk-{mac,linux}-PASSED.json` und
`build/ccd-step-source-freeze.json`. Dieser Nachweisnachtrag erfolgt nach
Paketierung; geprüfte Implementierung, Header und Testeingaben bleiben unverändert.
