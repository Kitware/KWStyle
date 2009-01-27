SET (CTEST_SOURCE_DIRECTORY "/home/kitware/Dashboards/KWStyleBuilds/IGSTKSandbox")
SET (CTEST_BINARY_DIRECTORY "/home/kitware/Dashboards/KWStyleBuilds/IGSTKSandbox-KWStyle")

SET (KWSTYLE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
"/home/kitware/Dashboards/CMake-build/bin/ctest -D NightlyStart"
"/home/kitware/Dashboards/CMake-build/bin/ctest -D NightlyUpdate"
"/home/kitware/Dashboards/CMake-build/bin/ctest -D NightlyConfigure"
"/home/kitware/Dashboards/KWStyle-Build/KWStyle -lesshtml -o ${KWSTYLE_DIRECTORY}/IGSTKOverwrite.txt -xml ${CTEST_BINARY_DIRECTORY}/IGSTK.kws.xml -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D ${CTEST_BINARY_DIRECTORY}/IGSTKFiles.txt" 
"ctest -D NightlySubmit -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
)


SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
SITE:STRING=dash21.kitware
BUILDNAME:STRING=KWStyle-Sanbox
ITK_DIR:PATH=/home/kitware/Dashboards/Insight-Build
VTK_DIR:PATH=/home/kitware/Dashboards/VTK-Build
IGSTK_DIR:PATH=/home/kitware/Dashboards/KWStyleBuilds/IGSTK-Linux
IGSTK_USE_KWSTYLE:BOOL=ON
KWSTYLE_EXECUTABLE:FILEPATH=/home/kitware/Dashboards/KWStyle-build/bin/KWStyle
")

