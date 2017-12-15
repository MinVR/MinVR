# The EasyBlendSDK is a proprietary library from Scalable Display Technologies of Cambridge, Massachusetts (scalabledisplay.com).
find_path(LIBSCALABLE_INCLUDE_DIR EasyBlendSDK.h
          HINTS 
	  $ENV{SCALABLE_INCLUDE_DIR} 
	  ${SCALABLE_INCLUDE_DIR}
	  ENV CPATH  # For OSCAR modules at Brown/CCV
	  /gpfs/runtime/opt/cave/ccv/include
	  )

find_library(LIBSCALABLE_LIBRARY NAMES mplEasyBlendSDK.lib libmplEasyBlendSDK.so
             HINTS 
	     $ENV{SCALABLE_LIB_DIR} 
	     ${SCALABLE_LIB_DIR}
	     ENV LD_LIBRARY_PATH  # For OSCAR modules at Brown/CCV
	     )

set(LIBSCALABLE_ALL_LIBRARIES ${LIBSCALABLE_LIBRARY})
set(LIBSCALABLE_INCLUDE_DIRS ${LIBSCALABLE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBSCALABLE  DEFAULT_MSG
                                  LIBSCALABLE_INCLUDE_DIR LIBSCALABLE_LIBRARY)

if(NOT LIBSCALABLE_FOUND)
  set(SCALABLE_INCLUDE_DIR $ENV{SCALABLE_INCLUDE_DIR} CACHE PATH "Set the directory location of the SCALABLE include folder")
  set(SCALABLE_LIB_DIR $ENV{SCALABLE_LIB_DIR} CACHE PATH "Set the directory location of the SCALABLE lib folder")
endif()

mark_as_advanced(LIBSCALABLE_INCLUDE_DIR LIBSCALABLE_LIBRARY )
