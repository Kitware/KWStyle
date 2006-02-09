SET (CTEST_SOURCE_DIRECTORY "/DartClient/SOViewer")
SET (CTEST_BINARY_DIRECTORY "/DartClient/SOViewer-KWStyle")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
#"/DartClient/CMake/bin/ctest -D Experimental -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
"/DartClient/CMake/bin/ctest -D ExperimentalStart"
"/DartClient/CMake/bin/ctest -D ExperimentalUpdate"
"/DartClient/CMake/bin/ctest -D ExperimentalConfigure"
"KWSTyle -dart ${CTEST_BINARY_DIRECTORY} -d ${CTEST_SOURCE_DIRECTORY}"
#"/DartClient/CMake/bin/ctest -D ExperimentalSubmit"
)

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "/DartClient/CMake/bin/cmake"
  )

# set any extra envionment varibles here
SET (CTEST_ENVIRONMENT
)

SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=voltaire.caddlab.unc
BUILDNAME:STRING=Linux-gcc-3.3
CVSCOMMAND:FILEPATH=/usr/bin/cvs
ITK_DIR:PATH=/DartClient/Insight-Linux
USE_FLTK:BOOL=ON
USE_QT:BOOL=ON
FLTK_BASE_LIBRARY:FILEPATH=/DartClient/FLTK/fltk-Linux/lib/libfltk.a
FLTK_FLUID_EXECUTABLE:FILEPATH=/DartClient/FLTK/fltk-Linux/bin/fluid
FLTK_INCLUDE_DIR:PATH=/DartClient/FLTK/fltk-Linux
QT_INCLUDE_DIR:PATH=/usr/lib/qt-3.1/include
QT_QT_LIBRARY:FILEPATH=/usr/lib/qt-3.1/lib/libqt.so
USE_VTK:BOOL=ON
BUILD_EXAMPLES:BOOL=ON
VTK_DIR:PATH=/DartClient/VTK-Linux
DROP_LOCATION:PATH=/DartClient
MEMORYCHECK_COMMAND:FILEPATH=/usr/local/bin/valgrind
MEMORYCHECK_COMMAND_OPTIONS:STRING=--weird-hacks=lax-ioctls --trace-children=yes -q --skin=memcheck --leak-check=yes --show-reachable=yes --workaround-gcc296-bugs=yes --num-callers=100 -v
#MEMORYCHECK_SUPPRESSIONS_FILE:FILEPATH=$ENV{HOME}/Dashboards/MyTests/ParaView/CMake/ParaViewValgrindSuppressions.supp
")
