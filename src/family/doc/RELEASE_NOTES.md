## Version 1.3.0

### Improvements

- The main window is rearranged around the family tree diagram, which now stays
  in its middle.
  - The diagram follows the person selected in the list of persons or the
    relation selected in the list of relations, and a double-click on a person
    or relation in the diagram selects it in the lists. The person or relation
    the diagram is built around glows, so it is easy to spot. Its controls are
    drawn over its corners, "Export tree" on the left and zoom on the right, so
    the diagram uses the whole height. "Show family tree" still opens the
    diagram in its own window.
  - "Persons", "Person details", "Relations", "Output" and "AI chat" are panels
    which can be moved to another side of the window, stacked, tabbed, detached
    into their own window or closed. Tabs of panels stacked at the left or the
    right side are placed on the outer edge of the window. The arrangement is
    remembered.
  - By default "Persons" and "Relations" share tabs on the left above "Person
    details"; "AI chat" and "Output" are hidden. The new arrangement is applied
    once to existing installations. On the first start the main window covers
    80 % of the screen width and height.
  - New "View" menu shows and hides the panels, and "Restore default layout"
    puts all panels and tool bars back to their default places. Its entries can
    be given keyboard shortcuts in the application settings and have help pages.
  - AI chat is a panel instead of a window staying on top. "AI chat" opens it,
    brings it to the front if it is behind another panel, and closes it if it
    already is in front.
  - The application output is no longer a tab of the main window. While it is
    not visible, the status bar shows the number of new warnings or errors, and
    a click on it shows the output.
- Dates can be entered without a day, or without a day and a month, when only
  the month or the year is known. The date editor has separate day, month and
  year fields; "Calendar" still picks a full date. Such dates are shown as
  "1850" or "February 1852" everywhere and are sorted by their earliest possible
  day in the lists of persons and relations. New "Approximate" option of the
  date editor marks a date as uncertain; until now this mark could only be set
  by suggested dates and by imports.
- "Suggest birth and death dates" is rewritten. It uses dates from the whole
  family tree instead of only the closest relatives, takes the sex of a parent,
  dates of partners and deaths of parents into account, and picks the most
  likely date from typical ages instead of the middle of the possible range.
  Suggested dates are approximate years. The dialog shows the possible range of
  each date and the approximate date it would replace, birth and death are
  accepted separately, and contradicting dates of relatives are reported instead
  of being averaged.
- Lists of persons and relations have a filter, which shows only rows containing
  all typed words. Ctrl+F moves to the filter of the list being used. The number
  of rows, and of the shown ones while filtering, is displayed beside the filter.
  - New "Always show highlighted" option below both lists keeps persons of the
    selected relations, and relations of the selected persons, visible even if
    they do not match the filter. New "Only highlighted" option below the list
    of relations shows only relations of the selected persons.
  - "Add", "Edit" and "Remove" buttons below the lists are replaced by small
    buttons beside the filter, and the same commands are in the context menu of
    each list.
  - Highlighted rows use a color which stays readable in the dark color scheme.
- Person details are easier to read and navigate. Names of parents, siblings,
  partners and children are links which select that person, listed one per line
  with short dates; rows and dates which are empty are left out. The picture is
  shown at the top. The title has the color of the person's card in the family
  tree diagram and the sections the color of a relation card, in both the light
  and the dark color scheme. Without a selected person a hint is shown, and more
  than three selected persons are shown as a summary of their names and dates.
- The user interface is translated to Czech, German, Spanish, French, Italian,
  Polish and Ukrainian in addition to English and Slovak, and all of them are
  offered in the language selection of the application settings. A language
  change takes effect after a restart.
- New optional ribbon layout shows commands on the pages "Family tree",
  "Persons", "Relations" and "Cloud and AI" instead of menus and tool bars. The
  "File" menu, "Save", Cloud, application settings and help stay in the line with
  the page tabs. Switch it with "Ribbon layout" in the File menu or on the
  application tool bar. "Compact ribbon" shows it in less vertical space, with
  smaller buttons and no group titles. Both choices, the selected page and the
  collapsed state are remembered.
