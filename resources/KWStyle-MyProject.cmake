SET (CTEST_SOURCE_DIRECTORY "MyProjectSource")
SET (CTEST_BINARY_DIRECTORY "MyProjectBinary")

# which ctest command to use for running the dashboard
SET (CTEST_COMMAND  
"/bin/ctest -D NightlyStart"
"/bin/ctest -D NightlyUpdate"
"/bin/ctest -D NightlyConfigure"
"KWStyle -lesshtml -o RulesOverwrite.txt -xml MyConfiguration.kws.xml -kwsurl http://66.194.253.24/KWStyleExamples -html /KWStyle/Examples -dart ${CTEST_BINARY_DIRECTORY} -1 1 -R -D MyFilesToCheck.txt"
"/bin/ctest -D NightlySubmit"
)

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "/bin/cmake"
  )

# set any extra envionment varibles here
SET (CTEST_ENVIRONMENT
)

SET (CTEST_INITIAL_CACHE 
"CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=insight.journal.kitware
BUILDNAME:STRING=KWStyle
")
