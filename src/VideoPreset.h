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
    VideoPreset(QString name, QString ext, QString video, QString videoPreset, QString audio, QString audioPreset);
	VideoPreset(const VideoPreset& other);
	VideoPreset& operator=(const VideoPreset& other);
	VideoPreset(VideoPreset&& other) noexcept;
    QVariant asVariant();
    QString asString();

public:
    QString Name;
    QString VideoCodec;
	QString AudioCodec;
	QString VideoCodecPreset;
	QString AudioCodecPreset;
    QString Extension;

};
Q_DECLARE_METATYPE(VideoPreset);

inline QDebug operator<<(QDebug debug, const VideoPreset& preset)
{
    debug.nospace() << "VideoPreset("
                    << preset.Name << ","
                    << preset.Extension << ","
		            << preset.VideoCodec << ","
		            << preset.VideoCodecPreset << ","
		            << preset.AudioCodec << ","
		            << preset.AudioCodecPreset
                    << ")";
    return debug.space();
}

#endif // VIDEOPRESET_H
