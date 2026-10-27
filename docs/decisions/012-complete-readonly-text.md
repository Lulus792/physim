# ADR 012: durchgehende schreibgeschützte Originaltexte

Status: angenommen für die native Umsetzung von Protokoll C, 26. Oktober 2026.
Grundlage: [ausgewählte Vorlage](../visuals/design-options/APPROVED.md) und
[Originaltext-Vertrag](../visuals/unified-workspace/PROTOCOL-DESIGN.md).

## Problem

Die Vorlage zeigt eine durchgehende Originalmeldung im Detailbereich. Die native
Zwischenlösung teilte sie in 12-KiB-Abschnitte, weil jeder Accessibility-Knoten
einen festen Textpuffer enthielt. Das war eine interne Speicherentscheidung,
keine unvermeidbare Grenze von Windows, Linux oder der nativen Oberfläche.
Eine pauschale Vergrößerung dieses Puffers in allen veröffentlichten und
vorbereiteten Knoten würde mehrere GiB pro Modell benötigen.

## Entscheidung

Längere schreibgeschützte Texte erhalten einen unveränderlichen, separat
referenzgezählten Speicherblock. Kleine Werte behalten den vorhandenen
Inline-Puffer. Veröffentlichter Zustand, nächster Entwurf und gegebenenfalls
native Vergleichszustände teilen denselben Block. Textänderung erzeugt einen
neuen Block; ein Speicherfehler erhält den bisherigen veröffentlichten Text.
Modellzugriff und Referenzzählung verwenden die vorhandene Serialisierung bzw.
den Modellmutex. Ein geliehener Textzeiger darf diesen Schutz nicht überleben.

Ein Original und die zusammengefassten externen Textbytes einer Veröffentlichung
sind auf 16 MiB begrenzt, zuzüglich Terminierung und Verwaltungsdaten. Damit ist
die Darstellung der im Protokoll auf insgesamt 4 MiB begrenzten Rohbytes auch bei
vierfacher Escape-Darstellung abgedeckt. Alte veröffentlichte Generationen und
native Textindizes können vorübergehend zusätzlich Speicher benötigen; 16 MiB
sind kein Limit für den gesamten Prozess. Beschreibbare Felder behalten ihre
bisherigen Eingabegrenzen.

Windows erzeugt BSTR-/TextPattern-Kopien unter dem Modellmutex und hält einen
eigenen Text für Änderungsereignisse. AT-SPI hält seine vorherigen Knoten korrekt
referenziert und kopiert auch größere Auswahlen ohne festen 12-KiB-Stackpuffer.
Der macOS-Adapter erzeugt seine Text-/UTF-16-Kopien ebenfalls unter dem Mutex;
seine tatsächliche Plattformabnahme bleibt erforderlich.

Protokoll C verwendet den vollständigen Text für Umbruch, Auswahl und Kopie.
Abschnittswechsel und zusätzliche Bytebereich-Zeilen entfallen. Die sichtbaren
Zeichenrechtecke bleiben begrenzt; der vollständige Inhalt bleibt über die
nativen Textschnittstellen lesbar. Die vorhandene Zuordnung von Unicode-Zeichen
zu Bytepositionen vermeidet erneutes Durchlaufen des gesamten Textanfangs pro
abgefragter Auswahlposition.

## Prüfung und Grenzen

Der [native Nachlauf](../visuals/unified-workspace/PROTOCOL-CONTINUOUS-ORIGINAL-NATIVE.md)
verknüpft tatsächlich ausgeführte Gegenproben, Speichertests und unabhängige
UIA-/AT-SPI-Läufe. Eine getestete 16-MiB-Modellveröffentlichung belegt keine
16-MiB-Layoutperformance. Der konkrete native Integrationsnachweis verwendet
64 KiB einschließlich Unicode, CRLF und Tabulatoren. macOS, tatsächliches
Screenreader-Lesen und die vollständige Gestaltung bleiben getrennte Nachweise.
