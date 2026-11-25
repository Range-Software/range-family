# Spezifikation der Stammbaum-JSON-Datei

## 1. Überblick

Dieses Dokument beschreibt das JSON-Dateiformat, in dem genealogische Daten eines Stammbaums gespeichert werden. Das Format speichert Personeneinträge mit biografischen Angaben und Beziehungseinträge, die familiäre Verbindungen festlegen.

## 2. Struktur der obersten Ebene

Die JSON-Datei enthält drei Schlüssel auf oberster Ebene:

| Schlüssel | Typ | Beschreibung |
|-----------|-----|--------------|
| persons | Array | Array von Person-Objekten mit biografischen Daten |
| relations | Array | Array von Relation-Objekten, die familiäre Verbindungen festlegen |
| version | Zeichenkette | Version des Dateiformats (z. B. "0.0.1") |

## 3. Person-Objekt

Jede Person im Stammbaum wird durch ein Person-Objekt mit folgender Struktur dargestellt:

### 3.1 Felder des Person-Objekts

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| id | Zeichenkette (UUID) | Ja | Eindeutige Kennung der Person |
| name | Name-Objekt | Nein | Angaben zum Namen der Person |
| sex | Zeichenkette | Nein | Geschlecht: "male", "female" oder "other" |
| birth | Event-Objekt | Nein | Datum und Ort der Geburt |
| death | Event-Objekt | Nein | Datum und Ort des Todes |
| picture | Picture-Objekt | Nein | Base64-kodiertes Foto der Person |
| text | Zeichenkette | Nein | Base64-kodierte Notizen oder Lebenslauf |

### 3.2 Name-Objekt

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| first-name | Zeichenkette | Nein | Vorname der Person |
| last-name | Zeichenkette | Nein | Nachname der Person |
| middle-name | Zeichenkette | Nein | Weitere Vornamen der Person |
| maiden-name | Zeichenkette | Nein | Geburtsname (falls nach der Heirat geändert) |

### 3.3 Event-Objekt (Geburt/Tod/Beginn und Ende einer Beziehung)

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| date | Date-Objekt | Nein | Datum des Ereignisses |
| place | Place-Objekt | Nein | Ort des Ereignisses |

### 3.4 Date-Objekt

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| year | Ganzzahl | Ja | Jahr (z. B. 1985) |
| month | Ganzzahl | Nein | Monat (1-12), entfällt, wenn unbekannt |
| day | Ganzzahl | Nein | Tag des Monats (1-31), entfällt, wenn unbekannt |
| fuzzy | Boolean | Nein | True, wenn das Datum ungefähr/unsicher ist, false, wenn weggelassen |

Lassen Sie `day` weg, wenn nur Monat und Jahr bekannt sind, und lassen Sie `day` und `month` weg, wenn nur das Jahr bekannt ist.

### 3.5 Place-Objekt

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| address | Address-Objekt | Nein | Strukturierte Adressangaben |
| geo-coordinate | Geo-Coordinate-Objekt | Nein | Geografische Lage |

### 3.6 Address-Objekt

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| city | Zeichenkette | Nein | Name der Stadt oder Gemeinde |
| country | Zeichenkette | Nein | Name des Landes |
| street | Zeichenkette | Nein | Straßenname |
| street-number | Zeichenkette | Nein | Hausnummer |
| postal-code | Zeichenkette | Nein | Postleitzahl |

### 3.6.1 Geo-Coordinate-Objekt

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| latitude | Zahl | Nein | Breitengrad in Grad |
| longitude | Zahl | Nein | Längengrad in Grad |
| altitude | Zahl | Nein | Höhe in Metern |

### 3.7 Picture-Objekt

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| title | Zeichenkette | Nein | Titel des Bildes |
| description | Zeichenkette | Nein | Beschreibung des Bildes |
| url | Zeichenkette | Nein | Pfad zu einer Bilddatei, relativ zur Stammbaumdatei |
| base64Data | Zeichenkette | Nein | Base64-kodierte Bilddaten (PNG-Format) |

