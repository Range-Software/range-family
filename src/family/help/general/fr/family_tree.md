# Arbre généalogique

Un arbre généalogique se compose de deux types d'éléments de base : les **personnes** et leurs **relations**.

- **Personne** - représente une personne réelle
- **Relation** - représente une relation entre plusieurs personnes

## Personne

- **ID** - Identifiant unique
- **Nom** - Le nom contient le **prénom**, le **deuxième prénom**, le **nom de famille** et le **nom de jeune fille**
- **Naissance** - La naissance d'une personne est définie par une date et un lieu
- **Décès** - Le décès d'une personne est défini par une date et un lieu
- **Sexe** - Homme, femme ou autre
- **Image** - Image d'une personne avec titre et description, stockée soit dans l'arbre généalogique, soit dans un fichier distinct
- **Notes** - Notes ou biographie d'une personne

Aucun paramètre sauf l'**ID** n'est unique, ce qui signifie que plusieurs personnes peuvent avoir le même nom ou d'autres informations personnelles identiques. De plus, tous les paramètres sauf l'**ID** peuvent être vides.

## Relation

- **ID** - Identifiant unique
- **Partenaires** - Liste des **partenaires** de la relation (les **partenaires** d'une relation ont aussi le rôle de **parents**)
- **Enfants** - Liste des **enfants** de la relation
- **Début** - Le début d'une relation est défini par une date et un lieu
- **Fin** - La fin d'une relation est définie par une date et un lieu

Aucun paramètre sauf l'**ID** n'est unique, ce qui signifie que plusieurs relations peuvent avoir les mêmes paramètres et les mêmes personnes dans les listes de **partenaires** et d'**enfants**. De plus, tous les paramètres sauf l'**ID** peuvent être vides.
