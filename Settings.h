#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QRect>
#include <VideoPreset.h>

/*!
 * \class Settings
 * \brief This class is for storing all the app's settings
 */
class Settings
{
public:
    Settings();

    void ReadSettings();
    void WriteSettings();

    QString ffmpeg();
    void setffmpeg(QString path);
    QString lastDir();
    void setLastDir(QString dir);
    QRect mainWndGeometry();
    void setMainWndGeometry(QRect rect);
    QList<VideoPreset> videoPresets();
    void setVideoPresets(QList<VideoPreset>& list);

private:
    QSettings* theSettings;
    QString theFFMPEG;
    QString theLastDir;
    QRect theMainWndGeometry;
    QList<VideoPreset> theVideoPresets;
};

#endif // SETTINGS_H
