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
| name | Name Object | Yes | Person's name information |
| sex | String | Yes | Gender: "male" or "female" |
| birth | Event Object | No | Birth date and place information |
| death | Event Object | No | Death date and place information |
| picture | Picture Object | No | Base64-encoded photo of the person |
| text | String | No | Base64-encoded notes or biography |

### 3.2 Name Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| first-name | String | Yes | Person's given/first name |
| last-name | String | Yes | Person's family/surname |
| middle-name | String | No | Person's middle name(s) |
| maiden-name | String | No | Person's birth surname (if changed after marriage) |

### 3.3 Event Object (Birth/Death)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| date | Date Object | No | Date of the event |
| place | Place Object | No | Location of the event |

### 3.4 Date Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| year | Integer | Yes | Year (e.g., 1985) |
| month | Integer | Yes | Month (1-12) |
| day | Integer | Yes | Day of month (1-31) |
| fuzzy | Boolean | Yes | True if date is approximate/uncertain |

### 3.5 Place Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| address | Address Object | No | Structured address information |

### 3.6 Address Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| city | String | No | City or town name |
| country | String | No | Country name |
| street-number | String | No | Street address or house number |

### 3.7 Picture Object

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| base64Data | String | Yes | Base64-encoded image data (PNG format) |

## 4. Relation Object

Relations define family connections between persons. Each relation represents a partnership (marriage or similar) and optionally lists children born from that partnership.

### 4.1 Relation Fields

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| id | String (UUID) | Yes | Unique identifier for the relation |
| partners | Array\<UUID\> | No | Array of person IDs (typically 2 partners) |
| children | Array\<UUID\> | No | Array of person IDs of children |
| begin | Event Object | No | Start of relationship (e.g., marriage date) |
| end | Event Object | No | End of relationship (e.g., divorce date) |

## 5. Data Statistics (Current File)

The following statistics describe the current family tree file:

| Metric | Value |
|--------|-------|
| Total persons | 322 |
| Male persons | 166 |
| Female persons | 156 |
| Total relations | 89 |
| Persons with birth date | 210 |
| Persons with death date | 77 |
| Persons with photo | 4 |
| Persons with notes | 14 |
| Relations with marriage date | 18 |
| Relations with end date | 1 |

## 6. Example

Below is an example of a minimal family tree with two persons and one relation:

```json
{
  "version": "0.0.1",
  "persons": [
    {
      "id": "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
      "name": { "first-name": "John", "last-name": "Smith" },
      "sex": "male",
      "birth": {
        "date": { "day": 15, "month": 6, "year": 1980, "fuzzy": false },
        "place": { "address": { "city": "Prague", "country": "Czech Republic" } }
      }
    },
    {
      "id": "e5f6g7h8-i9j0-1234-klmn-op5678901234",
      "name": { "first-name": "Jane", "last-name": "Smith", "maiden-name": "Doe" },
      "sex": "female"
    }
  ],
  "relations": [
    {
      "id": "r1s2t3u4-v5w6-7890-xyza-bc1234567890",
      "partners": ["a1b2c3d4-e5f6-7890-abcd-ef1234567890", "e5f6g7h8-i9j0-1234-klmn-op5678901234"],
      "children": [],
      "begin": { "date": { "day": 20, "month": 8, "year": 2010, "fuzzy": false } }
    }
  ]
}
```

## 7. Implementation Notes

**ID Format:** All identifiers use UUID v4 format (e.g., "4e28b000-0c04-434a-9382-792ea3543316").

**Referential Integrity:** All UUID references in the partners and children arrays must correspond to valid person IDs in the persons array.

**Partner Count:** Most relations have exactly 2 partners, but the format supports 0, 1, or 2 partners to accommodate incomplete records or single-parent situations.

**Character Encoding:** The file uses UTF-8 encoding to support international characters in names and place names.

**Base64 Fields:** The text and picture.base64Data fields contain Base64-encoded content to safely store binary data and formatted text within JSON.
