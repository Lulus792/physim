# Parameterhilfe im herausgelösten Workspace

Die native Prüfung erweitert die bisherige Hauptfensterabdeckung um das echte
SDL-Zusatzfenster. Sie liest dessen eigene Nuklear-Zeichenbefehle vor der
GPU-Übergabe und prüft Fensteridentität, tatsächlichen SDL-Tastaturfokus sowie
den Texteingabekanal. Eingaben selbst werden als SDL-Ereignisse erzeugt.

Zwei Fehler wurden reproduziert und korrigiert:

- Beim Herauslösen und Zurückholen wurde das Textfeld aktiviert, aber sein
  ausdrücklicher Tastaturbesitzer nicht vollständig übertragen. Der Zusatzkontext
  erhält jetzt denselben Fokusvertrag wie der Quelleditor im Hauptfenster.
- Eine verspätete Fokusverlustmeldung des Hauptfensters löschte beim erneuten
  Herauslösen den Besitzer des bereits aktiven Zusatzfensters. Die Behandlung
  lässt den herausgelösten Workspace jetzt bei seinem eigenen Fenster.

`signature_detached_workflow` durchläuft in C und Physim jeweils Modell-,
Analyse- und Dateieditor: automatische Parameterhilfe, getrennte Vorschlagsliste,
Tippen im Zusatzfenster, Rückkehr per Plattformbefehl+Umschalt+Enter und weiteres
Tippen im Hauptfenster. Quelle, Cursor, tatsächliche Auswahlanker und feste
Anordnung werden mit dem zuvor gespeicherten Zustand verglichen. Beim
16-Parameter-Aufruf liegen der gezeichnete Scrollgriff und das Ende der
Ziehbewegung nachweislich über dem Quelleditor; der Endpunkt liegt außerhalb
der Hilfekarte. Quelle, Cursor, Auswahl und Editor-Scrollposition bleiben dabei
erhalten. Anschließendes Tippen und Löschen werden ebenfalls geprüft.

Die neue Probe wurde anhand ihrer ersten Ergebnisse präzisiert: Der Debugger
belegt unveränderte Auswahlanker vor/nach der Rückkehr, obwohl der Cursor beim
Tippen weiterläuft. Verglichen werden deshalb die echten gespeicherten Anker.
Größenänderungen und die neu gezeichnete Hilfe werden beobachtet, bevor die
jeweilige Folgebedingung gilt. Ein mehrzeiliger Aufruf erzeugt die erforderliche
echte Überlappung; Testrechtecke werden nicht künstlich verschoben.

Die Fenster bleiben bei 520×400 beziehungsweise 520×320 im bestehenden Design.
Es kommen keine Bedienelemente, Leisten oder Abstände hinzu. Der bisher nur
anhand des Codes vermutete zusätzliche Drag-Fehler wurde in diesem Prüfumfang
nicht reproduziert; dafür wurde keine vorsorgliche Produktänderung eingebaut.

Der reguläre Abschluss besteht unter **Windows Release7/7 (run-69j1crw4)** und
**Linux Debug mit ASan/UBSan7/7 (run-xh60v0jy)**. Acht finale Ansichten wurden
tatsächlich geöffnet. Unter Linux benötigte die Probe außerdem eine
Synchronisation des erneuten Anzeigens vor ihrer zweiten Größenanforderung;
die beobachteten Zielgrößen und sämtliche Eingabeprüfungen bleiben verbindlich.
Der vorherige6/7-Versuch und die gezielte Debugger-Gegenprobe sind getrennt
erhalten. Das ist keine zusätzliche Änderung der normalen Fenstergeometrie.

Die abschließenden lokalen Ergebnisse, Quellarchive, Programme, ursprünglichen
Fehlversuche und gezielten Ansichten stehen im [strukturierten Nachweis](signature-detached-2026-11-05.json).
Physische Eingabe, Capture außerhalb eines OS-Fensters, Screenreader-Aufgaben
und die vollständige Oberfläche benötigen eigene Abnahmen.
