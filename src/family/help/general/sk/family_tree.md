# Rodinný strom

Rodinný strom pozostáva z dvoch typov prvkov a to **osôb** a ich **vzťahov**.

- **Osoba** - reprezentuje skutočnú osobu
- **Vzťah** - reprezentuje vzťah medzi osobami

## Osoba

- **ID** - Unikátny identifikátor
- **Meno** - Meno pozostáva z **prvého**, **stredného**, **posledného** a **rodného** mena
- **Narodenie** - Narodenie osoby je definované dátumom a miestom narodenia
- **Úmrtie** - Úmrtie osoby je definované dátumom a miestom úmrtia

Všetky parametre okrem **ID** nie sú unikátne čo znamená, že viaceré osoby môžu obsahovať rovnaké osobné údaje. Takisto všetky parametre okrem **ID** môžu byť prázdne.

## Vzťah

- **ID** - Unikátny identifikátor
- **Partneri** - Zoznam **partnerov** a **partneriek** vo vzťahu (**partneri/ky** vo vzťahu plnia rolu **rodiča**)
- **Deti** - Zoznam **detí** vo vzťahu
- **Začiatok** - Začiatok vzťahu je definovaný dátumom a miestom
- **Koniec** - Koniec vzťahu je definovaný dátumom a miestom

Všetky parametre okrem **ID** nie sú unikátne čo znamená, že viaceré vzťahy môžu obsahovať rovnaké údaje a osoby v zoznamoch **partnerov** a **detí**. Takisto všetky parametre okrem **ID** môžu byť prázdne.
