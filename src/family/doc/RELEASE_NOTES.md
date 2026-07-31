## Version 1.1.1

### Improvements

- Person pictures can be stored in separate image files next to the family
  tree file instead of being embedded in it, which keeps tree files small.
  Saving the tree under a new name copies the images alongside it.
- Documented the picture `url` field and the rest of the picture object in the
  family tree JSON file specification, in both English and Slovak
- Family tool accepts both `.json` and `.xml` family tree files. Until now it
  could only read `.xml`, even though it was able to write both. An
  unsupported file type is now reported as such instead of failing as a
  malformed XML file.

### Submodules

- range-ai-lib @ v1.0.0
- range-base-lib @ v1.0.0
- range-build-tools @ v1.0.0
- range-cloud-lib @ v1.0.0
- range-family-lib @ v1.1.0
- range-gui-lib @ v1.0.0

---

## Version 1.1.0

### Improvements

- Added AI assistant
- Can specify an active person in the family tree which will be treated as
  the main character in the family tree diagram and in the AI assistant
  responses
- Added scroll markers to persons and relationships lists
- Release notes displayed in a dedicated dialog with markdown rendering

### Bug fixes

- Fixed person details markdown view displaying birth date and place instead
  of death date and place in the "Died" field

### Submodules

- range-ai-lib @ v1.0.0
- range-base-lib @ v1.0.0
- range-build-tools @ v1.0.0
- range-cloud-lib @ v1.0.0
- range-family-lib @ v1.0.0
- range-gui-lib @ v1.0.0

---

## Version 1.0.4

- Translated tutorials to Slovak

### Bug fixes

- Fixed memory leak when building family tree diagram
- Fix SSL error on macOS by seeding system CA certificates
- Fixed spelling errors in user interface

---

## Version 1.0.3

- Automatic client certificate renewal
- Show warning when client certificate is about to expire

---

## Version 1.0.2

- Improve diagram graphics with modern visual styling
- Print time-stamps in output window
- Offer to save a tree when original file is deleted
- Log Qt debug messages

### Bug fixes

- Do not report that file has changed after saving it
- Automatic file synchronization with Range Cloud

---

## Version 1.0.1

### Bug fixes

- Fixed problem when too many new files appeared in upload directory

---

## Version 1.0.0

Initial release.
