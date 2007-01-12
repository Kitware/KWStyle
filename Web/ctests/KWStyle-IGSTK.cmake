SET (CTEST_SOURCE_DIRECTORY "/projects/KWStyle/IGSTK")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/IGSTK-Linux")

SET (KWSTYLE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlyStart"
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlyUpdate"
"/projects/KWStyle/CMake-Linux/bin/ctest -D NightlyConfigure"
"/projects/KWStyle/KWStyle-Linux/KWStyle -lesshtml -o ${KWSTYLE_DIRECTORY}/IGSTKOverwrite.txt -xml ${CTEST_BINARY_DIRECTORY}/IGSTK.kws.xml -kwsurl http://66.194.253.24/KWStyleExamples/IGSTK -html /projects/KWStyle/Examples/IGSTK -dart ${CTEST_BINARY_DIRECTORY} -1 1 -R -D ${KWSTYLE_DIRECTORY}/IGSTKFiles.txt"
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
BUILDNAME:STRING=KWStyle
CVSCOMMAND:FILEPATH=/usr/bin/cvs
ITK_DIR:PATH=/projects/KWStyle/Insight-Linux
VTK_DIR:PATH=/projects/KWStyle/VTK-Linux
FLTK_DIR:PATH=/projects/KWStyle/FLTK-Linux
IGSTK_USE_KWSTYLE:BOOL=ON
")
