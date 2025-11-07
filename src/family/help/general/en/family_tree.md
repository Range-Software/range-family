# Family tree

Family tree is build from two types of basic entities which are **persons** and their **relations**.

- **Person** - represents a real person
- **Relation** - represents a relation between multiple persons

## Person

- **ID** - Unique identifier
- **Name** - Name contains **first**, **middle**, **last** and **maiden** names
- **Birth** - Birth of a person is defined by date and place
- **Death** - Death of a person is defined by date and place

All parameters except **ID** are not unique which means that there can be multiple persons with the same name or any other personal information. Also all parameters except **ID** can be empty.

## Relation

- **ID** - Unique identifier
- **Partners** - List of **partners** in the relation (**partners** in relation have also **parent** role)
- **Children** - List of **children** in the relation
- **Begin** - Begin of a relation is defined by date and place
- **End** - End of a relation is defined by date and place

All parameters except **ID** are not unique which means that there can be multiple relations with same parameters and persons in **partners** and **children** lists. Also all parameters except **ID** can be empty.
