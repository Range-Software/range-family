qt_add_executable(family
    src/action.cpp
    src/address_edit_widget.cpp
    src/application.cpp
    src/calendar_dialog.cpp
    src/central_widget.cpp
    src/date_edit_widget.cpp
    src/diagram_dialog.cpp
    src/diagram_scene.cpp
    src/diagram_view.cpp
    src/diagram_widget.cpp
    src/editor_widget.cpp
    src/event_edit_widget.cpp
    src/geo_coordinates_edit_widget.cpp
    src/graphics_edge_item.cpp
    src/graphics_node_item.cpp
    src/graphics_person_item.cpp
    src/graphics_relation_item.cpp
    src/image_button.cpp
    src/main.cpp
    src/main_window.cpp
    src/name_edit_widget.cpp
    src/person_add_dialog.cpp
    src/person_details_widget.cpp
    src/person_edit_dialog.cpp
    src/person_edit_widget.cpp
    src/person_suggeted_dates_dialog.cpp
    src/persons_list_edit_widget.cpp
    src/persons_list_widget.cpp
    src/picture_edit_widget.cpp
    src/place_edit_widget.cpp
    src/relation_add_dialog.cpp
    src/relation_edit_dialog.cpp
    src/relation_edit_widget.cpp
    src/relations_list_widget.cpp
    src/scale_controll_widget.cpp
    src/session.cpp
    src/tree_diff_dialog.cpp
    src/tree_diff_update_person_dialog.cpp
    src/tree_diff_update_relation_dialog.cpp
    src/tree_diff_widget.cpp

    src/action.h
    src/address_edit_widget.h
    src/application.h
    src/calendar_dialog.h
    src/central_widget.h
    src/date_edit_widget.h
    src/diagram_dialog.h
    src/diagram_scene.h
    src/diagram_view.h
    src/diagram_widget.h
    src/editor_widget.h
    src/event_edit_widget.h
    src/geo_coordinates_edit_widget.h
    src/graphics_edge_item.h
    src/graphics_node_item.h
    src/graphics_person_item.h
    src/graphics_relation_item.h
    src/image_button.h
    src/main_window.h
    src/name_edit_widget.h
    src/person_add_dialog.h
    src/person_details_widget.h
    src/person_edit_dialog.h
    src/person_edit_widget.h
    src/person_suggeted_dates_dialog.h
    src/persons_list_edit_widget.h
    src/persons_list_widget.h
    src/picture_edit_widget.h
    src/place_edit_widget.h
    src/relation_add_dialog.h
    src/relation_edit_dialog.h
    src/relation_edit_widget.h
    src/relations_list_widget.h
    src/scale_controll_widget.h
    src/session.h
    src/tree_diff_dialog.h
    src/tree_diff_update_person_dialog.h
    src/tree_diff_update_relation_dialog.h
    src/tree_diff_widget.h
)

add_dependencies(family range-base-lib range-cloud-lib range-gui-lib)

target_link_libraries(family
    PRIVATE
        range-gui-lib
        range-family-lib
        common_defines
)

set_target_properties(family
    PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE "${PROJECT_PACKAGE_NAME}"
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_SHORT_NAME}"
)

if (WIN32)
    set(WIN32_ICON "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/${PROJECT_PACKAGE_NAME}.ico")
    set(ICON_RC "${CMAKE_CURRENT_BINARY_DIR}/family.rc")
    file(WRITE "${ICON_RC}"
        "IDI_ICON1 ICON \"${WIN32_ICON}\"\n"
    )
    set_source_files_properties("${ICON_RC}" PROPERTIES LANGUAGE RC)
    target_sources(family PRIVATE "${ICON_RC}")
elseif (APPLE)
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/${PROJECT_PACKAGE_NAME}.icns")
    set_source_files_properties("${APP_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(family PRIVATE "${APP_ICON}")

    add_custom_command(TARGET family POST_BUILD
        COMMAND /usr/libexec/PlistBuddy -c "Set :CFBundleName '${PROJECT_SHORT_NAME}'" "$<TARGET_BUNDLE_DIR_NAME:${PROJECT_TARGET_NAME}>/Contents/Info.plist" || /usr/libexec/PlistBuddy -c "Add :CFBundleName string '${PROJECT_SHORT_NAME}'" "$<TARGET_BUNDLE_DIR_NAME:${PROJECT_TARGET_NAME}>/Contents/Info.plist"
        COMMAND /usr/libexec/PlistBuddy -c "Set :CFBundleDisplayName '${PROJECT_SHORT_NAME}'" "$<TARGET_BUNDLE_DIR_NAME:${PROJECT_TARGET_NAME}>/Contents/Info.plist" || /usr/libexec/PlistBuddy -c "Add :CFBundleDisplayName string '${PROJECT_SHORT_NAME}'" "$<TARGET_BUNDLE_DIR_NAME:${PROJECT_TARGET_NAME}>/Contents/Info.plist"
        COMMENT "Set CFBundleName and CFBundleDisplayName"
    )
endif()

qt_add_resources(family icons-application
    PREFIX "/icons/application"
    FILES
        pixmaps/range-about.svg
        pixmaps/range-help.svg
        pixmaps/range-license.svg
        pixmaps/range-qt.svg
        pixmaps/range-quit.svg
        pixmaps/range-release_notes.svg
        pixmaps/range-settings.svg
        pixmaps/range-update.svg
)

qt_add_resources(family icons-action
    PREFIX "/icons/action"
    FILES
        pixmaps/range-add.svg
        pixmaps/range-cancel.svg
        pixmaps/range-clear.svg
        pixmaps/range-close.svg
        pixmaps/range-edit.svg
        pixmaps/range-ok.svg
        pixmaps/range-remove.svg
        pixmaps/range-undo.svg
)

qt_add_resources(family icons-logos
    PREFIX "/icons/logos"
    FILES
        pixmaps/range-app.svg
        pixmaps/range-family.svg
)

qt_add_resources(family icons-file
    PREFIX "/icons/file"
    FILES
        pixmaps/range-close.svg
        pixmaps/range-compare.svg
        pixmaps/range-import.svg
        pixmaps/range-new.svg
        pixmaps/range-open.svg
        pixmaps/range-save.svg
        pixmaps/range-save_as.svg
        pixmaps/range-export.svg
)

qt_add_resources(family icons-cloud
    PREFIX "/icons/cloud"
    FILES
        pixmaps/range-access_rights.svg
        pixmaps/range-download.svg
        pixmaps/range-file_manager.svg
        pixmaps/range-refresh.svg
        pixmaps/range-remove.svg
        pixmaps/range-session_manager.svg
        pixmaps/range-upload.svg
        pixmaps/range-upload_replace.svg
        pixmaps/range-upload_update.svg
)

qt_add_translations(family
    TS_FILES
        translations/en.ts
        translations/sk.ts
    RESOURCE_PREFIX /i18n/family
)
