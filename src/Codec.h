#pragma once
#include <QString>
#include <QHash>
extern "C" {
#include <libavcodec/codec.h>
#include <libavcodec/avcodec.h>
}

class Codec
{
public:
	enum class CodecType
	{
		Audio,
		Video
	};

public:
	virtual ~Codec() = default;
	virtual CodecType type() const = 0;
	virtual QString name() const = 0;
	virtual QStringList getAvailablePresets() const = 0;
	virtual void setPreset(const QString& preset, AVCodecContext* codecContext) = 0;
	virtual const AVCodec* getAVCodec() const = 0;
};

using CodecCreator = std::function<Codec* ()>;

class CodecFactory
{
public:
    static CodecFactory& instance()
    {
        static CodecFactory f;
        return f;
    }

    void registerPlugin(const QString& name, CodecCreator creator)
    {
        creators[name] = creator;
    }

    QStringList availablePlugins() const
    {
        return creators.keys();
    }

    Codec* create(const QString& name) const
    {
        auto it = creators.find(name);
        return it != creators.end() ? it.value()() : nullptr;
    }

private:
    QHash<QString, CodecCreator> creators;
    CodecFactory() = default;
};

template <typename T>
class CodecRegistrar
{
public:
    CodecRegistrar(const QString& className)
    {
        CodecFactory::instance().registerPlugin(className, []() -> Codec*
            {
            return new T();
            });
    }
};
