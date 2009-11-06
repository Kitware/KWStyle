SET (CTEST_SOURCE_DIRECTORY "/home/kitware/Dashboards/KWStyleBuilds/CMake")
SET (CTEST_BINARY_DIRECTORY "/home/kitware/Dashboards/KWStyleBuilds/CMake-KWStyle")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
"/home/kitware/Dashboards/cmake-2.8.0-rc5/bin/ctest -D NightlyStart"
"/home/kitware/Dashboards/cmake-2.8.0-rc5/bin/ctest -D NightlyUpdate"
"/home/kitware/Dashboards/cmake-2.8.0-rc5/bin/ctest -D NightlyConfigure"
"/home/kitware/Dashboards/KWStyle-Build/KWStyle -lesshtml -xml ${CTEST_BINARY_DIRECTORY}/CMake.kws.xml -B http://public.kitware.com/cgi-bin/viewcvs.cgi -dart ${CTEST_BINARY_DIRECTORY} -1 1 -o ${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle/CMakeOverwrite.txt -D ${CTEST_BINARY_DIRECTORY}/CMakeKWSFiles.txt "
"/home/kitware/Dashboards/cmake-2.8.0-rc5/bin/ctest -D NightlySubmit -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
)

SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=dash21.kitware
BUILDNAME:STRING=KWStyle
CTEST_TEST_CTEST:BOOL=false
BUILD_TESTING:BOOL=ON
CMAKE_USE_KWSTYLE:BOOL=ON
KWSTYLE_EXECUTABLE:FILEPATH=/home/kitware/Dashboards/KWStyle-Build/bin/KWStyle
")
