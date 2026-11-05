# Vollständiger Import des verschobenen SDK-Prüfarchivs

Der Debian-Release-Build zur Quelle `df9927c6` besteht **789/789 echte native
Daten-/Buildprüfungen**. Die Originalergebnisse wurden aus dem hashgeprüften
GitHub-Archiv gelesen. Die installierte App startet anschließend sowohl unter
Debian 12 als auch Ubuntu 24.04 ohne Compiler oder Python. Die späteren
SDK-Verbraucherprüfungen scheitern jedoch in beiden frischen Systemen: Ihr
eigenes Prüfarchiv enthält `tools/build.py`, aber nicht dessen lokal benötigtes
Modul `tools/artifact_paths.py`.

Das Modul wird jetzt ausdrücklich in das unabhängige Prüfarchiv aufgenommen.
Der Regressionstest lädt den tatsächlich verschobenen Build-Treiber mit
isoliertem Python (`-I`) aus dem gepackten Dateibestand. Damit kann weder der
Arbeitsordner noch `PYTHONPATH` das fehlende Modul aus dem Checkout liefern.
UTF-8 ist für die absichtlich enthaltenen Nicht-ASCII-Pfade explizit gesetzt.

Die gleiche Probe scheitert vor der Korrektur unter Windows und Alpine Linux
präzise am fehlenden Modul und besteht danach auf beiden Plattformen. Der
vorhandene Test prüft weiterhin alle Archivbytes und SHA-256-Werte, den Ausschluss
von Implementierung und SDK-Headern sowie fehlende/ungültige Eingaben und die
exklusive Veröffentlichung. Alle **212 gepackten Verbraucherdateien** wurden
gegen den tatsächlichen Git-Elternstand gelesen; fremdes WIP ist nicht enthalten.

Vier kompakte Originalarchive wurden vollständig gelesen und anhand ihrer
offiziellen Digests geprüft: Prüfbaukasten, Release-Testresultate und beide
Installationsberichte. Es wurde weder das 276-MB-Releasepaket heruntergeladen
noch ein neuer GUI-Build für diese Python-Korrektur erzeugt. Belege, Befehle,
Exitcodes und Grenzen stehen im [strukturierten Bericht](verification-kit-import-2026-11-05.json).

Die vollständigen neuen Debian-/Ubuntu-Verbraucherprüfungen bleiben bis zum
tatsächlichen Nachlauf mit korrigiertem Prüfarchiv offen. Die 789 bestandenen
Daten-/Buildfälle bestätigen keine vollständige UI-Abnahme.
