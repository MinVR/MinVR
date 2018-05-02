
find_path(GLFW_INCLUDE_DIR 
    GLFW/glfw3.h
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/include 
    ${CMAKE_INSTALL_PREFIX}/include/GLFW
)
if (NOT GLFW_INCLUDE_DIR)
    find_path(GLFW_INCLUDE_DIR 
        GLFW/glfw3.h
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/include 
        ${CMAKE_INSTALL_PREFIX}/include/GLFW
        $ENV{GLFW_ROOT}/include 
        $ENV{GLFW_ROOT}/include/GLFW 
        /usr/local/include
        /usr/local/include/GLFW
    )
endif()


find_library(GLFW_OPT_LIBRARIES 
  NAMES 
    libglfw3.a 
    glfw3.lib 
    glfw3
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
)
if (NOT GLFW_OPT_LIBRARIES)
    find_library(GLFW_OPT_LIBRARIES 
      NAMES 
        libglfw3.a 
        glfw3.lib 
        glfw3
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/lib 
        $ENV{GLFW_ROOT}/lib 
        /usr/local/lib
    )
endif()

          
find_library(GLFW_DEBUG_LIBRARIES 
  NAMES 
    libglfw3d.a 
    glfw3d.lib 
    glfw3d
  NO_DEFAULT_PATH
  HINTS 
    ${CMAKE_INSTALL_PREFIX}/lib 
)
if (NOT GLFW_DEBUG_LIBRARIES)
    find_library(GLFW_DEBUG_LIBRARIES 
      NAMES 
        libglfw3d.a 
        glfw3d.lib 
        glfw3d
      HINTS 
        ${CMAKE_INSTALL_PREFIX}/lib 
        $ENV{GLFW_ROOT}/lib 
        /usr/local/lib
    )
endif()


if (GLFW_OPT_LIBRARIES AND GLFW_DEBUG_LIBRARIES)
    set(GLFW_OPT_LIBRARIES optimized ${GLFW_OPT_LIBRARIES})
    set(GLFW_DEBUG_LIBRARIES debug ${GLFW_DEBUG_LIBRARIES})
elseif(GLFW_OPT_LIBRARIES AND NOT GLFW_DEBUG_LIBRARIES)
    # if only opt is found, use it for both
    set(GLFW_DEBUG_LIBRARIES debug ${GLFW_OPT_LIBRARIES})
    set(GLFW_OPT_LIBRARIES optimized ${GLFW_OPT_LIBRARIES})
elseif(NOT GLFW_OPT_LIBRARIES AND GLFW_DEBUG_LIBRARIES)
    # if only debug is found, use it for both
    set(GLFW_OPT_LIBRARIES optimized ${GLFW_DEBUG_LIBRARIES})
    set(GLFW_DEBUG_LIBRARIES debug ${GLFW_DEBUG_LIBRARIES})
endif()


set(GLFW_LIBRARIES ${GLFW_OPT_LIBRARIES} ${GLFW_DEBUG_LIBRARIES})


if(APPLE)
    set(GLFW_cocoa_LIBRARY "-framework Cocoa" CACHE STRING "Cocoa framework for OSX")
    set(GLFW_iokit_LIBRARY "-framework IOKit" CACHE STRING "IOKit framework for OSX")
    set(GLFW_corevideo_LIBRARY "-framework CoreVideo" CACHE STRING "CoreVideo framework for OSX")
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    GLFW
    DEFAULT_MSG
    GLFW_INCLUDE_DIR
    GLFW_OPT_LIBRARIES 
    GLFW_DEBUG_LIBRARIES 
    GLFW_LIBRARIES
)

mark_as_advanced(
    GLFW_INCLUDE_DIR 
    GLFW_OPT_LIBRARIES 
    GLFW_DEBUG_LIBRARIES 
    GLFW_LIBRARIES
    GLFW_cocoa_LIBRARY
    GLFW_iokit_LIBRARY
    GLFW_corevideo_LIBRARY
)
