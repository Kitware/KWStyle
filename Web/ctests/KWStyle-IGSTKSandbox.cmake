SET (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/IGSTKSandbox")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/IGSTKSandbox-Linux")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Nightly -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/projects/KWStyle/CMake/bin/ctest -D NightlyStart"
"/projects/KWStyle/CMake/bin/ctest -D NightlyUpdate"
"/projects/KWStyle/CMake/bin/ctest -D NightlyConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -lesshtml -o /projects/KWStyle/KWStyle/Web/SandboxOverwrite.txt -xml /projects/KWStyle/KWStyle/Web/IGSTK.kws.xml -kwsurl http://66.194.253.24/KWStyleExamples/IGSTKSandbox -html /projects/KWStyle/Examples/IGSTKSandbox -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D /projects/KWStyle/KWStyle/Web/SandboxFiles.txt" 
"/projects/KWStyle/CMake/bin/ctest -D NightlySubmit"
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
BUILDNAME:STRING=KWStyle-Sanbox
CVSCOMMAND:FILEPATH=/usr/bin/cvs
ITK_DIR:PATH=/projects/KWStyle/Insight-Linux
VTK_DIR:PATH=/projects/KWStyle/VTK-Linux
FLTK_DIR:PATH=/projects/KWStyle/FLTK-Linux
IGSTK_DIR:PATH=/projects/KWStyle/IGSTK-Linux
")
