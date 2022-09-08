#include "ffmpeg.h"
#include <QStringConverter>
#include <QDebug>

FFMPEG::FFMPEG(QObject *parent)
    : QObject{parent}
    , theProcess(nullptr)
    , theBinPath("ffmpeg.exe")
{
    theState = Undefined;
    theBinVersion = tr("unknown version");
}

FFMPEG::FFMPEG(QString path, QObject *parent)
    : FFMPEG(parent)
{
    setBinPath(path);
}

FFMPEG::State FFMPEG::state()
{
    return theState;
}

QString FFMPEG::binPath()
{
    return theBinPath;
}

void FFMPEG::setBinPath(QString path)
{
    theBinVersion = "";
    if (isBinaryAvailable(path, theBinVersion))
    {
        theBinPath = path;
        emit ffmpegBinPathChanged();
        emit ffmpegBinVersionChanged();
    }
}

QString FFMPEG::binVersion()
{
    return theBinVersion;
}

bool FFMPEG::isBinaryAvailable(QString binPath, QString &binVersion)
{
    bool ret = false;
    binVersion = tr("unknown version");
    if (theState == Undefined || theState == Available)
    {
        QProcess *ffmpeg = new QProcess(this);
        QStringList args;
        args << "-version";
        ffmpeg->start(binPath, args);
        if (ffmpeg->waitForReadyRead(2000))
        {
            QByteArray data = ffmpeg->readAll();
            auto utf8str = QStringDecoder(QStringDecoder::Utf8);
            QString dataStr = utf8str(data);
            QStringList dataLines = dataStr.split("\r\n", Qt::SkipEmptyParts, Qt::CaseInsensitive);
            if (dataLines.count() > 0)
            {
                qDebug() << dataLines[0];
                binVersion = dataLines[0];
            }
        }
        if (ffmpeg->waitForFinished(2000))
        {
            ret = true;
            theState = Available;
        }
    }
    return ret;
}

void FFMPEG::Run(QString inFile, QString codecArgs, TimelineMarks* marks, QString outFile)
{
    if (!binPath().isEmpty())
    {
        if (theProcess == nullptr)
        {
            theState = Starting;
            theProcess = new QProcess(this);
            connect(theProcess, &QProcess::started, this, [=]() {
                theState = Running;
            });
            connect(theProcess, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
                //
            });
            theProcess->setReadChannel(QProcess::StandardError);
            connect(theProcess, &QIODevice::readyRead, this, [=]() {
                QByteArray data = theProcess->readAll();
                auto utf8str = QStringDecoder(QStringDecoder::Utf8);
                QString dataStr = utf8str(data);
                qDebug() << dataStr;
                emit ffmpegRead(dataStr);
            });
            connect(theProcess, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitCode != 0)
                {
                    QByteArray data = theProcess->readAllStandardError();
                    auto utf8str = QStringDecoder(QStringDecoder::Utf8);
                    QString dataStr = utf8str(data);
                    QStringList dataLines = dataStr.split("\r\n", Qt::SkipEmptyParts, Qt::CaseInsensitive);
                    if (dataLines.count() > 0)
                    {
                        QString lastLine = dataLines[dataLines.count()-1];
                        qDebug() << lastLine;
                    }
                }
                theProcess->close();
                theProcess->disconnect();
                delete theProcess;
                theProcess = nullptr;
                theState = Available;
                emit ffmpegFinished();
            });

            QString nativeArgs = "-i \"" + inFile + "\"" +
                    " -y";

            if (marks->IsTrimmed())
            {
                nativeArgs += " -ss " + marks->TimecodeStart();
                nativeArgs += " -t " + marks->DurationTimecode();
            }

            nativeArgs += " " + codecArgs +
                    " \"" + outFile + "\"";

            theProcess->setNativeArguments(nativeArgs);
            theProcess->start(binPath());
        }
    }
}

