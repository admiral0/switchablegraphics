project(switchablegraphics)

find_package(KDE4 REQUIRED)
find_package(KDE4Workspace REQUIRED)
find_package(PkgConfig REQUIRED)
include (KDE4Defaults)

include_directories( ${KDE4_INCLUDES} ${KDE4WORKSPACE_INCLUDE_DIR} ${QT_INCLUDES} ${LIBPCI_INCLUDE_DIRS})

add_subdirectory(kded)
add_subdirectory(kcm)
add_subdirectory(applet)
add_subdirectory(i18n)