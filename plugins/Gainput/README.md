# MinVR Gainput Plugin

Library to support multiple external input devices. To check Gainput capabilities, go to http://gainput.johanneskuhlmann.de/
<br/>
To integrate with MinVR, build it enabling the **WITH_GAINPUT** option, and  add the following lines to the configuration file:

```xml
<GainputPlugin pluginType="MinVR_Gainput"/>	


<GamePad inputdeviceType="VRGainputDevice">
     <TrackerName>InputGamePad</TrackerName>
      <GamePadControllers>1</GamePadControllers>
</GamePad>
```



For guidance, follow the given example building MinVR with the option **WITH_GAMEPAD_TEST**. 
<br/>
Because the example requires the plugin installed, you have to build,compile and install MinVR_Gainput. 
<br/>
Please follow these steps:


1. Build, compile and install MinVR_Gainput plugin.
2. Run cmake script again, but this time enabling **WITH_GAMEPAD_TEST**.
3. Compile and run.

and execute the example with the configuration file 

*GamePadController.minvr*

```   
   itest-gamepad-opengl-shaderpipeline-with-api -c GamePadController.minvr
```
