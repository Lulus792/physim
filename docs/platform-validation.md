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


## Native Checkboxen und tatsächlicher Einstellungsentwurf

Die native Accessibility-Schicht veröffentlicht nun zweistufige Checkboxen mit
Namen, aktuellen booleschen Werten und genau einer Umschaltaktion. AppKit meldet
`AXCheckBox`, `NSNumber` 0/1 und Wertänderungen. AT-SPI meldet Rolle 7,
`checkable` (41), gegebenenfalls `checked` (4), `toggle`/`Umschalten` und
Checked-Zustandsereignisse. Die AT-SPI-Werte wurden zusätzlich gegen die tatsächlich
installierten PyAT-SPI-Konstanten geprüft. Optisch unbeschriftete Bibliotheks-
Checkboxen erhalten den jeweiligen Laufnamen. Core-API/ABI, Sprachvertrag und
Dateiformate bleiben unverändert. [Verträge und Grenzen](accessibility.md).

Die portable Modellprüfung prüft Wertänderungen unter stabiler Kennung, späte
Aktionen nach dem Zeichnen, Einzelzustellung und gesperrte/veraltete Ziele.
Der tatsächliche AppKit-Prüfer liest Zahlenwerte und Rollen, prüft Press-Aktionen,
Readonly-Selektoren und zurückbehaltene ungültige Elemente. Der gerenderte UI-
Prüfer schaltet beschriftete und optisch unbeschriftete Controls. Der unabhängige
PyAT-SPI-Client prüft zwei reale Fenster, unabhängige Werte, Checkable-/Checked-
Zustände, zwei Richtungen der Wertänderungsereignisse sowie deaktivierte,
verborgene und entfernte Checkboxen.

Ein zusätzlicher Prüfer führt die tatsächliche App aus. Native Aktionen ändern
den Einstellungsentwurf zweimal und erhalten die bereits angewandten
Darstellungsflags. Bestehende Tastaturprüfungen prüfen Speichern und Neustart.
Die ersten Zusatzprüfer scheiterten auf macOS mit 3/4 (`run-bveppbp9`) und
Linux mit 2/3 (`run-99bzum0f`), weil sie eine noch inaktive Dock-Fläche aktivieren
wollten. Deren Snapshot meldete korrekt deaktivierte Controls. Der korrigierte
Prüfer aktiviert zuerst den Einstellungsbereich; Checkbox-Klicks erfolgen danach
weiterhin ausschließlich über die native Schnittstelle. Das Hilfsverzeichnis
für Testeinstellungen wird vor dem Appstart angelegt.

Intel macOS 14.6.1/Apple Clang 16 besteht Modell, Geometrie, Handbuch, Referenzen
und Prüfkit 5/5 (`run-_qbv8k1s`), die abschließenden AppKit-/UI-/App- und
Einstellungstastaturabläufe 4/4 (`run-8aatbj7v`). Debian 12/GCC 12.2 besteht
die fünf Prüfungen ohne Fenster 5/5 (`run-0y02erkz`) und AT-SPI, tatsächliche App
sowie Einstellungstastaturführung 3/3 (`run-pxt0xohr`). Linux/Clang 14 mit
ASan/UBSan besteht dieselben drei Fensterprüfungen 3/3 (`run-xfv89loo`) und
Modell/Geometrie 2/2 (`run-fds_hcl3`). `detect_leaks=0` bleibt gesetzt;
Address-/UB-Prüfungen sind aktiv. macOS-Sanitizer, Windows/UIA, Wayland und
praktische VoiceOver-/Orca-Abnahme bleiben ungeprüft. PP-0710 bleibt unvollständig.


Das verschobene macOS-SDK besteht unter
`build/a11y-checkbox-final-sdk-checks/Native SDK ä _mmkcqy6`, das Linux-SDK unter
`build/a11y-checkbox-final-sdk-checks/Native SDK ä xu7hiise`. Beide enthalten 469
manifestierte Dateien und führen den tatsächlichen nativen Einstellungsablauf aus.
Der endgültige gemeinsame Prüfschritt wird sowohl von `--accessibility-only` als
auch der vollständigen SDK-Prüfung mit `--app-tests` verwendet. Die fokussierten
Nachweise sind keine erneute vollständige SDK-/Produktabnahme. Unter Linux kommt
der Prüfer ausschließlich aus `build/a11y-checkbox-final-independent-kit` mit 138
manifestierten Eingaben, ohne Entwicklerheader oder Physim-Implementierung.
Alle 31 Referenzdokumente sind weiterhin geprüft. Die letzten Handbuch-/Referenz-
und Paketgrenzen bestehen auf macOS mit 3/3 unter `run-h_mcwa4b`.

Quellgleichheit steht in `build/a11y-checkbox-source-freeze.json`, SDK-Belege in
`build/a11y-checkbox-sdk-{mac,linux}-PASSED.json`. Dieser Nachtrag erfolgt nach
Paketierung; geprüfte App-Implementierung und Testeingaben bleiben unverändert.
Der Plan behält alle 531 Originalblöcke; 33 besitzen Implementierungsnachweise,
7 bleiben konkret unvollständig und 491 weiterhin ungeprüft. Native Checkboxen
ersetzen keine vollständige Tastatur-/Fokus-/Screenreader- oder Plattformabnahme.


## Native benannte Optionsgruppen

Darstellung und beide Schriftgrößen besitzen nun echte native Gruppen mit
Eltern-/Kindbeziehungen, Geschwisterindizes und boolescher Auswahl. macOS meldet
`AXRadioGroup`/`AXRadioButton`, Zahlenwerte und ausgewählte Kinder; Werte und
Änderungen der ausgewählten Kinder werden gemeldet, auch wenn die ausgewählte
Option aus dem sichtbaren Snapshot verschwindet. AT-SPI meldet `grouping` (99)
und `radio button` (44), `checkable`/`checked`, Select-Aktionen, hierarchische
Caches, Indizes und Rahmen in Fenster-/Elternkoordinaten. Gruppenknoten zählen
zum bestehenden 256er-Budget. Core-API/ABI, Sprachvertrag und Dateiformate bleiben
unverändert. [Verträge und Grenzen](accessibility.md).

Die portable Gruppenprüfung belegt unterschiedliche Eltern für gleichlautende
Optionen, exklusive/idempotente Auswahl, späte/gesperrte Aktionen, Kapazität,
UTF-8 und einen ausdrücklich späteren Pointerwechsel nach einer nativen Auswahl.
Die AppKit-Prüfung liest Rollen, Eltern, Kinder, ausgewählte Kinder, echte Rahmen
und Zahlenwerte; eine zurückbehaltene, verschwundene Auswahl kann nicht aktiviert
werden. Der UI-Prüfer nutzt tatsächlich gezeichnete Optionen. Der unabhängige
AT-SPI-Client prüft zwei Fenster mit je zwei gleichlautenden Optionsgruppen,
beide Richtungen der Auswahl, unabhängige Gruppen/Fenster, Parent-Koordinaten,
Zustandsereignisse und deaktivierte/verborgene/veraltete Ziele.

Die tatsächliche App führt sechs native Auswahlen aus: Oberfläche 22→16 px,
Darstellung Hell→Dunkel und Editor 20→16 px. Entwürfe ändern sich; angewandte
Konfiguration bleibt erhalten. Speichern und Neustart bleiben im vorhandenen
Einstellungstastaturprüfer abgedeckt. Die ausgeführten nativen Prüfer sind keine
praktische VoiceOver-/Orca-Abnahme oder ein programmatischer Fokusdienst.

