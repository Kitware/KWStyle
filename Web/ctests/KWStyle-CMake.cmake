SET (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/CMakeCVS")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/CMake-Linux")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Experimental -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalStart"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalUpdate"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -xml /projects/KWStyle/KWStyle/Web/CMake.kws.xml -html /projects/KWStyle/Examples/CMake -kwsurl http://66.194.253.24/KWStyleExamples/CMake -dart ${CTEST_BINARY_DIRECTORY} -1 1 -R  -D /projects/KWStyle/KWStyle/Web/CMakeFiles.txt"
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
")
