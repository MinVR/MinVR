# This finds both libvrpn and libquat, since vrpn ships with and depends upon libquat
# The VRPN_INCLUDE_DIR and VRPN_LIBRARIES flags will contain paths to both vrpn and quat

# Search order:  This Find module forces the search to look first inside CMAKE_INSTALL_PREFIX
# before proceeding to other typical library install locations.  The reason is that this is
# where the lib will be installed if it was built using AutoBuild as part of the current 
# build process.  If this is the case, we want to be sure to pick up this most recent
# version of the library rather than some (older) version that might be installed elsewhere
# on the system.


# First, check for the main VRPN library

find_path(VRPN_INCLUDE_DIR 
    vrpn_Connection.h
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include 
    ${CMAKE_INSTALL_PREFIX}/include/vrpn
)
if (NOT VRPN_INCLUDE_DIR)
    find_path(VRPN_INCLUDE_DIR 
        vrpn_Connection.h
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/include 
        ${CMAKE_INSTALL_PREFIX}/include/vrpn
        $ENV{VRPN_ROOT}/include 
        $ENV{VRPN_ROOT}/include/vrpn
        /usr/local/include
        /usr/local/include/vrpn
    )
endif()


find_library(VRPN_OPT_LIBRARIES 
  NAMES 
    libvrpn.a 
    vrpn.lib 
    vrpn
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
)
if (NOT VRPN_OPT_LIBRARIES)
    find_library(VRPN_OPT_LIBRARIES 
      NAMES 
        libvrpn.a 
        vrpn.lib 
        vrpn
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/lib 
        $ENV{VRPN_ROOT}/lib 
        /usr/local/lib
    )
endif()

          
find_library(VRPN_DEBUG_LIBRARIES 
  NAMES 
    libvrpnd.a 
    vrpnd.lib 
    vrpnd
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
)
if (NOT VRPN_DEBUG_LIBRARIES)
    find_library(VRPN_DEBUG_LIBRARIES 
      NAMES 
        libvrpnd.a 
        vrpnd.lib 
        vrpnd
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/lib 
        $ENV{VRPN_ROOT}/lib 
        /usr/local/lib
    )
endif()


if (VRPN_OPT_LIBRARIES)
    set(VRPN_OPT_LIBRARIES optimized ${VRPN_OPT_LIBRARIES})
endif()
if (VRPN_DEBUG_LIBRARIES)
    set(VRPN_DEBUG_LIBRARIES debug ${VRPN_DEBUG_LIBRARIES})
endif()

set(VRPN_LIBRARIES ${VRPN_OPT_LIBRARIES} ${VRPN_DEBUG_LIBRARIES})




# VRPN depends on quatlib, so we need to also check for that

find_path(QUAT_INCLUDE_DIR 
    quat.h
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include 
    ${CMAKE_INSTALL_PREFIX}/include/vrpn
    ${CMAKE_INSTALL_PREFIX}/include/vrpn/quat
)
if (NOT QUAT_INCLUDE_DIR)
    find_path(QUAT_INCLUDE_DIR 
        vrpn_Connection.h
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/include 
        ${CMAKE_INSTALL_PREFIX}/include/vrpn
        ${CMAKE_INSTALL_PREFIX}/include/vrpn/quat
        $ENV{VRPN_ROOT}/include 
        $ENV{VRPN_ROOT}/include/vrpn
        $ENV{VRPN_ROOT}/include/vrpn/quat
        /usr/local/include
        /usr/local/include/vrpn
        /usr/local/include/vrpn/quat
    )
endif()


find_library(QUAT_OPT_LIBRARIES 
  NAMES 
    libquat.a 
    quat.lib 
    quat
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
)
if (NOT QUAT_OPT_LIBRARIES)
    find_library(QUAT_OPT_LIBRARIES 
      NAMES 
        libquat.a 
        quat.lib 
        quat
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/lib 
        $ENV{VRPN_ROOT}/lib 
        /usr/local/lib
    )
endif()

          
find_library(QUAT_DEBUG_LIBRARIES 
  NAMES 
    libquatd.a 
    quatd.lib 
    quatd
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
)
if (NOT QUAT_DEBUG_LIBRARIES)
    find_library(QUAT_DEBUG_LIBRARIES 
      NAMES 
        libquat.a 
        quatd.lib 
        quatd
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/lib 
        $ENV{VRPN_ROOT}/lib 
        /usr/local/lib
    )
endif()


if (QUAT_OPT_LIBRARIES)
    set(QUAT_OPT_LIBRARIES optimized ${QUAT_OPT_LIBRARIES})
endif()
if (QUAT_DEBUG_LIBRARIES)
    set(QUAT_DEBUG_LIBRARIES debug ${QUAT_DEBUG_LIBRARIES})
endif()

set(QUAT_LIBRARIES ${QUAT_OPT_LIBRARIES} ${QUAT_DEBUG_LIBRARIES})


# Add libquat flags to the vrpn flags
set(VRPN_INCLUDE_DIR ${VRPN_INCLUDE_DIR} ${QUAT_INCLUDE_DIR})
set(VRPN_LIBRARIES ${VRPN_LIBRARIES} ${QUAT_LIBRARIES})




include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    VRPN
    DEFAULT_MSG
    VRPN_INCLUDE_DIR
    VRPN_OPT_LIBRARIES 
    VRPN_DEBUG_LIBRARIES 
    VRPN_LIBRARIES
    QUAT_INCLUDE_DIR
    QUAT_OPT_LIBRARIES 
    QUAT_DEBUG_LIBRARIES 
    QUAT_LIBRARIES
)

mark_as_advanced(
    VRPN_INCLUDE_DIR 
    VRPN_OPT_LIBRARIES 
    VRPN_DEBUG_LIBRARIES 
    VRPN_LIBRARIES
    QUAT_INCLUDE_DIR
    QUAT_OPT_LIBRARIES 
    QUAT_DEBUG_LIBRARIES 
    QUAT_LIBRARIES
)