Intel macOS 14.6.1/Apple Clang 16 besteht Modell, Gruppenmodell, Geometrie,
Handbuch, Referenzen und Prüfkit 6/6 (`run-0yzbnazn`). Der endgültige AppKit-/UI-/
App-/Einstellungstastaturstand mit ausgewählten Kindern und verschwundener
Auswahl besteht 5/5 (`run-dhdot21h`). Debian 12/GCC 12.2 besteht die sechs
Prüfungen ohne Fenster 6/6 (`run-adj892wc`) und die vier AT-SPI-/App-/
Tastaturabläufe 4/4 (`run-h8b1xqqr`). Linux/Clang 14/ASan/UBSan besteht dieselben
vier Fensterprüfungen 4/4 (`run-zjum7e5z`) und die drei Modell-/Geometrieprüfungen
3/3 (`run-kl573nta`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen
bleiben aktiv. Eine abschließende Hit-Test-Korrektur gibt in Leerraum zwischen
Optionen die Gruppe selbst zurück. Der Prüfer verlangt ausdrücklich einen
realen Zwischenraum; der zusätzliche AT-SPI-Lauf besteht auf Linux/Release 1/1
(`run-5jwtme3r`) und ASan/UBSan 1/1 (`run-6lpnk13l`).

Die endgültigen verschobenen SDKs bestehen auf macOS unter
`build/a11y-radio-final-sdk-checks/Native SDK ä c4tfdtu1` und Linux unter
`build/a11y-radio-checked-sdk-checks/Native SDK ä ksz3mret`. Beide enthalten 469
manifestierte Dateien und führen die tatsächliche App mit Checkbox- und beiden
Richtungen der sechs Optionsauswahlen aus. Linux verwendet ausschließlich
`build/a11y-radio-checked-independent-kit` mit 138 manifestierten Eingaben, ohne
Entwicklerheader oder Physim-Implementierung. Derselbe fokussierte Prüfschritt
gehört auch zur vollständigen SDK-Prüfung mit `--app-tests`; diese fokussierten
Läufe sind keine erneute vollständige SDK-/Produktabnahme.

Belege stehen in `build/a11y-radio-source-freeze.json` und
`build/a11y-radio-sdk-{mac,linux}-PASSED.json`. Dieser Nachtrag erfolgt nach
Paketierung; geprüfte Implementierung und Testeingaben bleiben unverändert.
Der vollständige Plan bleibt erhalten. PP-0710 bleibt für Dropdowns, Listen,
Text-/Editor-/Fokusdienste, Windows/UIA, Wayland und praktische Screenreader-
Abnahme unvollständig. Die Zahl ungeprüfter Planblöcke bleibt unverändert.


## Nativer Tastaturfokus für einfache Controls

Buttons, Checkboxen und Optionen besitzen jetzt Fokusanforderungen, eindeutige
veröffentlichte Fokuszustände und Tastaturaktivierung. AppKit bietet Focus-Getter/
Setter und Fokusmitteilungen; AT-SPI bietet GrabFocus und Fokuszustandsereignisse.
Die D-Bus-Anforderung wartet höchstens eine Sekunde auf den tatsächlichen UI-
Fokus, gibt währenddessen die Servermutex frei und verwirft offene Anfragen bei
Misserfolg. Der UI-Thread aktiviert das besitzende Nuklear-/SDL-Fenster und
veröffentlicht Fokus nur bei tatsächlicher Tastatureigentümerschaft. Echte
Sperren, verdeckende Fenster, verschwundene Ziele und die abgeschlossene
Fensterreihenfolge werden erneut geprüft. Eine erst später im Frame gezeichnete
Überdeckung beendet Fokus und Bedienbarkeit. [Verträge](accessibility.md).

Enter/Leertaste aktiviert einmal, Key-Repeats wiederholen keine Aktivierung.
Tab/Shift+Tab führt durch sichtbare einfache Controls; pro Optionsgruppe ist
nur die ausgewählte sichtbare Option ein Tab-Stopp. Pfeile ändern Auswahl und
Fokus innerhalb der Optionsgruppe. Globale modifizierte App-Tastenkürzel bleiben
verfügbar. Pointerbetätigung und SDL-Fokusverlust geben diesen Fokus frei.
Texte/Container besitzen keine Fokusaktion. Die vorhandene Einstellungen-
Tastaturführung bleibt geprüft. Core-API/ABI, Sprache und Dateiformate ändern
sich nicht; der vollständige Barrierefreiheitsblock bleibt unvollständig.

Die portable Prüfung belegt aufgeschobene/eindeutige Zustellung, Tab-/Options-
navigation, Keyboard-Ownership und deaktivierte/veraltete Ziele. Die gezeichnete
AppKit/UI-Prüfung fokussiert eine Checkbox per Setter, aktiviert sie mit
Leertaste, unterdrückt Key-Repeat und weist ein verdeckendes Fenster ab. Der
unabhängige AT-SPI-Client prüft die bestätigte GrabFocus-Anforderung, Zustände,
Keyboardaktivierung und nicht fokussierbare Ziele an realen Fenstern.
Der tatsächliche App-Prüfer fokussiert `Standardwerte` im zuvor inaktiven Dock-
Bereich. Leertaste, Tab und Pfeiltaste verändern danach den Einstellungsentwurf;
die angewandte Konfiguration bleibt erhalten. Das macOS-Fokusbild unter
`run-nsvjxlco/accessibility_focus_app/files ä/settings/native-focus.bmp` wurde
verlustfrei nach `build/a11y-focus-preview-mac.png` konvertiert und visuell geprüft:
Der helle Rahmen um `Standardwerte` ist deutlich sichtbar.

Der erste zusätzliche App-Prüfer scheiterte auf macOS 5/6 (`run-smsr4jgq`) und
Linux 4/5 (`run-v17hz4cc`) an einem falschen erwarteten Abschlussmarker. Beide
Apps meldeten bereits den richtigen Focus-PASS und Exit 0. Der Marker ist
korrigiert. Die erste abschließende Stackprüfung verwendete irrtümlich Nuklears
private Fenster-Suche; der C17-Compile wies sie ab. Sie verwendet jetzt die
öffentliche `nk_window_find`-API.

Intel macOS 14.6.1/Apple Clang 16 besteht die sieben Modell-/Geometrie-/Handbuch-/
Referenz-/Paketprüfungen 7/7 (`run-engor7h7`) und die sechs AppKit-/UI-/App-/
Tastaturabläufe 6/6 (`run-nsvjxlco`). Debian 12/GCC 12.2 besteht dieselben sieben
Prüfungen ohne Fenster 7/7 (`run-dp8i68pk`), die abschließenden fünf AT-SPI-/App-/
Tastaturabläufe 5/5 (`run-q1_75fjw`). Linux/Clang 14/ASan/UBSan besteht diese
fünf Fensterprüfungen 5/5 (`run-6q7lnxgu`) und vier Modell-/Geometrieprüfungen
4/4 (`run-ut14me1r`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen
bleiben aktiv. macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Die verschobenen SDKs bestehen auf macOS unter
`build/a11y-focus-sdk-checks/Native SDK ä _mum46hk` und Linux unter
`build/a11y-focus-sdk-checks/Native SDK ä 96353203`. Beide enthalten 469
manifestierte Dateien und führen die tatsächlichen Checkbox-/Options-/Fokus-
Appabläufe aus. Linux verwendet ausschließlich `build/a11y-focus-independent-kit`
mit 138 manifestierten Eingaben, ohne Entwicklerheader oder Implementierung.
Die fokussierte Prüfung ist keine vollständige neue SDK-/Produktabnahme.
Belege stehen in `build/a11y-focus-source-freeze.json` und
`build/a11y-focus-sdk-{mac,linux}-PASSED.json`. Dieser Nachtrag erfolgt nach
Paketierung; geprüfte Implementierung und Testeingaben bleiben unverändert.

PP-0710 bleibt für Text-/Editor-/Menü-/Dropdownfokus, virtuelle Listen,
Scroll-to-Reveal, Windows/UIA, Wayland und praktische VoiceOver-/Orca-Abnahme
unvollständig. Alle 531 Originalblöcke des vollständigen Plans bleiben erhalten.


## Projektmigration der vorhandenen Formate 1 → 2

Gültige Format-1-Experimente lassen sich ausdrücklich über Build-Einstellungen
oder `physim-build --migrate-project --project DIR` aktualisieren. Neue App-
Projekte verwenden Format 2. Öffnen und normales Speichern erhalten das jeweilige
Format. Migration schreibt nur die Projektbeschreibung und deren vorherige
Fassung als Backup; Format 2 ist ein unveränderter Leerlauf. Der App-Snapshot
stammt vom Öffnen; externe Änderungen blockieren die Migration. Ungespeicherte
Experiment-/Analyse-/Einstellungsänderungen, laufende Jobs und Wiederherstellung
sperren die Aktion. [Bedienung](workspace.md#editor-und-build) und
[Dateivertrag](data-format.md#eigenständige-analyseprojekte).

Der Modelltest vergleicht unabhängige Sollbytes für LF/CRLF, fehlenden letzten
Zeilenumbruch, Unicodekommentare, Erweiterungseinträge, schon vorhandenen Typ,
maximalen Seed sowie gemischte C-/Physim-Quellen. Er prüft Originalbackup und
POSIX-Rechte 0700, veraltete Snapshots bei beiden Versionen, unbekannte/ungültige
Versionen, Größenlimit und nicht schreibbaren Backuppfad mit erhaltenen Ausgaben.
Der Workflow baut tatsächliche C- und Physim-Pendel vor und nach der Migration.
Er erhält Quellen, vorhandene Archive und sämtliche Cachebytes und belegt
identische Mess-/Abschlusspayloads mit eigenständiger Chunk-CRC-Prüfung. Der zweite
Build kompiliert/verlinkt nicht erneut. Die App-Prüfung klickt den sichtbaren
Button und prüft normales Upgrade, extern veränderten Snapshot und schmutzige
Einstellungen; Quellen und Archivbytes bleiben erhalten.

Intel macOS 14.6.1/Apple Clang 16 besteht die fünf Projekt-/Dokument-/Workflow-/
Prüfpaketfälle 5/5 (`run-u25vvaxz`) und die zwei App-/Einstellungsabläufe
2/2 (`run-im0_bati`). Debian 12/GCC 12.2 besteht dieselben fünf Fälle 5/5
(`run-i4wemoul`) und beide Appabläufe unter X11 2/2 (`run-y218evs3`).
Die Handbuch-/Prüfpaketfälle bestehen auf macOS 2/2 (`run-s62q_b99`), die
Referenzprüfung separat 1/1 (`run-mwn6d6aw`). Dies sind fokussierte Läufe,
keine vollständige neue Testmatrix.

Die verschobenen SDKs bestehen unter
`build/project-migration-sdk-checks/Native SDK ä 3p6m72wk` (macOS) und
`build/project-migration-sdk-checks/Native SDK ä 9z54pmav` (Linux). Beide enthalten
469 manifestierte Dateien. Der unabhängige Linux-Prüfer verwendet ausschließlich
`build/project-migration-independent-kit` mit 140 manifestierten Testeingaben,
ohne Implementierung oder Entwicklerheader. Er baut beide Quellsprachen,
vergleicht Messdaten/Cache/Dateien und führt die tatsächlichen drei Appfälle aus.
Die echten Abschlussmarker und Protokolle sind mit SHA-256 in
`build/project-migration-sdk-{mac,linux}-PASSED.json` erfasst; der eingefrorene
Quellstand steht in `build/project-migration-source-freeze-final.json`.
Dieser Dokumentationsnachtrag erfolgt nach Paketierung; die geprüfte
Implementierung und Testeingaben bleiben unverändert.

PP-0708 gilt für die vorhandenen Formate 1 und 2 als implementiert. Unbekannte
künftige Versionen werden erhalten und abgewiesen. Der bestehende Textwriter
besitzt weiterhin ein letztes Vergleich-/Umbenennungsfenster gegenüber parallelen
Autoren und keine Stromausfallgarantie. ACLs/erweiterte Attribute/besondere
Modusbits sind nicht Teil der Rechteerhaltung. Windows, Apple Silicon und die
vollständige Produktabnahme wurden hier nicht erneut ausgeführt. Alle 531
Originalplanblöcke bleiben erhalten: 34 implementiert, 7 unvollständig und
490 ungeprüft; das Gesamtziel bleibt offen.

Linux/Clang 14 mit ASan/UBSan besteht die zwei Fensterabläufe 2/2
(`run-poh95ran`) und die vier Projekt-/Datei-/Buildfälle 4/4 (`run-j9hjo4th`).
`ASAN_OPTIONS=detect_leaks=0` bleibt gesetzt; Address- und UndefinedBehavior-
Prüfungen bleiben aktiv. macOS-Sanitizer wurden wegen des bereits dokumentierten
fehlenden `ld64.lld` nicht ausgeführt.


## Ressourcenmessung für native Daten- und UI-Referenzlasten

Der private Plattformdienst liest Benutzer-/System-CPU-Zeit und Lebenszeit-Peak-
RAM des eigenen Prozesses, einschließlich aller Threads, ohne Kindprozesse.
macOS/Linux verwenden `getrusage` mit OS-spezifischer Byteumrechnung; Windows
besitzt den K32-/Prozesszeitenpfad, wurde hier aber nicht ausgeführt. Core-API/ABI,
Sprache und Dateiformate ändern sich nicht. Die fachlichen acht Datenlasten
bleiben unverändert. Schreib-/Lese-/Snapshotfälle messen vollständige logische
Run-Dateibytes/s; andere Fälle deklarieren null Dateibytes. Der SDL/OpenGL-
Prüfer ergänzt Aufbau-/Swapzeiten und CPU/Peak-RAM; GPU-Readback und Bildschreiben
finden außerhalb gemessener Frames statt. [Messvertrag](performance.md#prozessressourcen-und-logischer-datendurchsatz)
und [UI-Grenzen](ui-rendering.md).

Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2 bestehen die drei
Prozess-/Benchmark-Smoke-/Wrapperfälle jeweils 3/3 (`run-66m_8r9u` und
`run-zccqr80w`). Die Prozessprüfung berührt 32 MiB, verbraucht CPU und prüft
monotone Prozesszeiten sowie nach Freigabe erhaltenen Peak. Der Wrapper prüft
gleiche logische Run-Dateigrößen bei Schreiben/Lesen/Snapshot und berechneten
Durchsatz. Injizierte NaN-CPU-Werte, rückläufige Peaks, ungültige Wiederholungen
und fehlende Fälle müssen Rohdaten erhalten und dürfen keine Zusammenfassung
veröffentlichen. Schema-1-Baselines werden ausdrücklich abgewiesen.

Die OpenGL-Smokes bestehen auf macOS 1/1 (`run-xpnif057`) und Linux/X11 1/1
(`run-emlwce68`). Beide führen Bildvergleiche, Null-Allokationsprüfung im warmen
Konvertierungspfad, PNG-Export und Größenwechsel aus. Die vollständigen Release-
Messungen laufen auf beiden Systemen mit 100.000 Samples / drei Wiederholungen
und je 300 UI-Messbildern pro Fall. Sie bestehen alle fachlichen und pixelgenauen
Referenzen. Die Linux-VM verwendet Mesa 22.3.6/llvmpipe; Ergebnisse sind kein
nativer GPU- oder plattformübergreifender Geschwindigkeitsvergleich.
Zeitwerte und Rohdatenpfade stehen unter [Leistungsmessung](performance.md#ausgeführte-referenzmessungen-8-oktober-2026).

Der erste macOS-Compile zeigte das durch POSIX-Featureflags verdeckte Darwin-
`ru_maxrss`-Feld; der Plattformdienst fordert jetzt die Darwin-Erweiterung an.
Zwei Fehler im zusätzlichen Python-Testaufbau (überschriebener Modulname und
zu breit abgefangene Hilfsprozesse) sind korrigiert; die abschließenden drei
Prüfungen bestehen auf beiden Systemen. Es handelt sich um tatsächliche Läufe,
keine Ableitung aus vorhandenen CI-Jobs.

PP-0711 bleibt unvollständig für vollständige App-/Runner-/Mehrworkerprofile,
native Szenentessellierung, GPU-Fertigstellung, Startzeit und echte interaktive
Latenz. Peak-RAM ist ein Prozesshöchstwert, kein phasenlokales Allokationsbudget.
Windows und Apple Silicon wurden in dieser Etappe nicht ausgeführt. Alle 531
Originalplanblöcke bleiben erhalten: 34 implementiert, 8 unvollständig und
489 ungeprüft. Das Gesamtziel bleibt offen.

Linux/Clang 14 mit ASan/UBSan besteht die drei Prozess-/Daten-/Wrapperfälle
3/3 (`run-xqn_3z3j`) und den OpenGL-Fall 1/1 (`run-lb11kl4r`).
`detect_leaks=0` ist gesetzt; Address-/UB-Prüfungen bleiben aktiv. macOS-
Sanitizer wurden wegen des dokumentierten fehlenden `ld64.lld` nicht ausgeführt.


## Produktiver Szenenpfad und optionale OpenGL-Serverzeit

Der Renderer veröffentlicht die letzte erfolgreiche Vorbereitung, Tessellierung
und Submission samt Vertices, übertragenen Geometriebytes und drei dynamischen
CPU-Pufferkapazitäten. Fehler invalidieren die Statistik; fehlgeschlagene Getter
ändern keine Ausgabe. Alpha-/Indexkapazitäten werden getrennt erfasst, damit
teilweise erfolgreiche Reservevergrößerungen nicht unterschlagen werden.
Der normale Appbetrieb erstellt keine Timerqueries oder wartet auf sie.

`physim-scene-benchmark` verwendet vier feste Szenen: leer, 32 Kugeln,
31 gemischt transparente Boxen unter einem rotierten/skalierten Frame und eine
96-Punkte-Polyline mit Pfeil. Unabhängige Vertex-/Indexmengen müssen exakt passen.
Bytegleiche Aufnahmen vor/nach jeder Reihe und unterschiedliche Bilder für alle
vier Szenen prüfen die sichtbaren Ergebnisse. Der optionale GL_TIME_ELAPSED-
Timer umschließt den Szenenaufruf und sammelt Ergebnisse separat, mit Frist und
konservativer Überlaufprüfung. Nicht verfügbare/deaktivierte Zeitabfragen ergeben
fehlende GPU-Zeiten; CPU-/Geometrie-/Bildprüfungen laufen weiter.
[Vertrag](scene-rendering.md).

Intel macOS 14.6.1/Apple Clang 16 besteht die vier Grafik-/UI-/Szenen-/ohne-GPU-
Prüfungen 4/4 (`run-34589clt`); Debian 12/GCC 12.2 unter X11 dieselben 4/4
(`run-c7sm5eib`). Linux/Clang 14 mit ASan/UBSan besteht diese vier Fälle 4/4
(`run-t2bujbm7`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen bleiben
aktiv. macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.
Die Grafikprüfung bewahrt bestehende Tiefe-/Transparenz-/Picking-/Projektion-/
Framebufferprüfungen; der UI-Fall prüft Konvertierung, Exporte und Größenwechsel.

Die vollständigen Release-Referenzläufe bestehen auf beiden Systemen mit je
60 Messbildern pro Fall, jeweils mit und ohne Timer. Alle vier Bildhashes bleiben
zwischen beiden Modi je Plattform identisch. Die Zähler sind auf Intel/Iris-
macOS 32 Bit und unter Mesa/llvmpipe 64 Bit breit. Die Linux-VM liefert
Software-Serverzeiten, keine native GPU-Abnahme. Zeiten, Rohdaten und archivierte
Fingerprintpfade stehen im [Szenenbericht](scene-rendering.md#lokale-referenz-8-oktober-2026).
Die macOS-Aufnahmen wurden visuell als sichtbare Kugeln, transformierte Boxen
und Polyline/Pfeil geprüft.

Die ersten Referenzdaten überschritten die vorhandenen Limits von 32 Objekten
und 96 Punkten; die Boxhierarchie verwendete außerdem Slotnummern statt
Objekt-IDs. Die Fixtures sind korrigiert und Geometrievergleiche bleiben strikt.
Ein späterer macOS-Lauf 3/4 (`run-2zi31922`) wies einen realen Messfehler nach:
Die äußere CLOCK_MONOTONIC-Uhr war gröber als SDL_GetTicksNS, sodass Teilzeiten
gelegentlich größer als die Gesamtzeit erschienen. Beide Wandzeitintervalle
verwenden jetzt dieselbe SDL-Uhr; die Intervallprüfung bleibt unverändert.
Die abschließenden 4/4-Prüfungen und vollständigen Referenzläufe bestehen.
Quellbelege stehen in `build/scene-profiling-final-source-freeze.json`; dieser
Dokumentationsnachtrag ändert die geprüfte Implementierung nicht.

PP-0711 bleibt für vollständige App-/Runner-/Mehrworkerprofile, Startzeit,
echte Interaktionslatenz und GPU-Auslastung unvollständig. Das gemessene
Serverintervall einschließlich Stalls ersetzt diese Anforderungen nicht.
Windows und Apple Silicon wurden hier nicht ausgeführt. Alle 531 Originaltexte
bleiben erhalten: 34 implementierte, 8 unvollständige und 489 ungeprüfte Blöcke.
Das gesamte Projektziel bleibt offen.

Die Handbuch-/Referenz-/Prüfpaketfälle bestehen zusätzlich auf macOS 3/3
(`run-aa1ap1qg`) und Linux 3/3 (`run-31d7l_2o`). Die neue Seite ist im Offline-
Themenkatalog registriert und am Ende ergänzt, um vorhandene numerische Verweise
zu erhalten. Die Fensterprüfung enthielt unabhängig davon einen veralteten
Suchverweis auf die Mechanikreferenz; `ps_sweep_spheres` steht inzwischen in der
Kollisionsreferenz. Sie sucht nun die beiden erwarteten Dokumentpfade und prüft
weiterhin Treffer, Nichttreffer und Scrollnavigation. Fenster-/Tastaturprüfungen
bestehen abschließend jeweils 2/2 (`run-cn45y4n7`, `run-jg6unw4x`), einschließlich
des erfolgreichen Ladens sämtlicher registrierter Themen.


## Opt-in-Aufzeichnung tatsächlicher App-Abläufe

Die App zeichnet bei gesetztem `PHYSIM_PROFILE_DIR` numerische Framephasen,
Startbereitschaft ab `main`, eigene Prozess-CPU/Peak-RAM, die aktuelle Szene/UI
und tatsächlich aus Experiment-/Jobpipes gelesene Bytes auf. Ein eigener Writer
verwendet eine atomare Queue mit 1.024 nutzbaren Slots; Produzent/Consumer warten
nicht aufeinander, und nur der Worker greift auf Profildateien zu. End-of-stream
prüft nach dem Stopflag nochmals den letzten veröffentlichten Schreibindex.
Ausgelassene Frames und fehlende Abschlussdaten sind nicht vollständig.
Projekt-/Workspace-/Einstellungs-Speicherung und Jobabschlüsse kommen vor dem
Profilwriter-Join. [Vertrag und Auswertung](app-profiling.md).

Der native Test stellt 30.000 Records unter konkurrierendem Schreiben ein und
prüft FIFO über Queueumlauf, genaue angenommene/ausgelassene Anzahl, vollständiges
Drain und bytegleiche Erhaltung eines bestehenden Ausgabeordners. Prozess-/
Queueprüfungen bestehen auf Intel macOS 14.6.1/Apple Clang 16 mit 2/2
(`run-i9obrlvr`) und Debian 12/GCC 12.2 mit 2/2 (`run-hoehqqfl`).

Der tatsächliche App-Prüfer führt C- und vollständige Physim-Pendel inklusive
Compilerfehlerprüfung, Neubau, Simulation, Analyse, Szenen, Exporten und
Bibliotheksaktionen aus. Beide erhalten erfolgreiche fachliche Abschlussmarker
und Profile mit positiver Startbereitschaft, Szene-/Aufnahme-/normalen Frames,
CPU/RAM und empfangenen Runner-/Jobbytes. Je ein kurzer Tastaturablauf prüft
ausgeschaltete Aufzeichnung und abgewiesene Wiederverwendung eines bestehenden
Profils mit bytegleich erhaltenen Dateien. NaN, abgeschnittene CSV, alte
Szenenwerte ohne Szene und widersprüchliche Intervalle werden abgewiesen.
Teilaufzeichnungen verlangen ausdrücklich erlaubte Auswertung mit erhaltenem
Flag; bestehende Berichtdateien werden nicht überschrieben.

Dieser Workflow besteht auf macOS 1/1 (`run-7683ntty`) und Linux/X11 1/1
(`run-rmvt1uw_`). Die macOS-Referenz enthält 1.016 C-Frames und 819 Physim-Frames,
jeweils ohne Drops. Startbereitschaft beträgt 0,491 bzw. 0,601 Sekunden; dies
sind instrumentierte Selbsttests mit Einrichtung, kein Clean-Machine-Startbudget.
Die beobachteten Runnerbytes betragen 136.848 bzw. 13.905; wallclock-gesteuerte
Pause-/Stopaktionen erklären unterschiedliche Mengen, diese Zahlen beweisen
keine fachliche C-/Physim-Abweichung. Rohdaten stehen in den beiden Workflow-
Ordnern unter `run-7683ntty/app_profiling_workflow/files ä`.

Linux/Clang 14 mit ASan/UBSan besteht die zwei Prozess-/Queuefälle 2/2
(`run-rnt8hi2g`) und den gesamten App-Prüfer 1/1 (`run-9_tqrbcn`).
`detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen bleiben aktiv. Diese
Läufe sind keine ThreadSanitizer-Abnahme. macOS-Sanitizer bleiben wegen fehlendem
`ld64.lld` ungeprüft. Windows und Apple Silicon wurden hier nicht ausgeführt.

Der erste App-Versuch wurde wegen vollem Hostdatenträger abgebrochen (Exit 120),
kein erfolgreicher Nachweis. Zwei identische Archivkopien wurden vor Löschung
auf beiden Hosts per SHA-256 geprüft: Accessibility-Archiv
`d35026022c4ac2a13bfc77ea3ff3acfbaed3c6c564a5a6ee62228955f57bbc50` bleibt
auf `physim-debian-test:/home/physim/project/build/accessibility-archived-proofs.tar.gz`;
Motion-Archiv `bf58a19bc9bef46775bb7ddaa18a1566421dba8e3d356a992b2460f523e8e4ba`
bleibt unter `build/motion-space-archived-proofs.tar.gz` auf dem Mac. Die jeweils
zweite Kopie wurde entfernt; Inventare und Belege bleiben erhalten. Zusätzlich
wurden 276 Dateien des bereits bestandenen älteren Linux-Laufs `run-lhhzsves`
in `build/app-profiling-previous-results.tar.gz` archiviert, jedes Archivmitglied
gegen die Originalhashes geprüft und die Originale nochmals vor Entfernen
geprüft. `build/app-profiling-previous-results.json` hält den vollständigen
Wiederherstellungsnachweis. Keine Projektquellen oder Benutzerdaten wurden gelöscht.

PP-0711 bleibt für Kindprozess-/Mehrworkerressourcen, GPU-Auslastung, OS-Start
vor `main` und echte Eingabe-bis-Anzeige-Latenz unvollständig. Das gesamte
Projektziel bleibt offen; alle 531 Originalplanblöcke bleiben erhalten.

Die abschließende kombinierte Abnahme besteht auf macOS mit fünf Prozess-/
Queue-/Handbuch-/Referenz-/Prüfpaketfällen 5/5 (`run-ov14epno`) und drei App-/
Handbuch-/Tastaturabläufen 3/3 (`run-styn44n8`). Linux besteht dieselben Gruppen
5/5 (`run-ibfwh7fz`) und 3/3 (`run-czhcayip`). Die neue Handbuchseite ist am
Ende des Offlinekatalogs ergänzt und wird beim Fensterablauf mit allen Themen
geladen. Dieser Nachtrag ändert die geprüfte Implementierung nicht.


## Ressourcen abgeschlossener eigener Runner-/Jobprozesse

Der private Prozessdienst cached Benutzer-/System-CPU und Peakbytes beim
Reaping beziehungsweise vor dem Handleclose. POSIX verwendet pid-spezifisches
`wait4`, Windows Prozesszeiten/Working-Set-Abrechnung. Keine spätere PID-Suche
oder kumulative App-`RUSAGE_CHILDREN` wird verwendet. Getter erhalten Outputs
bei laufenden/fehlenden/ungültigen Daten; Close erhält den Cache und Neustart
invalidiert ihn. Kill und shutdownbedingtes Close erfassen ebenfalls Abschlüsse.
Die POSIX-/Windows-Scopes bleiben ausdrücklich verschieden und Peaks sind keine
gleichzeitige Baumsumme. [Vertrag](app-profiling.md#ressourcen-abgeschlossener-eigener-prozesse).

Schema 2 schreibt Frame- und Prozessrecords über dieselbe atomare Queue in
getrennte CSVs, mit getrennten geschriebenen/ausgelassenen Zählern. Ein neuer
Lebenszyklus setzt die App-Markierung zurück, sodass normales Polling und späterer
Shutdown keine doppelten Records erzeugen. Sequenzen unterscheiden spätere
PID-Wiederverwendung. Fehlende Ressourcen bleiben leer/null. Schema 1 bleibt
mit ausschließlich seinen Framewerten lesbar. Die Ressourcen stammen aus
abgeschlossenen eigenen Experiment-, Build-, Parameterbeschreibungs-, Analyse-
und Exportprozessen; intern verwaltete Batchworker sind nicht vollständig erfasst.

Die native Gegenprobe lässt zwei eigene Kinder 32/64 MiB berühren und CPU
verbrauchen. Selbstauskünfte werden gegen Finalwerte verglichen. Ein zunächst
blockiertes zweites Kind endet erst nach dem ersten; dessen Cache muss unverändert
bleiben. Wiederholte Getter, Getter nach Close, fehlgeschlagener Neustart, Kill
und Close eines laufenden Kindes sind geprüft. Queuegegenproben vergleichen
verfügbare und ausdrücklich fehlende Prozessfelder im tatsächlichen Writer.

Intel macOS 14.6.1/Apple Clang 16 besteht drei Ressourcen-/Kind-/Queuefälle 3/3
(`run-lv294fub`), Debian 12/GCC 12.2 dieselben 3/3 (`run-hphxtiy1`). Die initiale
macOS-Gegenprobe mit bestehender Parent-Watch-Prüfung besteht 3/3 (`run-hzs5uuwn`).
Die App-Abläufe für C und vollständiges Physim mit echter Kompilierung, Runnern,
Analyse und Bibliothek protokollieren verfügbare Experiment-/Builder-/Analysewerte
sowie den erwarteten Compilerfehler. Sie bestehen zunächst auf beiden Systemen
1/1 (`run-jm8d9a02`, `run-6eytro6m`). Die erweiterte macOS-Gegenprobe besteht
1/1 (`run-v7j_1bra`): NaN-Prozesszeiten, abgeschnittene Prozess-CSV und erfundene
Werte bei Nichtverfügbarkeit werden abgewiesen; tatsächliche Nichtverfügbarkeit
und Schema 1 bleiben korrekt auswertbar.

Linux/Clang 14 mit ASan/UBSan besteht alle vier Ressourcen-/Kind-/Queue-/
Parent-Watch-Fälle 4/4 (`run-rsvz7fng`) und den erweiterten App-Prüfer 1/1
(`run-f9beqjjt`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen bleiben
aktiv. Diese Läufe sind keine ThreadSanitizer-Abnahme. macOS-Sanitizer bleiben
wegen fehlendem `ld64.lld` ungeprüft; Windows/Apple Silicon wurden nicht ausgeführt.

Zur Freigabe des Linux-Prüfraums wurden 504 Dateien der bereits bestandenen
älteren Läufe `run-rmvt1uw_` und `run-9_tqrbcn` in
`build/child-profiling-previous-results.tar.gz` archiviert. Alle Datei-/Symlink-
mitglieder wurden gegen Originale und alle Originalhashes vor Entfernen erneut
geprüft. `build/child-profiling-previous-results.json` enthält den vollständigen
Wiederherstellungsnachweis. Es wurden nur diese archivierten Testoriginale entfernt.

PP-0711 bleibt für Live-/gleichzeitige Prozessbaum-/Mehrworkerressourcen,
GPU-Auslastung, OS-Start vor `main` und echte Eingabe-bis-Anzeige-Latenz
unvollständig. Die 531 Originalplanblöcke und das gesamte Ziel bleiben erhalten.

Die abschließenden sieben Ressourcen-/Kind-/Queue-/Parent-Watch-/Handbuch-/
Referenz-/Prüfpaketfälle bestehen auf macOS 7/7 (`run-uhxr6m61`) und Linux
7/7 (`run-hxwfq40o`). Der erweiterte Linux-App-Prüfer besteht außerdem
1/1 (`run-ha6zki2c`). Die getestete Implementierung und Testeingaben bleiben
bei diesem Dokumentationsnachtrag unverändert;
`build/child-profiling-final-code-freeze.json` erfasst ihren Quellstand.

## CI-Testkatalog, Compilerkodierung und Linux-Prüfabhängigkeit

Die tatsächlichen Ergebnisse von [C17-Lauf 37773257771](https://github.com/PhysicSimulator/physim/actions/runs/37773257771)
am Stand `0678f6057f6b7f1d68f98585595f4a2b1f36331b` zeigen 675/675 bestandene
native Fälle unter Linux/Clang und macOS/Apple Silicon. Der anschließende
Test-Runner-Selbsttest scheiterte am veralteten erwarteten Displaykatalog; dieser
Fehler wurde auf dem lokalen Intel-Mac reproduziert. Der Katalog erfasst nun
auch Migration, App-Profiling und die plattformspezifischen Checkbox-/Options-/
Fokusabläufe und prüft weiterhin exakte Fallmengen, Skripte und benötigte Argumente.

Windows/MSVC Debug bestand 674/675 Fälle. Der Migrationstest scheiterte beim
UTF-8-Decodieren der weitergereichten OEM-Compilerausgabe. Der Prüfer erhält
jetzt Originalbytes und prüft ASCII-Marker als Bytes; die bestehende Kontrolle
von Quellen, Backup, Cache und Messwerten bleibt erhalten. Der Integrationsfall
mischt zusätzlich absichtlich ungültige UTF-8-Diagnosebytes in echte
Builderausgaben. Dies prüft die Kodierungsgrenze auch auf macOS und Linux.

Die drei heruntergeladenen Artefakte wurden vor Auswertung gegen die SHA-256-
Digests der offiziellen GitHub-Artefaktmetadaten geprüft:

- `sdk-windows-v143-Debug`: `d3623863966e4b3901cca3e578cc0302f4f8793512dbe158d52a4c89e061eaad`
- `linux-ui-clang`: `65cdada87d8e0b02dd2f22797034840c6c5da7ffd4a145c8471232b65ae442b6`
- `macos-macos-15`: `30a967415fac215d408659be2076dfa6b0b9098cc01af06397854d14b597720c`

Im [Linux-Paketlauf 37762643835](https://github.com/PhysicSimulator/physim/actions/runs/37762643835)
erreichte das Debian-12-Prüfpaket alle neun GUI-Projektabläufe sowie Handbuch,
Migration und Einstellungen, scheiterte dann aber beim Import von `dbus` im
nativen Checkboxprüfer. Das Debian-Artefakt wurde gegen den offiziellen Digest
`d6a8a980863bf206519c365dcd014da14141391deb588b8a0ab87708161f8360` geprüft.
Beide Linux-Workflows installieren `python3-dbus` jetzt ausdrücklich. Im
Paketworkflow geschieht dies weiterhin erst nach der Startprüfung ohne Python.

Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2 bestehen jeweils den
Runner-Selbsttest und die direkten Bootstrap-Build-Gegenproben. Migration mit
eingemischten OEM-Bytes und Prüfpaket bestehen jeweils 2/2 (`run-j9l4p4oh`,
`run-ocqcofxc`). Auf Debian bestehen außerdem die drei tatsächlichen nativen
Checkbox-/Options-/Fokusabläufe mit dem bereits vorhandenen verlagerten SDK
und unabhängigen Prüfpaket (`build/ci-dbus-sdk-checks`). Dieser letzte Lauf
prüft die Abhängigkeit am bestehenden SDK, kein neu erzeugtes Gesamtpaket.

Die Korrekturen sind damit lokal auf beiden Systemen geprüft. Eine vollständige
erneute CI-Abnahme einschließlich Windows, Apple Silicon und frisch installierter
Debian-/Ubuntu-Pakete ist noch nicht belegt. Der gesamte Projektplan bleibt offen.

## Kraftvektoren des vertikalen Pendelablaufs

Die C-Pendelvorlage, alle fünf Sprachvarianten und beide Lernpfadexperimente
zeigen Gewicht und radiale Stangenkraft mit identischen Farben und einem
beschrifteten Maßstab von 0,05 m/N. Das C-Modell ergänzt bei eingeschaltetem
Luftwiderstand dessen tangentiale Kraft. Stabile IDs und Elternbeziehungen
erhalten Auswahl und Sichtbarkeit. Die Kräfte verändern weder Modellzustand
noch Messkanäle; der vorhandene Geschwindigkeitspfeil behält seinen Maßstab.
[Modellgleichungen](pendulum-tutorial.md#verfahren-auswählen).

`pendulum_forces` lädt neun tatsächliche C-/Physim-Module, einschließlich eines
C-Moduls mit Luftdichte 1,225 kg/m³. Drei Längen/Anfangswinkel, je 401 Szenen
und Reset prüfen Kraftsumme gegen eine unabhängige Newton-Gegenrechnung,
Gewichtsrichtung, dissipativen Widerstand, Pfeilursprung, Einheitenbeschriftung,
Farben und Hierarchie. Die bestehende Integratorprüfung vergleicht weiterhin
alle Szenenfelder und Messdaten; sie umfasst jetzt alle zwölf Lernpfadobjekte.

Intel macOS 14.6.1/Apple Clang 16 besteht zunächst Kraft-/Sprach-/ID-Prüfung
3/3 (`run-987n24li`), den vollständigen Pendellernpfad 1/1 (`run-glzge7id`)
und abschließend Kraft-/Quellen-/Prüfpaketfälle 3/3 (`run-9qil7a7b`).
Handbuch-/Quellengleichheit bestehen 2/2 (`run-mmp19n79`); der Runner-Selbsttest
besteht ebenfalls. Beide C-/Physim-Lernpfadfenster bestehen gemeinsam 1/1
(`run-r9zx8x6x`), die tatsächliche Sprachvorlage mit Build, Runner und Auswertung
1/1 (`run-giqtz_jk`). Ihre Lernpfadbilder wurden nebeneinander visuell geprüft.

Debian 12/GCC 12.2 besteht alle fünf Kraft-/Sprach-/ID-/Lernpfad-/Quellenfälle
5/5 (`run-c_ebe3wq`) und beide Fensterabläufe 2/2 (`run-rzh8hhwe`).
Linux/Clang 14 mit ASan/UBSan besteht die neun Module umfassende Kraftprüfung
1/1 (`run-4i8nz9ms`), mit `detect_leaks=0` und aktiver Address-/UB-Prüfung.
Windows und Apple Silicon wurden für diese Kraftänderung noch nicht ausgeführt.

Der erste Linux-Build scheiterte an vollem Datenträger und zählt nicht als
Nachweis. Zum Freigeben wurden 13.638 Mitglieder des älteren, vollständig
bestandenen 78-Fälle-Laufs `run-38r48_zv` auf dem Mac archiviert. Alle
Archivmitglieder und anschließend alle Originale wurden per SHA-256 verglichen,
erst dann wurde ausschließlich dieser Linux-Testordner entfernt. Die kanonische
Kopie liegt auf dem Mac unter `build/pendulum-forces-archived-linux-proofs.tar.gz`,
SHA-256 `96aad9d1b4fbcf8db3c32e7eed36ae9bbdbfb134a11797dae48fa5d655eae346`;
`build/pendulum-forces-archived-linux-receipt.json` enthält Pfade und Einzelhashes.
Projektquellen und Benutzerdaten wurden nicht entfernt.

PP-0776 besitzt damit einen konkreten begrenzten Nachweis für Pendel,
Kraftvektoren und Live-Werte. Die übrigen Anforderungen des vollständigen
vertikalen Anwendungsfalls und des Gesamtplans bleiben separat abzunehmen.

## CPU-Prüflast unter Windows/MSVC

Der [C17-Lauf 37777889619](https://github.com/PhysicSimulator/physim/actions/runs/37777889619)
am Stand `c229e359c94f70082e56b028448f24d39db7f1b7` liefert unter MSVC Debug
675/676 bestandene native Fälle. Die ergänzten Diagnosewerte des verbleibenden
`process_usage`-Fehlers zeigen jeweils 0 s Benutzer-/System-CPU vor und nach
der Last, aber einen korrekt gestiegenen Peak von 3.162.112 auf 36.728.832 Bytes
bei 33.554.432 berührten Bytes. Das heruntergeladene Artefakt wurde gegen den
offiziellen SHA-256-Digest
`791bb51edb33e5e694109ae94b21616a93dcc82a147114aab61bb80ad45505ba` geprüft.

Der Test beendete die Last nach 40 ms Wandzeit, ohne dass die OS-Abrechnung
CPU-Zeit veröffentlicht hatte. Er arbeitet jetzt bis mindestens 40 ms
zusätzliche Prozess-CPU erfasst sind, höchstens 20 s Wandzeit. Endliche,
monotone Benutzer-/Systemwerte werden bei jeder Abfrage geprüft. Die
Speicheruntergrenze, Lebenszeit-Peak-Erhaltung und Prüfung nach Freigabe bleiben
erhalten. Die API-Implementierung wird durch diese Testkorrektur nicht geändert.
[Prozesszeiten](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocesstimes)
beschreiben CPU-Abrechnung; sie sind keine Wandzeituhr.

Die Korrektur besteht auf Intel macOS/Apple Clang 16 1/1 (`run-zhos1g1r`)
und Debian 12/GCC 12.2 1/1 (`run-zed_431v`). Die erneute tatsächliche
Windows-Abnahme dieser Korrektur steht noch aus.

## Pendelanalyse als Physim-Projektvorlage

Neue Pendelprojekte mit Physim-Auswertung kopieren die bereits gepflegte
`examples/documentation/pendulum_analysis.phys`. Das gilt für C- und Physim-
Experimente. Die Analyse lädt die gespeicherten Läufe, zeigt Winkel und
Energieabweichung, berechnet positive Nulldurchgangsperioden und exportiert die
vollständigen CSV-Spalten. Kurze oder ruhende Läufe erhalten keinen erfundenen
Periodenwert. Vorhandene Projekte werden nicht umgeschrieben. Andere Modelle
und unabhängige Analyseprojekte behalten ihre eigene Vorlagenauswahl.

Die tatsächlichen `language_full_workflow`-/`language_mixed_workflow`-Fenster
prüfen kopierte Quellen, Build, Runner, gespeicherte Quelldateien, Bericht,
Energie-Kurvenlänge, Stichprobenzahl und Periodentabelle. Die Tastaturgegenprobe
baut, startet und analysiert alle 32 Vorlagen-/Sprachpaare und beide unabhängigen
Analysen bei 16 und 22 px, insgesamt 68 Projekte je Host. Alle drei Fälle
bestehen auf Intel macOS 14.6.1/Apple Clang 16 3/3 (`run-dgrqglsx`) und
Debian 12/GCC 12.2 3/3 (`run-ybh1ocsn`). Das tatsächlich geöffnete
Physim-Analysefenster wurde anhand seiner Aufnahme visuell geprüft.

`pendulum_template_analysis` prüft außerdem die echten Standardexperimente:
4000 Schritte des C-Pendels und des Physim-Pendels sowie zehn Schritte eines
ruhenden Modells. Die Physim-Auswertung und ein unabhängiger C-Berichtsprüfer
vergleichen die ursprünglichen Läufe mit Kurven, Energieabweichung, SI-Einheiten
und Periodentabelle. Zusammen mit Quellengleichheit und Prüfpaket bestehen
die drei nativen Fälle 3/3 auf macOS (`run-xl8ful2s`) und Linux
(`run-sn4h2119`). Linux/Clang 14 besteht den neuen Integrationsfall mit
ASan/UBSan 1/1 (`run-l6a9ae5i`, `detect_leaks=0`). Ein zuvor parallel
gestarteter macOS-Build wurde vom Buildlock abgewiesen und zählt nicht als
erfolgreicher Test; der erfolgreiche native Lauf verwendet einen eigenen Buildordner.

Die aktualisierte `check_modules`-Funktion des SDK-Prüfers wurde mit den
tatsächlichen Runner-/Analysemodulen des bereits vorhandenen installierten
macOS-SDK ausgeführt (`build/pendulum-analysis-sdk-branch-mac/results.json`).
Dies belegt den geänderten Zweig, keine neue vollständige SDK-Abnahme. Die
Prüfpaket-Gegenproben bestehen auf beiden Hosts mit unverändert 140 manifestierten
Eingaben. Windows/Apple Silicon und frisch erzeugte Gesamtpakete dieser
Vorlagenänderung bleiben separat zu prüfen. Der Gesamtplan bleibt offen.

## Bestätigte Windows-Ressourcenprüfung

Der [C17-Lauf 37780045308](https://github.com/PhysicSimulator/physim/actions/runs/37780045308)
am Stand `9e85dfde8af9f99a811b296fb5c17a733c735c22` besteht den vollständigen
MSVC-Debug-Job. Das Ergebnisartefakt enthält 676/676 bestandene native Fälle
(`run-ntop68h6`) und 10/10 gezielte AddressSanitizer-Fälle (`run-cdjpb81e`).
Der zuvor fehlgeschlagene Prozessressourcentest meldet jetzt 0,046875 s Benutzer-
CPU und einen Peak von 36.712.448 Bytes bei 33.554.432 berührten Bytes. Das
Artefakt wurde gegen den offiziellen SHA-256-Digest
`009a6f884d2c8a6a2e460ee235ec583e088cae51e88e36a3570031c1edd3cbf4` geprüft.
Die absichtlich fehlschlagenden Runner-Selbsttestfixtures bleiben Testdaten.
Diese Abnahme gilt für diesen Stand; sie beweist keine spätere Änderung oder
vollständige Gleichheit der noch laufenden gesamten Plattformmatrix.

## Energieplot und vollständiger CSV im C-Pendelablauf

Die C-Standardanalyse ergänzt für Läufe mit Winkel und Energie ein Diagramm
`E - E(0)` mit Zeit in Sekunden und Energieänderung in Joule. Die bestehende
Bilanzdarstellung für Modelle mit `energy.balance` bleibt erhalten. Ein
gesonderter `-energy.csv` enthält alle ursprünglichen Zeit-/Energiewerte und
die Änderung, unabhängig von der reduzierten Berichtsvorschau. Im konservativen
Pendel dient die Änderung als Energiefehler; im Widerstandsmodell enthält sie
auch physikalische Dissipation. Es entsteht keine Dämpfungsabschätzung aus
diesem Plot allein.

Der erweiterte `pendulum_template_analysis` prüft Standard-C- und Standard-
Physim-Läufe durch beide Analysemodule sowie einen ruhenden Lauf. Ein
unabhängiger C-Prüfer vergleicht den neuen Plot, SI-Achsen und sämtliche
Energie-/Periodenmetriken mit den ursprünglichen Daten. Ein unabhängiger
PSRUN-/CRC-/CSV-Decoder vergleicht jede exportierte Zeile, darunter sämtliche
6001 Werte des längeren C-Laufs. Die abschließenden nativen Fälle bestehen
auf Intel macOS/Apple Clang 16 1/1 (`run-oghr8d4n`) und Debian 12/GCC 12.2
1/1 (`run-mdlhi87f`). Linux/Clang 14 mit ASan/UBSan besteht die Gegenprobe
1/1 (`run-31o70csc`, `detect_leaks=0`); anschließend wurde nur das sichtbare
Minuszeichen der Beschriftung geändert, keine Rechnung oder Speicherlogik.

`pendulum_c_energy_workflow` erstellt tatsächliche C- und Physim-Pendelprojekte
mit C-Auswertung, führt je mindestens 20 s simulierte Zeit aus und prüft Build,
Runner, Energietabelle, bekannte nichtlineare Referenzperiode des C-Modells,
Plot, vollständigen CSV, gespeicherten Bericht und Wiederöffnen aus der
Bibliothek. SVG- und PNG-Exporte werden über echte Buttons gestartet. Der
Prüfer kontrolliert PNG-CRC, Kodierung, Abmessungen und sämtliche Scanlines
sowie den SVG-Titel. Beide Abläufe bestehen gemeinsam auf macOS 1/1
(`run-ehmtvfaw`) und Linux 1/1 (`run-8me4chhy`). App-Aufnahme und PNG wurden
visuell geprüft. Ein fehlender Test-Elternordner wurde korrigiert; die davor
gescheiterten Läufe sind keine erfolgreichen Nachweise. Die Bildprüfung führte
zur lesbaren ASCII-Beschriftung `E - E(0)`; die finalen Fensterläufe prüfen
diese Fassung. Der native Runner-Selbsttest besteht mit dem neuen Displayfall.

PP-0667, PP-0779 und PP-0780 besitzen damit konkrete begrenzte Nachweise für
gespeicherte Pendeldaten, Energie-/Periodenanalyse, Plot/Tabelle und Exporte.
Eine vollständige Dämpfungsanalyse und die übrigen Anforderungen des vertikalen
Anwendungsfalls bleiben getrennt offen. Windows/Apple Silicon dieser neuen
C-Energieplotänderung wurden noch nicht ausgeführt; der zuvor bestätigte
Windows-Job ist kein Nachweis für den späteren Quellstand.

## Einstellbares Medium und Winkelsensorrauschen des Pendels

Die allgemeine C-Pendelvorlage und alle fünf Physim-Varianten besitzen dieselben
sieben SI-Parameter: Länge, Anfangswinkel, Masse, Mediumdichte, konstanter
Widerstandskoeffizient, Querschnitt und Rauschstandardabweichung. Instanzwerte
ersetzen ausschließlich die bisherigen festen Defaults; Vakuum und ausgeschaltetes
Rauschen bleiben Standard. Die gemeinsame `ps_medium`-/`Medium`-Drag-API wirkt
entgegen der Tangentialgeschwindigkeit. Kraftpfeile berücksichtigen die ausgewählte
Masse und zeigen aktiven Widerstand. Positive Dichte, Koeffizient und Querschnitt
werden einzeln geprüft; ein unterlaufendes Produkt kann Velocity Verlet keinen
geschwindigkeitsabhängigen Term vortäuschen. Ein exakt ausgeschalteter Term ist
auch bei positiver Dichte zulässig.

Der wahre Winkel bleibt von `sensor.angle` getrennt. Gaußrauschen wird nur bei
positiver Standardabweichung gemessen, aus dem Laufseed wiederholbar zurückgesetzt
und beeinflusst keine Integration. Die C-Vorlage übernimmt Zustand, Messwerte
und Zufallszustand erst nach erfolgreichen endlichen Berechnungen. Die gepflegten
Pendelanalysen nennen die mechanische Änderung nun `Energy change`; tatsächliche
Dissipation wird damit nicht als ausschließlich numerischer Drift bezeichnet.
[Parameter und Modellannahmen](workspace.md).

`pendulum_medium` vergleicht neun tatsächliche Module in fünf C-/Physim-Paaren,
je 5001 Messungen mit ausgewählter Masse, Medium und Rauschen. Die Gegenprobe
prüft sieben Dimensionen, Kraftsummen gegen Newtons tangentiale/radiale Rechnung,
Energieverlust, Rauschmittelwert/-standardabweichung, Reset, getrennte Instanzen,
ungültige Masse und Verlet-Grenzen. Tiny-positive Eingaben werden bei Verlet
abgewiesen; exakt null Widerstandskoeffizient bleibt erlaubt. NaN und sehr große
Schritte erhalten Messwerte und Zufallszustand bei Fehlern.

Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2 bestehen alle sechs
Medium-/Sprach-/Kraft-/Standardanalyse-/Lernpfad-/Quellenfälle 6/6
(`run-1ul01yjy`, `run-f14lhjrd`). Die erweiterten Fehlergrenzen bestehen danach
auf macOS 1/1 (`run-zn_wzl4q`) und Linux/Clang 14 mit ASan/UBSan 1/1
(`run-hqir8mmd`, `detect_leaks=0`). Die vorherige Sanitizergegenprobe besteht
ebenfalls 1/1 (`run-nq90ooq9`). Der Runner-Selbsttest besteht mit dem neuen Fensterfall.

Die tatsächlichen Fensterabläufe bauen C-/Physim-Projekte, wählen Masse 2 kg,
Dichte 1,225 kg/m3, Koeffizient 0,8, Querschnitt 0,08 m2 und Rauschen 0,02 rad,
führen mindestens 6 s aus und prüfen gespeicherte Parameter/Einheiten, Kräfte,
Energieabnahme und Analyseberichte. Zusammen mit den bestehenden Parameterstudien
(einschließlich cm-Eingabe) und der Standard-Sprachvorlage bestehen sie auf
macOS 3/3 (`run-f2d7tdvv`) und Linux 3/3 (`run-sfh67nn3`). Die tatsächliche
Physim-Szene mit Widerstandskraft wurde visuell geprüft. Eine fehlende Test-
Headerdeklaration und ein ungültiger doppelter Override im Prüfer wurden korrigiert;
die zuvor fehlgeschlagenen Läufe zählen nicht als erfolgreiche Nachweise.

PP-0765/0766 besitzen jetzt begrenzte Nachweise für das Mediumkonzept und
Winkelsensorrauschen. PP-0756 bleibt als gesamtes erstes Experiment unvollständig:
insbesondere eine vollständige Dämpfungsabschätzung und die übrige vertikale
Abnahme bleiben offen. Diese Änderung wurde noch nicht unter Windows oder
Apple Silicon ausgeführt. Alle 531 Originalplanblöcke bleiben erhalten.

## Beobachtete Amplitudenabnahme gespeicherter Pendelläufe

`ps_series_positive_peaks` und `Series.positivePeaks` liefern gemeinsam
ausgerichtete Zeit-/Amplituden-/Segmentreihen. Ein Anstieg mit folgendem Abfall
erkennt eine positive Spitze; flache Spitzen erscheinen einmal in ihrer Zeitmitte.
Randwerte und konstante Segmente bleiben ausgeschlossen. Fehlende Signal-/Zeit-
zeilen trennen Segmente. Die Zeit muss über gültige Zeilen streng steigen.
Blockweise zwei Durchläufe benötigen 24 Scratchbytes je Spitze; Fehler erhalten
Ausgaben und Quota. Die Ausgaben besitzen eine neue Samplezuordnung und können
die Eingabehandles des Aufrufers überschreiben. API-/Bibliotheksreferenzen sind
regeneriert und geprüft.

Die C-Standardauswertung und beide Lernpfadanalysemodule berechnen aus zwei
geeigneten Spitzen δ = ln(A₁) − ln(A₂) und r = δ/(t₂−t₁). Nur Spitzen desselben
Segments werden verbunden. Eine Tabelle pro Bericht fasst Intervallzahl,
mittleres Dekrement und Mittelwert/Spannweite der Raten zusammen. Volle Spitzen-
und Intervall-CSVs bleiben unabhängig von Tabellen- und Plotgrenzen erhalten.
Wachstum besitzt negative Raten; unzureichende Spitzen liefern keine Zeile.
Es werden beobachtete Werte ohne Spitzeninterpolation, Rauschfilter oder
Anpassung eines konstanten viskosen Dämpfungsmodells ausgewiesen.
[Herleitung und Grenzen](pendulum-tutorial.md#beobachtete-amplitudenabnahme).

Die native Gegenprobe prüft flache Spitzen, Blockgrenzen, fehlende Segmente,
strenge Zeit, leere Ergebnisse, Eingabe-/Ausgabealias und Quota. Die tatsächliche
Physim-Analysebindung besitzt dieselben bekannten Spitzen- und Maskenwerte.
Sechs synthetische Dateien erzeugen bekannte exponentielle Amplitudenhüllen,
Wachstum, unregelmäßige Zeiten, flache Spitzen, Ruhe und nur eine Spitze. Drei
reale Analysemodule erzeugen daraus 18 unabhängig geprüfte Berichte. Je 400
Spitzen und 399 Intervalle übersteigen die 256 Tabellenzeilen; alle CSV-Zeiten,
Amplituden und signierten Raten werden trotzdem vollständig verglichen.
Diese Dateien sind ausdrücklich synthetische Signale, keine physikalischen Pendel.

Intel macOS 14.6.1/Apple Clang 16 besteht die abschließenden sechs Spitzen-/
Bindungs-/Referenz-/Standardanalyse-/Lernpfad-/Quellenfälle 6/6 (`run-kwnuc3yy`),
Debian 12/GCC 12.2 dieselben 6/6 (`run-si18cisp`). Linux/Clang 14 mit ASan/UBSan
besteht Spitzen, Bindung und die 18 Referenzberichte 3/3 (`run-xb4hqrcy`,
`detect_leaks=0`). Ein zunächst veralteter Handbuch-Codeblock wurde mit der
getesteten Quelle abgeglichen; der davor fehlgeschlagene Quellenfall gilt nicht
als erfolgreicher Nachweis. Der native Runner-Selbsttest besteht ebenfalls.

Die tatsächlichen Standard-Sprach-, Vakuumlernpfad- und Mediumfenster bestehen
auf macOS 3/3 (`run-1zs7qsy_`) und Linux 3/3 (`run-6cgec3wr`). C- und Physim-
Mediumprojekte zeigen nach mindestens sechs Sekunden positive beobachtete
Abnahmeraten in ihrer dritten Tabelle; die tatsächlich geöffnete Physim-Tabelle
wurde visuell geprüft. Fehlende Dämpfung in Ruhe-/Kurzläufen bleibt ausdrücklich
von einer gemessenen Rate null verschieden. Windows/Apple Silicon und neue
Gesamtpakete dieser API-/Analyseänderung wurden noch nicht ausgeführt.

PP-0767 besitzt damit einen begrenzten Nachweis für Periodendauer, beobachtete
Amplitudenabnahme und Energieabweichung. Dies ist keine vollständige Abnahme
des gesamten vertikalen Anwendungsfalls oder des Gesamtplans.

## Direkte lineare Pendelgeschwindigkeit und Berichtsgegenprobe

Die allgemeine C-Vorlage, alle fünf Physim-Pendelvorlagen sowie beide
Lernpfadmodelle speichern nun `velocity.x`, `velocity.y` und `speed` als
kanonische SI-Kanäle in `m/s`. Die ursprünglichen sechs Kanalindizes bleiben
erhalten; neun Kanäle werden ohne Format-/ABI-Änderung geschrieben. Komponenten
folgen der Ableitung der Kreisbahn, der Betrag ist `L |ω|`. Messwerte stammen
vom akzeptierten Modellzustand, nicht von einer Ableitung des gespeicherten
Sensorsignals. C prüft sämtliche neun Werte vor dem Commit; fehlgeschlagene
Schritte erhalten auch die neuen Kanäle und den RNG-Zustand.

`pendulum_medium` vergleicht neun Module in fünf C-/Physim-Paaren über je 5001
Zustände. Kanalnamen und SI-Dimensionen, Vektorbetrag, Orthogonalität zur Stange
und Energie aus der linearen Geschwindigkeit werden unabhängig geprüft,
zusammen mit Mediumkräften, Rauschen, Reset und Fehlergrenzen. Der Lernpfadtest
prüft die neuen Werte in allen gespeicherten Zeilen, einschließlich fünf
Integratoren, geänderter Länge/Amplitude, Ruhe und adaptiver Zeiten. Seine
PSRUN-/Szenen-Gegenprobe bleibt unabhängig vom Bibliotheksreader. Der
Sprachintegrationstest prüft zusätzliche echte Runnerdateien.

Intel macOS 14.6.1/Apple Clang 16 besteht fünf native Fälle 5/5
(`run-c624o33a`); Debian 12/GCC 12.2 dieselben 5/5 (`run-7kzt1qd5`).
Linux/Clang 14 mit ASan/UBSan besteht Lernpfad und Medium 2/2
(`run-wjjohn89`, `detect_leaks=0`). Die tatsächlichen Standard-Sprach-, Lernpfad-
und Mediumfenster bestehen macOS 3/3 (`run-cabirlw8`) und Linux 3/3
(`run-5s3oovhd`). Alle 14 geänderten Implementierungs-/Testeingaben wurden
zwischen beiden Testsystemen per SHA-256 abgeglichen.

Der offizielle Linux-Paketlauf `37790113179` am vorherigen Stand `4ac9028`
endete mit 677/678 bestandenen nativen Fällen. Das Artefakt
`linux-release-build-tests` wurde vor dem Lesen gegen den offiziellen Digest
`e0cdfa246f5b5a2d2851ceb001af94bd8e80aa8625fb83167d4e6331bd76ce50`
geprüft. `derived_reference` erwartete noch drei Plots/zwei Tabellen, obwohl
die Energieänderung und die neue Amplitudenabnahme den Bericht auf vier
Plots/drei Tabellen erweitert haben. Die Gegenprobe prüft jetzt zusätzlich
Energieeinheit, vollständige Kurvenquellen und kleine Vakuumenergieabweichung
sowie Titel, Intervalle, Rate und Einheit der Abnahmetabelle. Sie besteht
macOS 1/1 (`run-y7_q8fd7`) und Linux 1/1 (`run-cxzh78ag`). Ein zwischenzeitlicher
zweiter Linux-Build wurde von der aktiven Buildsperre abgewiesen und erst nach
Abschluss des ersten Builds erneut erfolgreich ausgeführt; dies ist kein
zusätzlicher erfolgreicher Testlauf.

PP-0763 ist damit im beschriebenen Umfang nachgewiesen. PP-0756 und die
vollständige Plattform-/Paketabnahme bleiben offen; Windows und Apple Silicon
wurden für diese Änderung nicht ausgeführt. Der neue Gesamtpaketlauf ist noch
kein bestätigter Nachweis.

## Laufparameter für allgemeine Pendel-Integratoren

Die C-Pendelvorlage und alle fünf Physim-Pendelvorlagen besitzen jetzt einen
zusätzlichen dimensionslosen Parameter `integrator` mit ganzzahligem Bereich
0..4: Euler, symplektischer Euler, RK4, Velocity Verlet, Dormand–Prince 5(4).
Die bisherigen Standardverfahren bleiben erhalten; `PS_PENDULUM_METHOD`
bestimmt weiterhin den C-Standard. Die gewählte Methode gehört zur jeweiligen
Instanz und erscheint in deren Laufmetadaten. Nichtganzzahlige Werte und
Werte außerhalb des Bereichs werden abgewiesen. Verlet mit positiven Werten
für Dichte, Widerstandskoeffizient und Fläche bleibt auch bei unterlaufendem
Produkt unzulässig. Die adaptive Laufsteuerung verwendet weiterhin getrennt
Dormand–Prince; feste Integratorwahl und adaptive Methode sind beide dokumentiert.

Die Medium-Gegenprobe prüft jede der fünf Physim-Vorlagen gegen dasselbe
C-Modell mit allen fünf ausgewählten Verfahren, Vakuum und zulässigem Medium
über 500 Schritte je Konfiguration. Euler und symplektischer Euler besitzen
zusätzlich eine unabhängige erste Schrittformel. Namen der gewählten Methode,
alle neun Messwerte, reproduzierbares Rauschen und Reset werden verglichen.
Gleichzeitig lebende Instanzen verwenden verschiedene Integratoren und
Parameter; ein Euler-Schritt der zweiten Instanz erhält die erste unverändert.
Negative, zu große und nichtganzzahlige Auswahlen sowie die Verletgrenzen
werden für jede Vorlage provoziert.

Intel macOS 14.6.1/Apple Clang 16 besteht die abschließenden vier Fälle
`language_experiment`, `derived_reference`, `pendulum_template_analysis` und
`pendulum_medium` 4/4 (`run-0w_kykim`). Debian 12/GCC 12.2 besteht dieselben
4/4 (`run-4mgo3rcq`). Linux/Clang 14 ASan/UBSan besteht die vollständige
Medium-/Methodengegenprobe 1/1 (`run-1nmkd17f`, `detect_leaks=0`).
Eine zunächst fest gebliebene RK45-Metadatenangabe wurde vom neuen Test
entdeckt und korrigiert; davor fehlgeschlagene Läufe gelten nicht als Nachweis.
Die neun geänderten Implementierungs-/Testeingaben stimmen zwischen macOS
und Linux per SHA-256 überein.

Die tatsächlichen App-Workflows bestehen macOS 2/2 (`run-5aoagl1r`) und
Linux 2/2 (`run-djqqn6dr`). Das C-/Physim-Mediumfenster wählt `integrator=2`
im Parameterformular, speichert die Auswahl und prüft sie zusammen mit der
RK4-Metadatenangabe, Messkanälen, Kräften und Auswertung. Die adaptive
Parameterstudie bleibt für beide Sprachen einschließlich cm-Eingaben und
Wiederöffnung erfolgreich. Windows/Apple Silicon und neue vollständige
SDK-/App-Pakete wurden für diese Änderung nicht ausgeführt.

PP-0764 und PP-0773 besitzen damit begrenzte konkrete Nachweise. PP-0756,
aktuelle vollständige Plattform-/Paketabnahme und Gesamtplan bleiben offen.

## Verschobene aktuelle SDK-Pendelketten

Am Implementierungsstand `a0592d5` wurden neue Release-SDKs ohne Oberfläche
mit allen kompilierten C-/Physim-Beispielen tatsächlich gebaut: Intel macOS
14.6.1/Apple Clang 16 und Debian 12/GCC 12.2. Die aktuelle Pendelgegenprobe
liegt im unabhängigen Kit und wird nun auch vom vollständigen SDK-Prüfer
aufgerufen. `--pendulum-only` führt denselben Teil isoliert aus. Der alte
adaptive SDK-Probe erwartete sechs Kanäle; er prüft jetzt neun Kanäle,
Geschwindigkeitsnamen/-einheiten und Kinematik. Allgemeine Sprachberichte
behalten ihre eigenen zwei Plots ohne Tabellen; sie werden ausdrücklich
nicht mit den gesonderten Pendelberichten verwechselt.

Das Kit umfasst 146 explizite Prüfeingaben, kein Physim-Core und keine
öffentlichen SDK-Header. Seine vollständigen Bytes wurden beim Entpacken
gegen das enthaltene SHA-256-Manifest geprüft. Der Kit-Selbsttest besteht
mit Grenzen, fehlenden/dynamischen Pfaden und exklusiver Veröffentlichung.
Die Ausführung erfolgte ausschließlich mit dem aus dem Kit entpackten
Prüfer; Compilerquellen und SDK-Header stammen aus dem verschobenen Paket.
Das Paketmanifest wird vor dem ersten Compile vollständig geprüft.

Je System werden installierte und aus Paketquellen neu gebaute Bibliothek
getrennt geprüft. Sechs allgemeine Modelle werden mit fünf ausgewählten
Verfahren, Vakuum und zulässigem Medium ausgeführt. Ein unabhängiger
PSRUN-Decoder prüft CRCs, Footer, Metadaten, neun Kanalnamen, SI-Dimensionen,
Kinematik, Tangentialbedingung und Energie. Euler/symplektischer Euler
besitzen eigene erste Schrittformeln. Unzulässige Auswahlwerte und Verlet
mit Luftwiderstand scheitern erwartungsgemäß. Je Bibliothek werden außerdem
zwölf adaptive C-/Physim-Berichte gegen den aktualisierten SDK-Probe geprüft.

Beide Lernpfadmodelle durchlaufen je Bibliothek die vollständige bestehende
Gegenprobe: 40010 primäre Messzeilen, fünf Integratoren, Szenen, unabhängige
nichtlineare Perioden-/Taylorreferenzen, Verfeinerung, adaptive Zeiten und
beide gemischten Analysesprachen. Drei Analysemodule erzeugen jeweils 18
Referenzberichte mit 400 Spitzen, vollständigen CSVs, Wachstum, ungleichen
Zeiten, Plateaus, Ruhe und nur einer Spitze. Positive-Spitzen-API und reale
Sprachbindung werden ebenfalls gegen jede Bibliothek ausgeführt.

Die abschließenden verschobenen SDK-Prüfungen bestehen vollständig:

- macOS: `build/pendulum-sdk-kit-final-proof-mac/Native SDK ä u1wizah6`.
- Linux: `build/pendulum-sdk-kit-final-proof-linux/Native SDK ä wr6n9hd4`.

Beide besitzen `PASSED.txt` und vollständige `verification.log`. Die getrennten
C-Lernpfadquellen im Consumer wurden zusätzlich gegen die Paketmanifest-Hashes
geprüft. Ein vorheriger macOS-Verifierentwurf verwendete kollidierende
Dateinamen für C- und generierte Physim-Lernpfadquellen; dessen Lauf gilt daher
nicht als C-/Physim-SDK-Nachweis. Die abschließenden Läufe verwenden getrennte
Dateien und unabhängige echte C-/Physim-Module.

Die Pakete sind Headless-SDKs; sie enthalten keine App oder Projektbuilder.
Dies ist keine vollständige neue SDK-/GUI- oder Plattformmatrixabnahme.
Windows und Apple Silicon wurden für diese Gegenprobe nicht ausgeführt.
Der ältere offizielle Linux-Paketlauf `37780798990` am Stand `66ea2bf` besitzt
zusätzlich tatsächlich erfolgreiche Paket-, Debian-12- und Ubuntu-24.04-Jobs;
er ist ausdrücklich kein Nachweis für die späteren Pendeländerungen.
PP-0756 bleibt unvollständig, und die unveränderten 531 Originalplanblöcke
bleiben Grundlage der weiteren Umsetzung.

## Tatsächliche Medium-Pendelexporte aus der App

Der vorhandene C-/Physim-Mediumworkflow endet jetzt nach den tatsächlich
betätigten Exportknöpfen für Abnahmetabelle als CSV, Energiediagramm als SVG
und PNG sowie sämtliche Rohmessdaten als CSV. Die App behält den bisherigen
Exportpfad; lediglich die Bounds des Rohdatenknopfes werden für die tatsächliche
Eingabegegenprobe erfasst. Berichtansicht, Projektbuild, Parameterwahl, isolierter
Runner, Simulation, finalisierte Daten und beide Analysesprachen liegen in
jeweils derselben tatsächlichen Projektkette.

`tests/check_pendulum_exports.py` dekodiert die Originaldatei unabhängig vom
Physim-Reader. Er prüft CRCs, Footer, alle neun Kanalnamen/Dimensionen, Lauf-
und Parameterdaten, Kinematik, Energie und Sensorrauschstatistik. Die über den
Appknopf exportierte Rohdaten-CSV stimmt in jeder Originalzeile und Spalte
exakt überein. Vollständige Energie-/Änderungs-CSVs, lokale positive Spitzen,
alle Dekremente/Raten und sämtliche Werte der exportierten Zusammenfassung
werden ebenfalls unabhängig verglichen. Die gespeicherte Datei bleibt während
der Gegenprobe per SHA-256 unverändert.

Die manuell über die App angeforderten SVGs werden als XML gelesen und auf
Energietitel/Einheit geprüft. Die PNGs in der tatsächlichen Standardgröße
2400 × 1700 besitzen geprüfte Chunk-CRCs, IEND, vollständige dekodierte RGB-
Scanlines und Bildinhalt. Ein tatsächliches C-Energie-PNG wurde visuell geprüft:
Titel, Zeit-/Jouleachsen, abnehmende Energie und Quellenzeilenzahl sind lesbar.
Die Darstellung bezeichnet ihre reduzierten Berichtsdaten ausdrücklich;
volle Originaldaten stehen in den getrennten CSVs.

Intel macOS 14.6.1/Apple Clang 16 besteht `pendulum_medium_workflow` 1/1
(`run-1apm3hbv`) mit tatsächlichem C- und Physim-Projekt. Debian 12/GCC 12.2
besteht denselben Workflow 1/1 (`run-41feacy4`). Die abschließende unabhängige
Gegenprobe einschließlich der danach ergänzten vollständigen Energie-CSV-
Vergleiche wurde gegen alle vier tatsächlichen Projekte ausgeführt:

- macOS C: 3070 originale Rohdaten-/Energiezeilen.
- macOS Physim: 3091 originale Rohdaten-/Energiezeilen.
- Linux C: 3157 originale Rohdaten-/Energiezeilen.
- Linux Physim: 3035 originale Rohdaten-/Energiezeilen.

Alle vier besitzen zwei echte positive Spitzen und ein positives beobachtetes
Abnahmeintervall. Unterschiedliche Zeilenzahlen stammen vom interaktiv
angeforderten Stop nach mindestens sechs Sekunden; die Prüfung vergleicht
jedes Projekt gegen seine vollständige eigene Zeitachse. Die fünf geänderten
Implementierungs-/Prüfeingaben stimmen zwischen beiden Systemen per SHA-256
überein. Zunächst zählte der neue Prüfer auch ein automatisch von der C-Analyse
geschriebenes SVG mit; die Gegenprobe wurde auf die tatsächlichen `-diagramm`
Buttonexporte korrigiert. Die zunächst fehlgeschlagenen Gesamtläufe werden
nicht als erfolgreiche Nachweise gezählt.

PP-0777 ist damit konkret nachgewiesen; PP-0779/0780 besitzen zusätzliche
Medium-/Exportnachweise. Windows/Apple Silicon und neue gesamte Pakete wurden
für diese Änderung nicht ausgeführt. Die gesamte vertikale Plattformabnahme
und der Gesamtprojektplan bleiben offen.

## SDK-Studiengate und aktuelle Plattformnachweise

Der offizielle C17-Lauf `37800124412` am Stand `7fea5f2` besteht unter
Linux/GCC und Linux/Clang jeweils alle 681 nativen Fälle (`run-fy9xavxm`,
`run-frebudc9`). Beide nachfolgenden SDK-Prüfungen scheitern konkret am
unveränderten `tools/sdk_series_probe.c`: Die tatsächlich erzeugte adaptive
Pendelstudie besitzt neun Kanäle; der Probe erwartet noch sechs. Die Archive
`linux-ui-gcc` und `linux-ui-clang` wurden vor dem Lesen gegen die offiziellen
SHA-256-Digests geprüft:

- GCC: `ea9b8847ad07bc902591ce8b4e8c9022ed0738ae5e8b4c2fea84c4f5c6297578`.
- Clang: `9cb55d724356106927041ea911cafef248c87bba985c0b8ded1cc80806655301`.

Die Studiengegenprobe erwartet jetzt neun Kanäle und prüft sämtliche Namen,
Geschwindigkeitseinheiten/-dimensionen und Komponenten/Betrag an jeder
Originalzeile. Die bisherigen PCHIP-, Sweep-, variablen Schrittzahl-, exakten
Endzeit-, Energie-, Studienkurven- und SVG-Prüfungen bleiben bestehen.
Die unabhängige Pendel-SDK-Prüfung führt zusätzlich denselben tatsächlichen
Batchpfad mit sechs Modellen, je drei Längen und drei Workern aus, gegen
installierte und aus Paketquellen neu gebaute Bibliothek.

Die aus dem bytegeprüften unabhängigen Kit entpackte Gegenprobe besteht:

- Intel macOS/Apple Clang 16:
  `build/target-pendulum-proof-mac/Native SDK ä bh2do0yk`.
- Debian 12/GCC 12.2:
  `build/target-pendulum-proof-linux/Native SDK ä mpym0irx`.

Je System bestehen zwölf komplette Studien mit insgesamt 36 Läufen, alle
bisherigen Pendel-SDK-Referenzen und beide Bibliotheken. Das Kit besitzt
weiterhin 146 unabhängige Eingaben; sein Selbsttest besteht. Dies belegt die
konkrete Korrektur des fehlgeschlagenen Gates, nicht den gesamten danach
folgenden SDK-/GUI-Lauf.

Derselbe offizielle Stand `7fea5f2` besteht unter Windows/MSVC Debug und
Clang-Cl Debug jeweils 681/681 native Fälle (`run-7wyphq5w`, `run-dx0a1n89`)
sowie jeweils 10/10 gezielte Sanitizerfälle (`run-w7j5mk58`, `run-mu2544k5`).
Die Artefaktdigests wurden vor der Auswertung geprüft:

- MSVC Debug: `f3f09fdb33c9db9cad236ab333b4554307f2a462bf2b163f2ccb0df029e7a4b7`.
- Clang-Cl Debug: `a90e2ff57a77f041a1dc2fb8bed876253a0a5c900329dd3ae745ac853651f7d9`.

Absichtlich scheiternde Nested-Runnerfälle bleiben Teil der Selbstprüfung,
keine Produktfehler. Windows/Clang-Cl Release scheitert laut offiziellem
Jobstatus ebenfalls im Schritt „Native SDK package and relocation“; sein
konkretes Diagnoseartefakt war noch nicht lesbar, daher wird keine identische
Ursache behauptet. Die macOS-Matrixjobs waren beim letzten Abruf weiterhin
queued. Eine vollständige neue Matrix-/Release-Paketabnahme ist nicht bewiesen.

Am Stand `35793ee` wurden beide lokalen vollständigen nativen Release-Läufe
mit SDL gestartet. Intel macOS/Apple Clang 16 besteht alle **681/681** Fälle
in `run-q5rzlvo1` (`build/current-complete-native-mac.log`). Linux ist noch
nicht als erfolgreicher Gesamtlauf abgenommen. Alle 1295 Git-Dateien wurden
zuvor zwischen Mac und VM per SHA-256 abgeglichen. Die während der laufenden
Nativsuite korrigierten SDK-Harnessdateien sind keine Compiler-Eingaben
jener Nativfälle und werden separat wie oben geprüft.
Beim Linux-Lauf übertrug das macOS-Tar zusätzlich 735 AppleDouble-Begleitdateien;
dadurch scheiterte die Headerinventur des Dokumentationschecks. Diese
Transferdateien wurden nach Magic-/Gegenstückprüfung bytegeprüft unter
`build/source-transfer-metadata.tar.gz` gesichert und ausschließlich daraus
bereinigt. Der direkte Referenzcheck besteht danach mit 31 Seiten. Der bereits
fehlgeschlagene Gesamtlauf wird nicht als bestanden gezählt oder verschwiegen.

## Wiederherstellbare ältere Linux-SDK-Nachweise

Die älteren Prüfverzeichnisse `build/project-manager-keyboard-sdk-proof-linux`,
`build/saved-run-tutorial-sdk-proof-linux` und
`build/pendulum-tutorial-sdk-final-linux` sind vollständig unter
`/Users/lulus/Projects/physim/build/historical-linux-sdk-proofs.tar.gz`
auf dem Mac erhalten. Das Archiv ist 457044705 Bytes groß, umfasst 17060
reguläre Dateien mit ursprünglich 1692191898 Bytes und besitzt SHA-256
`cdf5854385d6ab93b411f602c8de5e8d318dcca1df685ada3c94d8c8f8661886`.
Die nebenliegende `historical-linux-sdk-proofs-receipt.json` enthält jede
Originaldatei mit Größe/Prüfsumme und den Wiederherstellungspfad. Die Receipt
liegt auch in der VM. Alle Archivmitglieder wurden vollständig gegen die
Originale geprüft; vor der Bereinigung wurden die Originaldateien nochmals
vollständig abgeglichen. Nur danach wurden diese drei VM-Verzeichnisse entfernt.
Die Dateien können mit ihren ursprünglichen projektbezogenen Archivpfaden
wiederhergestellt werden. Die einzige Archivkopie liegt auf dem Mac; das
Archiv bleibt deshalb erhalten. Linux hatte danach wieder rund 2 GB Platz.

Die zusätzliche Platzbereinigung auf dem Mac bewahrt vollständig die älteren
Verzeichnisse `build/Quantity sums SDK ä mac cv7rk3x8` und
`build/Linear systems SDK ä mac izmu2853` unter
`build/historical-mac-sdk-proofs.tar.gz` auf dem Mac.
Das Archiv enthält 29148 reguläre Dateien mit ursprünglich 1495530695 Bytes,
ist 431791069 Bytes groß und besitzt SHA-256
`0381599e5db1fe5b9a02f03958820b3c21cc26bf92835cce6e8c0e344a5d07dd`.
Die Receipt liegt auf beiden Systemen. Alle Archivmitglieder und anschließend
sämtliche Originale wurden byteweise gegen ihre Prüfsummen abgeglichen;
erst danach wurden ausschließlich diese zwei Mac-Verzeichnisse entfernt.
Die Archivkopie wurde vollständig per SHA-256 zurück auf den Mac geprüft,
bevor die VM-Kopie entfernt wurde. Die einzige Archivkopie liegt jetzt auf dem
Mac und bleibt deshalb erhalten.
Die nebenliegenden Receipts und `build/app-profiling-proof-locations.json`
dokumentieren beide Archive und die Wiederherstellungspfade.

Ein neues vollständiges macOS-App-SDK wurde tatsächlich gebaut unter
`build/current-complete-sdk-mac`. Seine erste komplette Prüfung in
`build/current-complete-sdk-proof-mac/Native SDK ä ljx1_xif` scheiterte beim
Anlegen der Datei `phys-3.psrun`, während die verfügbare Mac-Kapazität zuvor
auf ungefähr 124 MB gefallen war. Der Runner meldete nur den allgemeinen
Run-Create-Fehler; Speicherplatz als genaue Ursache ist damit nicht bewiesen.
Der gleiche Runner-/Modul-/Verletfall besteht direkt mit 4001 Samples unter
einem neuen Dateinamen. Der Fehlversuch bleibt erhalten und zählt nicht als
Gesamtpaketnachweis. Nach der verifizierten Archivierung hatte der Mac rund
4,3 GB frei; ein neuer vollständiger SDK-Lauf wurde in einem neuen, getrennten
Prüfverzeichnis gestartet. Dieser bleibt bis zu seinem tatsächlichen Ende offen.

Der erste lokale Linux-Release-Gesamtlauf endet mit **680/681** in
`run-8axb5g8f`. Ausschließlich `documentation_reference` scheitert wegen der
oben belegten AppleDouble-Transferdateien; alle nativen Compiler-/Laufzeitfälle
bestehen. Der direkte Dokumentationscheck besteht nach deren Bereinigung.
Ein neuer vollständiger Linux-Lauf auf dem bereinigten Baum mit den korrigierten
SDK-Prüfern wurde gestartet und bleibt bis zu seinem Ende offen. Die 680/681
werden ausdrücklich nicht zu einem bestandenen Gesamtlauf umetikettiert.


## Ausgeführte Verfahren und störfeste Migrationstests

Die unverkürzten lokalen Release-Nativsuiten des Ausgangsstands bestehen
auf Intel macOS 14.6.1/Apple Clang 16 **681/681** in
`build/contact-world-language-release-mac/test-results/run-q5rzlvo1` und auf
Debian 12/GCC 12.2 **681/681** in
`build/contact-world-language-release-linux/test-results/run-r9u97k77`.
Der zweite Linux-Lauf verwendet den bereinigten Transferbaum; der frühere
680/681-Versuch bleibt als Fehlversuch erhalten.

Ein tatsächlicher allgemeiner C-Lauf mit `integrator=0 --adaptive` speicherte
korrekt Dormand–Prince als adaptive Methode, wurde im Lernpfadbericht jedoch
als Euler beschriftet (`build/pendulum-label-repro/oldreport.psreport`).
C- und Physim-Auswertung wählen jetzt bei `step_mode=adaptive` die ausgeführte
adaptive Methode und kennzeichnen sie als adaptiv. Ältere feste Läufe bleiben
lesbar; ältere adaptive Lernpfadläufe werden nur beim belegten RK45-Verfahren
akzeptiert. Unbekannte oder fehlende adaptive Methoden anderer Verfahren
werden abgewiesen, ohne einen fertigen Bericht zu veröffentlichen.

`pendulum_report_labels` prüft acht tatsächlich gerechnete gemischte C-/Physim-
Läufe gegen unabhängig vorgegebene Legenden in beiden Plots und allen drei
Tabellen. Hinzu kommen alte feste Metadaten, der ältere adaptive Lernpfad und
vier abgewiesene Berichte. Die vier betroffenen nativen Pendelfälle bestehen
auf macOS **4/4** in `build/pendulum-template-native-mac/test-results/run-c923sd_m`
und Linux **4/4** in `build/pendulum-template-native-linux/test-results/run-66ap9b1a`.
Der tatsächliche Lernpfad-Appfall besteht jeweils **1/1** in
`run-_79kpcta` (macOS) und `run-1p5xz0wj` (Linux), jeweils unter dem
`contact-world-language-release-*`-Testverzeichnis. Linux/Clang 14 Debug mit
ASan/UBSan besteht die neue Legendenprüfung **1/1** in
`build/atspi-asan-linux/test-results/run-zvu17tpk`;
`ASAN_OPTIONS=detect_leaks=0` bleibt auf SDL/Mesa-Leaks begrenzt.

Der zweite vollständige alte macOS-SDK-Versuch in
`build/current-complete-sdk-proof-mac/Native SDK ä 0qyzod7z` besteht die
vorangehenden Library-/Modell- und neun Appabläufe sowie Dokumentations-,
Typografie- und Einstellungstests, scheitert aber beim normalen Migrationstest
in Stufe 3. Seine Ergebnisse sind kein vollständiger SDK-Nachweis. Der
Migrationstest fehlte im vorhandenen Filter für fremde Eingabeereignisse.
Gezielt eingestreute native Mausbewegungs-/Fokusereignisse reproduzieren den
Stufe-3-Fehler vor der Korrektur (`build/migration-noise-before-mac.log`).
Die Filterergänzung gilt für den ausdrücklich gestarteten Testmodus.
Der reguläre Produkt-Eingabepfad bleibt unverändert. Eine Aufzeichnung des
ursprünglichen externen Ereignisses liegt nicht vor.

Die dauerhafte Gegenprobe führt zwölf tatsächliche Migrationen durch:
zwei Fenstergrößen, mit/ohne fremde Ereignisse, jeweils normal, extern
geändert oder mit ungespeicherten Einstellungen. Sie prüft Backup und
Manifest byteweise sowie erhaltene Quellen/Laufdaten und beide Schutzfälle.
Der native Katalogfall besteht auf macOS **1/1** in `run-c_zd5i8i` und Linux
**1/1** in `run-znf7v7vt`, jeweils im `contact-world-language-release-*`-
Testverzeichnis. Die separat ausgeführten zwölf Fälle bestehen ebenfalls
auf beiden Systemen (`build/migration-integrated-proof-{mac,linux}.log`).

Die unabhängige SDK-Prüfmappe enthält jetzt 148 verifizierte Eingaben und
prüft die Verfahrenslegenden gegen installierte und neu gebaute Bibliotheken.
Ein neues App-SDK ist unter `build/report-method-migration-sdk-mac` gebaut.
Seine vollständige Prüfung in
`build/report-method-migration-sdk-proof-mac/Native SDK ä u9ms4eew` endet mit
Fehler im Projektil-Appfall, Stufe 62 beim Wiederöffnen eines gespeicherten
Berichts. Zuvor bestehen die Library-/Modellprüfungen einschließlich der
neuen Legendenkontrolle gegen beide Bibliotheken, adaptive Studien, alle neun
neu gebauten Beispielprojekte und der Pendel-Appablauf. Das SDK besitzt kein
`PASSED.txt` und zählt nicht als vollständiger Paketnachweis. Der Fehler und
`verification.log` bleiben erhalten; die ursprüngliche Ereignisursache ist
noch offen.


## Frische Linux-Pakete und Windows-SDK am Studiengate

Die Paket-CI am Commit `40712ce` besteht auf frischen Ubuntu-24.04- und
Debian-12-Systemen. Die offiziellen Artefakte wurden vor dem Lesen anhand
ihrer API-SHA-256-Digests geprüft:

- `build/sdk-gate-installed-linux-ubuntu-24.04.zip`:
  `839f76f46b930dd7e7464c827b23a29d63fe91d866142fb8a7fff34111ca6b77`;
  vollständiger SDK-Nachweis in `Native SDK ä 4egw6igf/PASSED.txt`.
- `build/sdk-gate-installed-linux-debian-12.zip`:
  `bef76388b2b63c57a3cc2a149a9cdefcf5c11f22a59ff98c012507d30914eef3`;
  vollständiger SDK-Nachweis in `Native SDK ä eueq3f7v/PASSED.txt`.

Beide Archive enthalten tatsächlich ausgeführte App-, Tastatur-,
Projektmanager- und alte Migrationstests sowie eigene bestandene Zenity-
Dateidialogprüfungen mit Unicodepfaden, Abbruch und fehlendem Backend.
[CI-Paketlauf](https://github.com/PhysicSimulator/physim/actions/runs/37809527308).
Die zwölf neuen störbehafteten Migrationen und Verfahrenslegenden gehören
zum späteren Commit `005339a` und werden diesen älteren Paketen nicht zugerechnet.

Auch Windows/MSVC Release besitzt am Commit `40712ce` eine bestandene
vollständige SDK-Prüfung ohne App-Displaytests in
`native/Native SDK ä avl5n_zi/PASSED.txt`. Das zugehörige offizielle Archiv
`build/report-label-windows-v143-release.zip` ist gegen SHA-256
`f1dc4065340ee61614388cdb3c15c747e000f47b3997c47e5a13ae5a62883840`
geprüft. Der [gesamte Windows-Job](https://github.com/PhysicSimulator/physim/actions/runs/37809527284/job/113422452838)
scheitert erst im separaten Schritt `Record verified Release workloads`.
Die konkrete Benchmarkursache ist nicht belegt; der Joblogzugriff liefert
HTTP 403 und es liegt kein Benchmarkartefakt vor. Dieser Job wird trotz
bestandenem SDK ausdrücklich nicht als vollständig erfolgreich bewertet.


## Fremde Ereignisse beim Wiederöffnen im allgemeinen Appskript

Der fehlgeschlagene SDK-Projektilfall oben besitzt eine kontrollierte
Gegenprobe: `build/workflow-noise-before-absolute-mac.log` reproduziert exakt
Stufe 62 mit leerem Bericht nach fremden Mausbewegungs-, Freigabe-, Rad- und
Fokusereignissen unmittelbar nach dem tatsächlichen Öffnen-Button.
Der allgemeine `--self-test`-Ablauf fehlte ebenfalls im vorhandenen Filter.
Er ist jetzt ausdrücklich eingeschlossen; reale Nutzerabläufe und die
bestehenden absichtlich ungefilterten Dialog-/Dokumenttests ändern sich nicht.
Der ursprüngliche native Ereignisstrom des SDK-Fehlversuchs wurde nicht
aufgezeichnet; die kontrollierte Gegenprobe beweist diese Fehlerklasse.

Der neue Katalogfall `workflow_pointer_isolation` öffnet jeweils den
ursprünglichen Bericht und Lauf mit genau diesen Ereignissen erneut, bei
1440×940 und 1080×740. Er prüft die vollständigen bestehenden Appablauf-
Assertions sowie beide tatsächlichen Ereignisinjektionen, Screenshotdateien
und erhaltene zusammengehörige Lauf-/Berichtsartefakte. Intel macOS besteht **1/1** in
`build/contact-world-language-release-mac/test-results/run-fn2v62mb` und
Debian/GCC **1/1** in
`build/contact-world-language-release-linux/test-results/run-x6jkhejg`.
Die Logs liegen unter `build/workflow-input-isolation-{mac,linux}-proof.log`.
Ein früherer Harnessversuch ohne angelegten Elternordner und ein Linux-
Aufruf mit falschem SDL-Prefix starteten die Gegenprobe nicht erfolgreich;
sie werden nicht als Produktnachweise gezählt.

Der vollständige SDK-Prüfer übernimmt dieselbe Gegenprobe. Das neue App-SDK
`build/workflow-input-sdk-mac` ist tatsächlich gebaut; die vorherige
SDK-Gesamtprüfung bleibt fehlgeschlagen. Eine nachfolgende vollständige
Prüfung wird erst nach ihrem tatsächlichen Ende bewertet.


Die neue unabhängige Prüfmappe besteht ihren Paketgrenzen-Selbsttest mit
**149** exakten Eingaben, SHA-256, fehlenden/dynamischen Pfaden und exklusiver
Veröffentlichung (`build/workflow-input-kit-selftest.log`). Die extrahierten
149 Dateien wurden vollständig gegen ihr Manifest abgeglichen. Die neue
vollständige macOS-App-SDK-Prüfung läuft aus dieser Mappe unter
`build/workflow-input-sdk-proof-mac`; ihr Log ist
`build/workflow-input-sdk-full-mac.log`. Solange sie nicht erfolgreich endet,
bleibt dieser vollständige Paketnachweis offen.
