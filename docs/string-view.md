# String-Views: Textbereiche ohne Kopie

`physim/string_view.h` beschreibt einen lesbaren Bereich durch `data` und `size`.
Der Bereich besitzt den Speicher nicht. Es gibt keine Allokation, versteckte
Terminatorsuche oder Änderung der Eingabe. Der Aufrufer hält die zugrunde liegenden
Bytes für die gesamte Nutzungsdauer gültig. Eine Freigabe oder Reallokation des
Originalspeichers macht auch sämtliche daraus abgeleiteten Views ungültig.

`ps_string_view_make(data, size, &out)` verwendet eine explizite Länge.
`ps_string_view_cstr(text, &out)` ist die einzige Funktion, die einen gültigen,
nullterminierten C-String erwartet und dessen Länge mit `strlen` bestimmt.
Ein NULL-Zeiger ist nur für einen Bereich der Länge null zulässig; `{0}` ist ein
gültiger leerer View. Die Prüfung eines Views kann nicht feststellen, ob ein
nichtleerer Bereich tatsächlich lesbar ist: Diese Speichergrenze garantiert der
Aufrufer.

## Bytes, Nullzeichen und UTF-8

Alle Längen und Positionen sind Byteanzahlen. Eingebettete Nullbytes zählen als
normale Daten. Vergleiche ordnen Bytes vorzeichenlos und sind unabhängig von Locale
und dem Vorzeichen von `char`. UTF-8 kann gespeichert und bytegleich verglichen
werden. Es gibt weder Unicode-Normalisierung noch sprachabhängige Sortierung,
Groß-/Kleinschreibung oder automatische Codepoint-Grenzen. Ein Teilbereich kann
deshalb mitten in einer mehrbyteigen UTF-8-Sequenz beginnen.

| Funktion | Ergebnis |
| --- | --- |
| `compare` | Lexikografische Ordnung -1, 0 oder 1 |
| `equal` | Bytegleichheit; ungültige Views sind stets ungleich |
| `slice` | Teilbereich mit geprüftem Start und Umfang |
| `find` | Erster Treffer ab einem Startindex; leerer Suchbereich trifft dort |
| `split` | Bereiche vor und nach dem ersten nichtleeren Trennzeichen |
| `trim_ascii` | Entfernt außen Leerzeichen, Tab, CR, LF, Form Feed und Vertical Tab |
| `copy` | Kopiert alle Bytes und hängt ein Nullbyte an |

Die vollständigen Funktionsnamen beginnen mit `ps_string_view_`. Ein nicht
gefundenes Such- oder Trennzeichen ergibt `PS_EOF`. Leere Felder am Anfang oder
Ende eines gesplitteten Bereichs bleiben erhalten. Bei `find` ist auch der Index
direkt hinter dem letzten Byte gültig; nur ein leerer Suchbereich trifft dort.
Die einfache Suchimplementierung benötigt im ungünstigsten Fall O(n*m) Vergleiche.

`copy` unterstützt überlappende Quell-/Zielbereiche. Der Zielpuffer benötigt
mindestens `size+1` Bytes. Bei zu geringer Kapazität folgt `PS_LIMIT` ohne
Teilkopie. Enthält der View bereits Nullbytes, bleiben diese erhalten: Der Puffer
enthält dann nicht einen einzigen konventionellen C-Textstring.

## Beispiel: Schlüssel und Wert aus einem begrenzten Bereich

```c
#include "physim/string_view.h"

ps_result split_setting(ps_string_view line, ps_string_view *key, ps_string_view *value) {
    if (!key || !value || key == value) return PS_INVALID;
    ps_string_view left, right;
    ps_result r = ps_string_view_split(line, (ps_string_view){"=", 1}, &left, &right);
    if (r != PS_OK) return r;
    r = ps_string_view_trim_ascii(left, &left);
    if (r != PS_OK) return r;
    r = ps_string_view_trim_ascii(right, &right);
    if (r != PS_OK) return r;
    if (!left.size) return PS_INVALID;
    *key = left;
    *value = right;
    return PS_OK;
}
```

Aus ` radius = 0.1 ` entstehen Views auf `radius` und `0.1` im ursprünglichen
Puffer. Das Beispiel führt keine Zahlenkonvertierung aus. Fehlendes Gleichheitszeichen
ergibt `PS_EOF`, ein leerer Schlüssel `PS_INVALID`. Beide Ausgaben bleiben bei
Fehlern unverändert. `key` und `value` dürfen nicht dasselbe Ausgabeobjekt sein.

Die Bibliotheksfunktionen melden ungültige Parameter mit `PS_INVALID` und lassen
ihre Ausgaben bei allen Fehlern unverändert. Der Inhalt hinter einem View kann
über andere Zeiger weiterhin veränderbar sein; gemeinsamer Zugriff benötigt
gegebenenfalls Synchronisation durch den Aufrufer.

## Prüfungen

Der Test `string_view` vergleicht 5.000 deterministische Suchfälle mit einer
unabhängigen Referenz und prüft alle 65.536 Einzelbyte-Vergleichspaare. Weitere
Fälle decken exakt begrenzte, nichtterminierte Allokationen, eingebettete Nullbytes,
leere Felder, ASCII-Whitespace, UTF-8-Bytegrenzen, extreme Indexwerte, unveränderte
Fehlerausgaben und überlappendes Kopieren ab.
