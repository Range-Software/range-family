# Árbol genealógico

Un árbol genealógico se compone de dos tipos de elementos básicos: **personas** y sus **relaciones**.

- **Persona** - representa una persona real
- **Relación** - representa una relación entre varias personas

## Persona

- **ID** - Identificador único
- **Nombre** - El nombre contiene el **nombre de pila**, el **segundo nombre**, el **apellido** y el **apellido de soltera**
- **Nacimiento** - El nacimiento de una persona se define por la fecha y el lugar
- **Defunción** - La defunción de una persona se define por la fecha y el lugar
- **Sexo** - Hombre, mujer u otro
- **Imagen** - Imagen de una persona con título y descripción, almacenada en el árbol genealógico o en un archivo independiente
- **Notas** - Notas o biografía de una persona

Ningún parámetro excepto el **ID** es único, lo que significa que puede haber varias personas con el mismo nombre u otros datos personales iguales. Además, todos los parámetros excepto el **ID** pueden estar vacíos.

## Relación

- **ID** - Identificador único
- **Parejas** - Lista de las **parejas** de la relación (las **parejas** de una relación también tienen el papel de **padres**)
- **Hijos** - Lista de los **hijos** de la relación
- **Inicio** - El inicio de una relación se define por la fecha y el lugar
- **Fin** - El fin de una relación se define por la fecha y el lugar

Ningún parámetro excepto el **ID** es único, lo que significa que puede haber varias relaciones con los mismos parámetros y las mismas personas en las listas de **parejas** e **hijos**. Además, todos los parámetros excepto el **ID** pueden estar vacíos.
