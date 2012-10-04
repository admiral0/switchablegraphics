# In this CMakeLists.txt we define which files
# are used to compile the application
set(switchablegraphics_SRCS
   switchablegraphics.cpp
   main.cpp
 )

# Run ui files through uic
kde4_add_ui_files(switchablegraphics_SRCS switchablegraphics.ui)

# Set the name of the application
kde4_add_executable(switchablegraphics ${switchablegraphics_SRCS})

# Select which libraries we need to link to
target_link_libraries(switchablegraphics ${KDE4_KDEUI_LIBS})

# Tell cmake to install the application binary
install(TARGETS switchablegraphics ${INSTALL_TARGETS_DEFAULT_ARGS})

# Install the .desktop file
install( PROGRAMS switchablegraphics.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
