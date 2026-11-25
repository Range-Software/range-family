# Family Tree JSON File Specification

## 1. Overview

This document describes the JSON file format used to store genealogical data for family tree applications. The format supports storing person records with biographical information and relationship records that define family connections.

## 2. Top-Level Structure

The JSON file contains three top-level keys:

| Key | Type | Description |
|-----|------|-------------|
| persons | Array | Array of Person objects containing biographical data |
| relations | Array | Array of Relation objects defining family connections |
| version | String | File format version (e.g., "0.0.1") |

## 3. Person Object

Each person in the family tree is represented by a Person object with the following structure:

### 3.1 Person Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| id | String (UUID) | Yes | Unique identifier for the person |
| name | Name Object | No | Person's name information |
| sex | String | No | Sex: "male", "female" or "other" |
| birth | Event Object | No | Birth date and place information |
| death | Event Object | No | Death date and place information |
| picture | Picture Object | No | Base64-encoded photo of the person |
| text | String | No | Base64-encoded notes or biography |

### 3.2 Name Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| first-name | String | No | Person's given/first name |
| last-name | String | No | Person's family/surname |
| middle-name | String | No | Person's middle name(s) |
| maiden-name | String | No | Person's birth surname (if changed after marriage) |

### 3.3 Event Object (Birth/Death/Relation begin and end)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| date | Date Object | No | Date of the event |
| place | Place Object | No | Location of the event |

### 3.4 Date Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| year | Integer | Yes | Year (e.g., 1985) |
| month | Integer | No | Month (1-12), left out if unknown |
| day | Integer | No | Day of month (1-31), left out if unknown |
| fuzzy | Boolean | No | True if date is approximate/uncertain, false if left out |

Leave out `day` if only the month and year are known, leave out both `day` and `month` if only the year is known.

### 3.5 Place Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| address | Address Object | No | Structured address information |
| geo-coordinate | Geo Coordinate Object | No | Geographic location |

### 3.6 Address Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| city | String | No | City or town name |
| country | String | No | Country name |
| street | String | No | Street name |
| street-number | String | No | House number |
| postal-code | String | No | Postal code |

### 3.6.1 Geo Coordinate Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| latitude | Number | No | Latitude in degrees |
| longitude | Number | No | Longitude in degrees |
| altitude | Number | No | Altitude in meters |

### 3.7 Picture Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| title | String | No | Picture title |
| description | String | No | Picture description |
| url | String | No | Path to an image file, relative to the family tree file |
| base64Data | String | No | Base64-encoded image data (PNG format) |

The image itself is stored either in a separate file referenced by `url`, or
embedded in the tree file as `base64Data`.

When `url` is present, it is resolved relative to the directory containing the
family tree file and must point to a file at or below that directory. Paths
leading outside of it (for example `../photos/john.png` or a path on a different
drive) are rejected, and the image is embedded as `base64Data` instead. A picture
successfully stored in a referenced file carries no `base64Data`.

## 4. Relation Object

Relations define family connections between persons. Each relation represents a partnership (marriage or similar) and optionally lists children born from that partnership.

### 4.1 Relation Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| id | String (UUID) | Yes | Unique identifier for the relation |
| partners | Array\<UUID\> | No | Array of person IDs (typically 2 partners), partners are also parents of the children |
| children | Array\<UUID\> | No | Array of person IDs of children |
| begin | Event Object | No | Start of relationship (e.g., marriage date) |
| end | Event Object | No | End of relationship (e.g., divorce date) |

## 5. Example

Below is an example of a minimal family tree with two persons and one relation:

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

## 6. Implementation Notes

**ID Format:** All identifiers use UUID v4 format (e.g., "4e28b000-0c04-434a-9382-792ea3543316").

**Referential Integrity:** All UUID references in the partners and children arrays must correspond to valid person IDs in the persons array.

**Partner Count:** Most relations have exactly 2 partners, but the format supports 0, 1, or 2 partners to accommodate incomplete records or single-parent situations.

**Character Encoding:** The file uses UTF-8 encoding to support international characters in names and place names.

**Base64 Fields:** The text and picture.base64Data fields contain Base64-encoded content to safely store binary data and formatted text within JSON.
