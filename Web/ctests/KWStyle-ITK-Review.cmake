SET (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/Insight")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/Insight-Linux")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Experimental -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalStart"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalUpdate"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -lesshtml -o /projects/KWStyle/KWStyle/Web/ITKOverwrite.txt -xml /projects/KWStyle/KWStyle/Web/ITKReview.kws.xml -html /projects/KWStyle/Examples/Insight/Review -kwsurl http://66.194.253.24/KWStyleExamples/Insight/Review -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D /projects/KWStyle/KWStyle/Web/ITKReviewFiles.txt" 
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalSubmit"
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
BUILDNAME:STRING=KWStyle-Review
CVSCOMMAND:FILEPATH=/usr/bin/cvs
")
