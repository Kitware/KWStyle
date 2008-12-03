# Generate the KWStyleConfig.cmake file in the build tree. Also configure
# one for installation. The file tells external projects how to use
# KWStyle.


#-----------------------------------------------------------------------------
# Settings specific to the build tree.


# The "use" file.
SET(KWStyle_USE_FILE_CONFIG 
  ${CMAKE_CURRENT_SOURCE_DIR}/UseKWStyle.cmake)

# The build settings file.
SET(KWStyle_BUILD_SETTINGS_FILE_CONFIG 
  ${KWStyle_BINARY_DIR}/KWStyleBuildSettings.cmake)

# The library directories.
SET(KWStyle_LIBRARY_DIRS ${KWStyle_LIBRARY_DIRS})

# The EXECUTABLE directories.
SET(KWStyle_EXECUTABLE_DIRS ${KWStyle_EXECUTABLE_DIRS})

# The include directories.
SET(KWStyle_INCLUDE_DIRS_CONFIG ${KWStyle_INCLUDE_PATH})

# The library dependencies file.
SET(KWStyle_LIBRARY_DEPENDS_FILE 
  ${KWStyle_BINARY_DIR}/KWStyleLibraryDepends.cmake)

# The CMake macros dir.
SET(KWStyle_CMAKE_DIR_CONFIG 
  ${KWStyle_CMAKE_DIR})

# The build configuration information.
SET(KWStyle_CONFIGURATION_TYPES_CONFIG ${KWStyle_CONFIGURATION_TYPES})
SET(KWStyle_BUILD_TYPE_CONFIG ${CMAKE_BUILD_TYPE})

# Configure KWStyleConfig.cmake for the build tree.
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/KWStyleConfig.cmake.in
  ${KWStyle_BINARY_DIR}/KWStyleConfig.cmake @ONLY IMMEDIATE)
