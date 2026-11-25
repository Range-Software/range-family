# Drzewo genealogiczne

Drzewo genealogiczne składa się z dwóch rodzajów podstawowych elementów: **osób** i ich **relacji**.

- **Osoba** - reprezentuje rzeczywistą osobę
- **Relacja** - reprezentuje relację między wieloma osobami

## Osoba

- **ID** - Unikalny identyfikator
- **Imię i nazwisko** - Zawiera **pierwsze imię**, **drugie imię**, **nazwisko** i **nazwisko rodowe**
- **Urodzenie** - Urodzenie osoby jest określone datą i miejscem
- **Śmierć** - Śmierć osoby jest określona datą i miejscem
- **Płeć** - Mężczyzna, kobieta lub inna
- **Zdjęcie** - Zdjęcie osoby z tytułem i opisem, zapisane w drzewie genealogicznym albo w osobnym pliku
- **Notatki** - Notatki lub biografia osoby

Żaden parametr oprócz **ID** nie jest unikalny, co oznacza, że może istnieć wiele osób o tym samym imieniu i nazwisku lub innych takich samych danych osobowych. Ponadto wszystkie parametry oprócz **ID** mogą być puste.

## Relacja

- **ID** - Unikalny identyfikator
- **Partnerzy** - Lista **partnerów** w relacji (**partnerzy** w relacji pełnią również rolę **rodziców**)
- **Dzieci** - Lista **dzieci** w relacji
- **Początek** - Początek relacji jest określony datą i miejscem
- **Koniec** - Koniec relacji jest określony datą i miejscem

Żaden parametr oprócz **ID** nie jest unikalny, co oznacza, że może istnieć wiele relacji o tych samych parametrach i tych samych osobach na listach **partnerów** i **dzieci**. Ponadto wszystkie parametry oprócz **ID** mogą być puste.
