# Native C-Vervollständigung mit lokalen Namen — 5. November 2026

Die zuvor geprüften C-Scopes sind jetzt an das Completion-Modell und die native
Oberfläche angeschlossen. In Modell, Analyse und eigenen C-Dateien erscheinen
sichtbare Variablen und Parameter vor den API-Namen, auch im leeren Argument nach
`(` bzw. `,`. Die fünf sichtbaren Zeilen begrenzen weiterhin nur die Ansicht.
Innere Deklarationen überdecken äußere Namen und gleichnamige API-Einträge.

Die Kurzinfo zeigt den geschriebenen Typ, die Deklaration und die tatsächliche
Datei/Zeile. Beispielsweise bleiben `double *velocity` oder
`int (*callback)(double nested)` echte Deklaratorformen. Lokale Namen öffnen
keinen erfundenen API-Handbucheintrag, auch beim direkten Referenzaufruf im
Editor. Einfügen ersetzt nur den Bezeichnerbereich; Undo stellt ihn mit einem
Schritt wieder her. Das freigegebene D-Layout und die gemeinsamen Syntaxrollen
werden weiterverwendet. UTF-8-/XID-Namen, C-`$` und geschriebene UCN-Escapes haben
gemeinsame Bezeichnergrenzen in Abfrage und Syntaxanzeige; keine UCN-Normalisierung.

Das Modell besitzt Quelle, Typ-/Signaturtexte und stabile lokale IDs. Sein Cache
berücksichtigt Besitzer, Sprache, Cursor und sämtliche Quellbytes. Speicherfehler
erhalten den vorherigen Zustand. Die C-Aufrufermittlung verwendet echte
Grammatik-/Erholungsknoten und originale Klammerpositionen; verschachtelte
Kommata, Indizes und bedingte Ausdrücke schalten nicht den äußeren Parameter
weiter. Ein gemeinsamer C-Guard unterdrückt Kommentare/Literale und berücksichtigt
Zeilenfortsetzungen. Ein schneller Hinweis plant nur die verzögerte Abfrage;
der tatsächliche Aufrufkontext bestätigt sie vor dem Anzeigen.

**Noch keine separate Aufrufsignatur-/Parameterhilfe.** Deren eigener Ablauf,
Benutzerfunktionen und manueller Zugang bleiben offen. Ebenso keine Präprozessor-,
Include-, semantische Typ-/Wertauflösung oder vollständige C-Compilerannahme.
Die Laufzeit ist ausschließlich in App/Tests eingebunden; Core bleibt unabhängig.

## Tatsächlich ausgeführt

[Kommandos, Originalergebnisse, Quell-/Artefakthashes, Bilder und Grenzen](completion-c-2026-11-05.json).
Windows Release/MSVC und Linux Debug/GCC ASan/UBSan jeweils **Unit 8/8 + GUI 6/6**.
Windows `run-p7ftjck6`/`run-mxoufvz4`, Linux `run-kp55mj_d`/`run-pqjm5dl2`.
Unit- und GUI-Phasen verwenden pro Plattform dieselben tatsächlichen Appbytes.

Units: C-Scopes, 38 C-Aufrufkontexte, Completion-Abfrage, Physim-Locals, C-Locals,
Kurzinfo, Syntax und API-Referenz. C-Modell: **121/121** Allokationsfehler erhalten
Snapshot/Besitzer und geben genau frei; Neuversuche bestehen. Geschriebene Typen,
exakter Signaturnamenbereich, Unicode/UCN, Sprach-/Besitzerwechsel, Cache und volle
Trefferzahl sind geprüft. Die bestehenden 35 C-Scope-Fälle, 182 Cursor-/Dokumentpaare,
65 Scope-Allokationsfehler und 1.024 fehlerhaften Byteeingaben bestehen ebenfalls.
Linux aktiviert für die drei C-/Modellfälle ausdrücklich Leak-Prüfung; keine
pauschale leakfreie GUI behauptet. Zusätzliche direkte strenge C17-ASan/UBSan-
Prüfungen bestehen ohne Warnungen; reguläre Builds behalten Bestandswarnungen.

