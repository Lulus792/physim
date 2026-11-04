# Anschlussnachweis · 4. November 2026

## Gehosteter Mac-Nachlauf des Einheitenfokus

Der Nutzer-Mac ist ausgeschaltet und wurde weder kontaktiert noch auf
Erreichbarkeit geprüft. Dieser Nachweis liest ausschließlich vorhandene
GitHub-Ergebnisse von Quellstand `7064a38c91073d447ced79d6c4f5b4308d6bcfda`:
[Workflow 41182399084](https://github.com/Lulus792/Physim/actions/runs/41182399084).
Keine Wiederholung bereits geschlossener Matrizen und kein neuer CI-Start.

| Gehosteter Runner | Job | Tatsächlich ausgeführt | Ergebnis |
| --- | --- | --- | --- |
| Apple Silicon | 124482298883 | Header 3, Typografie 1, kombinierte Gruppe 10, Preferences 1 | 15/15 |
| Intel | 124482299110 | Header 3, Typografie 1, kombinierte Gruppe 10 | 13/14; Preferences übersprungen |

Beide ZIPs unter `build/validation/continuation-2026-11-04` gespeichert,
SHA-256 mit GitHub-Digest abgeglichen, alle vier/drei `results.json` gelesen.
Artifact-IDs, Einzelprüfungen, Digests und Fehlertext stehen im
[versionierten Ergebnisbericht](unit-focus-macos-2026-11-04.json).
Der ursprüngliche lokale Commitabgleich erfasst 1.109 Compiler-/Testdateien.

`unit_task_window` und `channel_units_workflow` bestehen auf beiden Architekturen.
Intel scheitert in `settings_keyboard_workflow` an Stufe 33 nach dem erneuten
Öffnen der Einstellungen; Apple Silicon besteht diesen Ablauf. Das Log zeigt
noch keine belegte Ursache. Kein Produkt-/Timingfehler nur aus dem Symptom
ableiten. Gezielter nächster Schritt: Tab-Fokushelfer, Fensterfokus und Warte-
bedingungen dieses Schritts untersuchen, bevor ein geänderter Nachlauf startet.

## Referenzgenerator: Ursache und Korrektur

Die bestehenden breiten C17-/Paketprüfungen melden dieselben zwei veralteten
Dateien: `docs/reference/experiment.md` und `docs/reference/language-library.md`.
Die seit `6f527f65` vorhandenen Editorbeispiele für `ps_channel_add` und `Channel`
samt Beschreibung waren nur im erzeugten Text ergänzt worden. Der Generator
hätte diese Inhalte entfernt. Er besitzt sie jetzt ebenfalls als geprüfte
Eingaben; keine Signatur, kein SDK-Vertrag und kein Referenztext wird geändert.

Die reine Neuerzeugung im WIP-Baum wurde vor dem Patch geprüft: Sie entfernte die
Beispiele und scheiterte anschließend am schon vorhandenen fremden Status-Link.
Die beiden eigenen Textänderungen wurden vollständig auf die Git-Fassung
zurückgeführt. Erst danach wurde der Generator korrigiert. Die Referenzen haben
im endgültigen Diff keine Änderung.

Der bestehende Prüfeinstieg ist `python -B tools/generate-reference.py --check`.
Für den sauberen Vergleich lädt der lokale Helfer
`docs/verification/reference-generator-2026-11-04.py` jeweils die zwei
Generator-Eingaben aus dem festgeschriebenen `58185206` bzw. dem Patch im Speicher
und ruft `main()` mit
`--check` gegen den unveränderten, eingefrorenen `unit-focus-compact`-Quellbaum
auf. Er schreibt keine Datei in diesen Quellbaum. Somit beeinflusst die fremde
Statusänderung den Vorher-/Nachhervergleich nicht.

- Windows: Python 3, `python -X utf8 -B docs/verification/reference-generator-2026-11-04.py`.
- Linux: `wsl -d Alpine-Dev --exec python3 -B /mnt/d/Physim/docs/verification/reference-generator-2026-11-04.py`.
- Je Plattform vorher Exit 1 mit genau den zwei veralteten Referenzen, nachher
  Exit 0 mit **31 reference documents verified**. Vollständiger Markdown-/
  Offline-Linkabgleich des sauberen Bestands bleibt aktiv.
- Im tatsächlichen WIP-Baum nach dem Patch weiterhin Exit 1:
  `docs/status.md -> docs/storage-cleanup-2026-10-22.md` ist kein registriertes
  F1-Thema. Diese getrennte fremde Änderung bleibt erhalten. Das ist kein
  bestandener Gesamtcheck dieses Arbeitsverzeichnisses.

[Kommandos, Eingabehashes und Originalausgaben](reference-generator-2026-11-04.json).
Keine neue native Binärdatei, GUI-Matrix oder plattformübergreifend vollständig
grüne CI behauptet. Der Snapshot nennt die weiteren tatsächlich beobachteten
CI-Fehler separat; sie bleiben offen. Alle neuen Daten liegen auf D:, keine
neuen Bilder oder Zugriffe auf F:.
