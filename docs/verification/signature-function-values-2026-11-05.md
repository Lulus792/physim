# Parameterhilfe für ausdrücklich typisierte Funktionswerte

Die Parameterhilfe behandelte `callback: func(Float64, Vec3) -> Float64` wie eine
gewöhnliche maskierende Variable. Der echte Aufrufpfad war bekannt, zeigte aber
keine Signatur. Eine neue Gegenprobe kompiliert am bisherigen Stand strikt und
scheitert präzise an `MASKED` statt der erwarteten aufrufbaren Signatur.

Die neue Abfrage `ps_lang_editor_function_type` verwendet die vorhandene echte
Typ-Produktion. Sie liefert vollständige Parameter- und Rückgabespannen aus einer
geschriebenen Annotation, auch bei Gruppierung, verschachtelten Funktionstypen,
Arrays und optionalen Parametertypen. Ein optionaler Funktionstyp, ein Array von
Funktionen oder ein bloßer Aliasname wird dadurch selbst nicht aufrufbar.
Unvollständige/lexikalisch fehlerhafte Typen veröffentlichen keine Teilsignatur;
Ausgabe-, Knoten-, Tiefen-, NUL- und Quellgrößengrenzen sind geprüft. Die Abfrage
allokiert nicht und führt keinen Initialisierer aus.

Das bestehende Signaturmodell projiziert den tatsächlichen sichtbaren Bindungstyp
zu `callback(Float64, Vec3) -> Float64`. Die Annotation schreibt keine Parameternamen;
die Anzeige erfindet deshalb keine. Maskierung, Gültigkeitsbereich, Source-/Owner-
Bindung, Cache und Fehlerpfade bleiben erhalten. Eine später deklarierte gewöhnliche
Variable kann den Funktionswert weiterhin verdecken. C-Verträge bleiben erhalten.

## Ausgeführt

- **Windows Release und Alpine/X11 ASan/UBSan jeweils10/10 Daten+2/2 UI-Gruppen.**
  Parser, Checker, Modul-/Cursor-Signaturen, Aufrufziele, Locals und gemeinsames
  Signaturmodell; native Parameterhilfe und Variablenvorschläge.
- **50 Modellfälle**, darunter gruppierte/verschachtelte und nicht aufrufbare
  Annotationen, API-Namensmaskierung, fehlende Annotation und überzählige Argumente.
  **9/9 neue Allokationsfehler** erhalten den vorherigen Besitzstand, verbergen
  überholte Bindungen und erlauben anschließend erfolgreiches Wiederholen.
  Bisherige60C-/8Physim-/4geliehene Fehlerpfade bleiben erfolgreich.
- Der neue Parserpfad und das Signaturmodell bestehen separat **striktes C17 mit
  `-Wall -Wextra -Werror`, ASan/UBSan und Leakprüfung**. Die294 vorhandenen strikten
  AST-/Präfixvergleiche bleiben erfolgreich; zu kleine Parameterpuffer bleiben
  vollständig unverändert.
- **Sechs neue echte Zeichen-/Eingabesequenzen**: drei Physim-Editorziele je OS,
  Dunkel/Hell/hoher Kontrast sowie16/22px Oberfläche. Vollständige Syntaxrollen,
  ehrliche unbenannte Parameter, erstes/zweites aktives Argument und Fußzeile
  geprüft. Die vorhandene C-/Physim-Navigation, IME-/Auswahlunterbrechung,
  Maskierung, Escape-Reihenfolge und Fehlerveröffentlichung bleiben Teil der Gruppe.
  Für die Erweiterung wurde keine zusätzliche Screenshotserie eingeführt.

Die Benutzeranleitung behauptete noch, die separate Parameterhilfe sei nicht
verfügbar. [Arbeitsbereich](../workspace.md) beschreibt jetzt den tatsächlich
geprüften Zugriff, das Verhalten und die Grenzen einschließlich Funktionswerten.
Der geänderte Text wurde separat mit dem echten nativen Markdownparser und
ASan/UBSan/Leakprüfung gelesen (201Blöcke). Eine zusätzliche Werror-Probe dieses
unveränderten Dokumentationsparsers scheiterte an seiner bestehenden Warnung in
`documentation.c:411`; die normale native Warnungspolitik besteht. Beide
Originalbefunde bleiben erhalten; keine strikte Abnahme dieses alten Parsers behauptet.

[Befehle, Originalhashes und Ergebnisse](signature-function-values-2026-11-05.json).
Alle307 eigenen Eingaben und acht Code-/Teständerungen wurden vor Build in Root,
Isolation und Manifest geprüft. Danach wurde ausschließlich die separat geparste
Anleitung ergänzt; der abschließende Snapshot erfasst neun Änderungen und die
unveränderten geprüften Produktbinaries. Alle referenzierten Quell-/Runtimebytes
sind eingefroren und zurückgelesen. Fremdes AT-SPI-/Orca-WIP blieb unberührt.

## Neue gehostete F1-Befunde und nächste Aufgaben

Beide ursprünglichen Joblogs zur vorherigen Quelle **dc6d3264** wurden vollständig
gelesen: Ubuntu/Clang **26/26 Daten+48/48 UI**, GCC **26/26+47/48**. Fontbedarf und
native Linkfehler bestehen in beiden. GCC scheitert allein an
`test_accessibility_atspi.py:200`, `reader.queryComponent().grabFocus()` liefert false.
[Quellbindung und Einzelbefunde](f1-glibc-followup-2026-11-05.json). Dieser Lauf ist
vollständig terminal; keine erneute Beobachtung oder Wiederholung aus Kontext.
Es wurden hierfür keine großen Artefaktarchive zusätzlich heruntergeladen.

Der neue Funktionswertschritt braucht seine eigenen gehosteten Plattformnachläufe.
190 Spezial-/Property-/Katalogverträge, Intrinsics, Import-/Aliasauflösung,
schriftlich unbekannte Werte/Rückgaben, große C-Edits und die vollständige
physische/assistive/UI-Abnahme bleiben offen. Die früher dokumentierte private
Mac-Ausmeldung und alle Löschablehnungen bleiben verbindlich.
