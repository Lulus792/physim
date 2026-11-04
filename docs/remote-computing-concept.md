# Konzept: Rechnen auf verbundenen Geräten

Stand: 4. November 2026 · Konzept und UI-Skizzen, keine Implementierung oder Designfreigabe.

## Ziel und vorhandene Grundlage

Physim soll einzelne lange Rechnungen auf einen anderen Rechner auslagern und unabhängige Läufe einer Serie auf mehrere Rechner verteilen können. LAN und WLAN sind mögliche Verbindungen im lokalen Netz. Weitere Rechner werden bewusst in den Einstellungen verknüpft; erst danach erscheinen zusätzliche Ausführungsziele in den Rechenabläufen. Ein integrierter Test vergleicht die Physim-Rechenleistung des Hauptrechners mit verknüpften Geräten.

Der bestehende Batch-Controller bietet lokale Runner-Prozesse, bis zu acht Worker, Parameterstudien, Seeds, Limits und archivierte Wiederaufnahme abgeschlossener Läufe (siehe [Batch-Referenz](reference/batch.md) und [Anleitung](batch-language.md)). Er ist kein Netzwerk-Scheduler. Fortsetzung einer Serie bedeutet nicht, dass ein unterbrochener Einzellauf mitten im Rechenzustand fortgesetzt werden kann. Die vorhandenen Grenzen sind für eine verteilte Ausführung gesondert zu prüfen, nicht stillschweigend aufzuheben.

## Bedienweg

1. Auf dem zweiten Rechner einen passenden Physim-Rechendienst ohne Oberfläche installieren und bewusst starten. Die konkrete Bereitstellung und unterstützte Betriebssysteme sind noch zu klären.
2. In **Einstellungen → Rechnen & Geräte** „Gerät verknüpfen“ wählen. Ein Assistent nimmt Adresse und Port entgegen; lokale Suche kann ergänzen, manuelle Eingabe bleibt möglich. Beim ersten Verbinden wird eine Geräteidentität mit einmaligem Code auf beiden Seiten geprüft und die Berechtigung ausdrücklich erteilt. Alle Verbindungen nach erfolgreicher Verknüpfung authentifizieren und verschlüsseln; Details und sichere Geheimnisablage vor Umsetzung auswählen und prüfen.
3. Das Gerät erscheint mit Name, erreichbar/nicht erreichbar, Kompatibilität und verfügbaren Ressourcen. Nutzer können umbenennen, Verbindung testen, Berechtigung widerrufen und Gerät entfernen. Eine Suche im LAN ist optional und darf keine Berechtigung erteilen. Ein entfernter Dienst soll nur absichtlich bereitgestellt und auf das gewählte Netz begrenzt werden.
4. Optional den Leistungstest lokal und auf ausgewählten Geräten starten. Ergebnisse zeigen Testversion, Datum und Rahmenbedingungen.
5. Beim Start eines einzelnen Laufs nach Verknüpfung einen Ausführungsort wählen. Bei Serien zusätzlich mehrere Geräte und optional den Hauptrechner auswählen. Ohne Verknüpfung bleibt der bisherige lokale Start unverändert und die zusätzlichen Ziele werden nicht eingeblendet. Einstellungen und die Möglichkeit, ein Gerät zu verknüpfen, bleiben erreichbar. Offline-Geräte erscheinen nach Verknüpfung mit klarer Erklärung, nicht als nutzbare Ziele.
6. Aufgabenleiste B und ihr Pop-out zeigen dieselbe Aufgabe mit Ort, Phase und ehrlichem Fortschritt. Nach Fertigstellung werden validierte Ergebnisse in die vorhandenen Lauf-/Serienansichten übernommen.

