# F1-Linkfehler außerhalb des sichtbaren Ausschnitts lesen

Ein ungültiger interner Link setzte den vorhandenen Fehlertext korrekt. Liegt
die Metadatenzeile über dem sichtbaren Artikel, ließ die bisherige geclipte
Accessibility-Veröffentlichung ihr Label jedoch weg. Der unabhängige AT-SPI-
Client fand deshalb keine Fehlermeldung. Dies ist in beiden glibc-CI-Joblogs zur
Quelle01d2c0d0 und lokal mit dem unveränderten bisherigen Programm reproduziert.

Die Metadatenzeile verwendet jetzt gezielt die bereits vorhandene vollständige
Veröffentlichung mit echten Bounds und Clipping. Anschließend wird der vorherige
Veröffentlichungsmodus sofort wiederhergestellt. Der Hinweis bleibt auslesbar,
korrekt als außerhalb der Ansicht gekennzeichnet und unfokussiert. Zeichenbefehle,
Layout, Navigation und Darstellung werden dadurch nicht verändert.

## Tatsächlich ausgeführte Prüfung

- **Windows Release:** native UIA-Links, F1-Tastatur und Metadatenlayout **3/3**.
- **Alpine/X11 ASan/UBSan:** Tastatur und Metadatenlayout bestanden. Der abschließende
  unabhängige AT-SPI-Client besteht mit demselben unveränderten Programm ebenfalls.
  Er prüft das Offscreen-Label, erhaltenen Quelllinkfokus und dessen exakte Position,
  Text-/Linkidentitäten, echte anschließende Navigation sowie ungültig gewordene
  alte Link-, Tabellen- und Zellreferenzen.
- **Gegenprobe:** Derselbe endgültige Client scheitert mit dem hashgeprüft aus dem
  bestehenden Archiv wiederhergestellten alten Programm weiterhin ausschließlich
  am fehlenden Fehlerlabel. Keine neue vollständige Laufzeitkopie hergestellt.
- Ein tatsächlicher nativer Linkfokuszustand ist vor/nach Korrektur als vollständige
  BMP-Datei bytegleich. Das belegt diesen Zustand, keine vollständige Designabnahme.

Die erste zusätzliche Fokusassertion war zu weitgehend: Native Aktivierung
fokussiert bewusst den aktivierten Link, nicht einen zuvor anderen Link. Der
endgültige Aufbau fokussiert deshalb zuerst die Quelle. Eine weitere Sofortmessung
traf den Zwischenframe einer eingereihten nativen Ausrichtung: y60, kurz y79,
danach24 beobachtete Werte wieder y60. Der Prüfer wartet jetzt begrenzt auf die
**exakte** ursprüngliche Geometrie, ohne Toleranzvergrößerung. Der Linux-Zwischenlauf
bleibt als2/3 protokolliert; nur der betreffende unabhängige Client wurde erneut
ausgeführt. Produktbinaries blieben dabei hashgleich.

[Befehle, Programme, Zwischenbefunde und Ergebnisse](f1-error-semantics-2026-11-05.json)
sowie [F1-Fontprobe im tatsächlichen Einstellungsfenster](font-settings-fault-2026-11-05.md).
Alle307 eigenen Eingaben sind vor Build/Prüfung zwischen Root, Isolation und Manifest
geprüft. Fremde AT-SPI-/Orca-Arbeiten wurden nicht übernommen. Die gezielte
Veröffentlichungskorrektur benötigt weiterhin neue glibc-/macOS-Nachläufe und
vollständige assistive Bedienaufgaben. GUI-ASan nutzt `detect_leaks=0` für externe
Grafiklaufzeiten; keine LeakSanitizer-Abnahme.

Die temporär wiederhergestellte Gegenprobe und sechs als bytegleich erkannte
Diagnosebilder sollten nach Abschluss entfernt werden. Die automatische
Freigabeprüfung hat diesen Löschbefehl mit **blocked by policy** abgelehnt.
[Plan und Ablehnung](f1-error-cleanup-2026-11-05.json): alle sieben Dateien bleiben
vorhanden, **0 Bytes freigegeben**. Kein alternativer Löschversuch erfolgt.