- Commands are reorganized.
  - Adding, editing and removing of persons and relations, "Show family tree",
    "Set selected person as active", "Merge selected persons" and "Suggest birth
    and death dates" are application actions: they are found in the new "Edit"
    menu, on the tool bar and the ribbon, can be given keyboard shortcuts in the
    application settings and have help pages.
  - Tool bars are split into "File", "Family tree", "Persons and relations" and
    "Application" tool bars, each of which can be moved or hidden on its own.
    Related commands share one drop-down button: "Save" and "Save as", storing
    of images, and adding, editing and removing of persons and of relations.
    "Quit" and "About" are no longer on a tool bar. The new arrangement is
    applied once to existing installations.
- Cloud requests are faster. A request completes as soon as the reply arrives,
  where it used to wait up to an extra second, and connections to the Cloud are
  kept open and reused between requests instead of being opened for each one.
- New "Verification" setting of the Cloud host in the Cloud session manager
  (see also the certificate verification fix below). "Full verification"
  (default) checks that the host certificate is issued by a trusted certificate
  authority and matches the host name. "Trusted certificate only (skip host
  name)" is meant for a server reached by its IP address: set its certificate
  authority as the session's host "Certificate" and the host name is not
  checked. "No verification" accepts any certificate and does not guarantee the
  identity of the host.
- Confirmation messages of the Cloud file manager name the affected file: Cloud
  files with their path and identifier, downloaded files with their local file
  name.
- All icons are redesigned in one clean, light style shared with Range FEA: a
  24 px grid, uniform line width and a common color palette where each color
  carries one meaning (green add/confirm, red remove/cancel, amber edit, blue
  files and navigation, violet AI). Icons used for the same purpose in Range FEA
  are reused.
  - Every icon has a dark variant which is used while a dark color scheme is
    active. Icons already shown switch as soon as the color scheme changes.
  - "About", "License" and "Release notes" have their own icons instead of the
    application logo.
  - The Windows application icon fills the whole icon area, so it no longer
    looks smaller than other Windows applications. Its small sizes are drawn
    with heavier lines to stay readable. macOS keeps its own icon.
- New "Folders" submenu in the File menu opens the application folders -
  "Certificates", "Data", "Documents", "Logs" and "Temporary files" - in the file
  browser of the operating system (Finder on macOS, File Explorer on Windows, the
  file manager on Linux). In the Apple App Store version these folders are kept
  inside the application container, where they are otherwise hard to reach.
- Help and release notes are easier to use. Links in help pages and release
  notes work: links to other help pages open them, links to headings scroll to
  them and web or e-mail links open in the default external application. The
  release notes dialog opens on the first topic instead of an empty view and
  always keeps one topic selected.
- New "Privacy policy" action in the Help menu shows the privacy policy, which
  is now installed together with the license and the release notes.
- The software manager lists only versions newer than the one being run, unless
  "show all" is checked.
- New macOS application setting draws the menu bar inside the application window
  instead of the system menu bar. It takes effect after a restart.
- On macOS the private key and the certificate signing request for Cloud are
  generated with the Apple Security framework instead of the `openssl` tool, and
  so is the text shown by the private key view. The `openssl` tool is no longer
  bundled with the macOS application, only the OpenSSL libraries used for TLS
  connections, and the paths of the OpenSSL tool and its configuration are
  hidden in the application settings. Configure with
  `-DRANGE_NATIVE_KEY_TOOL=OFF` to keep using the `openssl` tool.
- New `--reset-settings` command line option resets all application settings to
  their default values. It replaces `--reset-defaults`.
- The application can be published in the Apple App Store and the Microsoft
  Store. Store builds produce a submission ready artifact: a sandboxed bundle
  signed for the App Store and wrapped in a `.pkg`, and an MSIX package. Both are
  built by their own target, so an ordinary release build is left untouched.
  Each upload takes a build number, so resubmitting the same release after a
  rejection is accepted. Store builds do not check for software updates: the
  automatic check, its application setting and the "Check for update" action
  are left out.
- "AI chat" and "Cloud AI query" ask for permission before the family tree is
  sent to an AI service for the first time. The question names the service and
  the data to be sent: for "AI chat" the question and the family tree without
  pictures and notes, for "Cloud AI query" the question, the family tree file
  stored in Cloud and the user's name, e-mail address and territory. Nothing is
  sent when the permission is not given. All given permissions can be withdrawn
  in the "AI services" group on the "Network" tab of the application settings.
- Store builds send usage information only after the user turns it on, either
  on the first start or in the application settings.