Verknüpfung ist ein bewusster Vorgang mit unmittelbarer Wirkung, keine bloße Vorschau: Abbrechen eines Einstellungsentwurfs darf keine bereits gewährte Geräteberechtigung vortäuschen oder still zurückrollen. Für veränderbare Einstellungen gelten weiterhin Entwurf, Prüfung, Übernehmen und Abbrechen nach [Einstellungen A](decisions/018-selected-settings-a.md). Die endgültige Trennung von Geräteverwaltung und Einstellungsentwurf ist bei der Umsetzung auszuarbeiten.

## Ausführung und Verteilung

**Einzellauf:** Der Hauptrechner erstellt einen unveränderlichen Auftragssnapshot aus Modell/Quelle, benötigten Eingaben, Parametern, Seed, Laufzeitversion und Limits. Nach Kompatibilitätsprüfung und Übertragung führt das ausgewählte Gerät den Lauf aus und meldet Status. Ein längerer Einzellauf läuft auf genau einem Rechner; mehrere Geräte beschleunigen ihn dadurch nicht automatisch. Auftragsabschluss erst anzeigen, wenn das Ergebnis vollständig übertragen und geprüft sowie lokal verfügbar ist.

**Serie:** Ein Koordinator vergibt unabhängige Lauf-Indizes samt stabiler Parameter und Seeds an ausgewählte Geräte. Die Zuteilung ist dynamisch: Ein freier Rechner erhält die nächste geeignete Arbeit. Der lokale Rechner ist optional ein weiterer Ausführungsort. Zunächst begrenzte Parallelität je Gerät nach Nutzerfreigabe und Ressourcen; Benchmarkwerte können bei der Wahl helfen, tatsächliche Laufzeiten und Fähigkeiten müssen Vorrang haben. Ergebnisse werden nach ursprünglichem Index zusammengeführt; bereits bestätigte Resultate dürfen nicht doppelt in Statistiken eingehen. Lokaler Durchsatz hängt auch von Belastung, Modell, Datentransfer und verfügbaren Kernen ab.

**Trennung und Wiederaufnahme:** Der Rechendienst führt angenommene Aufträge bei Netzunterbrechung zunächst weiter und speichert Status und Ergebnisse. Der Client zeigt „Verbindung unterbrochen – Zustand unbekannt“, fragt bei Wiederverbindung anhand stabiler Auftrags-/Versuchsidentitäten nach und lädt fehlende Ergebnisse. Zeitüberschreitung belegt keinen Stillstand: Wenn ein Lauf vorsorglich erneut vergeben wird, kann er zeitweise doppelt rechnen. Atomare Annahme genau eines validierten Ergebnisses je Lauf und Verwerfen späterer Duplikate sind erforderlich; Nebenwirkungen fremden Modellcodes bleiben ein gesondertes Sicherheitsproblem. Abbruch wird erst nach Bestätigung angezeigt, ein offline übermittelter Abbruch ist nur angefordert. Fertige Serienläufe bleiben erhalten. Dienst-/Rechnerneustart: dauerhafte Auftragsannahme und Recovery prüfen; ein laufender Einzelprozess ist ohne passenden Rechenzustands-Checkpoint unter Umständen neu zu starten. Verhalten bei Schließen des Hauptprogramms und Aufbewahrungsdauer sind noch festzulegen.

**Plattformen und Vertrauen:** Quelltext, native Module und Abhängigkeiten sind nicht automatisch zwischen Windows, Linux, macOS oder CPU-Architekturen austauschbar. Übertragung ausführbarer Binärmodule versus Build auf dem Ziel, Toolchains, ABI und Abhängigkeiten müssen vorab geklärt werden. Inkompatibilität vor Start sichtbar machen. Ein gekoppelter Dienst führt potenziell fremden Code aus: Authentifizierung ersetzt keine Isolation; Rechte, Ressourcenlimits, Datenzugriff, Protokollierung und Vertrauensgrenzen brauchen eigenes Sicherheitskonzept. Kein Internetzugriff oder Cloud-Dienst vorausgesetzt oder durch lokale Netzfreigabe automatisch aktiviert.

## Leistungstest wie ein Physim-Benchmark

