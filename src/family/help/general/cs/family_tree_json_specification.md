# Specifikace souboru rodokmenu JSON

## 1. Přehled

Tento dokument popisuje formát souboru JSON, který se používá k ukládání genealogických dat rodokmenu. Formát umožňuje ukládat záznamy osob s životopisnými údaji a záznamy vztahů, které definují rodinné vazby.

## 2. Struktura nejvyšší úrovně

Soubor JSON obsahuje tři klíče nejvyšší úrovně:

| Klíč | Typ | Popis |
|------|-----|-------|
| persons | Pole | Pole objektů Person s životopisnými údaji |
| relations | Pole | Pole objektů Relation definujících rodinné vazby |
| version | Řetězec | Verze formátu souboru (např. "0.0.1") |

## 3. Objekt Person (Osoba)

Každá osoba v rodokmenu je reprezentována objektem Person s následující strukturou:

### 3.1 Pole objektu Person

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| id | Řetězec (UUID) | Ano | Jedinečný identifikátor osoby |
| name | Objekt Name | Ne | Údaje o jménu osoby |
| sex | Řetězec | Ne | Pohlaví: "male", "female" nebo "other" |
| birth | Objekt Event | Ne | Datum a místo narození |
| death | Objekt Event | Ne | Datum a místo úmrtí |
| picture | Objekt Picture | Ne | Fotografie osoby kódovaná v Base64 |
| text | Řetězec | Ne | Poznámky nebo životopis kódované v Base64 |

### 3.2 Objekt Name (Jméno)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| first-name | Řetězec | Ne | Křestní jméno osoby |
| last-name | Řetězec | Ne | Příjmení osoby |
| middle-name | Řetězec | Ne | Prostřední jméno/jména osoby |
| maiden-name | Řetězec | Ne | Rodné příjmení (pokud se po sňatku změnilo) |

### 3.3 Objekt Event (Událost - Narození/Úmrtí/Začátek a konec vztahu)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| date | Objekt Date | Ne | Datum události |
| place | Objekt Place | Ne | Místo události |

### 3.4 Objekt Date (Datum)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| year | Celé číslo | Ano | Rok (např. 1985) |
| month | Celé číslo | Ne | Měsíc (1-12), vynechá se, pokud není znám |
| day | Celé číslo | Ne | Den v měsíci (1-31), vynechá se, pokud není znám |
| fuzzy | Boolean | Ne | True, pokud je datum přibližné/nejisté, false, pokud je vynecháno |

Pokud je znám pouze měsíc a rok, vynechá se `day`, pokud je znám pouze rok, vynechají se `day` i `month`.

### 3.5 Objekt Place (Místo)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| address | Objekt Address | Ne | Strukturované údaje o adrese |
| geo-coordinate | Objekt Geo Coordinate | Ne | Zeměpisná poloha |

### 3.6 Objekt Address (Adresa)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| city | Řetězec | Ne | Název města nebo obce |
| country | Řetězec | Ne | Název země |
| street | Řetězec | Ne | Název ulice |
| street-number | Řetězec | Ne | Číslo domu |
| postal-code | Řetězec | Ne | Poštovní směrovací číslo |

### 3.6.1 Objekt Geo Coordinate (Zeměpisné souřadnice)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| latitude | Číslo | Ne | Zeměpisná šířka ve stupních |
| longitude | Číslo | Ne | Zeměpisná délka ve stupních |
| altitude | Číslo | Ne | Nadmořská výška v metrech |

### 3.7 Objekt Picture (Obrázek)

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| title | Řetězec | Ne | Název obrázku |
| description | Řetězec | Ne | Popis obrázku |
| url | Řetězec | Ne | Cesta k souboru s obrázkem, relativní k souboru rodokmenu |
| base64Data | Řetězec | Ne | Obrazová data kódovaná v Base64 (formát PNG) |

Samotný obrázek je uložen buď v samostatném souboru, na který odkazuje `url`,
nebo je vložen přímo do souboru rodokmenu jako `base64Data`.

Pokud je `url` uvedena, vyhodnocuje se relativně k adresáři, ve kterém je
soubor rodokmenu, a musí odkazovat na soubor v tomto adresáři nebo pod ním. Cesty
vedoucí mimo něj (například `../fotky/jan.png` nebo cesta na jiném disku) jsou
odmítnuty a obrázek se místo toho vloží jako `base64Data`. Obrázek úspěšně
uložený do odkazovaného souboru neobsahuje `base64Data`.

## 4. Objekt Relation (Vztah)

Vztahy definují rodinné vazby mezi osobami. Každý vztah představuje partnerství (manželství nebo podobný svazek) a volitelně uvádí děti z tohoto partnerství.

### 4.1 Pole objektu Relation

| Pole | Typ | Povinné | Popis |
|------|-----|---------|-------|
| id | Řetězec (UUID) | Ano | Jedinečný identifikátor vztahu |
| partners | Pole\<UUID\> | Ne | Pole ID osob (obvykle 2 partneři), partneři jsou zároveň rodiči dětí |
| children | Pole\<UUID\> | Ne | Pole ID osob dětí |
| begin | Objekt Event | Ne | Začátek vztahu (např. datum sňatku) |
| end | Objekt Event | Ne | Konec vztahu (např. datum rozvodu) |

## 5. Příklad

Níže je příklad minimálního rodokmenu se dvěma osobami a jedním vztahem:

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

## 6. Poznámky k implementaci

**Formát ID:** Všechny identifikátory používají formát UUID v4 (např. "4e28b000-0c04-434a-9382-792ea3543316").

**Referenční integrita:** Všechny odkazy UUID v polích partners a children musí odpovídat platným ID osob v poli persons.

**Počet partnerů:** Většina vztahů má právě 2 partnery, ale formát podporuje 0, 1 nebo 2 partnery pro neúplné záznamy nebo situace s jedním rodičem.

**Kódování znaků:** Soubor používá kódování UTF-8 pro podporu mezinárodních znaků ve jménech a názvech míst.

**Pole Base64:** Pole text a picture.base64Data obsahují obsah kódovaný v Base64 pro bezpečné uložení binárních dat a formátovaného textu v JSON.
