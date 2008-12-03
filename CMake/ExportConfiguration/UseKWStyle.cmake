#
# This module is provided as KWStyle_USE_FILE by KWStyleConfig.cmake.
# It can be INCLUDEd in a project to load the needed compiler and linker
# settings to use KWStyle:
#   FIND_PACKAGE(KWStyle REQUIRED)
#   INCLUDE(${KWStyle_USE_FILE})

IF(NOT KWStyle_USE_FILE_INCLUDED)
  SET(KWStyle_USE_FILE_INCLUDED 1)

  # Load the compiler settings used for KWStyle.
  IF(KWStyle_BUILD_SETTINGS_FILE)
    INCLUDE(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
    CMAKE_IMPORT_BUILD_SETTINGS(${KWStyle_BUILD_SETTINGS_FILE})
  ENDIF(KWStyle_BUILD_SETTINGS_FILE)

  # Add compiler flags needed to use KWStyle.
  SET(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} ${KWStyle_REQUIRED_C_FLAGS}")
  SET(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${KWStyle_REQUIRED_CXX_FLAGS}")
  SET(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} ${KWStyle_REQUIRED_EXE_LINKER_FLAGS}")
  SET(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} ${KWStyle_REQUIRED_SHARED_LINKER_FLAGS}")
  SET(CMAKE_MODULE_LINKER_FLAGS
    "${CMAKE_MODULE_LINKER_FLAGS} ${KWStyle_REQUIRED_MODULE_LINKER_FLAGS}")

  # Add include directories needed to use KWStyle.
  INCLUDE_DIRECTORIES(${KWStyle_INCLUDE_DIRS})

  # Add link directories needed to use KWStyle.
  LINK_DIRECTORIES(${KWStyle_LIBRARY_DIRS})

  # Add cmake module path.
  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${KWStyle_CMAKE_DIR}")

ENDIF(NOT KWStyle_USE_FILE_INCLUDED)
