#include "Settings.h"

Settings::Settings()
    : theSettings(nullptr)
    , theFFMPEG("ffmpeg.exe")
    , theLastDir(".")
    , theMainWndGeometry(QRect(640, 250, 800, 500))
{
    theSettings = new QSettings("VideoEditor.ini", QSettings::IniFormat);
}

void Settings::ReadSettings()
{
    theSettings->beginGroup("General");
    theFFMPEG = theSettings->value("FFmpegPath", "ffmpeg.exe").value<QString>();
    theLastDir = theSettings->value("LastDir", ".").value<QString>();
    theSettings->endGroup();

    theSettings->beginGroup("Geometry");
    theMainWndGeometry = theSettings->value("MainWindowGeometry").value<QRect>();
    theSettings->endGroup();

    theSettings->beginGroup("Video presets");
    theVideoPresets.clear();
    QStringList presets = theSettings->allKeys();
    if (presets.count() > 0)
    {
        foreach (QString presetKey, presets)
        {
            QString presetEncoded = theSettings->value(presetKey).value<QString>();
            QStringList list = presetEncoded.split("\r\n", Qt::KeepEmptyParts, Qt::CaseInsensitive);
            if (list.count() == 3)
            {
                theVideoPresets.append(VideoPreset(list[0], list[1], list[2]));
            }
        }
    }
    else
    {
        theVideoPresets.append(VideoPreset("H.264 AAC", ".mp4", "-c:v libx264 -preset medium -tune film -c:a aac"));
        theVideoPresets.append(VideoPreset("DNxHD 185Mbps PCM s24LE", ".mov", "-c:v dnxhd -b:v 185M -c:a pcm_s24le"));
        theVideoPresets.append(VideoPreset("ProRes YUV422", ".mov", "-c:v prores_ks -profile:v 3 -vendor ap10 -pix_fmt yuv422p10le"));
    }
    theSettings->endGroup();
}

void Settings::WriteSettings()
{
    theSettings->beginGroup("General");
    theSettings->setValue("FFmpegPath", theFFMPEG);
    theSettings->setValue("LastDir", theLastDir);
    theSettings->endGroup();

    theSettings->beginGroup("Geometry");
    theSettings->setValue("MainWindowGeometry", theMainWndGeometry);
    theSettings->endGroup();

    theSettings->beginGroup("Video presets");
    theSettings->remove("");
    foreach (VideoPreset preset, theVideoPresets)
    {
        theSettings->setValue(preset.Name, preset.asString());
    }
    theSettings->endGroup();

    theSettings->sync();
}

QString Settings::ffmpeg()
{
    return theFFMPEG;
}

void Settings::setffmpeg(QString path)
{
    theFFMPEG = path;
}

QString Settings::lastDir()
{
    return theLastDir;
}

void Settings::setLastDir(QString dir)
{
    theLastDir = dir;
}

QRect Settings::mainWndGeometry()
{
    return theMainWndGeometry;
}

void Settings::setMainWndGeometry(QRect rect)
{
    theMainWndGeometry = rect;
}

QList<VideoPreset> Settings::videoPresets()
{
    return theVideoPresets;
}

void Settings::setVideoPresets(QList<VideoPreset>& list)
{
    theVideoPresets.clear();
    theVideoPresets.append(list);
}