### Bug fixes

- The application froze when an error or a warning was reported while the
  "Output" panel was hidden, e.g. after a "Cloud AI query" was sent while Cloud
  was not accessible. Messages are now passed to the "Output" panel after the
  logger has finished, so the panel can safely update the error count shown in
  the status bar or show a notice dialog.
- With "Synchronize data directory" turned on, every file of the family tree
  could be stored in Cloud several times, and each save added more copies.
  Saving the tree shortly after the start, before the list of Cloud files had
  been received, uploaded all files again, repeatedly while the folder was still
  changing. Files are now compared only with a fresh list of Cloud files, a file
  is never stored twice and copies left by earlier versions are removed, keeping
  the most recent one.
- The Cloud host certificate was not verified, so a connection to a host
  presenting a certificate from an untrusted authority or for another host name
  was accepted and only reported as SSL errors in the log. Such connections are
  now refused. A session whose host uses a private certificate authority needs
  that authority set as its host "Certificate", or its "Verification" relaxed.
- Saving the family tree was followed by the question whether to load the
  modified file. The file is now compared with what the application last read
  or wrote, so the question is asked only when another tool or a synchronization
  has changed its content. A change made while the tree is being saved is no
  longer missed, and a file replaced by another tool stays watched.
- "Suggest birth and death dates" replaced a known death date with a suggested
  one when the birth date was missing or approximate, and suggested a death at
  an age of over a hundred years for a person known only by the parents.
- "Cloud AI query" did not notice when the family tree file had not been
  synchronized to Cloud yet. Instead of saying so, it sent the query with a
  reference to a file which does not exist, and Cloud answered with an error.
- Saving the tree rewrote all pictures even when they did not change, so Cloud
  and iCloud transferred them again. Only changed pictures are written now.
- Files which belong to one computer are no longer kept in the Documents
  folder. The log files, the temporary folder (including the Cloud
  synchronization state), the certificates with private keys and the Cloud
  sessions are kept in `Library/Application Support/Range Software/Family` on
  macOS, `AppData/Local/Range Software/Family` on Windows and
  `~/.local/share/Range Software/Family` on Linux. When Documents is
  synchronized across computers, e.g. by iCloud, their state no longer mixes up
  and private keys stay on the computer they belong to. Existing certificates
  and sessions are copied to the new folder on the first start; the old ones
  stay in the Documents folder and can be deleted once every computer has been
  started with this version. Use "File > Folders" to find them.
- The Apple App Store version could not write to its folders. They were kept in
  `Documents/Range Software/Family`, which the App Sandbox does not allow to be
  written to. The sandboxed version now keeps all of them in
  `Library/Application Support/Range Software/Family` inside its container
  (`~/Library/Containers/com.range-software.family/Data`); other versions keep
  the remaining folders in the Documents folder.
- The Apple App Store version could not open, import or save a family tree
  outside of its container, e.g. in the Documents or Downloads folder. Its file
  dialogs listed only the container, because only the native macOS file dialog
  grants a sandboxed application access to the file the user selects. The
  sandboxed version now uses the native dialog; other versions keep the
  application's own one.
- The Apple App Store version could not save a family tree it was given access
  to outside of its container, because the file was written into a temporary
  file next to it first. Pictures of such a tree are embedded in the tree file,
  as no picture files may be created next to it.
- The Apple App Store version did not reopen the last family tree on start when
  it was stored outside of its container. The access to the file is now kept in
  the settings and restored on the next start.
- After the color scheme changed, the person details kept the colors of the
  previous scheme, the labels of the "Editor" and "Output" tabs kept the previous
  text color, so a dark scheme left them white on a light background, and text
  already printed to the application output kept the previous colors. Changing
  the style or the color scheme also shrank the tool bar icons to the default
  size of the style; tool bars now keep the icon size set in the application
  settings.
- Certificate signing requests generated on macOS store subject fields with
  characters outside ASCII, such as `Žilina`, correctly. The `openssl` tool used
  before stored them in a mangled form (`Å½ilina`).

### Submodules

- range-ai-lib @ v1.2.0
- range-base-lib @ v1.2.2
- range-build-tools @ v1.2.0
- range-cloud-lib @ v1.3.1
- range-family-lib @ v1.2.0
- range-gui-lib @ v1.3.3

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
