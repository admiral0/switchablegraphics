set( KCM_SRCS switchablegraphics.cpp)

set_property(SOURCE ../common/switchablegraphics.xml PROPERTY NO_NAMESPACE 1)
set_property(SOURCE ../common/switchablegraphics.xml PROPERTY INCLUDE ../common/dbusdefs.h)
QT4_ADD_DBUS_INTERFACE(KCM_DBUS ../common/switchablegraphics.xml SwitchableGraphicsIface)

kde4_add_ui_files(KCM_SRCS switchablegraphicsconfig.ui)
kde4_add_plugin(kcm_switchablegraphics ${KCM_SRCS} ${KCM_DBUS})
target_link_libraries(kcm_switchablegraphics
    ${KDE4_KDEUI_LIBS}
    # Other necessary libraries go there
)

install(TARGETS kcm_switchablegraphics
    DESTINATION ${PLUGIN_INSTALL_DIR}
)
 
install(FILES switchablegraphicsconfig.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)