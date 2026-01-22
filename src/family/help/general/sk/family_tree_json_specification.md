# Špecifikácia JSON súboru rodokmeňa

## 1. Prehľad

Tento dokument popisuje formát JSON súboru používaný na ukladanie genealogických dát pre aplikácie rodokmeňa. Formát podporuje ukladanie záznamov osôb s biografickými informáciami a záznamov vzťahov, ktoré definujú rodinné väzby.

## 2. Štruktúra najvyššej úrovne

JSON súbor obsahuje tri kľúče najvyššej úrovne:

| Kľúč | Typ | Popis |
|------|-----|-------|
| persons | Pole | Pole objektov Person obsahujúcich biografické údaje |
| relations | Pole | Pole objektov Relation definujúcich rodinné väzby |
| version | Reťazec | Verzia formátu súboru (napr. "0.0.1") |

## 3. Objekt Person (Osoba)

Každá osoba v rodokmeni je reprezentovaná objektom Person s nasledujúcou štruktúrou:

### 3.1 Polia objektu Person

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| id | Reťazec (UUID) | Áno | Jedinečný identifikátor osoby |
| name | Objekt Name | Áno | Informácie o mene osoby |
| sex | Reťazec | Áno | Pohlavie: "male" alebo "female" |
| birth | Objekt Event | Nie | Informácie o dátume a mieste narodenia |
| death | Objekt Event | Nie | Informácie o dátume a mieste úmrtia |
| picture | Objekt Picture | Nie | Fotografia osoby kódovaná v Base64 |
| text | Reťazec | Nie | Poznámky alebo životopis kódované v Base64 |

### 3.2 Objekt Name (Meno)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| first-name | Reťazec | Áno | Krstné meno osoby |
| last-name | Reťazec | Áno | Priezvisko osoby |
| middle-name | Reťazec | Nie | Stredné meno/mená osoby |
| maiden-name | Reťazec | Nie | Rodné priezvisko (ak sa zmenilo po svadbe) |

### 3.3 Objekt Event (Udalosť - Narodenie/Úmrtie)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| date | Objekt Date | Nie | Dátum udalosti |
| place | Objekt Place | Nie | Miesto udalosti |

### 3.4 Objekt Date (Dátum)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| year | Celé číslo | Áno | Rok (napr. 1985) |
| month | Celé číslo | Áno | Mesiac (1-12) |
| day | Celé číslo | Áno | Deň v mesiaci (1-31) |
| fuzzy | Boolean | Áno | True ak je dátum približný/neistý |

### 3.5 Objekt Place (Miesto)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| address | Objekt Address | Nie | Štruktúrované informácie o adrese |

### 3.6 Objekt Address (Adresa)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| city | Reťazec | Nie | Názov mesta alebo obce |
| country | Reťazec | Nie | Názov krajiny |
| street-number | Reťazec | Nie | Ulica a číslo domu |

### 3.7 Objekt Picture (Obrázok)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| base64Data | Reťazec | Áno | Obrazové dáta kódované v Base64 (formát PNG) |

## 4. Objekt Relation (Vzťah)

Vzťahy definujú rodinné väzby medzi osobami. Každý vzťah predstavuje partnerstvo (manželstvo alebo podobný zväzok) a voliteľne uvádza deti narodené z tohto partnerstva.

### 4.1 Polia objektu Relation

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| id | Reťazec (UUID) | Áno | Jedinečný identifikátor vzťahu |
| partners | Pole\<UUID\> | Nie | Pole ID osôb (zvyčajne 2 partneri) |
| children | Pole\<UUID\> | Nie | Pole ID osôb detí |
| begin | Objekt Event | Nie | Začiatok vzťahu (napr. dátum svadby) |
| end | Objekt Event | Nie | Koniec vzťahu (napr. dátum rozvodu) |

## 5. Štatistiky dát (aktuálny súbor)

Nasledujúce štatistiky popisujú aktuálny súbor rodokmeňa:

| Metrika | Hodnota |
|---------|---------|
| Celkový počet osôb | 322 |
| Muži | 166 |
| Ženy | 156 |
| Celkový počet vzťahov | 89 |
| Osoby s dátumom narodenia | 210 |
| Osoby s dátumom úmrtia | 77 |
| Osoby s fotografiou | 4 |
| Osoby s poznámkami | 14 |
| Vzťahy s dátumom svadby | 18 |
| Vzťahy s dátumom ukončenia | 1 |

## 6. Príklad

Nižšie je uvedený príklad minimálneho rodokmeňa s dvoma osobami a jedným vzťahom:

```json
{
  "version": "0.0.1",
  "persons": [
    {
      "id": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
      "name": { "first-name": "Ján", "last-name": "Novák" },
      "sex": "male",
      "birth": {
        "date": { "day": 15, "month": 6, "year": 1980, "fuzzy": false },
        "place": { "address": { "city": "Bratislava", "country": "Slovensko" } }
      }
    },
    {
      "id": "e5f6g7h8-i9j0-1234-klmn-op5678901234",
      "name": { "first-name": "Mária", "last-name": "Nováková", "maiden-name": "Horváthová" },
      "sex": "female"
    }
  ],
  "relations": [
    {
      "id": "r1s2t3u4-v5w6-7890-xyza-bc1234567890",
      "partners": ["a1b2c3d4-e5f6-7890-abcd-ef1234567890", "e5f6g7h8-i9j0-1234-klmn-op5678901234"],
      "children": [],
      "begin": { "date": { "day": 20, "month": 8, "year": 2010, "fuzzy": false } }
    }
  ]
}
```

## 7. Implementačné poznámky

**Formát ID:** Všetky identifikátory používajú formát UUID v4 (napr. "4e28b000-0c04-434a-9382-792ea3543316").

**Referenčná integrita:** Všetky UUID referencie v poliach partners a children musia zodpovedať platným ID osôb v poli persons.

**Počet partnerov:** Väčšina vzťahov má presne 2 partnerov, ale formát podporuje 0, 1 alebo 2 partnerov pre prípady neúplných záznamov alebo situácií s jedným rodičom.

**Kódovanie znakov:** Súbor používa kódovanie UTF-8 na podporu medzinárodných znakov v menách a názvoch miest.

**Polia Base64:** Polia text a picture.base64Data obsahujú obsah kódovaný v Base64 na bezpečné ukladanie binárnych dát a formátovaného textu v rámci JSON.
