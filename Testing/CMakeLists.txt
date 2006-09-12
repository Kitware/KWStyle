INCLUDE_DIRECTORIES(${KWStyle_SOURCE_DIR})

SET(KWSTYLE_TESTS  ${CXX_TEST_PATH}/KWStyleTests)

SET(
 KWStyleTests_SRCS
 kwsSemiColonSpaceTest.cxx
 kwsStatementPerLineTest.cxx
 kwsVariablePerLineTest.cxx
 kwsSwitchCaseTest.cxx
 kwsBadCharactersTest.cxx
)

FOREACH(TestFiles ${KWStyleTests_SRCS})   
  GET_FILENAME_COMPONENT(TestProgram ${TestFiles} NAME_WE)   
  ADD_TEST(${TestProgram} ${KWSTYLE_TESTS} ${TestProgram}) 
ENDFOREACH(TestFiles)

ADD_EXECUTABLE( KWStyleTests KWStyleTests.cxx ${KWStyleTests_SRCS})
TARGET_LINK_LIBRARIES(KWStyleTests KWStyleLib)
