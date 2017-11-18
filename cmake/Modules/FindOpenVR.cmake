find_path(OPENVR_INCLUDE_DIR openvr.h
          HINTS $ENV{OPENVR_INCLUDE_DIR} ${OPENVR_INCLUDE_DIR})

find_library(OPENVR_LIBRARY NAMES openvr_api.lib openvr_api.so openvr_api
             HINTS $ENV{OPENVR_LIB_DIR} ${OPENVR_LIB_DIR})

set(OPENVR_LIBRARIES ${OPENVR_LIBRARY})
set(OPENVR_INCLUDE_DIRS ${OPENVR_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OPENVR  DEFAULT_MSG
                                  OPENVR_INCLUDE_DIR OPENVR_LIBRARY)

if(NOT OPENVR_FOUND)
  set(OPENVR_INCLUDE_DIR $ENV{OPENVR_INCLUDE_DIR} CACHE PATH "Set the directory location of the OpenVR include folder")
  set(OPENVR_LIB_DIR $ENV{OPENVR_LIB_DIR} CACHE PATH "Set the directory location of the OpenVR lib folder")
endif()

mark_as_advanced(OPENVR_FOUND OPENVR_INCLUDE_DIR OPENVR_LIBRARY )

