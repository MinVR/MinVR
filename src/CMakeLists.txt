# This file is part of the MinVR cmake build system.
# See the main MinVR/CMakeLists.txt file for authors, copyright, and license info.


set(vr_config_cpp
  src/config/VRDataIndex.cpp
  src/config/VRDataQueue.cpp
  src/config/VRDatum.cpp
  src/config/VRDatumFactory.cpp
  src/config/Cxml/attribute.cpp
  src/config/Cxml/Cxml.cpp
  src/config/Cxml/element.cpp
  src/config/Cxml/utils.cpp
  src/config/Cxml/XML.cpp
  src/config/base64/base64.cpp
)

set(vr_config_h_config
  src/config/VRCoreTypes.h
  src/config/VRDataIndex.h
  src/config/VRDataQueue.h
  src/config/VRDatum.h
  src/config/VRDatumFactory.h
  src/config/VRWritable.h
)

set(vr_config_h_cxml
  src/config/Cxml/attribute.h
  src/config/Cxml/Cxml.h
  src/config/Cxml/element.h
  src/config/Cxml/utils.h
  src/config/Cxml/XML.h
)

set(vr_config_h_base
  src/config/base64/base64.h
)

set(vr_config_h
  ${vr_config_h_config}
  ${vr_config_h_cxml}
  ${vr_config_h_base}
)

set(vr_display_cpp
  src/display/VRConsoleNode.cpp
  src/display/VRDisplayNode.cpp
  src/display/VRGraphicsWindowNode.cpp
  src/display/VRGroupNode.cpp
  src/display/VRHeadTrackingNode.cpp
  src/display/VROffAxisProjectionNode.cpp
  src/display/VRProjectionNode.cpp
  src/display/VRStereoNode.cpp
  src/display/VRViewportNode.cpp
  src/display/VRLookAtNode.cpp
)

set(vr_display_h
  src/display/VRConsoleNode.h
  src/display/VRDisplayNode.h
  src/display/VRGraphicsToolkit.h
  src/display/VRGraphicsWindowNode.h
  src/display/VRGroupNode.h
  src/display/VRHeadTrackingNode.h
  src/display/VROffAxisProjectionNode.h
  src/display/VRProjectionNode.h
  src/display/VRStereoNode.h
  src/display/VRViewportNode.h
  src/display/VRWindowToolkit.h
  src/display/VRLookAtNode.h
)

set(vr_input_cpp
  src/input/VRFakeHandTrackerDevice.cpp
  src/input/VRFakeHeadTrackerDevice.cpp
  src/input/VRFakeTrackerDevice.cpp
)

set(vr_input_h
  src/input/VRFakeHandTrackerDevice.h
  src/input/VRFakeHeadTrackerDevice.h
  src/input/VRFakeTrackerDevice.h
  src/input/VRInputDevice.h
)

set(vr_api_cpp
  src/api/impl/VRApp.cpp
  src/api/impl/VRAnalogEvent.cpp
  src/api/impl/VRAudioState.cpp
  src/api/impl/VRButtonEvent.cpp
  src/api/impl/VRConsoleState.cpp
  src/api/impl/VRCursorEvent.cpp
  src/api/impl/VRGraphicsState.cpp
  src/api/impl/VRHapticsState.cpp
  src/api/impl/VRTrackerEvent.cpp
)

set(vr_api_h
  src/api/MinVR.h
  src/api/VRApp.h
  src/api/VRAnalogEvent.h
  src/api/VRAudioState.h
  src/api/VRButtonEvent.h
  src/api/VRConsoleState.h
  src/api/VRCursorEvent.h
  src/api/VRGraphicsState.h
  src/api/VRHapticsState.h
  src/api/VRTrackerEvent.h
)

set(vr_main_cpp
  src/main/VRFactory.cpp
  src/main/VRMain.cpp
  src/main/VRSearchPath.cpp
  src/main/VRSystem.cpp
)

