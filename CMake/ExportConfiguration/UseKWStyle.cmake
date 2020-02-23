#
# This module is provided as KWStyle_USE_FILE by KWStyleConfig.cmake.
# It can be INCLUDEd in a project to load the needed compiler and linker
# settings to use KWStyle:
#   find_package(KWStyle REQUIRED)
#   include(${KWStyle_USE_FILE})

if(NOT KWStyle_USE_FILE_INCLUDED)
  set(KWStyle_USE_FILE_INCLUDED 1)

  # Load the compiler settings used for KWStyle.
  if(KWStyle_BUILD_SETTINGS_FILE)
    include(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
    CMAKE_IMPORT_BUILD_SETTINGS(${KWStyle_BUILD_SETTINGS_FILE})
  endif(KWStyle_BUILD_SETTINGS_FILE)

  # Add compiler flags needed to use KWStyle.
  set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} ${KWStyle_REQUIRED_C_FLAGS}")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} ${KWStyle_REQUIRED_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} ${KWStyle_REQUIRED_EXE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} ${KWStyle_REQUIRED_SHARED_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS
    "${CMAKE_MODULE_LINKER_FLAGS} ${KWStyle_REQUIRED_MODULE_LINKER_FLAGS}")

  # Add include directories needed to use KWStyle.
  include_directories(${KWStyle_INCLUDE_DIRS})

  # Add link directories needed to use KWStyle.
  link_directories(${KWStyle_LIBRARY_DIRS})

  # Add cmake module path.
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${KWStyle_CMAKE_DIR}")

endif(NOT KWStyle_USE_FILE_INCLUDED)
