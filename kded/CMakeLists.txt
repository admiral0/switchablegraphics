add_subdirectory(backends)
pkg_check_modules(LIBPCI REQUIRED libpci)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/backends/vgaswitcheroo/ ${CMAKE_BINARY_DIR}/common)
set(BACKENDS_SRCS 
    backends/switcherbackend.cpp
    backends/null/nullbackend.cpp
    backends/vgaswitcheroo/vgaswitcheroobackend.cpp
)
QT4_ADD_DBUS_ADAPTOR(BACKENDS_SRCS ../common/switchablegraphics.xml switchablegraphics.h SwitchableGraphics)
KDE4_ADD_PLUGIN(kded_switchablegraphics ${BACKENDS_SRCS} ${DBUS_ADAPTOR_SRCS} backendmanager.cpp switchablegraphics.cpp )
target_link_libraries(kded_switchablegraphics ${KDE4_KDECORE_LIBS} ${KDE4_SOLID_LIBS} ${KDE4WORKSPACE_KWORKSPACE_LIBRARY} ${KDE4_KDEUI_LIBS} ${LIBPCI_LIBRARIES})

install(TARGETS kded_switchablegraphics  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES switchablegraphics.desktop  DESTINATION  ${SERVICES_INSTALL_DIR}/kded )