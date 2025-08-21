# VideoEditor

VideoEditor is a simple application for quick format conversions and easy trimming. It can also rescale the video. In the future, it will probably get some other options - and the first in the queue is video cropping.

![Video Editor screenshot](/docs/screenshot.png?raw=true "Video Editor screenshot")

### Scaling output
If you want to rescale the output video, check *Scaling* and set your desired video size. If you set one of the dimensions to *-1*. it will be calculated with an aspect ratio to the other dimension - so that's the preferred way of using it. You can also set your chosen scaling filter. There're few of them available: fast bilinear, bilinear, bicubic, area, gaussian and lanczos.

![Video Editor screenshot](/docs/scaling.png?raw=true "Video scaling options")


### Building
VideoEditor is a Qt based app, built using CMake.

If you have FFMPEG folder like this:
+ some_dir/ffmpeg\
Ͱ--+ bin\
Ͱ------ ffmpeg.exe \
Ͱ--+ include\
Ͱ------+ libavcodec\
Ͱ---------- avcodec.h\
Ͱ------ etc.\
Ͱ--+ lib\
Ͱ------ avcodec.lib\
Ͱ------ etc.

then adding *some_dir/ffmpeg* to a PATH should be enough for CMake to find *include* and *lib* directories.

For Qt6, add *QTDIR* env variable, leading to a root of Qt6, and then, when invoking cmake, add `-DQt6_DIR="%QTDIR%\lib\cmake\Qt6"`, so it can find a related *Qt6Config.cmake* file.

Also define an *INSTALL_DIR* env variable.

Final cmake invocation should look like this:

```
cmake -DCMAKE_INSTALL_PREFIX:PATH="%INSTALL_DIR%" -DQt6_DIR="%QTDIR%\lib\cmake\Qt6" ..\VideoEditor
```

### Running
After first run, two sample presets are created, review and/or modify them or add any new ones by clicking *Settings* button.

Settings are being stored in a *VideoEditor.ini* file.
