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
| name | Objekt Name | Nie | Informácie o mene osoby |
| sex | Reťazec | Nie | Pohlavie: "male", "female" alebo "other" |
| birth | Objekt Event | Nie | Informácie o dátume a mieste narodenia |
| death | Objekt Event | Nie | Informácie o dátume a mieste úmrtia |
| picture | Objekt Picture | Nie | Fotografia osoby kódovaná v Base64 |
| text | Reťazec | Nie | Poznámky alebo životopis kódované v Base64 |

### 3.2 Objekt Name (Meno)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| first-name | Reťazec | Nie | Krstné meno osoby |
| last-name | Reťazec | Nie | Priezvisko osoby |
| middle-name | Reťazec | Nie | Stredné meno/mená osoby |
| maiden-name | Reťazec | Nie | Rodné priezvisko (ak sa zmenilo po svadbe) |

### 3.3 Objekt Event (Udalosť - Narodenie/Úmrtie/Začiatok a koniec vzťahu)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| date | Objekt Date | Nie | Dátum udalosti |
| place | Objekt Place | Nie | Miesto udalosti |

### 3.4 Objekt Date (Dátum)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| year | Celé číslo | Áno | Rok (napr. 1985) |
| month | Celé číslo | Nie | Mesiac (1-12), vynechá sa ak nie je známy |
| day | Celé číslo | Nie | Deň v mesiaci (1-31), vynechá sa ak nie je známy |
| fuzzy | Boolean | Nie | True ak je dátum približný/neistý, false ak je vynechané |

Ak je známy iba mesiac a rok, vynechá sa `day`, ak je známy iba rok, vynechajú sa `day` aj `month`.

### 3.5 Objekt Place (Miesto)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| address | Objekt Address | Nie | Štruktúrované informácie o adrese |
| geo-coordinate | Objekt Geo Coordinate | Nie | Zemepisná poloha |

### 3.6 Objekt Address (Adresa)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| city | Reťazec | Nie | Názov mesta alebo obce |
| country | Reťazec | Nie | Názov krajiny |
| street | Reťazec | Nie | Názov ulice |
| street-number | Reťazec | Nie | Číslo domu |
| postal-code | Reťazec | Nie | Poštové smerovacie číslo |

### 3.6.1 Objekt Geo Coordinate (Zemepisné súradnice)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| latitude | Číslo | Nie | Zemepisná šírka v stupňoch |
| longitude | Číslo | Nie | Zemepisná dĺžka v stupňoch |
| altitude | Číslo | Nie | Nadmorská výška v metroch |

### 3.7 Objekt Picture (Obrázok)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| title | Reťazec | Nie | Názov obrázka |
| description | Reťazec | Nie | Popis obrázka |
| url | Reťazec | Nie | Cesta k súboru s obrázkom, relatívna k súboru rodokmeňa |
| base64Data | Reťazec | Nie | Obrazové dáta kódované v Base64 (formát PNG) |

Samotný obrázok je uložený buď v samostatnom súbore, na ktorý odkazuje `url`,
alebo je vložený priamo v súbore rodokmeňa ako `base64Data`.

Ak je `url` uvedená, vyhodnocuje sa relatívne k adresáru, v ktorom sa nachádza
súbor rodokmeňa, a musí odkazovať na súbor v tomto adresári alebo pod ním. Cesty
smerujúce mimo neho (napríklad `../fotky/jan.png` alebo cesta na inom disku) sú
odmietnuté a obrázok sa namiesto toho vloží ako `base64Data`. Obrázok úspešne
uložený do odkazovaného súboru neobsahuje `base64Data`.

## 4. Objekt Relation (Vzťah)

Vzťahy definujú rodinné väzby medzi osobami. Každý vzťah predstavuje partnerstvo (manželstvo alebo podobný zväzok) a voliteľne uvádza deti narodené z tohto partnerstva.

### 4.1 Polia objektu Relation

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| id | Reťazec (UUID) | Áno | Jedinečný identifikátor vzťahu |
| partners | Pole\<UUID\> | Nie | Pole ID osôb (zvyčajne 2 partneri), partneri sú zároveň rodičmi detí |
| children | Pole\<UUID\> | Nie | Pole ID osôb detí |
| begin | Objekt Event | Nie | Začiatok vzťahu (napr. dátum svadby) |
| end | Objekt Event | Nie | Koniec vzťahu (napr. dátum rozvodu) |

## 5. Príklad

Nižšie je uvedený príklad minimálneho rodokmeňa s dvoma osobami a jedným vzťahom:

```json
{
  "version": "0.0.1",
  "persons": [
    {
      "id": "a1b2c3d4-e5f6-4789-abcd-ef1234567890",
      "name": { "first-name": "Ján", "last-name": "Novák" },
      "sex": "male",
      "birth": {
        "date": { "day": 15, "month": 6, "year": 1980, "fuzzy": false },
        "place": { "address": { "city": "Bratislava", "country": "Slovensko" } }
      }
    },
    {
      "id": "b7c8d9e0-f1a2-4b3c-8d4e-5f6a7b8c9d0e",
      "name": { "first-name": "Mária", "last-name": "Nováková", "maiden-name": "Horváthová" },
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

## 6. Implementačné poznámky

**Formát ID:** Všetky identifikátory používajú formát UUID v4 (napr. "4e28b000-0c04-434a-9382-792ea3543316").

**Referenčná integrita:** Všetky UUID referencie v poliach partners a children musia zodpovedať platným ID osôb v poli persons.

**Počet partnerov:** Väčšina vzťahov má presne 2 partnerov, ale formát podporuje 0, 1 alebo 2 partnerov pre prípady neúplných záznamov alebo situácií s jedným rodičom.

**Kódovanie znakov:** Súbor používa kódovanie UTF-8 na podporu medzinárodných znakov v menách a názvoch miest.

**Polia Base64:** Polia text a picture.base64Data obsahujú obsah kódovaný v Base64 na bezpečné ukladanie binárnych dát a formátovaného textu v rámci JSON.