Das Bild selbst wird entweder in einer separaten Datei gespeichert, auf die `url`
verweist, oder als `base64Data` in die Stammbaumdatei eingebettet.

Ist `url` angegeben, wird sie relativ zum Verzeichnis der Stammbaumdatei
aufgelöst und muss auf eine Datei in diesem Verzeichnis oder darunter verweisen.
Pfade, die aus ihm herausführen (zum Beispiel `../fotos/johann.png` oder ein Pfad
auf einem anderen Laufwerk), werden abgelehnt und das Bild wird stattdessen als
`base64Data` eingebettet. Ein erfolgreich in einer referenzierten Datei
gespeichertes Bild enthält kein `base64Data`.

## 4. Relation-Objekt

Beziehungen legen familiäre Verbindungen zwischen Personen fest. Jede Beziehung stellt eine Partnerschaft (Ehe oder Ähnliches) dar und führt optional die aus dieser Partnerschaft hervorgegangenen Kinder auf.

### 4.1 Felder des Relation-Objekts

| Feld | Typ | Pflicht | Beschreibung |
|------|-----|---------|--------------|
| id | Zeichenkette (UUID) | Ja | Eindeutige Kennung der Beziehung |
| partners | Array\<UUID\> | Nein | Array von Personen-IDs (meist 2 Partner), Partner sind zugleich Eltern der Kinder |
| children | Array\<UUID\> | Nein | Array von Personen-IDs der Kinder |
| begin | Event-Objekt | Nein | Beginn der Beziehung (z. B. Heiratsdatum) |
| end | Event-Objekt | Nein | Ende der Beziehung (z. B. Scheidungsdatum) |

## 5. Beispiel

Unten ist ein Beispiel für einen minimalen Stammbaum mit zwei Personen und einer Beziehung:

```json
{
  "version": "0.0.1",
  "persons": [
    {
      "id": "a1b2c3d4-e5f6-4789-abcd-ef1234567890",
      "name": { "first-name": "John", "last-name": "Smith" },
      "sex": "male",
      "birth": {
        "date": { "day": 15, "month": 6, "year": 1980, "fuzzy": false },
        "place": { "address": { "city": "Prague", "country": "Czech Republic" } }
      }
    },
    {
      "id": "b7c8d9e0-f1a2-4b3c-8d4e-5f6a7b8c9d0e",
      "name": { "first-name": "Jane", "last-name": "Smith", "maiden-name": "Doe" },
      "sex": "female"
    }
  ],
  "relations": [
    {
      "id": "c3d4e5f6-a7b8-4c9d-9e0f-1a2b3c4d5e6f",
      "partners": ["a1b2c3d4-e5f6-4789-abcd-ef1234567890", "b7c8d9e0-f1a2-4b3c-8d4e-5f6a7b8c9d0e"],
      "children": [],
      "begin": { "date": { "day": 20, "month": 8, "year": 2010, "fuzzy": false } }
    }
  ]
}
```

## 6. Hinweise zur Implementierung

**ID-Format:** Alle Kennungen verwenden das Format UUID v4 (z. B. "4e28b000-0c04-434a-9382-792ea3543316").

**Referenzielle Integrität:** Alle UUID-Verweise in den Arrays partners und children müssen gültigen Personen-IDs im Array persons entsprechen.

**Anzahl der Partner:** Die meisten Beziehungen haben genau 2 Partner, das Format unterstützt jedoch 0, 1 oder 2 Partner für unvollständige Einträge oder Alleinerziehende.

**Zeichenkodierung:** Die Datei verwendet die Kodierung UTF-8, um internationale Zeichen in Namen und Ortsnamen zu unterstützen.

**Base64-Felder:** Die Felder text und picture.base64Data enthalten Base64-kodierten Inhalt, um Binärdaten und formatierten Text sicher in JSON zu speichern.
