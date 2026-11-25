# Especificación del archivo JSON del árbol genealógico

## 1. Descripción general

Este documento describe el formato de archivo JSON utilizado para almacenar los datos genealógicos de un árbol genealógico. El formato permite almacenar registros de personas con información biográfica y registros de relaciones que definen los vínculos familiares.

## 2. Estructura de nivel superior

El archivo JSON contiene tres claves de nivel superior:

| Clave | Tipo | Descripción |
|-------|------|-------------|
| persons | Array | Array de objetos Person con datos biográficos |
| relations | Array | Array de objetos Relation que definen los vínculos familiares |
| version | Cadena | Versión del formato de archivo (p. ej., "0.0.1") |

## 3. Objeto Person (Persona)

Cada persona del árbol genealógico se representa mediante un objeto Person con la siguiente estructura:

### 3.1 Campos del objeto Person

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| id | Cadena (UUID) | Sí | Identificador único de la persona |
| name | Objeto Name | No | Información del nombre de la persona |
| sex | Cadena | No | Sexo: "male", "female" u "other" |
| birth | Objeto Event | No | Fecha y lugar de nacimiento |
| death | Objeto Event | No | Fecha y lugar de defunción |
| picture | Objeto Picture | No | Foto de la persona codificada en Base64 |
| text | Cadena | No | Notas o biografía codificadas en Base64 |

### 3.2 Objeto Name (Nombre)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| first-name | Cadena | No | Nombre de pila de la persona |
| last-name | Cadena | No | Apellido de la persona |
| middle-name | Cadena | No | Segundo nombre o nombres de la persona |
| maiden-name | Cadena | No | Apellido de nacimiento (si cambió tras el matrimonio) |

### 3.3 Objeto Event (Evento - Nacimiento/Defunción/Inicio y fin de una relación)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| date | Objeto Date | No | Fecha del evento |
| place | Objeto Place | No | Lugar del evento |

### 3.4 Objeto Date (Fecha)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| year | Entero | Sí | Año (p. ej., 1985) |
| month | Entero | No | Mes (1-12), se omite si se desconoce |
| day | Entero | No | Día del mes (1-31), se omite si se desconoce |
| fuzzy | Booleano | No | True si la fecha es aproximada/incierta, false si se omite |

Omita `day` si solo se conocen el mes y el año, y omita `day` y `month` si solo se conoce el año.

### 3.5 Objeto Place (Lugar)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| address | Objeto Address | No | Información estructurada de la dirección |
| geo-coordinate | Objeto Geo Coordinate | No | Ubicación geográfica |

### 3.6 Objeto Address (Dirección)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| city | Cadena | No | Nombre de la ciudad o localidad |
| country | Cadena | No | Nombre del país |
| street | Cadena | No | Nombre de la calle |
| street-number | Cadena | No | Número de la casa |
| postal-code | Cadena | No | Código postal |

### 3.6.1 Objeto Geo Coordinate (Coordenadas geográficas)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| latitude | Número | No | Latitud en grados |
| longitude | Número | No | Longitud en grados |
| altitude | Número | No | Altitud en metros |

### 3.7 Objeto Picture (Imagen)

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| title | Cadena | No | Título de la imagen |
| description | Cadena | No | Descripción de la imagen |
| url | Cadena | No | Ruta a un archivo de imagen, relativa al archivo del árbol genealógico |
| base64Data | Cadena | No | Datos de la imagen codificados en Base64 (formato PNG) |

La imagen se almacena en un archivo independiente al que hace referencia `url`
o se incrusta en el archivo del árbol como `base64Data`.

Si `url` está presente, se resuelve de forma relativa al directorio que contiene
el archivo del árbol genealógico y debe apuntar a un archivo en ese directorio o
por debajo de él. Las rutas que salen de él (por ejemplo `../fotos/juan.png` o una
ruta en otra unidad) se rechazan y, en su lugar, la imagen se incrusta como
`base64Data`. Una imagen almacenada correctamente en un archivo referenciado no
contiene `base64Data`.

## 4. Objeto Relation (Relación)

Las relaciones definen los vínculos familiares entre personas. Cada relación representa una pareja (matrimonio o similar) y, opcionalmente, enumera los hijos nacidos de esa pareja.

### 4.1 Campos del objeto Relation

| Campo | Tipo | Obligatorio | Descripción |
|-------|------|-------------|-------------|
| id | Cadena (UUID) | Sí | Identificador único de la relación |
| partners | Array\<UUID\> | No | Array de ID de personas (normalmente 2 parejas), las parejas son también los padres de los hijos |
| children | Array\<UUID\> | No | Array de ID de personas de los hijos |
| begin | Objeto Event | No | Inicio de la relación (p. ej., fecha de matrimonio) |
| end | Objeto Event | No | Fin de la relación (p. ej., fecha de divorcio) |

## 5. Ejemplo

A continuación se muestra un ejemplo de un árbol genealógico mínimo con dos personas y una relación:

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

## 6. Notas de implementación

**Formato de ID:** Todos los identificadores usan el formato UUID v4 (p. ej., "4e28b000-0c04-434a-9382-792ea3543316").

**Integridad referencial:** Todas las referencias UUID de los arrays partners y children deben corresponder a ID de personas válidos del array persons.

**Número de parejas:** La mayoría de las relaciones tienen exactamente 2 parejas, pero el formato admite 0, 1 o 2 parejas para registros incompletos o familias monoparentales.

**Codificación de caracteres:** El archivo usa la codificación UTF-8 para admitir caracteres internacionales en nombres y topónimos.

**Campos Base64:** Los campos text y picture.base64Data contienen contenido codificado en Base64 para almacenar de forma segura datos binarios y texto con formato dentro de JSON.
