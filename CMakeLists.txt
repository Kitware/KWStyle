PROJECT(KWStyle)

# Output directories
SET(EXECUTABLE_OUTPUT_PATH ${KWStyle_BINARY_DIR} CACHE INTERNAL "Output directory for executable")
SET(LIBRARY_OUTPUT_PATH ${KWStyle_BINARY_DIR} CACHE INTERNAL "Output directory for libraries")

SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

SET(KWStyle_VERSION_MAJOR "1")
SET(KWStyle_VERSION_MINOR "0")

# Disable MSVC 8 warnings
IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(WIN32)

# Add testing
MARK_AS_ADVANCED( DART_ROOT )
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)

IF (BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF (BUILD_TESTING)

INCLUDE_DIRECTORIES(${KWStyle_BINARY_DIR}/Utilities)

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
)

LINK_LIBRARIES(
kwssys
)

SUBDIRS(Utilities)

ADD_LIBRARY(KWStyleLib ${KWStyle_SRCS} ${KWStyle_HDRS})
ADD_EXECUTABLE(KWStyle kwsStyle.cxx)
TARGET_LINK_LIBRARIES(KWStyle KWStyleLib)

# Installation file
INSTALL(TARGETS KWStyle
        RUNTIME DESTINATION bin)
#INSTALL(FILES ${KWStyle_HDRS} DESTINATION include/KWStyle)
#INSTALL(FILES ${KWStyle_SRCS} DESTINATION source)


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
