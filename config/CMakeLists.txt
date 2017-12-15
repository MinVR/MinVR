# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.

project(config)

set(vr_config_files
  default.minvr
  desktop.minvr
  YURT.minvr
  ivlabcave.minvr
)

set_source_files_properties(${vr_config_files} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("Config Files" FILES ${vr_config_files})

install(FILES ${vr_config_files} DESTINATION ${INSTALL_CONFIGFILES_DEST} COMPONENT CoreLib)

