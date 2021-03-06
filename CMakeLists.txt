cmake_minimum_required(VERSION 3.10.2)

foreach(p
    # Place newer policies here if needed for cmake greater than 3.10.2
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# ==== Define language standard configurations requiring at least c++11 standard
if(CMAKE_CXX_STANDARD EQUAL "98" )
   message(FATAL_ERROR "CMAKE_CXX_STANDARD:STRING=98 is not supported in ITK version 5 and greater.")
endif()

#####
##  Set the default target properties for ITK
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11) # Supported values are ``11``, ``14``, and ``17``.
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

project(KWStyle
   VERSION 1.1.0
   DESCRIPTION "Kitware Style Checker"
   LANGUAGES C CXX
)


#-----------------------------------------------------------------------------
if(NOT COMMAND SETIFEMPTY)
  macro(SETIFEMPTY)
    set(KEY ${ARGV0})
    set(VALUE ${ARGV1})
    if(NOT ${KEY})
      set(${ARGV})
    endif()
  endmacro()
endif()

#-----------------------------------------------------------------------------
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${KWStyle_BINARY_DIR}/bin)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${KWStyle_BINARY_DIR}/bin)
SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${KWStyle_BINARY_DIR}/bin)
SETIFEMPTY(CMAKE_BUNDLE_OUTPUT_DIRECTORY  ${KWStyle_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CXX_TEST_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Disable MSVC 8 warnings
if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

# Add testing
enable_testing()
include(CTest)

include_directories(
${CMAKE_CURRENT_BINARY_DIR}
${KWStyle_BINARY_DIR}/Utilities
${KWStyle_SOURCE_DIR}/Utilities
)

set(KWStyle_EXECUTABLE_DIRS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(KWStyle_LIBRARY_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Configure the export configuration
add_subdirectory(CMake/ExportConfiguration)

configure_file(${KWStyle_SOURCE_DIR}/KWStyleConfigure.h.in
  ${KWStyle_BINARY_DIR}/KWStyleConfigure.h)

set(KWStyle_HDRS
kwsParser.h
kwsHeader.h
kwsXMLReader.h
kwsGenerator.h
kwsTestFile.h
metaCommand.h
)

set(KWStyle_SRCS
metaCommand.cxx
kwsParser.cxx
kwsXMLReader.cxx
kwsGenerator.cxx
kwsCheckLineLength.cxx
kwsCheckHeader.cxx
kwsCheckSemicolonSpace.cxx
kwsCheckIfNDefDefine.cxx
kwsCheckTypedefs.cxx
kwsCheckIndent.cxx
kwsCheckEndOfFileNewLine.cxx
kwsCheckDeclarationOrder.cxx
kwsCheckInternalVariables.cxx
kwsCheckNamespace.cxx
kwsCheckComments.cxx
kwsCheckTabs.cxx
kwsCheckNameOfClass.cxx
kwsCheckEmptyLines.cxx
kwsCheckTemplate.cxx
kwsCheckOperator.cxx
kwsCheckBlackList.cxx
kwsCheckExtraSpaces.cxx
kwsCheckStatementPerLine.cxx
kwsCheckVariablePerLine.cxx
kwsCheckBadCharacters.cxx
kwsCheckStruct.cxx
kwsCheckVariables.cxx
kwsCheckMemberFunctions.cxx
kwsCheckFunctions.cxx
kwsCheckUsingDirectives.cxx
kwsCheckRelativePathInInclude.cxx
kwsCheckIfWhileForUntil.cxx
kwsCheckComma.cxx
kwsCheckParenthesis.cxx
)

add_subdirectory(Utilities)

add_library(KWStyleLib ${KWStyle_SRCS} ${KWStyle_HDRS})
add_executable(KWStyle kwsStyle.cxx)
target_link_libraries(KWStyle KWStyleLib kwssys)

# Move the executable to the top binary tree
add_custom_command(
   TARGET KWStyle
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:KWStyle>
     ${KWStyle_BINARY_DIR}/$<TARGET_FILE_NAME:KWStyle>
 )

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

# Installation file
install(TARGETS KWStyle
        RUNTIME DESTINATION bin)

install(FILES ${KWStyle_SOURCE_DIR}/Documentation/Install/ITK.kws.xml DESTINATION share/KWStyle)
install(FILES ${KWStyle_SOURCE_DIR}/Documentation/Install/VTK.kws.xml DESTINATION share/KWStyle)

# Install the headers
install(FILES ${KWStyle_SOURCE_DIR}/Web/headers/VTKHeader.h DESTINATION share/KWStyle/headers)
install(FILES ${KWStyle_SOURCE_DIR}/Web/headers/ITKHeader.h DESTINATION share/KWStyle/headers)

# Install the logo
install(FILES ${KWStyle_SOURCE_DIR}/Documentation/logosmall.jpg DESTINATION share/KWStyle)

# CPack options
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KWStyle")
set(CPACK_PACKAGE_VENDOR "Kitware Inc.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")

set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "KWStyle ${KWStyle_VERSION_MAJOR}.${KWStyle_VERSION_MINOR}")
if(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_PACKAGE_ICON "${KWStyle_SOURCE_DIR}/Web/images\\\\TitleBar.bmp")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "relwithdebinfo\\\\KWStyle.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} The source code checker")
  set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\public.kitware.com/KWStyle")
  set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\public.kitware.com/KWStyle")
  set(CPACK_NSIS_MODIFY_PATH ON)
else()
  set(CPACK_STRIP_FILES "bin/KWStyle")
  set(CPACK_SOURCE_STRIP_FILES "")
endif()
#set(CPACK_PACKAGE_EXECUTABLES "KWStyle" "KWStyle")
include(CPack)
