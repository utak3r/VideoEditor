#ifndef VIDEOPRESET_H
#define VIDEOPRESET_H

#include <QMetaType>
#include <QVariant>
#include <QString>

class VideoPreset
{
public:
    VideoPreset();
    VideoPreset(QString name, QString ext, QString cmd);
    QVariant asVariant();

public:
    QString Name;
    QString CommandLine;
    QString Extension;

};
Q_DECLARE_METATYPE(VideoPreset);

inline QDebug operator<<(QDebug debug, const VideoPreset& preset)
{
    debug.nospace() << "VideoPreset("
                    << preset.Name << ","
                    << preset.Extension << ","
                    << preset.CommandLine << ")";
    return debug.space();
}

#endif // VIDEOPRESET_H
