#-----------------------------------------------------------------------------
# Create the kwsys library for ITK.
SET(KWSYS_NAMESPACE kwssys)
SET(KWSYS_USE_SystemTools 1)
SET(KWSYS_USE_RegularExpression 1)
SET(KWSYS_USE_Glob 1)
SET(KWSYS_USE_Directory 1)
SET(KWSYS_USE_Base64 1)
SET(KWSYS_USE_FundamentalType 1)
SET(KWSYS_USE_CommandLineArguments 1)
SET(KWSYS_HEADER_ROOT ${KWStyle_BINARY_DIR}/Utilities)
#SET(KWSYS_LIBRARY_INSTALL_DIR /lib/KWStyle)
#SET(KWSYS_HEADER_INSTALL_DIR /include/KWStyle/Utilities)

SUBDIRS(KWSys)
