PROJECT(KWStyle)

# Disable MSVC 8 warnings
IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(WIN32)

FIND_PACKAGE(ITK)
IF( USE_ITK_FILE )
  INCLUDE( ${USE_ITK_FILE} )
ENDIF( USE_ITK_FILE )

INCLUDE_DIRECTORIES(
${ITK_SOURCE_DIR}/Utilities/MetaIO
#${ITK_DIR}/Utilities/itksys
)

SET(KWStyle_SRCS
kwsStyle.cxx
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
)

LINK_LIBRARIES(
${ITK_LIBRARIES}
)

ADD_EXECUTABLE( KWStyle ${KWStyle_SRCS})

#CONFIGURE_FILE(${KWStyle_SOURCE_DIR}/kwstyle.xml
#               ${KWStyle_BINARY_DIR}/kwstyle.xml COPYONLY)


