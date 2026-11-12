## Version 1.3.0

### Improvements

- Redesigned all icons in one clean, light style shared with Range FEA: a 24 px
  grid, uniform line width and a common color palette where each color carries
  one meaning (green add/confirm, red remove/cancel, amber edit, blue files and
  navigation, violet AI). Icons used for the same purpose in Range FEA are reused.
- "About", "License" and "Release notes" have their own icons instead of the
  application logo.
- Every icon has a dark variant which is used while a dark color scheme is
  active. Icons already shown switch as soon as the color scheme changes.
- The Windows application icon fills the whole icon area, so it no longer looks
  smaller than other Windows applications. Its small sizes are drawn with
  heavier lines to stay readable. macOS keeps its own icon.
- New macOS application setting to draw the menu bar inside the application
  window instead of the system menu bar. It takes effect after a restart.
- New `--reset-settings` command line option resets all application settings
  to their default values. It replaces `--reset-defaults`.
- Links in help pages and release notes work: links to other help pages open
  them, links to headings scroll to them and web or e-mail links open in the
  default external application.
- The release notes dialog opens on the first topic instead of an empty view
  and always keeps one topic selected.
- Builds for the Apple App Store and Microsoft Store do not check for software
  updates: the automatic check, its application setting and the "Check for
  update" action are left out.
- On macOS the private key and the certificate signing request for Cloud are
  generated with the Apple Security framework instead of the `openssl` tool, and
  so is the text shown by the private key view. The `openssl` tool is no longer
  bundled with the application, only the OpenSSL libraries used for TLS
  connections. The paths of the OpenSSL tool and its configuration are hidden in
  the application settings. Configure with `-DRANGE_NATIVE_KEY_TOOL=OFF` to keep
  using the `openssl` tool.
- New "Folders" submenu in the File menu opens the application folders -
  "Certificates", "Data", "Documents", "Logs" and "Temporary files" - in the file
  browser of the operating system (Finder on macOS, File Explorer on Windows, the
  file manager on Linux). In the Apple App Store version these folders are kept
  inside the application container, where they are otherwise hard to reach.
- New "Privacy policy" action in the Help menu shows the privacy policy, which is
  now installed together with the license and the release notes.
- Builds for the Apple App Store and Microsoft Store produce a submission ready
  artifact: a sandboxed bundle signed for the App Store and wrapped in a `.pkg`,
  and an MSIX package. Both are built by their own target, so an ordinary release
  build is left untouched. Each upload takes a build number, so resubmitting the
  same release after a rejection is accepted.
- Confirmation messages of the Cloud file manager name the affected file: Cloud
  files with their path and identifier, downloaded files with their local file
  name.
- The software manager lists only versions newer than the one being run, unless
  "show all" is checked.
- The user interface is translated to Czech, German, Spanish, French, Italian,
  Polish and Ukrainian in addition to English and Slovak, and all of them are
  offered in the language selection of the application settings. A language
  change takes effect after a restart.
- New "Verification" setting of the Cloud host in the Cloud session manager.
  "Full verification" (default) checks that the host certificate is issued by a
  trusted certificate authority and matches the host name. "Trusted certificate
  only (skip host name)" is meant for a server reached by its IP address: set its
  certificate authority as the session's host "Certificate" and the host name is
  not checked. "No verification" accepts any certificate and does not guarantee
  the identity of the host.
- Cloud requests are faster. A request completes as soon as the reply arrives,
  where it used to wait up to an extra second, and connections to the Cloud are
  kept open and reused between requests instead of being opened for each one.
- New optional ribbon layout shows commands on the pages "Family tree",
  "Persons", "Relations" and "Cloud and AI" instead of menus and tool bars. The
  "File" menu, "Save", Cloud, application settings and help stay in the line with
  the page tabs. Switch it with "Ribbon layout" in the File menu or on the
  application tool bar. "Compact ribbon" shows it in less vertical space, with
  smaller buttons and no group titles. Both choices, the selected page and the
  collapsed state are remembered.
- Tool bars are reorganized into "File", "Family tree", "Persons and relations"
  and "Application" tool bars, each of which can be moved or hidden on its own.
  Related commands share one drop-down button: "Save" and "Save as", storing of
  images, and adding, editing and removing of persons and of relations. "Quit"
  and "About" are no longer on a tool bar. The new arrangement is applied once
  to existing installations.
- Adding, editing and removing of persons and relations, "Show family tree",
  "Set selected person as active", "Merge selected persons" and "Suggest birth
  and death dates" are application actions: they are found in the new "Edit"
  menu, on the tool bar and the ribbon, can be given keyboard shortcuts in the
  application settings and have help pages.

### Bug fixes

- Changing the style or the color scheme while the application runs shrank the
  tool bar icons to the default size of the style. Tool bars now keep the icon
  size set in the application settings.
- After the color scheme changed, the person details kept the colors of the
  previous scheme, the labels of the "Editor" and "Output" tabs kept the previous
  text color, so a dark scheme left them white on a light background, and text
  already printed to the application output kept the previous colors.

- The Apple App Store version could not write to its folders. They were kept in
  `Documents/Range Software/Family`, which the App Sandbox does not allow to be
  written to. The sandboxed version now keeps them in
  `Library/Application Support/Range Software/Family` inside its container
  (`~/Library/Containers/com.range-software.family/Data`); other versions keep
  using the Documents folder. Use "File > Folders" to find them.
- Certificate signing requests generated on macOS store subject fields with
  characters outside ASCII, such as `Žilina`, correctly. The `openssl` tool used
  before stored them in a mangled form (`Å½ilina`).
- The Cloud host certificate was not verified, so a connection to a host
  presenting a certificate from an untrusted authority or for another host name
  was accepted and only reported as SSL errors in the log. Such connections are
  now refused. A session whose host uses a private certificate authority needs
  that authority set as its host "Certificate", or its "Verification" relaxed.

### Submodules

- range-ai-lib @ v1.2.0
- range-base-lib @ v1.2.1
- range-build-tools @ v1.1.1
- range-cloud-lib @ v1.3.0
- range-family-lib @ v1.2.0
- range-gui-lib @ v1.3.1

---

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

- range-ai-lib @ v1.1.0
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
