QAudioSwitcher is a tool that helps switch the default audio sink quickly and easily. Written in Qt, is using PulseAudio for device/sinks management. 
It exposes a DBus service, which can be used to configure a shortcut for quick switching. 
In KDE, to configure a shortcut that can switch the audio device follow these steps:

- Open System Settings
- Activate the Shortcuts panel
- Select "Custom Shortcuts"
- Add new shortcut
- Give it some name
- Pick a trigger key (ctrl+shift+` for example)
- Setup the Action:
	- Remote application: com.zergiu.QAudioSwitcher.SwitchAudio
	- Remote object: /
	- Function: switchSink
	- Arguments: 

