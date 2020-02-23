set (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/itkUNC")
set (CTEST_BINARY_DIRECTORY "/projects/KWStyle/itkUNC-Linux")

# which ctest command to use for running the dashboard
set (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Experimental -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalStart"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalUpdate"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -xml /projects/KWStyle/KWStyle/Web/ITK.kws.xml -dart ${CTEST_BINARY_DIRECTORY} -1 1 -kwsurl http://66.194.253.24/KWStyleExamples/itkUNC -R -d ${CTEST_SOURCE_DIRECTORY}"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalSubmit"
)

# what cmake command to use for configuring this dashboard
set (CTEST_CMAKE_COMMAND 
  "/projects/KWStyle/CMake/bin/cmake"
  )

# set any extra envionment varibles here
set (CTEST_ENVIRONMENT
)

set (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=insight.journal.kitware
BUILDNAME:STRING=KWStyle
CVSCOMMAND:FILEPATH=/usr/bin/cvs
ITK_DIR:PATH=/projects/KWStyle/Insight-Linux
USE_SOV:BOOL=OFF
DROP_LOCATION:PATH=/DartClient
")
