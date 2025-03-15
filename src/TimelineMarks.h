#ifndef TIMELINEMARKS_H
#define TIMELINEMARKS_H

#include <QString>

/*!
 * \class TimelineMarks
 * \brief TimelineMarks holds the in and out marks for the video.
 */
class TimelineMarks
{
public:
    TimelineMarks();
    qint64 MarkIn();
    void setMarkIn(qint64 mark);
    qint64 MarkOut();
    void setMarkOut(qint64 mark);

public:
    QString AsString();
    qint64 Duration() const;
    void Reset(qint64 duration = 0);
    static QString MillisecondsToTimecode(qint64 time, uint secondsFractionPrecision = 3);
    QString TimecodeStart(uint secondsFractionPrecision = 3) const;
    QString TimecodeEnd(uint secondsFractionPrecision = 3) const;
    QString DurationTimecode();
    QString CurrentRange(uint secondsFractionPrecision = 3) const;
    bool IsTrimmed();

private:
    qint64 theMarkIn;
    qint64 theMarkOut;
    qint64 theVideoDuration;
};

#endif // TIMELINEMARKS_H
