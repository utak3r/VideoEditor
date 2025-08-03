#ifndef TIMELINEMARKS_H
#define TIMELINEMARKS_H

#include <QString>
#include <QMetaType>
#include <QDebug>

/*!
 * \class TimelineMarks
 * \brief TimelineMarks holds the in and out marks for the video.
 */
class TimelineMarks
{
public:
    TimelineMarks();
	TimelineMarks(TimelineMarks& other);
    qint64 MarkIn() const;
    void setMarkIn(qint64 mark);
    qint64 MarkOut() const;
    void setMarkOut(qint64 mark);

public:
    QString AsString();
    qint64 Duration() const;
    void Reset(qint64 duration = 0);
    static QString MillisecondsToTimecode(qint64 time, uint secondsFractionPrecision = 3);
    QString TimecodeStart(uint secondsFractionPrecision = 3) const;
    QString TimecodeEnd(uint secondsFractionPrecision = 3) const;
    qint64 MillisecondsStart() const;
    qint64 MillisecondsEnd() const;
    QString DurationTimecode();
    QString CurrentRange(uint secondsFractionPrecision = 3) const;
    bool IsTrimmed();
    bool IsFullVideo() const;
	TimelineMarks& operator=(const TimelineMarks& other);
    friend bool operator==(const TimelineMarks& lhs, const TimelineMarks& rhs);
    friend bool operator!=(const TimelineMarks& lhs, const TimelineMarks& rhs);

private:
    qint64 theMarkIn;
    qint64 theMarkOut;
    qint64 theVideoDuration;
};
Q_DECLARE_METATYPE(TimelineMarks);

inline QDebug operator<<(QDebug dbg, const TimelineMarks& marks)
{
    dbg.nospace() << "TimelineMarks(" << marks.MarkIn() << ", " << marks.MarkOut() << ")";
    return dbg.space();
}

#endif // TIMELINEMARKS_H
