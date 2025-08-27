#pragma once
#include <QString>
#include <QStringList>
#include "ffmpegInterface.h"
#include "Codec.h"

class CodecX264 : public Codec
{
public:
	explicit CodecX264(IFFmpeg* ff) : ff(ff) {}
	~CodecX264() override = default;

	CodecType type() const override;
	QString name() const override;
	QStringList getAvailablePresets() const override;
	void setPreset(const QString& preset, AVCodecContext* codecContext) override;
	const AVCodec* getAVCodec() const override;

private:
	IFFmpeg* ff;
};
