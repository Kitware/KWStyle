include_directories(${KWStyle_SOURCE_DIR})

set(KWSTYLE_TESTS  ${CXX_TEST_PATH}/KWStyleTests)

set(
 KWStyleTests_SRCS
 kwsIndentTest.cxx
 kwsSemiColonSpaceTest.cxx
 kwsStatementPerLineTest.cxx
 kwsVariablePerLineTest.cxx
 kwsSwitchCaseTest.cxx
 kwsBadCharactersTest.cxx
)

foreach(TestFiles ${KWStyleTests_SRCS})
  get_filename_component(TestProgram ${TestFiles} NAME_WE)
  add_test(${TestProgram} ${KWSTYLE_TESTS} ${TestProgram})
endforeach(TestFiles)

set(KWStyleTests_SRCS
 ${KWStyleTests_SRCS}
 kwsRunKWStyleTest.cxx
)

add_executable( KWStyleTests KWStyleTests.cxx ${KWStyleTests_SRCS})
target_link_libraries(KWStyleTests KWStyleLib kwssys)


macro(ADD_TEST_FROM_BUG_TRACKER BUG_NUMBER FILENAME)
  add_test(Test${BUG_NUMBER} ${KWSTYLE_TESTS}
  --compare ${KWStyle_SOURCE_DIR}/Testing/Data/${BUG_NUMBER}-output.xml
            ${KWStyle_BINARY_DIR}/${BUG_NUMBER}-output.xml
  kwsRunKWStyleTest ${KWStyle_BINARY_DIR}/KWStyle -xml ${KWStyle_SOURCE_DIR}/Testing/Data/${BUG_NUMBER}-KWStyleConfiguration.kws.xml
  -exportxml ${KWStyle_BINARY_DIR}/${BUG_NUMBER}-output.xml
  ${KWStyle_SOURCE_DIR}/Testing/Data/${BUG_NUMBER}-${FILENAME})

endmacro(ADD_TEST_FROM_BUG_TRACKER BUG_NUMBER FILENAME)

ADD_TEST_FROM_BUG_TRACKER(kwsTabsTest test.cxx)
ADD_TEST_FROM_BUG_TRACKER(0001 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4204 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4227 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4259 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4260 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4262 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4296 test.h)
ADD_TEST_FROM_BUG_TRACKER(4297 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4301 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4302 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4303 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4316 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4317 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4318 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4319 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4327 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4344 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4345 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(4518 test.h)
ADD_TEST_FROM_BUG_TRACKER(4531 test.h)
ADD_TEST_FROM_BUG_TRACKER(4549 test.h)
ADD_TEST_FROM_BUG_TRACKER(5031 test.cpp)
ADD_TEST_FROM_BUG_TRACKER(5032 test.h)
ADD_TEST_FROM_BUG_TRACKER(5059 test.h)
ADD_TEST_FROM_BUG_TRACKER(5086 test.h)
ADD_TEST_FROM_BUG_TRACKER(9934 test.h)
ADD_TEST_FROM_BUG_TRACKER(9935 test.cxx)
ADD_TEST_FROM_BUG_TRACKER(9937 test.cxx)
ADD_TEST_FROM_BUG_TRACKER(9938 test.cxx)
ADD_TEST_FROM_BUG_TRACKER(9939 test.cxx)
ADD_TEST_FROM_BUG_TRACKER(10872 test.cxx)
ADD_TEST_FROM_BUG_TRACKER(kwsIfWhileForUntilTest test.cxx)
ADD_TEST_FROM_BUG_TRACKER(kwsCommaTest test.cxx)
ADD_TEST_FROM_BUG_TRACKER(kwsParenthesisTest test.cxx)
