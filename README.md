# VideoEditor

VideoEditor is a simple application for quick format conversions and easy trimming. It can also rescale the video. In the future, it will probably get some other options - and the first in the queue is video cropping.

![Video Editor screenshot](/docs/screenshot.png?raw=true "Video Editor screenshot")

### Scaling output
If you want to rescale the output video, check *Scaling* and set your desired video size. If you set one of the dimensions to *-1*. it will be calculated with an aspect ratio to the other dimension - so that's the preferred way of using it. You can also set your chosen scaling filter. There're few of them available: fast bilinear, bilinear, bicubic, area, gaussian and lanczos.

![Video Editor screenshot](/docs/scaling.png?raw=true "Video scaling options")


### Building
VideoEditor is a Qt based app, built using CMake. I suggest using an IDE supporting both CMake and Qt, like *Visual Code* where you can scan for *kits* and select it for building.

It's also using **FFmpeg**. While *Qt* does have it, it's stripped off of encoders, so it's unusable. Download FFmpeg for yourself (for example [from here](https://www.gyan.dev/ffmpeg/builds/)) and specify its location during configuring CMake using `-DFFMPEG_ROOT=your/path_to/ffmpeg`.

Also define an *INSTALL_DIR* env variable.

Configure, then build and then install. Install step gathers all required files from *Qt* and *FFmpeg*, so the application can run.

### Running
After first run, two sample presets are created, review and/or modify them or add any new ones by clicking *Settings* button.

Settings are being stored in a *VideoEditor.ini* file.
