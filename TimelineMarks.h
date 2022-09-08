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

public:
    QString AsString();
    int Duration();
    void Reset(int duration);
    QString MillisecondsToTimecode(int time);
    QString TimecodeStart();
    QString TimecodeEnd();
    QString DurationTimecode();
    QString CurrentRange();
    bool IsTrimmed();

public:
    int MarkIn;
    int MarkOut;
    int VideoDuration;
};

#endif // TIMELINEMARKS_H
