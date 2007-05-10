SET (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/CMakeCVS")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/CMake-Linux")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalStart"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalUpdate"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -lesshtml -xml ${CTEST_BINARY_DIRECTORY}/CMake.kws.xml -html /projects/KWStyle/Examples/CMake -kwsurl http://66.194.253.24/KWStyleExamples/CMake -dart ${CTEST_BINARY_DIRECTORY} -1 1 -o ${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle/CMakeOverwrite.txt -D ${CTEST_BINARY_DIRECTORY}/CMakeKWSFiles.txt "
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalSubmit -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
)

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "/projects/KWStyle/CMake/bin/cmake"
  )

# set any extra envionment varibles here
SET (CTEST_ENVIRONMENT
)

SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=insight.journal.kitware
BUILDNAME:STRING=KWStyle
CVSCOMMAND:FILEPATH=/usr/bin/cvs
CTEST_TEST_CTEST:BOOL=false
BUILD_TESTING:BOOL=ON
CMAKE_USE_KWSTYLE:BOOL=ON
")
