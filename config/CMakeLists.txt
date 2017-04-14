
project(config)

set(config_files
  desktop.minvr
)

set_source_files_properties(${config_files} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Config Files" FILES ${config_files})


