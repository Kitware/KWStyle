INCLUDE_DIRECTORIES(${KWStyle_SOURCE_DIR})

SET(KWSTYLE_TESTS  ${CXX_TEST_PATH}/KWStyleTests)

ADD_TEST(kwsSemiColonSpaceTest ${KWSTYLE_TESTS} 
         kwsSemiColonSpaceTest)

ADD_TEST(kwsStatementPerLineTest ${KWSTYLE_TESTS} 
         kwsStatementPerLineTest)

ADD_TEST(kwsVariablePerLineTest ${KWSTYLE_TESTS} 
         kwsVariablePerLineTest)

SET(
 KWStyleTests_SRCS
 kwsSemiColonSpaceTest.cxx
 kwsStatementPerLineTest.cxx
 kwsVariablePerLineTest.cxx 
)

ADD_EXECUTABLE( KWStyleTests KWStyleTests.cxx ${KWStyleTests_SRCS})
TARGET_LINK_LIBRARIES(KWStyleTests KWStyleLib)
