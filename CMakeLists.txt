PROJECT(KWStyle)

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
kwsGenerator.cxx
kwsGenerator.h
)

LINK_LIBRARIES(
${ITK_LIBRARIES}
)

ADD_EXECUTABLE( KWStyle ${KWStyle_SRCS})

CONFIGURE_FILE(${KWStyle_SOURCE_DIR}/kwstyle.xml
               ${KWStyle_BINARY_DIR}/kwstyle.xml COPYONLY)


