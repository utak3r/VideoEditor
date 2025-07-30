#include "VideoPreset.h"

/*!
 * \brief VideoPreset::VideoPreset default constructor
 */
VideoPreset::VideoPreset()
    :
	Name(""),
    Extension(""),
    VideoCodec(""),
	AudioCodec(""),
	VideoCodecPreset("medium"),
	VideoCodecTune(""),
    VideoCodecProfile("")
{
}

/*!
 * \brief VideoPreset::VideoPreset constructor initializing all data at once
 * \param name name of the preset
 * \param ext file extension to be used
 * \param cmd ffnpeg arguments to be used during conversion
 */
VideoPreset::VideoPreset(QString name, QString ext, QString video, QString videoPreset, QString videoTune, QString videoProfile, QString audio)
    : VideoPreset()
{
	Name = name.trimmed();
    Extension = ext.trimmed();
    VideoCodec = video.trimmed();
	VideoCodecPreset = videoPreset.trimmed();
    VideoCodecTune = videoTune.trimmed();
	VideoCodecProfile = videoProfile.trimmed();
    AudioCodec = audio.trimmed();
}

VideoPreset::VideoPreset(const VideoPreset& other)
{
    Name = other.Name;
    Extension = other.Extension;
    VideoCodec = other.VideoCodec;
    AudioCodec = other.AudioCodec;
    VideoCodecPreset = other.VideoCodecPreset;
    VideoCodecTune = other.VideoCodecTune;
	VideoCodecProfile = other.VideoCodecProfile;
}

VideoPreset& VideoPreset::operator=(const VideoPreset& other)
{
    if (this != &other) {
        Name = other.Name;
        Extension = other.Extension;
        VideoCodec = other.VideoCodec;
        AudioCodec = other.AudioCodec;
        VideoCodecPreset = other.VideoCodecPreset;
        VideoCodecTune = other.VideoCodecTune;
        VideoCodecProfile = other.VideoCodecProfile;
	}
	return *this;
}

VideoPreset::VideoPreset(VideoPreset&& other) noexcept
{
    if (this == &other) {
        return; // self-move, do nothing
	}
    Name = std::move(other.Name);
    Extension = std::move(other.Extension);
    VideoCodec = std::move(other.VideoCodec);
    AudioCodec = std::move(other.AudioCodec);
    VideoCodecPreset = std::move(other.VideoCodecPreset);
    VideoCodecTune = std::move(other.VideoCodecTune);
	VideoCodecProfile = std::move(other.VideoCodecProfile);
}

/*!
 * \brief VideoPreset::asString encodes values as \r\n separated string
 */
QString VideoPreset::asString()
{
    QString str = Name + "\r\n" + Extension + "\r\n" + 
        VideoCodec + "\r\n" + VideoCodecPreset + "\r\n" + VideoCodecTune + "\r\n" + VideoCodecProfile + "\r\n" +
        AudioCodec;
    return str;
}

/*!
 * \brief VideoPreset::asVariant packs current object into QVariant for storing as UserData
 */
QVariant VideoPreset::asVariant()
{
    return QVariant::fromValue(*this);
}
