SET (CTEST_SOURCE_DIRECTORY "/home/Dashboards/KWStyleBuilds/Insight")
SET (CTEST_BINARY_DIRECTORY "/projects/KWStyle/KWStyleBuilds/Insight-Linux")

SET (KWSTYLE_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
"ctest -D NightlyStart"
"ctest -D NightlyUpdate"
"ctest -D NightlyConfigure"
"/home/Dashboards/KWStyle-Build/KWStyle -lesshtml -o ${CTEST_SOURCE_DIRECTORY}/Utilities/KWStyle/ITKOverwrite.txt -xml ${CTEST_BINARY_DIRECTORY}/Utilities/KWStyle/ITK.kws.xml -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D ${CTEST_BINARY_DIRECTORY}/Utilities/KWStyle/ITKFiles.txt" 
"ctest -D NightlySubmit -A ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
)

SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=dash21.kitware
BUILDNAME:STRING=KWStyle
ITK_USE_KWSTYLE:BOOL=ON
")

