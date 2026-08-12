## Version 1.2.0

### Improvements

- Person pictures can be stored in separate image files next to the family tree
  file instead of being embedded in it, which keeps tree files small. Selecting a
  person picture asks where it should be stored. A picture kept within the family
  tree is reduced to occupy a maximum of 1 MB, a picture stored separately is
  written next to the tree file as `<tree name>_<person id>.png`.
- A newly added person is assigned its ID up front, so its picture can be stored
  as a separate image file already in the "Add person" dialog.
- Saving the tree under a new name moves the separate image files along with it
  and renames them after the new tree file name. Opening a family tree file which
  is not stored in the data directory offers to move it, together with its image
  files, to the data directory.
- Documented the picture `url` field and the rest of the picture object in the
  family tree JSON file specification, in both English and Slovak.
- New "Cloud AI query" action, available from the Cloud menu and the main toolbar,
  asks a question which is answered by an AI service through Range Cloud. No AI
  agent has to be configured, only the language of the answer is selected. The
  question is submitted by pressing Enter.
- The Cloud AI query window is not modal, so the family tree can be worked with
  while the window is open. Only one such window is opened at a time and closing
  it cancels a query which is still being processed.
- Questions refer to the family tree file stored in Cloud, so the answers are
  based on the opened tree. A note is displayed when the tree cannot be taken
  into account, because it has not been saved, is not stored in the data
  directory, has not been synchronized to Cloud yet or the synchronization of the
  data directory is turned off. A note is displayed as well when the file stored
  in Cloud differs from the opened one or contains unsaved changes.
- Previous questions and their answers are sent along with every following
  question, so follow-up questions can be asked. The conversation is restarted
  once another family tree is loaded. User name, e-mail and territory taken from
  the application settings are sent with the first question of a conversation.
- "AI settings manager" and "AI chat" were removed from the main toolbar. Both
  remain available from the AI menu.
- Family tool accepts both `.json` and `.xml` family tree files. Until now it
  could only read `.xml`, even though it was able to write both. An unsupported
  file type is now reported as such instead of failing as a malformed XML file.

### Submodules

- range-ai-lib @ v1.0.0
- range-base-lib @ v1.1.0
- range-build-tools @ v1.0.0
- range-cloud-lib @ v1.1.0
- range-family-lib @ v1.1.0
- range-gui-lib @ v1.1.0

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
