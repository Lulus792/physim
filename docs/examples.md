# Physim Examples

Wähle ein Beispiel nach deiner Frage. Jeder vollständige Lernpfad verbindet
Modellannahmen, Experiment, gespeicherte Messwerte, Analyse und eine Erwartung,
die du selbst überprüfen kannst. C und Physim bearbeiten dieselbe physikalische
Aufgabe. Beginne mit einem überschaubaren Modell und verändere anschließend eine Größe.

## Einen ersten Zusammenhang messen

| Beispiel | Frage | Überprüfbares Ergebnis |
| --- | --- | --- |
| [Gleichförmige Bewegung in C](experiment-tutorial.md) oder [Physim](language-tutorial.md) | Wie wird aus einem Zustand eine Messreihe? | Nach 1 s liegt die Position bei 1,5 m; die abgeleitete Geschwindigkeit bei 1,5 m/s |
| [Wurf im Vakuum](projectile-tutorial.md) | Stimmen numerische Bahn und bekannte Lösung überein? | Positionen folgen der analytischen Wurfgleichung innerhalb der beschriebenen Toleranz |
| [Pendel](pendulum-tutorial.md) | Wie verändern Verfahren und Zeitschritt das Ergebnis? | Periode und Energiebilanz lassen sich gegen eine unabhängige Referenz prüfen |

**Deine Variante:** Verdopple bei der gleichförmigen Bewegung nur die
Geschwindigkeit. Sage den Endwert nach einer Sekunde voraus und prüfe ihn im
Archiv. Du brauchst für diese Aufgabe weder Sensorrauschen noch eine Zufallsserie.

## Kräfte und Verluste verstehen

| Beispiel | Frage | Worauf du achtest |
| --- | --- | --- |
| [Wurf mit Luftwiderstand](projectile-drag-tutorial.md) | Warum weicht die Bahn vom Vakuummodell ab? | Medium, Kraftgesetz und Integrator getrennt vergleichen |
| [Feder und Dämpfung](spring.md) | Wie unterscheiden sich vier Dämpfungsfälle? | Amplitude, dissipierte Arbeit und numerischen Bilanzfehler auseinanderhalten |
| [Kugelstoß](collision-tutorial.md) | Was bleibt beim Stoß erhalten? | Impuls, Restitution, ungleiche Massen und Energieverlust prüfen |
| [Material und Medium](material-tutorial.md) | Wie kommen eigene Stoffwerte in ein Modell? | Dichte, Masse, Auftrieb und viskosen Widerstand mit SI-Einheiten verbinden |

**Deine Variante:** Vergleiche einen Vakuum- und einen Widerstandslauf mit
identischem Anfangszustand. Erkläre vor dem Start, welche Energiegröße konstant
bleiben sollte. Prüfe anschließend, ob du mechanische Energie oder die Bilanz
mit dissipierter Energie betrachtet hast.

## Messung und Auswertung lernen

| Beispiel | Frage | Ergebnis |
| --- | --- | --- |
| [Gespeicherten Lauf auswerten](saved-run-tutorial.md) | Kann ich ein Archiv ohne erneute Simulation untersuchen? | Ableitung, Rückintegration und einen eigenen Bericht prüfen |
| [Mehrlaufanalyse in Physim](language-analysis-tutorial.md) | Wie fasse ich passende Läufe zusammen? | Endwerte, Diagramm, Histogramm und Tabellen erzeugen |
| [Monte Carlo](monte-carlo-tutorial.md) | Wie unterscheide ich Streuung und Mittelwertunsicherheit? | Seeds, Quantile und ein Mittelwertintervall anhand gespeicherter Läufe prüfen |

**Deine Variante:** Erläutere ohne Nachschlagen, weshalb derselbe Seed allein
keinen fairen Vergleich zweier verschiedener Modelle garantiert. Prüfe deine
Erklärung anschließend anhand von Eingaben, Modell und numerischen Einstellungen.

## Räumliche und weitere Modelle

Die [räumliche Pendelerklärung](spatial-explanations.md) bietet eine lokale
drehbare Ansicht direkt in der App samt Textalternative. [Koordinatenrahmen](scene-rendering.md) verbinden lokale und globale Koordinaten.
[Kontaktwelten](contact-world.md) behandeln persistente Kontakte; die
[Mechanik](mechanics.md) erklärt Kräfte, Gelenke und Stoßantworten.
Weitere Beispiele vertiefen [Thermodynamik](thermodynamics.md),
[Elektromagnetismus](electromagnetism.md), [Wellen und Optik](waves-optics.md) sowie
[Strömung](fluid.md). Lies ihre Modellgrenzen, bevor du Ergebnisse überträgst.
Die geplanten räumlichen CFD-/FEM-Solver sind dadurch noch nicht abgeschlossen.

## Quellen finden und Beispiele verändern

Die vollständigen Quellen stehen im jeweiligen Lernpfad und zusätzlich unter
`examples/documentation`, `examples/language` beziehungsweise den Modellordnern
unter `examples`. `tests/tutorial_sources.json` ordnet veröffentlichte
Quellen den Lernpfaden zu. Die Dokumentationsprüfung erkennt abweichende
Codefassungen; die physikalischen Tests prüfen konkrete Referenzen und Grenzen.

1. Lies Modell und erwartetes Ergebnis.
2. Baue die vollständige Ausgangsfassung und erhalte ihren ersten Lauf.
3. Ändere eine Größe und formuliere eine Vorhersage.
4. Erzeuge einen zweiten Lauf und vergleiche dieselbe Zielgröße.
5. Prüfe, ob die Abweichung aus dem Modell, der Messung oder der Numerik kommt.

[User Guide](user-guide.md) · [C Lernweg](c-guide.md) ·
[Physim Lernweg](physim-guide.md) · [Handbuch](guide.md)
