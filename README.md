# VideoEditor

VideoEditor is a simple application for quick format conversions and easy trimming. It can also rescale the video. In the future, it will probably get some other options - and the first in the queue is video cropping.

![Video Editor screenshot](/docs/screenshot.png?raw=true "Video Editor screenshot")

### Building
VideoEditor is a Qt based app, built using CMake. The easiest way is to load the *CMakeLists.txt* file into QT Creator and build.
Use *install* target for deploying necessary dependencies into a *distrib* folder. It also needs setting the *FFMPEG_DIR* variable.

### Running
After first run, two sample presets are created, review and/or modify them or add any new ones by clicking *Settings* button.

Settings are being stored in a *VideoEditor.ini* file.
