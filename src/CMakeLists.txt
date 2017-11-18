# This file is part of the MinVR cmake build system.  
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.


project(libMinVR)


set(vr_config_cpp
  ${vr_src_dir}/config/VRDataIndex.cpp
  ${vr_src_dir}/config/VRDataQueue.cpp
  ${vr_src_dir}/config/VRDatum.cpp
  ${vr_src_dir}/config/VRDatumFactory.cpp
  ${vr_src_dir}/config/Cxml/attribute.cpp
  ${vr_src_dir}/config/Cxml/Cxml.cpp
  ${vr_src_dir}/config/Cxml/element.cpp
  ${vr_src_dir}/config/Cxml/utils.cpp
  ${vr_src_dir}/config/Cxml/XML.cpp
  ${vr_src_dir}/config/base64/base64.cpp
)

set(vr_config_h_config
  ${vr_src_dir}/config/VRCoreTypes.h
  ${vr_src_dir}/config/VRDataIndex.h
  ${vr_src_dir}/config/VRDataQueue.h
  ${vr_src_dir}/config/VRDatum.h
  ${vr_src_dir}/config/VRDatumFactory.h
  ${vr_src_dir}/config/VRWritable.h
)

set(vr_config_h_cxml
  ${vr_src_dir}/config/Cxml/attribute.h
  ${vr_src_dir}/config/Cxml/Cxml.h
  ${vr_src_dir}/config/Cxml/element.h
  ${vr_src_dir}/config/Cxml/utils.h
  ${vr_src_dir}/config/Cxml/XML.h
)

set(vr_config_h_base
  ${vr_src_dir}/config/base64/base64.h
)

set(vr_config_h
  ${vr_config_h_config}
  ${vr_config_h_cxml}
  ${vr_config_h_base}
)

set(vr_display_cpp
  ${vr_src_dir}/display/VRConsoleNode.cpp
  ${vr_src_dir}/display/VRDisplayNode.cpp
  ${vr_src_dir}/display/VRGraphicsWindowNode.cpp
  ${vr_src_dir}/display/VRGroupNode.cpp
  ${vr_src_dir}/display/VRHeadTrackingNode.cpp
  ${vr_src_dir}/display/VROffAxisProjectionNode.cpp
  ${vr_src_dir}/display/VRProjectionNode.cpp
  ${vr_src_dir}/display/VRStereoNode.cpp
  ${vr_src_dir}/display/VRViewportNode.cpp
  ${vr_src_dir}/display/VRLookAtNode.cpp
)

set(vr_display_h
  ${vr_src_dir}/display/VRConsoleNode.h
  ${vr_src_dir}/display/VRDisplayNode.h
  ${vr_src_dir}/display/VRGraphicsToolkit.h
  ${vr_src_dir}/display/VRGraphicsWindowNode.h
  ${vr_src_dir}/display/VRGroupNode.h
  ${vr_src_dir}/display/VRHeadTrackingNode.h
  ${vr_src_dir}/display/VROffAxisProjectionNode.h
  ${vr_src_dir}/display/VRProjectionNode.h
  ${vr_src_dir}/display/VRStereoNode.h
  ${vr_src_dir}/display/VRViewportNode.h
  ${vr_src_dir}/display/VRWindowToolkit.h
  ${vr_src_dir}/display/VRLookAtNode.h
)

set(vr_input_cpp
  ${vr_src_dir}/input/VRFakeHandTrackerDevice.cpp
  ${vr_src_dir}/input/VRFakeHeadTrackerDevice.cpp
  ${vr_src_dir}/input/VRFakeTrackerDevice.cpp
)

set(vr_input_h
  ${vr_src_dir}/input/VRFakeHandTrackerDevice.h
  ${vr_src_dir}/input/VRFakeHeadTrackerDevice.h
  ${vr_src_dir}/input/VRFakeTrackerDevice.h
  ${vr_src_dir}/input/VRInputDevice.h
)

set(vr_api_cpp
  ${vr_src_dir}/api/impl/VRApp.cpp
  ${vr_src_dir}/api/impl/VRAnalogEvent.cpp
  ${vr_src_dir}/api/impl/VRAudioState.cpp
  ${vr_src_dir}/api/impl/VRButtonEvent.cpp
  ${vr_src_dir}/api/impl/VRConsoleState.cpp
  ${vr_src_dir}/api/impl/VRCursorEvent.cpp
  ${vr_src_dir}/api/impl/VRGraphicsState.cpp
  ${vr_src_dir}/api/impl/VRHapticsState.cpp
  ${vr_src_dir}/api/impl/VRTrackerEvent.cpp
)

