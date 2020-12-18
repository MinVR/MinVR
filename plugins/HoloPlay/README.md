### Description
The Holoplay plugin integrates the [HoloplayCoreSDK](https://lookingglassfactory.com/holoplay-core) for the holographic displays from [LookingGlassFactory](https://lookingglassfactory.com/). It provides a displaynode to render in a holographic view. 

### Building

To build first request acces to the HoloplayCoreSDK from LookingGlassFactory as it is currently still in beta. [HoloplayCoreSDK](https://lookingglassfactory.com/holoplay-core)
 
During the cmake configuration step first enable the option to build the plugin (WITH_PLUGIN_HOLOPLAY) and then point HOLOPLAY_DIR to the folder which contains the library. The libraries as well as the include will be then automatically set.

### Usage

The HoloPlay plugin acts as an displaynode.
To add it first load the plugin
```xml
  <HoloPlayPlugin pluginType="MinVR_HoloPlay"/>
```
Then add the displaydevice to your displaygraph. The camera position can be controlled via the VRLookAtNode, e.g.
```xml
<WindowNode displaynodeType="VRGraphicsWindowNode">
	  <LookAtUp type="floatarray">0,1,0</LookAtUp>
	  <LookAtEye type="floatarray">0,0,1</LookAtEye>
	  <LookAtCenter type="floatarray">0,0,-1</LookAtCenter>
      <LookAtNode displaynodeType="VRLookAtNode">
          <HoloPlayNode displaynodeType="VRHoloPlayNode">
            <DUMMY/>
          </HoloPlayNode>
      </LookAtNode>
    </WindowNode>
```
Different parameters can be set to configure the rendering. For more information also refer to the 
[HoloPlay documentations](https://docs.lookingglassfactory.com/HoloPlayCore/)
```xml
<HoloCameraSize>2.0</HoloCameraSize>
```
**HoloCameraSize** sets the size of the camera viewpoint, default 2
```xml
  <HoloFieldOfView>14.0</HoloFieldOfView>
```
**HoloFieldOfView** sets the field of view of the display - depending on the distance to the display, default 14
```xml
<HoloDeviceIndex>0</HoloDeviceIndex>
```
**HoloDeviceIndex** sets the device index of the holographic display, default 0 - the first device
```xml
  <HoloQuiverQuality>1</HoloQuiverQuality>
```
**HoloQuiverQuality** sets the quality of the quiver :  
0 : 32 views
**1 : 45 views, default setting**
2 : 45 views for 8k display
```xml
  <NearClip>0.001</NearClip>
```
**NearClip** sets the near plane, default 0.01
```xml
  <FarClip>100.0</FarClip>
```
**FarClip**  sets the far plane, default 100

### Generated Events

The Holoplay device will also generate button events for the 4 buttons on the display.(Note: This is currently not working due to a problem with the SDK). The generate Button events are 

```xml
  HoloPlay_Button_Square_Up
  HoloPlay_Button_Square_Down
  HoloPlay_Button_Left_Up
  HoloPlay_Button_Left_Down
  HoloPlay_Button_Right_Up
  HoloPlay_Button_Right_Down
  HoloPlay_Button_Circle_Up
  HoloPlay_Button_Circle_Down
  
```

### Considerations

Quality of the rendering is highly dependent on the scene itself and the camera setup. Please refer to [HoloPlay documentations](https://docs.lookingglassfactory.com/HoloPlayCore/) for more information