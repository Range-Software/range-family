# Specifica del file JSON dell'albero genealogico

## 1. Panoramica

Questo documento descrive il formato di file JSON usato per archiviare i dati genealogici di un albero genealogico. Il formato consente di archiviare schede delle persone con informazioni biografiche e schede delle relazioni che definiscono i legami familiari.

## 2. Struttura di primo livello

Il file JSON contiene tre chiavi di primo livello:

| Chiave | Tipo | Descrizione |
|--------|------|-------------|
| persons | Array | Array di oggetti Person con i dati biografici |
| relations | Array | Array di oggetti Relation che definiscono i legami familiari |
| version | Stringa | Versione del formato del file (ad es. "0.0.1") |

## 3. Oggetto Person (Persona)

Ogni persona dell'albero genealogico è rappresentata da un oggetto Person con la seguente struttura:

### 3.1 Campi dell'oggetto Person

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| id | Stringa (UUID) | Sì | Identificatore univoco della persona |
| name | Oggetto Name | No | Informazioni sul nome della persona |
| sex | Stringa | No | Sesso: "male", "female" o "other" |
| birth | Oggetto Event | No | Data e luogo di nascita |
| death | Oggetto Event | No | Data e luogo di morte |
| picture | Oggetto Picture | No | Foto della persona codificata in Base64 |
| text | Stringa | No | Note o biografia codificate in Base64 |

### 3.2 Oggetto Name (Nome)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| first-name | Stringa | No | Nome di battesimo della persona |
| last-name | Stringa | No | Cognome della persona |
| middle-name | Stringa | No | Secondo nome o nomi della persona |
| maiden-name | Stringa | No | Cognome di nascita (se cambiato dopo il matrimonio) |

### 3.3 Oggetto Event (Evento - Nascita/Morte/Inizio e fine di una relazione)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| date | Oggetto Date | No | Data dell'evento |
| place | Oggetto Place | No | Luogo dell'evento |

### 3.4 Oggetto Date (Data)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| year | Intero | Sì | Anno (ad es. 1985) |
| month | Intero | No | Mese (1-12), omesso se sconosciuto |
| day | Intero | No | Giorno del mese (1-31), omesso se sconosciuto |
| fuzzy | Booleano | No | True se la data è approssimativa/incerta, false se omesso |

Omettere `day` se sono noti solo il mese e l'anno, omettere sia `day` sia `month` se è noto solo l'anno.

### 3.5 Oggetto Place (Luogo)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| address | Oggetto Address | No | Informazioni strutturate sull'indirizzo |
| geo-coordinate | Oggetto Geo Coordinate | No | Posizione geografica |

### 3.6 Oggetto Address (Indirizzo)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| city | Stringa | No | Nome della città o del comune |
| country | Stringa | No | Nome del paese |
| street | Stringa | No | Nome della via |
| street-number | Stringa | No | Numero civico |
| postal-code | Stringa | No | Codice postale |

### 3.6.1 Oggetto Geo Coordinate (Coordinate geografiche)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| latitude | Numero | No | Latitudine in gradi |
| longitude | Numero | No | Longitudine in gradi |
| altitude | Numero | No | Altitudine in metri |

### 3.7 Oggetto Picture (Immagine)

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| title | Stringa | No | Titolo dell'immagine |
| description | Stringa | No | Descrizione dell'immagine |
| url | Stringa | No | Percorso di un file immagine, relativo al file dell'albero genealogico |
| base64Data | Stringa | No | Dati dell'immagine codificati in Base64 (formato PNG) |

L'immagine è archiviata in un file separato a cui fa riferimento `url`, oppure è
incorporata nel file dell'albero come `base64Data`.

Se `url` è presente, viene risolto rispetto alla directory che contiene il file
dell'albero genealogico e deve puntare a un file in quella directory o al di sotto
di essa. I percorsi che ne escono (ad esempio `../foto/giovanni.png` o un percorso
su un'altra unità) vengono rifiutati e l'immagine viene invece incorporata come
`base64Data`. Un'immagine archiviata correttamente in un file referenziato non
contiene `base64Data`.

## 4. Oggetto Relation (Relazione)

Le relazioni definiscono i legami familiari tra le persone. Ogni relazione rappresenta una coppia (matrimonio o unione simile) e, facoltativamente, elenca i figli nati da quella coppia.

### 4.1 Campi dell'oggetto Relation

| Campo | Tipo | Obbligatorio | Descrizione |
|-------|------|--------------|-------------|
| id | Stringa (UUID) | Sì | Identificatore univoco della relazione |
| partners | Array\<UUID\> | No | Array di ID di persone (di solito 2 partner), i partner sono anche i genitori dei figli |
| children | Array\<UUID\> | No | Array di ID di persone dei figli |
| begin | Oggetto Event | No | Inizio della relazione (ad es. data del matrimonio) |
| end | Oggetto Event | No | Fine della relazione (ad es. data del divorzio) |

## 5. Esempio

Di seguito un esempio di albero genealogico minimo con due persone e una relazione:

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

## 6. Note di implementazione

**Formato degli ID:** Tutti gli identificatori usano il formato UUID v4 (ad es. "4e28b000-0c04-434a-9382-792ea3543316").

**Integrità referenziale:** Tutti i riferimenti UUID negli array partners e children devono corrispondere a ID di persone validi nell'array persons.

**Numero di partner:** La maggior parte delle relazioni ha esattamente 2 partner, ma il formato supporta 0, 1 o 2 partner per schede incomplete o famiglie monoparentali.

**Codifica dei caratteri:** Il file usa la codifica UTF-8 per supportare i caratteri internazionali nei nomi di persone e luoghi.

**Campi Base64:** I campi text e picture.base64Data contengono contenuto codificato in Base64 per archiviare in modo sicuro dati binari e testo formattato in JSON.
