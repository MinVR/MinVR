### Description
The Photon plugin integrates the  [PhotonEngine multiplayer SDK](https://www.photonengine.com/ "PhotonEngine multiplayer SDK") to share MinVR events across different setups. 

### Building

To build first register a free account on the with [photon](https://dashboard.photonengine.com/en-US/Account/SignUp "photon"). The account is free and also gives you an online server for up to 20 concurrent users. After registering you can download the SDK for Windows, Mac or Linux.
 
During the cmake configuration step first enable the option to build the plugin (WITH_PLUGIN_PHOTON) and then point PHOTON_DIR to the folder where you extracted the library. The libraries as well as the include will be then automatically set.

### Usage

The Photon plugin acts as an input device. It reads the currents events of the applications, sends them to the server which redistributes them to the other connected users and then attaches the events received from other users.

In order to work properly all other events on the systems have to be added to the queue before the photon plugin reads them. This can be achieved by adding the Photon plugin as the last input device in the config file.

To add it first load the plugin
```xml
  <PhotonPlugin pluginType="MinVR_Photon"/>
```
Then add the input device as last input device in the config file
```xml
 <Photon inputdeviceType="VRPhotonDevice">
	<AppName>APPLICATION_NAME</AppName>
	<AppID>APPLICATION_ID</AppID>
	<AppVersion>APPLICATION_VERSION</AppVersion>
	<PlayerName>PLAYERNAME</PlayerName>
</Photon>
```
The values **APPLICATION_NAME**, **APPLICATION_VERSION** and **APPLICATION_ID**  should point to the values of the used server. You can find it on the dashboard of the photon account. The **PLAYERNAME** should be chosen for each user connecting to the server.

There several other optional settings which can be used to customize the event exchange.

```xml
<ReceiveOnly\>
```
**ReceiveOnly** can be used to stop the client from sending its event to the server. It will only receive events from other users.

```xml
<Whitelist type="stringarray">FrameStart,Mouse_Move</Whitelist>	
```
The **Whitelist** option can be used to select events from being sent. In this example the events *FrameStart* and *MouseMove* will be the only events shared. If no whitelist is defined all events will be sent.

```xml
<Replacements type="stringarray">LHandTracker_Move,L_Move,RHandTracker_Move,R_Move</Replacements>
```
The **Replacements** option can be used to change the names of events before being sent. In this example the even *LHandTracker_Move* will be renamed to *L_Move* and the event *RHandTracker_Move* will be renamed to *R_Move*

```xml
<PhotonUpdateSpeed>33</PhotonUpdateSpeed>
```
The **PhotonUpdateSpeed** option can be used to change the frequency of the update calls to the Photon library. This allows to define the frequency in which packets are received and send. The value defines the time between the update calls in ms.


### Considerations

The plugin should be used to simply share events across different systems. How the events are being used depends on the application itself. In the simplest case one users sends the events to all other clients which have their plugin set to ReceiveOnly. 

In order to work correctly it should be considered how the different systems are being setup. E.g. loading a file from the filesystem will not work properly as the different systems will have different filesystems. Also as the system is event based all systems need to be connected to the server before interaction with the environment is started and all systems need to have the same state, e.g. have loaded the same data.

It should also be taken into accout that events from several render loops on the sender side can be received on the receiver side in the same render loop. In order to make sure that the applications on both side have the same state it might be required to apply application updates for every render loop of the sender. To facilitate this the sender send an analog event named *PhotonLoopFinished* to indicate when the events from on renderloop of the sender are finished which allows the receiver to update the application.

Finally another problem might arise with time-sensitive interactions as the events will arrive with slight delay and the timedifferences between events might differ between different systems. If timing is an issue it is possible to make use of the timestamps of the events as these are the timestamps from the sending machine.