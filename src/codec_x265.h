#pragma once
#include <QString>
#include <QStringList>
#include "ffmpegInterface.h"
#include "Codec.h"

class CodecX265 : public Codec
{
public:
	explicit CodecX265(IFFmpeg* ff) : ff(ff) {}
	~CodecX265() override = default;

	CodecType type() const override;
	QString name() const override;
	QStringList getAvailablePresets() const override;
	void setPreset(const QString& preset, AVCodecContext* codecContext) override;
	const AVCodec* getAVCodec() const override;

private:
	IFFmpeg* ff;
};
