# VideoEditor

VideoEditor is a GUI for FFMPEG actions.
Primarily it is intended for quick format conversions and easy trimming. In the future, it will probably get some other options, like scaling, cropping etc.

### Building
VideoEditor is a Qt based app, built using CMake. The easiest way is to load the *CMakeLists.txt* file into QT Creator and build.
Use *install* target for deploying necessary dependencies into a *distrib* folder.

### Running
After first run, open Settings dialog, set the path to a FFMPEG binary and define some codec presets.
For example, a *DNxHD 185Mbps PCM s24LE* preset should get a *.mov* file extension and the following line for a codec: 

  -c:v dnxhd -b:v 185M -c:a pcm_s24le

Settings are being stored in a *VideoEditor.ini* file.
