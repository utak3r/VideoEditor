#pragma once
#include <QString>
#include <QStringList>
#include "ffmpegInterface.h"
#include "Codec.h"

class CodecPCMs24le : public Codec
{
public:
	explicit CodecPCMs24le(IFFmpeg* ff) : ff(ff) {}
	~CodecPCMs24le() override = default;

	CodecType type() const override;
	QString name() const override;
	QStringList getAvailablePresets() const override;
	void setPreset(const QString& preset, AVCodecContext* codecContext) override;
	const AVCodec* getAVCodec() const override;

private:
	IFFmpeg* ff;
};
