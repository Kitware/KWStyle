SET (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/Insight")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/Insight-Linux")

SET (KWSTYLE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Nightly -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlyStart"
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlyUpdate"
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlyConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -lesshtml -o ${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle/ITKOverwrite.txt -xml ${CTEST_BINARY_DIRECTORY}/ITK.kws.xml -html /projects/KWStyle/Examples/Insight/Review -kwsurl http://66.194.253.24/KWStyleExamples/Insight/Review -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D ${CTEST_BINARY_DIRECTORY}/ITKReviewFiles.txt" 
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlySubmit"
)

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "/projects/KWStyle/CMake-Linux/bin/cmake"
  )

# set any extra envionment varibles here
SET (CTEST_ENVIRONMENT
)

SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=insight.journal.kitware
BUILDNAME:STRING=KWStyle-Review
CVSCOMMAND:FILEPATH=/usr/bin/cvs
ITK_USE_KWSTYLE:BOOL=ON
")
