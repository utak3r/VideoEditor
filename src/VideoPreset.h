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
    VideoPreset(QString name, QString ext, QString video, QString videoPreset, QString videoTune, QString videoProfile, QString audio);
	VideoPreset(const VideoPreset& other);
	VideoPreset& operator=(const VideoPreset& other);
	VideoPreset(VideoPreset&& other) noexcept;
    QVariant asVariant();
    QString asString();

public:
    QString Name;
    QString VideoCodec;
	QString AudioCodec;
	QString VideoCodecPreset; // ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow
	QString VideoCodecTune; // film, animation, grain, stillimage, zerolatency
	QString VideoCodecProfile; // baseline, main, high, high10, high422, high444, dnxhr_hq, dnxhr_hqx, dnxhr_444, etc.
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
		            << preset.VideoCodecTune << ","
                    << preset.VideoCodecProfile << ","
		            << preset.AudioCodec
                    << ")";
    return debug.space();
}

#endif // VIDEOPRESET_H
