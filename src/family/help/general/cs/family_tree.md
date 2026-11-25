# Rodokmen

Rodokmen se skládá ze dvou typů základních prvků: **osob** a jejich **vztahů**.

- **Osoba** - představuje skutečnou osobu
- **Vztah** - představuje vztah mezi více osobami

## Osoba

- **ID** - Jedinečný identifikátor
- **Jméno** - Jméno obsahuje **křestní**, **prostřední**, **příjmení** a **rodné příjmení**
- **Narození** - Narození osoby je definováno datem a místem
- **Úmrtí** - Úmrtí osoby je definováno datem a místem
- **Pohlaví** - Muž, žena nebo jiné
- **Obrázek** - Obrázek osoby s názvem a popisem, uložený buď v rodokmenu, nebo v samostatném souboru
- **Poznámky** - Poznámky nebo životopis osoby

Všechny parametry kromě **ID** nejsou jedinečné, což znamená, že může existovat více osob se stejným jménem nebo jinými osobními údaji. Také všechny parametry kromě **ID** mohou být prázdné.

## Vztah

- **ID** - Jedinečný identifikátor
- **Partneři** - Seznam **partnerů** ve vztahu (**partneři** ve vztahu mají také roli **rodiče**)
- **Děti** - Seznam **dětí** ve vztahu
- **Začátek** - Začátek vztahu je definován datem a místem
- **Konec** - Konec vztahu je definován datem a místem

Všechny parametry kromě **ID** nejsou jedinečné, což znamená, že může existovat více vztahů se stejnými parametry a osobami v seznamech **partnerů** a **dětí**. Také všechny parametry kromě **ID** mohou být prázdné.