set(vr_api_h
  ${vr_src_dir}/api/MinVR.h
  ${vr_src_dir}/api/VRApp.h
  ${vr_src_dir}/api/VRAnalogEvent.h
  ${vr_src_dir}/api/VRAudioState.h
  ${vr_src_dir}/api/VRButtonEvent.h
  ${vr_src_dir}/api/VRConsoleState.h
  ${vr_src_dir}/api/VRCursorEvent.h
  ${vr_src_dir}/api/VRGraphicsState.h
  ${vr_src_dir}/api/VRHapticsState.h
  ${vr_src_dir}/api/VRTrackerEvent.h
)

set(vr_main_cpp
  ${vr_src_dir}/main/VRFactory.cpp
  ${vr_src_dir}/main/VRMain.cpp
  ${vr_src_dir}/main/VRSearchPath.cpp
  ${vr_src_dir}/main/VRSystem.cpp
)

set(vr_main_h
  ${vr_src_dir}/main/VREventHandler.h
  ${vr_src_dir}/main/VRFactory.h
  ${vr_src_dir}/main/VRItemFactory.h
  ${vr_src_dir}/main/VRMain.h
  ${vr_src_dir}/main/VRMainInterface.h
  ${vr_src_dir}/main/VRRenderHandler.h
  ${vr_src_dir}/main/VRSearchPath.h
  ${vr_src_dir}/main/VRSystem.h
  ${vr_src_dir}/main/VRError.h
  ${vr_src_dir}/main/VRSearchPath.h
)

set(vr_math_cpp
  ${vr_src_dir}/math/VRMath.cpp
  ${vr_src_dir}/math/VRRect.cpp
)

set(vr_math_h
  ${vr_src_dir}/math/VRMath.h
  ${vr_src_dir}/math/VRRect.h
)

set(vr_net_cpp
  ${vr_src_dir}/net/VRNetClient.cpp
  ${vr_src_dir}/net/VRNetInterface.cpp
  ${vr_src_dir}/net/VRNetServer.cpp
)

set(vr_net_h
  ${vr_src_dir}/net/VRNetClient.h
  ${vr_src_dir}/net/VRNetInterface.h
  ${vr_src_dir}/net/VRNetServer.h
)

set(vr_plugin_cpp
  ${vr_src_dir}/plugin/VRPluginManager.cpp
  ${vr_src_dir}/plugin/VRSharedLibrary.cpp
)

set(vr_plugin_h
  ${vr_src_dir}/plugin/VRPlugin.h
  ${vr_src_dir}/plugin/VRPluginManager.h
  ${vr_src_dir}/plugin/VRPluginVersion.h
  ${vr_src_dir}/plugin/VRSharedLibrary.h
)

set(vr_sources
  ${vr_config_files}
  ${vr_config_cpp} ${vr_config_h}
  ${vr_display_cpp} ${vr_display_h}
  ${vr_api_cpp} ${vr_api_h}
  ${vr_input_cpp} ${vr_input_h}
  ${vr_main_cpp} ${vr_main_h}
  ${vr_math_cpp} ${vr_math_h}
  ${vr_net_cpp} ${vr_net_h}
  ${vr_plugin_cpp} ${vr_plugin_h}
)

source_group("config\\Header Files" FILES ${vr_config_h})
source_group("config\\Source Files" FILES ${vr_config_cpp})
source_group("display\\Header Files" FILES ${vr_display_h})
source_group("display\\Source Files" FILES ${vr_display_cpp})
source_group("api\\Header Files" FILES ${vr_api_h})
source_group("api\\Source Files" FILES ${vr_api_cpp})
source_group("input\\Header Files" FILES ${vr_input_h})
source_group("input\\Source Files" FILES ${vr_input_cpp})
source_group("main\\Header Files" FILES ${vr_main_h})
source_group("main\\Source Files" FILES ${vr_main_cpp})
source_group("math\\Header Files" FILES ${vr_math_h})
source_group("math\\Source Files" FILES ${vr_math_cpp})
source_group("net\\Header Files" FILES ${vr_net_h})
source_group("net\\Source Files" FILES ${vr_net_cpp})
source_group("plugin\\Header Files" FILES ${vr_plugin_h})
source_group("plugin\\Source Files" FILES ${vr_plugin_cpp})



add_library(MinVR ${vr_sources})

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  message(STATUS "Using libdl.")
  target_link_libraries(MinVR dl)
endif()




# Install built libraries
install(TARGETS MinVR EXPORT MinVR
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# Install CMake export file to help with importing in other cmake projects
install(EXPORT MinVR DESTINATION ${CMAKE_INSTALL_PREFIX}/share/MinVR/cmake)

# Install header files
install(FILES ${vr_api_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/api)
install(FILES ${vr_config_h_config} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/config)
install(FILES ${vr_config_h_cxml} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/config/Cxml)
install(FILES ${vr_config_h_base} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/config/base64)
install(FILES ${vr_display_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/display)
install(FILES ${vr_max_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/max)
install(FILES ${vr_input_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/input)
install(FILES ${vr_main_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/main)
install(FILES ${vr_math_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/math)
install(FILES ${vr_net_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/net)
install(FILES ${vr_plugin_h} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MinVR/plugin)


