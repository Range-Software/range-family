# Stammbaum

Ein Stammbaum besteht aus zwei Arten von Grundelementen: **Personen** und ihren **Beziehungen**.

- **Person** - stellt eine reale Person dar
- **Beziehung** - stellt eine Beziehung zwischen mehreren Personen dar

## Person

- **ID** - Eindeutige Kennung
- **Name** - Der Name enthält **Vorname**, **zweiten Vornamen**, **Nachname** und **Geburtsname**
- **Geburt** - Die Geburt einer Person wird durch Datum und Ort festgelegt
- **Tod** - Der Tod einer Person wird durch Datum und Ort festgelegt
- **Geschlecht** - Männlich, weiblich oder andere
- **Bild** - Bild einer Person mit Titel und Beschreibung, entweder im Stammbaum oder in einer separaten Datei gespeichert
- **Notizen** - Notizen oder Lebenslauf einer Person

Alle Parameter außer der **ID** sind nicht eindeutig, das heißt, es kann mehrere Personen mit demselben Namen oder anderen gleichen persönlichen Angaben geben. Außerdem können alle Parameter außer der **ID** leer sein.

## Beziehung

- **ID** - Eindeutige Kennung
- **Partner** - Liste der **Partner** in der Beziehung (**Partner** in einer Beziehung haben auch die Rolle der **Eltern**)
- **Kinder** - Liste der **Kinder** in der Beziehung
- **Beginn** - Der Beginn einer Beziehung wird durch Datum und Ort festgelegt
- **Ende** - Das Ende einer Beziehung wird durch Datum und Ort festgelegt

Alle Parameter außer der **ID** sind nicht eindeutig, das heißt, es kann mehrere Beziehungen mit denselben Parametern und Personen in den Listen der **Partner** und **Kinder** geben. Außerdem können alle Parameter außer der **ID** leer sein.
