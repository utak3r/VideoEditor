#include "ffmpeg.h"
#include "ProcessRunnerDialog.h"
#include <QStringConverter>
#include <QDebug>

/*!
 * \brief FFMPEG::FFMPEG default constructor
 */
FFMPEG::FFMPEG(QObject *parent)
    : QObject{parent}
    , theProcess(nullptr)
    , theBinPath("ffmpeg.exe")
{
    theState = Undefined;
    theBinVersion = tr("unknown FFMPEG version");
}

/*!
 * \brief FFMPEG::FFMPEG constructor initializing the ffmpeg's binary path
 */
FFMPEG::FFMPEG(QString path, QObject *parent)
    : FFMPEG(parent)
{
    setBinPath(path);
}

/*!
 * \brief FFMPEG::state returns current state as FFMPEG::State enum
 */
FFMPEG::State FFMPEG::state()
{
    return theState;
}

/*!
 * \brief FFMPEG::binPath returns ffmpeg's binary path currently set
 */
QString FFMPEG::binPath()
{
    return theBinPath;
}

/*!
 * \brief FFMPEG::setBinPath sets ffmpeg's binary path.
 * Before setting the path, it runs a test if given path
 * allows you to run it.
 */
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

/*!
 * \brief FFMPEG::binVersion returns currently used ffmpeg's version.
 * The version is being checked during setting its path.
 */
QString FFMPEG::binVersion()
{
    return theBinVersion;
}

/*!
 * \brief FFMPEG::isBinaryAvailable checks if a given path leads to an existing binary
 * \param binPath given binary path
 * \param binVersion is being set to ffmpeg's version
 * \return
 */
bool FFMPEG::isBinaryAvailable(QString binPath, QString &binVersion)
{
    bool ret = false;
    binVersion = tr("unknown FFMPEG version");
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

/*!
 * \brief FFMPEG::getScalingTuple constructs a tuple containing scaling params
 */
std::tuple<bool, int, int, QString> FFMPEG::getScalingTuple(bool enabled, int width, int height, int filter)
{
    QString filterStr = "lanczos";
    switch (filter)
    {
    case 0:
        {
        filterStr = "fast_bilinear";
        break;
        }
    case 1:
        {
        filterStr = "bilinear";
        break;
        }
    case 2:
        {
        filterStr = "bicubic";
        break;
        }
    case 3:
        {
        filterStr = "area";
        break;
        }
    case 4:
        {
        filterStr = "lanczos";
        break;
        }
    }
    std::tuple<bool, int, int, QString> value(enabled, width, height, filterStr);
    return value;
}

/*!
 * \brief FFMPEG::Convert converts given video using a set of params
 * \param inFile input video
 * \param codecArgs codec settings for conversion
 * \param marks video trimming marks (if any)
 * \param outFile output video file name
 */
void FFMPEG::Convert(QString inFile, QString codecArgs, TimelineMarks* marks, std::tuple<bool, int, int, QString> scaling, QString outFile)
{
    if (!binPath().isEmpty())
    {
        if (theProcess == nullptr)
        {
            theState = Starting;
            theProcess = new QProcess(this);

            ProcessRunnerDialog* dlg = new ProcessRunnerDialog(qobject_cast<QWidget*>(this->parent()));

            connect(theProcess, &QProcess::started, this, [=]() {
                theState = Running;
                emit ffmpegStarted();
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

            // Due to ffmpeg's complex args semantics,
            // I'm using the "native args" feature.
            // If one would want to make it working on MacOS or Linux,
            // it'd have to be changed!
            QString nativeArgs = "-i \"" + inFile + "\"" +
                    " -y";
            if (marks->IsTrimmed())
            {
                nativeArgs += " -ss " + marks->TimecodeStart();
                nativeArgs += " -t " + marks->DurationTimecode();
            }

            if (std::get<0>(scaling))
            {
                nativeArgs += QString(" -vf scale=%1:%2:flags=%3")
                        .arg(std::get<1>(scaling))
                        .arg(std::get<2>(scaling))
                        .arg(std::get<3>(scaling));
            }

            nativeArgs += " " + codecArgs +
                    " \"" + outFile + "\"";
            theProcess->setNativeArguments(nativeArgs);
            qDebug() << "FFMPEG args: " << nativeArgs;

            connect(this, &FFMPEG::ffmpegRead, dlg, &ProcessRunnerDialog::logAdded);
            connect(this, &FFMPEG::ffmpegStarted, dlg, &ProcessRunnerDialog::processStarted);
            connect(this, &FFMPEG::ffmpegFinished, dlg, &ProcessRunnerDialog::processFinished);
            connect(dlg, &ProcessRunnerDialog::Cancel, this, [=]() {
                theProcess->kill();
            });
            dlg->setModal(true);
            dlg->show();
            theProcess->start(binPath());
        }
    }
}

