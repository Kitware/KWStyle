INCLUDE_DIRECTORIES(${KWStyle_SOURCE_DIR})

SET(KWSTYLE_TESTS  ${CXX_TEST_PATH}/KWStyleTests)

ADD_TEST(kwsSemilColonSpaceTest ${KWSTYLE_TESTS} 
         kwsSemilColonSpaceTest)

SET(
 KWStyleTests_SRCS
 kwsSemiColonSpaceTest.cxx
 kwsStatementPerLineTest.cxx 
)

ADD_EXECUTABLE( KWStyleTests KWStyleTests.cxx ${KWStyleTests_SRCS})
TARGET_LINK_LIBRARIES(KWStyleTests KWStyleLib)