Start in Einstellungen für diesen Rechner, einzelne Geräte oder alle ausgewählten Geräte. Referenzrechner und Gerät führen **dieselbe versionierte, fest definierte Physim-Arbeitslast** aus. Vergleich nur bei passender Benchmarkversion, vollständigem gültigem Ergebnis und vergleichbaren Testoptionen. Zwei getrennte Messungen vorschlagen:

- **Einzellauf:** Zeit für denselben repräsentativen langen Lauf; kleinere Zeit ist besser. Separat ein relativer Faktor = Zeit des Hauptrechners / Zeit des Geräts. 1,50× bedeutet für diese Testlast etwa 50 % höherer Durchsatz, nicht garantierte Beschleunigung jedes Modells.
- **Serien-Durchsatz:** feste Anzahl unabhängiger Läufe, offen ausgewiesene Parallelität und tatsächliche Gesamtdauer; abgeschlossene gültige Läufe pro Minute. Verhältnis zum Hauptrechner bei gleicher Testkonfiguration als Vergleich, keine Garantie für beliebige Serien. Die genaue Arbeitslast und Normalisierung werden nach Messungen festgelegt.

Wiederholungen, Aufwärmphase und Zusammenfassung (etwa Median) festlegen; Berechnungszeit getrennt von Netz-/Übertragungszeit messen und Letztere optional zusätzlich ausweisen. Keine FPS oder frei erfundenen Werte verwenden. Korrektheitsprüfung innerhalb definierter numerischer Toleranzen vor Wertung; Abbruch, Überhitzung/Drosselung soweit erkennbar, Zeitlimit und fehlerhafte Ergebnisse kenntlich machen. Version, Plattform/Architektur, genutzte Worker, Zeitpunkt und Testbedingungen speichern; alte Werte nach relevanten Änderungen als möglicherweise veraltet kennzeichnen. Gerätekonfiguration, Energiezustand, Last und Netzwerk können Ergebnisse verändern. Kein Test läuft automatisch ohne bewussten Start; andere laufende Aufträge und Ressourcenlimits berücksichtigen.

## UI-Skizzen — Vorschläge, keine freigegebenen Designs

Die drei Spalten von Einstellungen A (Kategorien, Optionen, Live-Vorschau) und die unabhängigen Aufgabenfenster bleiben Grundlage. Die Skizzen zeigen Informationshierarchie, keine festen Maße oder endgültigen Farben. Bestehende Freigaben bleiben unverändert.

**1 · Einstellungen / Geräte, noch nichts verknüpft**

```text
Einstellungen                         Suche …
┌ Kategorien ──────┬ Rechnen & Geräte ──────────────────┬ Vorschau ───────────────┐
│ Darstellung     │ Dieser Rechner                     │ Dieser Rechner          │
│ Editor …        │ Rechnungen werden lokal ausgeführt.│ Lokal · verfügbar       │
│ Rechnen & Geräte│                                   │                         │
│ …               │ Andere Geräte                     │ Weitere Geräte können   │
│                 │ Noch keine Geräte verknüpft.      │ hier verbunden werden.  │
│                 │ [Gerät verknüpfen]                │                         │
│                 │                                   │                         │
│                 │ Leistungstest                      │                         │
│                 │ [Test auf diesem Rechner starten] │                         │
└─────────────────┴───────────────────────────────────┴─────────────────────────┘
 Standardwerte           Änderungen prüfen   Abbrechen   Übernehmen
```

**2 · Geräteübersicht und Benchmark nach Verknüpfung**

```text
Rechnen & Geräte                                      [Gerät verknüpfen]
Dieser Rechner        Verfügbar       Einzellauf  42 s   Serie  24 Läufe/min
Werkstatt-PC          Verbunden       Einzellauf  28 s   Serie  60 Läufe/min
Linux-Rechner         Nicht erreichbar                  Letzter Test: älter

[Test auswählen …]  [Ausgewählte Geräte testen]     [Gerät verwalten …]

Vergleich · Beispielwerte, keine Messung
Werkstatt-PC   Einzellauf 1,50×   Serien-Durchsatz 2,50×
Testversion 1 · identische Testoptionen · Details und Rahmenbedingungen ›
```

