PROJECT(KWStyle)

SET(EXECUTABLE_OUTPUT_PATH ${KWStyle_BINARY_DIR} CACHE INTERNAL "Output directory for executable")
SET(LIBRARY_OUTPUT_PATH ${KWStyle_BINARY_DIR} CACHE INTERNAL "Output directory for libraries")

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

SET(KWStyle_SRCS
metaCommand.cxx
kwsParser.cxx
kwsParser.h
kwsXMLReader.h
kwsXMLReader.cxx
kwsGenerator.cxx
kwsGenerator.h
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
kwsStatementPerLine.cxx
kwsVariablePerLine.cxx
)

LINK_LIBRARIES(
kwssys
)

SUBDIRS(Utilities)

ADD_LIBRARY(KWStyleLib ${KWStyle_SRCS})
ADD_EXECUTABLE(KWStyle kwsStyle.cxx)
TARGET_LINK_LIBRARIES(KWStyle KWStyleLib)