GUI: manuelle/automatische Completion, erzwungene Info-Frist, Kurzinfo, API-Referenz
und lokale Namen. Der Locals-Fall führt C **und** Physim jeweils über Modell,
Analyse und eigene Datei: echte Namens-/Quellglyphen, Unicode-Präfix in Semibold,
lokale Karte ohne falschen Handbuchlink, API-Abschirmung, Ersetzen/Undo, Scopes,
alle Treffer und automatische leere Argumente. Sechs neue C-Aufnahmen unter
Windows/Linux angesehen: dunkel/12 px, hell/22 px, hoher Kontrast/12 px, 1080×740.
Die unveränderte Physim-Ansicht erzeugt dafür keine neuen Bilder. Bekannte
Explorer-Umbrüche bei 22 px bleiben offen; keine physische DPI-/Caret-/Scrollabnahme.

Die Vorherregression am alten C-Modell kompiliert ohne Warnungen, lehnt den neuen
Fall aber mit Exit 1 ab: C-Namen fehlten. Die erste Aufrufprobe erkannte einen
offenen Blockkommentar nicht; der gemeinsame Guard behebt dies. Der erste
Gesamtbuild scheiterte zusätzlich an einer versehentlich entfernten, vom
Zahlenlexer noch verwendeten Hilfsfunktion; nach Wiederherstellung bestehen die
obigen Prüfungen. Diese Aufbaufehler sind **keine** native GUI-Vorherabnahme.

Windows-Installation: alle **6.422** Manifestdateien tatsächlich gelesen, App
identisch zum geprüften Windows-Build; Runtime-MIT, C-Grammatik-MIT und eingebettete
ICU-Lizenz bytegenau vorhanden. ELF-/PE-Prüfung der Apps: keine dynamischen
Tree-sitter-Exporte. Snapshots `completion-c-first`/`completion-c-restored`: jeweils
281 eigene / 1.220 Compiler-/Testeingaben; alle referenzierten Archivbytes gelesen.
Keine Wiederholung geschlossener F1-/Protokoll-/Settings-Matrizen. Gehosteter
Der Nutzer-Mac blieb aus und unberührt; gehosteter Mac-Abschluss siehe unten.

Gehosteter Intel-Nachlauf `41960990299` / Job `127027035424` zu Quellcommit
`694bb586`: **8/8** tatsächlich bestanden, Original-ZIP/Digest/alle Einträge gelesen;
C und Physim jeweils über drei Editorziele. [Intel-Originalauswertung](completion-c-macos-intel-2026-11-05.json).
Anschließend ARM-Job `127027035143` ebenfalls **8/8**. Beide Original-ZIPs,
Digests und alle Einträge gelesen; C/Physim in beiden Architekturen über je drei
Ziele bestätigt. [Gemeinsamer Mac-Abschluss](completion-c-macos-2026-11-05.json).
Daraus keine gesamte Mac-, physische oder neue Mac-Allokationsfehler-Abnahme ableiten.

Quellcommit **`694bb586`** in beiden Repositories gepusht. Alle **1.239** erfassten
Eingaben gegen tatsächliche Git-Blobs gelesen, Vendor bytegenau, sonst nur CRLF/LF
normalisiert. Die Benutzeranleitung korrigiert anschließend lediglich den früheren
C-Verfügbarkeitssatz; Überschriften und Links bleiben gleich. Diese spätere reine
Textkorrektur ist getrennt vom Quellabgleich dokumentiert.
[Commitabschluss und Mac-Anschluss](completion-c-commit-2026-11-05.json).

28 bytegleiche Bildduplikate/**89.514.264 Bytes** entfernt; sechs neue C-Referenzen
und sämtliche Originalergebnisse erhalten. Die automatische Freigabeprüfung lehnte
den kombinierten rekursiven SDK-/Bild-Löschschritt ab („blocked by policy“, ohne
weiteren Grund). Danach wurden nur die einzeln geprüften Bildduplikate entfernt;
die geprüfte SDK-Kopie bleibt bestehen und wird nicht durch einen anderen Löschweg
umgangen. [Bereinigung und genaue Grenzen](completion-c-cleanup-2026-11-05.json).
