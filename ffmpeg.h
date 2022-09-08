#ifndef FFMPEG_H
#define FFMPEG_H

#include <QObject>
#include <QProcess>
#include <TimelineMarks.h>

/*!
 * \class FFMPEG
 * \brief FFMPEG serves for any ffmpeg related actions
 */
class FFMPEG : public QObject
{
    Q_OBJECT
    /*!
     * \property FFMPEG::binPath
     * \brief holds ffmpeg's binary path
     */
    Q_PROPERTY(QString binPath READ binPath WRITE setBinPath NOTIFY ffmpegBinPathChanged)
    /*!
     * \property FFMPEG::binVersion
     * \brief holds ffmpeg's version
     */
    Q_PROPERTY(QString binVersion READ binVersion NOTIFY ffmpegBinVersionChanged)
    /*!
     * \property FFMPEG::state
     * \brief holds current state
     */
    Q_PROPERTY(FFMPEG::State state READ state NOTIFY ffmpegStateChanged)

public:
    enum State {
        Undefined = 0,
        Available,
        Starting,
        Running
    };
    Q_ENUM(State)

public:
    explicit FFMPEG(QObject *parent = nullptr);
    FFMPEG(QString path, QObject *parent = nullptr);

    QString binPath();
    void setBinPath(QString path);
    QString binVersion();
    FFMPEG::State state();
    void Convert(QString inFile, QString codecArgs, TimelineMarks* marks, QString outFile);

signals:
    void ffmpegBinPathChanged();
    void ffmpegBinVersionChanged();
    void ffmpegStateChanged(FFMPEG::State newState);
    void ffmpegRead(QString data);
    void ffmpegStarted();
    void ffmpegFinished();

private:
    bool isBinaryAvailable(QString binPath, QString &binVersion);

private:
    QProcess* theProcess;
    QString theBinPath;
    FFMPEG::State theState;
    QString theBinVersion;

};

#endif // FFMPEG_H
