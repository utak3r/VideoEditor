#pragma once
#include <QString>
#include <QStringList>
#include "Codec.h"

class CodecX264 : public Codec
{
public:
	CodecX264() = default;
	~CodecX264() override = default;

	CodecType type() const override;
	QString name() const override;
	QStringList getAvailablePresets() const override;
	void setPreset(const QString& preset, AVCodecContext* codecContext) override;
	const AVCodec* getAVCodec() const override;
};
