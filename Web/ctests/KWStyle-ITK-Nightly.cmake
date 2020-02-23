
set(CTEST_SITE "dash21.kitware")
set(CTEST_BUILD_NAME "KWStyle")

set (CTEST_SOURCE_DIRECTORY "/home/kitware/Dashboards/KWStyleBuilds/Insight")
set (CTEST_BINARY_DIRECTORY "/home/kitware/Dashboards/KWStyleBuilds/Insight-KWStyle")

set (KWSTYLE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle")

# which ctest command to use for running the dashboard
set (CTEST_COMMAND
"/home/kitware/Dashboards/cmake-2.8.0/bin/ctest -D NightlyStart"
"/home/kitware/Dashboards/cmake-2.8.0/bin/ctest -D NightlyUpdate"
"/home/kitware/Dashboards/cmake-2.8.0/bin/ctest -D NightlyConfigure"
"/home/kitware/Dashboards/KWStyle-Build/KWStyle -lesshtml -o ${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle/ITKOverwrite.txt -xml ${CTEST_BINARY_DIRECTORY}/Utilities/KWStyle/ITK.kws.xml -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D ${CTEST_BINARY_DIRECTORY}/Utilities/KWStyle/ITKFiles.txt"
"/home/kitware/Dashboards/cmake-2.8.0/bin/ctest -D NightlySubmit -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
)

set (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=dash21.kitware
BUILDNAME:STRING=KWStyle
ITK_USE_KWSTYLE:BOOL=ON
KWSTYLE_EXECUTABLE:FILEPATH=/home/kitware/Dashboards/KWStyle-Build/bin/KWStyle
")
