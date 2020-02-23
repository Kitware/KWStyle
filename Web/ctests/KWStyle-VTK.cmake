set (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/VTK")
set (CTEST_BINARY_DIRECTORY "/projects/KWStyle/VTK-Linux")

# which ctest command to use for running the dashboard
set (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Experimental -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalStart"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalUpdate"
"/projects/KWStyle/CMake/bin/ctest -D ExperimentalConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -lesshtml -o /projects/KWStyle/KWStyle/Web/VTKOverwrite.txt -xml /projects/KWStyle/KWStyle/Web/VTK.kws.xml -html /projects/KWStyle/Examples/VTK -kwsurl http://66.194.253.24/KWStyleExamples/VTK -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D /projects/KWStyle/KWStyle/Web/VTKFiles.txt" 
#"/projects/KWStyle/CMake/bin/ctest -D ExperimentalSubmit"
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
")
