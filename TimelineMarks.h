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
    int MarkIn();
    void setMarkIn(int mark);
    int MarkOut();
    void setMarkOut(int mark);

public:
    QString AsString();
    int Duration();
    void Reset(int duration = 0);
    QString MillisecondsToTimecode(int time);
    QString TimecodeStart();
    QString TimecodeEnd();
    QString DurationTimecode();
    QString CurrentRange();
    bool IsTrimmed();

private:
    int theMarkIn;
    int theMarkOut;
    int theVideoDuration;
};

#endif // TIMELINEMARKS_H
