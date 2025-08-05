#pragma once
#include <QString>
#include <QStringList>
#include "Codec.h"

class CodecPCMs24le : public Codec
{
public:
	CodecPCMs24le() = default;
	~CodecPCMs24le() override = default;

	CodecType type() const override;
	QString name() const override;
	QStringList getAvailablePresets() const override;
	void setPreset(const QString& preset, AVCodecContext* codecContext) override;
	const AVCodec* getAVCodec() const override;
};
