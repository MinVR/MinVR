# MinVR Gainput Plugin

Library to support multiple external input devices. To check Gainput capabilities, go to http://gainput.johanneskuhlmann.de/
To integrate with MinVR, build it enabling the “WITH_GAINPUT” option, and  add the following lines to the configuration file:

<GainputPlugin pluginType="MinVR_Gainput"/>	


<GamePad inputdeviceType="VRGainputDevice">
      <TrackerName>InputGamePad</TrackerName>
      <GamePadControllers>1</GamePadControllers>
</GamePad>

For guidance, build MinVR with the option “WITH_GAMEPAD_TEST”, and execute the example with the file configuration file GamePadController.minvr

   
   itest-gamepad-opengl-shaderpipeline-with-api -c GamePadController.minvr

