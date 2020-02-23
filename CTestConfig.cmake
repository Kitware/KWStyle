## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   enable_testing()
##   include(Dart)
set(CTEST_PROJECT_NAME "KWStyle")
set(CTEST_NIGHTLY_START_TIME "21:00:00 EST")

set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=KWStyle")
set(CTEST_DROP_SITE_CDASH TRUE)
