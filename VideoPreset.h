#ifndef VIDEOPRESET_H
#define VIDEOPRESET_H

#include <QMetaType>
#include <QVariant>
#include <QString>

/*!
 * \class VideoPreset
 * \brief The VideoPreset class holds video conversion data
 */
class VideoPreset
{
public:
    VideoPreset();
    VideoPreset(QString name, QString ext, QString cmd);
    QVariant asVariant();
    QString asString();

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