set(vr_main_h
  src/main/VREventHandler.h
  src/main/VRModelHandler.h
  src/main/VRFactory.h
  src/main/VRItemFactory.h
  src/main/VRLog.h
  src/main/VRMain.h
  src/main/VRMainInterface.h
  src/main/VRRenderHandler.h
  src/main/VRSearchPath.h
  src/main/VRSystem.h
  src/main/VRError.h
  src/main/VRSearchPath.h
)

set(vr_math_cpp
  src/math/VRMath.cpp
  src/math/VRRect.cpp
)

set(vr_math_h
  src/math/VRMath.h
  src/math/VRRect.h
)

set(vr_net_cpp
  src/net/VRNetClient.cpp
  src/net/VRNetInterface.cpp
  src/net/VRNetServer.cpp
)

set(vr_net_h
  src/net/VRNetClient.h
  src/net/VRNetInterface.h
  src/net/VRNetServer.h
)

set(vr_plugin_cpp
  src/plugin/VRPluginManager.cpp
  src/plugin/VRSharedLibrary.cpp
)

set(vr_plugin_h
  src/plugin/VRPlugin.h
  src/plugin/VRPluginManager.h
  src/plugin/VRPluginVersion.h
  src/plugin/VRSharedLibrary.h
)

set(vr_sources
  ${vr_config_files}
  ${vr_config_cpp}  ${vr_config_h}
  ${vr_display_cpp} ${vr_display_h}
  ${vr_api_cpp}     ${vr_api_h}
  ${vr_input_cpp}   ${vr_input_h}
  ${vr_main_cpp}    ${vr_main_h}
  ${vr_math_cpp}    ${vr_math_h}
  ${vr_net_cpp}     ${vr_net_h}
  ${vr_plugin_cpp}  ${vr_plugin_h}
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

# Using target_include_directories() rather than just include_directories() is
# critical in order to support generating a MinVRConfig.cmake file.  It supports
# generator expressions, so we can point to two different include dirs depending
# on whether building or using the installed version.
target_include_directories(MinVR PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>  # for headers when building
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DEST}>        # for client in install mode
)


install(TARGETS MinVR EXPORT MinVRTargets COMPONENT CoreLib
  LIBRARY DESTINATION ${INSTALL_LIB_DEST}
  ARCHIVE DESTINATION ${INSTALL_LIB_DEST}
  RUNTIME DESTINATION ${INSTALL_BIN_DEST}
)

install(FILES ${vr_api_h} DESTINATION ${INSTALL_INCLUDE_DEST}/api COMPONENT CoreLib)
install(FILES ${vr_config_h_config} DESTINATION ${INSTALL_INCLUDE_DEST}/config COMPONENT CoreLib)
install(FILES ${vr_config_h_cxml} DESTINATION ${INSTALL_INCLUDE_DEST}/config/Cxml COMPONENT CoreLib)
install(FILES ${vr_config_h_base} DESTINATION ${INSTALL_INCLUDE_DEST}/config/base64 COMPONENT CoreLib)
install(FILES ${vr_display_h} DESTINATION ${INSTALL_INCLUDE_DEST}/display COMPONENT CoreLib)
install(FILES ${vr_input_h} DESTINATION ${INSTALL_INCLUDE_DEST}/input COMPONENT CoreLib)
install(FILES ${vr_main_h} DESTINATION ${INSTALL_INCLUDE_DEST}/main COMPONENT CoreLib)
install(FILES ${vr_math_h} DESTINATION ${INSTALL_INCLUDE_DEST}/math COMPONENT CoreLib)
install(FILES ${vr_net_h} DESTINATION ${INSTALL_INCLUDE_DEST}/net COMPONENT CoreLib)
install(FILES ${vr_plugin_h} DESTINATION ${INSTALL_INCLUDE_DEST}/plugin COMPONENT CoreLib)

