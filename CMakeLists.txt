CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

PROJECT(KWStyle)

# Output directories
IF(NOT LIBRARY_OUTPUT_PATH)
  SET (LIBRARY_OUTPUT_PATH ${KWStyle_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET (EXECUTABLE_OUTPUT_PATH ${KWStyle_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(KWStyle_MAJOR_VERSION "1")
SET(KWStyle_MINOR_VERSION "0")
SET(KWStyle_PATCH_VERSION "1")

# Disable MSVC 8 warnings
IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(WIN32)

# Add testing
ENABLE_TESTING()
INCLUDE(CTest)

IF (BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF (BUILD_TESTING)

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_BINARY_DIR}
${KWStyle_BINARY_DIR}/Utilities
${KWStyle_SOURCE_DIR}/Utilities
)

SET(KWStyle_HDRS
kwsParser.h
kwsHeader.h
kwsXMLReader.h
kwsGenerator.h
kwsTestFile.h
metaCommand.h
)

SET(KWStyle_SRCS
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
)

SUBDIRS(Utilities)

SET(KWStyle_EXECUTABLE_DIRS ${EXECUTABLE_OUTPUT_PATH})
SET(KWStyle_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})

# Configure the export configuration
SUBDIRS(CMake/ExportConfiguration)

CONFIGURE_FILE(${KWStyle_SOURCE_DIR}/KWStyleConfigure.h.in 
  ${KWStyle_BINARY_DIR}/KWStyleConfigure.h)

ADD_LIBRARY(KWStyleLib ${KWStyle_SRCS} ${KWStyle_HDRS})
ADD_EXECUTABLE(KWStyle kwsStyle.cxx)
TARGET_LINK_LIBRARIES(KWStyle KWStyleLib kwssys)

# Move the executable to the top binary tree
GET_TARGET_PROPERTY(EXECUTABLE_PATH KWStyle LOCATION)
GET_FILENAME_COMPONENT(EXECUTABLE_NAME ${EXECUTABLE_PATH} NAME)

ADD_CUSTOM_COMMAND(
   TARGET KWStyle
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_PATH} ${KWStyle_BINARY_DIR}/${EXECUTABLE_NAME}
 )

# Installation file
INSTALL(TARGETS KWStyle
        RUNTIME DESTINATION bin)

INSTALL(FILES ${KWStyle_SOURCE_DIR}/Documentation/Install/ITK.kws.xml DESTINATION share/KWStyle)
INSTALL(FILES ${KWStyle_SOURCE_DIR}/Documentation/Install/VTK.kws.xml DESTINATION share/KWStyle)

# Install the headers
INSTALL(FILES ${KWStyle_SOURCE_DIR}/Web/headers/VTKHeader.h DESTINATION share/KWStyle/headers)
INSTALL(FILES ${KWStyle_SOURCE_DIR}/Web/headers/ITKHeader.h DESTINATION share/KWStyle/headers)

# Install the logo
INSTALL(FILES ${KWStyle_SOURCE_DIR}/Documentation/logosmall.jpg DESTINATION share/KWStyle)

# CPack options
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KWStyle")
SET(CPACK_PACKAGE_VENDOR "Kitware Inc.")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "KWStyle ${KWStyle_VERSION_MAJOR}.${KWStyle_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${KWStyle_SOURCE_DIR}/Web/images\\\\TitleBar.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "relwithdebinfo\\\\KWStyle.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} The source code checker")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\public.kitware.com/KWStyle")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\public.kitware.com/KWStyle")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/KWStyle")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "KWStyle" "KWStyle")
INCLUDE(CPack)
