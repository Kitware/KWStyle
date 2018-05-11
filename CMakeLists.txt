cmake_minimum_required(VERSION 2.8.4)

foreach(p
    CMP0003
    CMP0026
    CMP0054
    )
  if(POLICY ${p})
    CMAKE_POLICY(SET ${p} NEW)
  endif()
endforeach()

project(KWStyle)

# Output directories
if(NOT LIBRARY_OUTPUT_PATH)
  set(LIBRARY_OUTPUT_PATH ${KWStyle_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
endif()
if(NOT EXECUTABLE_OUTPUT_PATH)
  set(EXECUTABLE_OUTPUT_PATH ${KWStyle_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
endif()
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

set(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})

set(KWStyle_MAJOR_VERSION "1")
set(KWStyle_MINOR_VERSION "0")
set(KWStyle_PATCH_VERSION "1")

# Disable MSVC 8 warnings
if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

# Add testing
enable_testing()
include(CTest)

if(BUILD_TESTING)
  subdirs(Testing)
endif()

include_directories(
${CMAKE_CURRENT_BINARY_DIR}
${KWStyle_BINARY_DIR}/Utilities
${KWStyle_SOURCE_DIR}/Utilities
)

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

subdirs(Utilities)

set(KWStyle_EXECUTABLE_DIRS ${EXECUTABLE_OUTPUT_PATH})
set(KWStyle_LIBRARY_DIRS ${LIBRARY_OUTPUT_PATH})

# Configure the export configuration
subdirs(CMake/ExportConfiguration)

configure_file(${KWStyle_SOURCE_DIR}/KWStyleConfigure.h.in 
  ${KWStyle_BINARY_DIR}/KWStyleConfigure.h)

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
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
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
