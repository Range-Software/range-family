# Albero genealogico

Un albero genealogico è composto da due tipi di elementi di base: le **persone** e le loro **relazioni**.

- **Persona** - rappresenta una persona reale
- **Relazione** - rappresenta una relazione tra più persone

## Persona

- **ID** - Identificatore univoco
- **Nome** - Il nome contiene il **nome di battesimo**, il **secondo nome**, il **cognome** e il **cognome da nubile**
- **Nascita** - La nascita di una persona è definita da data e luogo
- **Morte** - La morte di una persona è definita da data e luogo
- **Sesso** - Maschio, femmina o altro
- **Immagine** - Immagine di una persona con titolo e descrizione, archiviata nell'albero genealogico o in un file separato
- **Note** - Note o biografia di una persona

Nessun parametro tranne l'**ID** è univoco, il che significa che possono esistere più persone con lo stesso nome o con altri dati personali uguali. Inoltre tutti i parametri tranne l'**ID** possono essere vuoti.

## Relazione

- **ID** - Identificatore univoco
- **Partner** - Elenco dei **partner** della relazione (i **partner** di una relazione hanno anche il ruolo di **genitori**)
- **Figli** - Elenco dei **figli** della relazione
- **Inizio** - L'inizio di una relazione è definito da data e luogo
- **Fine** - La fine di una relazione è definita da data e luogo

Nessun parametro tranne l'**ID** è univoco, il che significa che possono esistere più relazioni con gli stessi parametri e le stesse persone negli elenchi di **partner** e **figli**. Inoltre tutti i parametri tranne l'**ID** possono essere vuoti.
