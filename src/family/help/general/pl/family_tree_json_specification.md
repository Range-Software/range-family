# Specyfikacja pliku JSON drzewa genealogicznego

## 1. Przegląd

Ten dokument opisuje format pliku JSON służący do przechowywania danych genealogicznych drzewa genealogicznego. Format pozwala przechowywać rekordy osób z informacjami biograficznymi oraz rekordy relacji określające więzi rodzinne.

## 2. Struktura najwyższego poziomu

Plik JSON zawiera trzy klucze najwyższego poziomu:

| Klucz | Typ | Opis |
|-------|-----|------|
| persons | Tablica | Tablica obiektów Person z danymi biograficznymi |
| relations | Tablica | Tablica obiektów Relation określających więzi rodzinne |
| version | Ciąg znaków | Wersja formatu pliku (np. "0.0.1") |

## 3. Obiekt Person (Osoba)

Każda osoba w drzewie genealogicznym jest reprezentowana przez obiekt Person o następującej strukturze:

### 3.1 Pola obiektu Person

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| id | Ciąg znaków (UUID) | Tak | Unikalny identyfikator osoby |
| name | Obiekt Name | Nie | Informacje o imieniu i nazwisku osoby |
| sex | Ciąg znaków | Nie | Płeć: "male", "female" lub "other" |
| birth | Obiekt Event | Nie | Data i miejsce urodzenia |
| death | Obiekt Event | Nie | Data i miejsce śmierci |
| picture | Obiekt Picture | Nie | Zdjęcie osoby zakodowane w Base64 |
| text | Ciąg znaków | Nie | Notatki lub biografia zakodowane w Base64 |

### 3.2 Obiekt Name (Imię i nazwisko)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| first-name | Ciąg znaków | Nie | Pierwsze imię osoby |
| last-name | Ciąg znaków | Nie | Nazwisko osoby |
| middle-name | Ciąg znaków | Nie | Drugie imię lub imiona osoby |
| maiden-name | Ciąg znaków | Nie | Nazwisko rodowe (jeśli zmieniło się po ślubie) |

### 3.3 Obiekt Event (Zdarzenie - Urodzenie/Śmierć/Początek i koniec relacji)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| date | Obiekt Date | Nie | Data zdarzenia |
| place | Obiekt Place | Nie | Miejsce zdarzenia |

### 3.4 Obiekt Date (Data)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| year | Liczba całkowita | Tak | Rok (np. 1985) |
| month | Liczba całkowita | Nie | Miesiąc (1-12), pomijany, jeśli nieznany |
| day | Liczba całkowita | Nie | Dzień miesiąca (1-31), pomijany, jeśli nieznany |
| fuzzy | Boolean | Nie | True, jeśli data jest przybliżona/niepewna, false, jeśli pominięte |

Pomiń `day`, jeśli znany jest tylko miesiąc i rok, a pomiń zarówno `day`, jak i `month`, jeśli znany jest tylko rok.

### 3.5 Obiekt Place (Miejsce)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| address | Obiekt Address | Nie | Ustrukturyzowane informacje o adresie |
| geo-coordinate | Obiekt Geo Coordinate | Nie | Położenie geograficzne |

### 3.6 Obiekt Address (Adres)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| city | Ciąg znaków | Nie | Nazwa miasta lub miejscowości |
| country | Ciąg znaków | Nie | Nazwa kraju |
| street | Ciąg znaków | Nie | Nazwa ulicy |
| street-number | Ciąg znaków | Nie | Numer domu |
| postal-code | Ciąg znaków | Nie | Kod pocztowy |

### 3.6.1 Obiekt Geo Coordinate (Współrzędne geograficzne)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| latitude | Liczba | Nie | Szerokość geograficzna w stopniach |
| longitude | Liczba | Nie | Długość geograficzna w stopniach |
| altitude | Liczba | Nie | Wysokość w metrach |

### 3.7 Obiekt Picture (Zdjęcie)

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| title | Ciąg znaków | Nie | Tytuł zdjęcia |
| description | Ciąg znaków | Nie | Opis zdjęcia |
| url | Ciąg znaków | Nie | Ścieżka do pliku obrazu, względna wobec pliku drzewa genealogicznego |
| base64Data | Ciąg znaków | Nie | Dane obrazu zakodowane w Base64 (format PNG) |

Sam obraz jest przechowywany w osobnym pliku, do którego odwołuje się `url`,
albo osadzony w pliku drzewa jako `base64Data`.

Jeśli podano `url`, jest on rozwiązywany względem katalogu zawierającego plik
drzewa genealogicznego i musi wskazywać na plik w tym katalogu lub poniżej niego.
Ścieżki wychodzące poza niego (na przykład `../zdjecia/jan.png` lub ścieżka na
innym dysku) są odrzucane, a obraz jest zamiast tego osadzany jako `base64Data`.
Zdjęcie poprawnie zapisane w pliku, do którego się odwołuje, nie zawiera
`base64Data`.

## 4. Obiekt Relation (Relacja)

Relacje określają więzi rodzinne między osobami. Każda relacja reprezentuje związek (małżeństwo lub podobny) i opcjonalnie wymienia dzieci urodzone w tym związku.

### 4.1 Pola obiektu Relation

| Pole | Typ | Wymagane | Opis |
|------|-----|----------|------|
| id | Ciąg znaków (UUID) | Tak | Unikalny identyfikator relacji |
| partners | Tablica\<UUID\> | Nie | Tablica ID osób (zwykle 2 partnerów), partnerzy są zarazem rodzicami dzieci |
| children | Tablica\<UUID\> | Nie | Tablica ID osób dzieci |
| begin | Obiekt Event | Nie | Początek relacji (np. data ślubu) |
| end | Obiekt Event | Nie | Koniec relacji (np. data rozwodu) |

## 5. Przykład

Poniżej znajduje się przykład minimalnego drzewa genealogicznego z dwiema osobami i jedną relacją:

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

## 6. Uwagi dotyczące implementacji

**Format ID:** Wszystkie identyfikatory używają formatu UUID v4 (np. "4e28b000-0c04-434a-9382-792ea3543316").

**Integralność referencyjna:** Wszystkie odwołania UUID w tablicach partners i children muszą odpowiadać prawidłowym ID osób w tablicy persons.

**Liczba partnerów:** Większość relacji ma dokładnie 2 partnerów, ale format obsługuje 0, 1 lub 2 partnerów na potrzeby niepełnych rekordów lub rodzin z jednym rodzicem.

**Kodowanie znaków:** Plik używa kodowania UTF-8, aby obsługiwać znaki międzynarodowe w nazwiskach i nazwach miejsc.

**Pola Base64:** Pola text i picture.base64Data zawierają treść zakodowaną w Base64, aby bezpiecznie przechowywać dane binarne i sformatowany tekst w JSON.
