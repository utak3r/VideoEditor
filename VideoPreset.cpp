#include "VideoPreset.h"

/*!
 * \brief VideoPreset::VideoPreset default constructor
 */
VideoPreset::VideoPreset()
{
    Name = "";
    Extension = "";
    CommandLine = "";
}

/*!
 * \brief VideoPreset::VideoPreset constructor initializing all data at once
 * \param name name of the preset
 * \param ext file extension to be used
 * \param cmd ffnpeg arguments to be used during conversion
 */
VideoPreset::VideoPreset(QString name, QString ext, QString cmd)
    : VideoPreset()
{
    Name = name;
    Extension = ext;
    CommandLine = cmd;
}

/*!
 * \brief VideoPreset::asString encodes values as \r\n separated string
 */
QString VideoPreset::asString()
{
    QString str = Name + "\r\n" + Extension + "\r\n" + CommandLine;
    return str;
}

/*!
 * \brief VideoPreset::asVariant packs current object into QVariant for storing as UserData
 */
QVariant VideoPreset::asVariant()
{
    return QVariant::fromValue(*this);
}