Vorschau rechts zeigt das ausgewählte Gerät und dessen Status; bei schmalem Fenster unter den Inhalt verschieben. Der Test öffnet eine überprüfbare Auswahl mit Geräten, Arbeitslast und Ressourcen; Fortschritt in Aufgabenleiste B. Werte zusätzlich als Text, nicht allein durch Farbe oder Balken vermitteln.

**3 · Start einer Serie nach Verknüpfung**

```text
Serie starten
Ausführungsorte
[x] Dieser Rechner               verfügbar
[x] Werkstatt-PC                 verbunden
[ ] Linux-Rechner                nicht erreichbar
Verteilung: Automatisch · freie Kapazität nutzen
Parallelität und Limits …

100 Läufe · Ergebnisse werden nach Abschluss lokal bereitgestellt
[Abbrechen]                                  [Serie starten]
```

Bei einem Einzellauf stattdessen genau einen Ort wählen. Ein gesperrtes Gerät erklärt den Grund. Kein Gerät ausgewählt: Start gesperrt mit Erklärung. Offline-/inkompatibler Zustand kann sich bis zum Start ändern und ist erneut zu prüfen.

**4 · Aufgabe / Pop-out**

```text
Serie · 38 von 100 Läufen abgeschlossen      [Details] [↗]
Werkstatt-PC: 22 · Dieser Rechner: 16 · Linux-Rechner: Verbindung unterbrochen
```

Details zeigen wartend/laufend/abgeschlossen/fehlgeschlagen sowie Übertragung und Abbruchstatus pro Gerät. Anzeige derselben Aufgabe in Leiste und Pop-out; keine doppelten Worker oder Ereignisse. Bekannte Laufzahl ist Mengenfortschritt, keine erfundene Restzeit. Bei Einzelaufgaben ohne messbaren Fortschritt nur Phase. Zugänglicher Name und Tooltip für das Pop-out-Icon gemäß [Fortschritt B](decisions/020-selected-progress-b-popout.md).

## Zu klären und mögliche Reihenfolge

Offen: Bedienung auf dem headless Gerät bei der Erstkopplung; welche Betriebssystem-/Architekturkombinationen und Modellarten zuerst; ob und wie lange Aufgaben nach Client- oder Dienstneustart überleben; Aufbewahrung und Löschung von Eingaben/Resultaten; Benutzer- und Ressourcenrechte; konkrete Testlast und statistische Wiederholungszahl; Nutzersteuerung von parallelen Aufträgen und automatischer Wiedervergabe. Die Skizzen sind ein Gesprächsangebot und keine Designentscheidung.

Mögliche Schritte: (1) Protokoll, Vertrauensgrenzen und portable Auftrags-/Ergebnisverträge anhand bestehender Runner prüfen; (2) Verbindung und einzelne entfernte Ausführung samt Wiederverbindung; (3) Benchmark und transparente Ergebnisse; (4) mehrere Geräte und robuste Serienverteilung; (5) reale Bedien-, Fehler-, Sicherheits- und Plattformprüfungen. Die Schritte sind eine Planungsoption, keine Termin- oder Funktionszusage.

Abnahme später unter anderem: keine Zielauswahl ohne Verknüpfung; explizite Kopplung und Widerruf; korrekter lokaler/entfernter Einzellauf; Serie mit unterschiedlichen Geräten und stabilen Seeds; Netztrennung, Duplikat und Abbruchrennen; inkompatible Plattformen; valide Benchmarkvergleiche und abgebrochene Tests; Tastatur, Screenreader, kleine Fenster und echtes Pop-out. Windows, Linux und macOS nur nach tatsächlich ausgeführten Prüfungen bewerten. Aktuell ist weder eine native Oberfläche noch ein Rechendienst implementiert oder getestet.
