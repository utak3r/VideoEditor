#include "VideoPreset.h"

VideoPreset::VideoPreset()
{
    Name = "";
    Extension = "";
    CommandLine = "";
}

VideoPreset::VideoPreset(QString name, QString ext, QString cmd)
    : VideoPreset()
{
    Name = name;
    Extension = ext;
    CommandLine = cmd;
}

QVariant VideoPreset::asVariant()
{
    return QVariant::fromValue(*this);
}
