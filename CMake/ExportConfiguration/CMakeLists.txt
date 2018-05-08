# Generate the KWStyleConfig.cmake file in the build tree. Also configure
# one for installation. The file tells external projects how to use
# KWStyle.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.


# The "use" file.
set(KWStyle_USE_FILE_CONFIG 
  ${CMAKE_CURRENT_SOURCE_DIR}/UseKWStyle.cmake)

# The build settings file.
set(KWStyle_BUILD_SETTINGS_FILE_CONFIG 
  ${KWStyle_BINARY_DIR}/KWStyleBuildSettings.cmake)

# The library directories.
set(KWStyle_LIBRARY_DIRS ${KWStyle_LIBRARY_DIRS})

# The EXECUTABLE directories.
set(KWStyle_EXECUTABLE_DIRS ${KWStyle_EXECUTABLE_DIRS})

# The include directories.
set(KWStyle_INCLUDE_DIRS_CONFIG ${KWStyle_INCLUDE_PATH})

# The library dependencies file.
set(KWStyle_LIBRARY_DEPENDS_FILE 
  ${KWStyle_BINARY_DIR}/KWStyleLibraryDepends.cmake)

# The CMake macros dir.
set(KWStyle_CMAKE_DIR_CONFIG 
  ${KWStyle_CMAKE_DIR})

# The build configuration information.
set(KWStyle_CONFIGURATION_TYPES_CONFIG ${KWStyle_CONFIGURATION_TYPES})
set(KWStyle_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

# Configure KWStyleConfig.cmake for the build tree.
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/KWStyleConfig.cmake.in
  ${KWStyle_BINARY_DIR}/KWStyleConfig.cmake @ONLY IMMEDIATE)
