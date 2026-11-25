# Spécification du fichier JSON de l'arbre généalogique

## 1. Présentation

Ce document décrit le format de fichier JSON utilisé pour stocker les données généalogiques d'un arbre généalogique. Le format permet de stocker des fiches de personnes avec des informations biographiques et des fiches de relations qui définissent les liens familiaux.

## 2. Structure de premier niveau

Le fichier JSON contient trois clés de premier niveau :

| Clé | Type | Description |
|-----|------|-------------|
| persons | Tableau | Tableau d'objets Person contenant les données biographiques |
| relations | Tableau | Tableau d'objets Relation définissant les liens familiaux |
| version | Chaîne | Version du format de fichier (par ex. "0.0.1") |

## 3. Objet Person (Personne)

Chaque personne de l'arbre généalogique est représentée par un objet Person ayant la structure suivante :

### 3.1 Champs de l'objet Person

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| id | Chaîne (UUID) | Oui | Identifiant unique de la personne |
| name | Objet Name | Non | Informations sur le nom de la personne |
| sex | Chaîne | Non | Sexe : "male", "female" ou "other" |
| birth | Objet Event | Non | Date et lieu de naissance |
| death | Objet Event | Non | Date et lieu de décès |
| picture | Objet Picture | Non | Photo de la personne encodée en Base64 |
| text | Chaîne | Non | Notes ou biographie encodées en Base64 |

### 3.2 Objet Name (Nom)

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| first-name | Chaîne | Non | Prénom de la personne |
| last-name | Chaîne | Non | Nom de famille de la personne |
| middle-name | Chaîne | Non | Deuxième(s) prénom(s) de la personne |
| maiden-name | Chaîne | Non | Nom de naissance (s'il a changé après le mariage) |

### 3.3 Objet Event (Événement - Naissance/Décès/Début et fin d'une relation)

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| date | Objet Date | Non | Date de l'événement |
| place | Objet Place | Non | Lieu de l'événement |

### 3.4 Objet Date

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| year | Entier | Oui | Année (par ex. 1985) |
| month | Entier | Non | Mois (1-12), omis s'il est inconnu |
| day | Entier | Non | Jour du mois (1-31), omis s'il est inconnu |
| fuzzy | Booléen | Non | True si la date est approximative/incertaine, false s'il est omis |

Omettez `day` si seuls le mois et l'année sont connus, et omettez `day` et `month` si seule l'année est connue.

### 3.5 Objet Place (Lieu)

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| address | Objet Address | Non | Informations d'adresse structurées |
| geo-coordinate | Objet Geo Coordinate | Non | Position géographique |

### 3.6 Objet Address (Adresse)

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| city | Chaîne | Non | Nom de la ville ou de la commune |
| country | Chaîne | Non | Nom du pays |
| street | Chaîne | Non | Nom de la rue |
| street-number | Chaîne | Non | Numéro de la maison |
| postal-code | Chaîne | Non | Code postal |

### 3.6.1 Objet Geo Coordinate (Coordonnées géographiques)

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| latitude | Nombre | Non | Latitude en degrés |
| longitude | Nombre | Non | Longitude en degrés |
| altitude | Nombre | Non | Altitude en mètres |

### 3.7 Objet Picture (Image)

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| title | Chaîne | Non | Titre de l'image |
| description | Chaîne | Non | Description de l'image |
| url | Chaîne | Non | Chemin vers un fichier image, relatif au fichier de l'arbre généalogique |
| base64Data | Chaîne | Non | Données de l'image encodées en Base64 (format PNG) |

L'image elle-même est stockée soit dans un fichier distinct référencé par `url`,
soit intégrée au fichier de l'arbre sous forme de `base64Data`.

Lorsque `url` est présent, il est résolu par rapport au répertoire contenant le
fichier de l'arbre généalogique et doit désigner un fichier situé dans ce
répertoire ou en dessous. Les chemins qui en sortent (par exemple
`../photos/jean.png` ou un chemin sur un autre lecteur) sont refusés et l'image
est alors intégrée sous forme de `base64Data`. Une image correctement stockée dans
un fichier référencé ne contient pas de `base64Data`.

## 4. Objet Relation

Les relations définissent les liens familiaux entre les personnes. Chaque relation représente un couple (mariage ou union similaire) et indique éventuellement les enfants issus de ce couple.

### 4.1 Champs de l'objet Relation

| Champ | Type | Obligatoire | Description |
|-------|------|-------------|-------------|
| id | Chaîne (UUID) | Oui | Identifiant unique de la relation |
| partners | Tableau\<UUID\> | Non | Tableau d'ID de personnes (généralement 2 partenaires), les partenaires sont aussi les parents des enfants |
| children | Tableau\<UUID\> | Non | Tableau d'ID de personnes des enfants |
| begin | Objet Event | Non | Début de la relation (par ex. date du mariage) |
| end | Objet Event | Non | Fin de la relation (par ex. date du divorce) |

## 5. Exemple

Voici un exemple d'arbre généalogique minimal avec deux personnes et une relation :

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

## 6. Notes d'implémentation

**Format des ID :** Tous les identifiants utilisent le format UUID v4 (par ex. "4e28b000-0c04-434a-9382-792ea3543316").

**Intégrité référentielle :** Toutes les références UUID des tableaux partners et children doivent correspondre à des ID de personnes valides du tableau persons.

**Nombre de partenaires :** La plupart des relations ont exactement 2 partenaires, mais le format accepte 0, 1 ou 2 partenaires pour les fiches incomplètes ou les familles monoparentales.

**Encodage des caractères :** Le fichier utilise l'encodage UTF-8 pour prendre en charge les caractères internationaux dans les noms de personnes et de lieux.

**Champs Base64 :** Les champs text et picture.base64Data contiennent un contenu encodé en Base64 afin de stocker en toute sécurité des données binaires et du texte formaté dans le JSON.
