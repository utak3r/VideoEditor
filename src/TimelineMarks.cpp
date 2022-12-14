#include "TimelineMarks.h"
#include <QtMath>

/*!
 * \brief TimelineMarks::TimelineMarks: default constructor
 */
TimelineMarks::TimelineMarks()
    : theMarkIn(-1)
    , theMarkOut(-1)
    , theVideoDuration(0)
{

}

/*!
 * \brief TimelineMarks::MarkIn
 */
int TimelineMarks::MarkIn()
{
    return theMarkIn;
}

/*!
 * \brief TimelineMarks::setMarkIn
 */
void TimelineMarks::setMarkIn(int mark)
{
    if (mark < theMarkOut || theMarkOut == -1)
        theMarkIn = mark;
}

/*!
 * \brief TimelineMarks::MarkOut
 */
int TimelineMarks::MarkOut()
{
    return theMarkOut;
}

/*!
 * \brief TimelineMarks::setMarkOut
 */
void TimelineMarks::setMarkOut(int mark)
{
    if (mark > theMarkIn || theMarkIn == -1)
        theMarkOut = mark;
}

/*!
 * \brief TimelineMarks::AsString: returns current range in a timecode format.
 */
QString TimelineMarks::AsString()
{
    return CurrentRange();
}

/*!
 * \brief TimelineMarks::duration: Duration between the marks.
 */
int TimelineMarks::Duration()
{
    int duration = 0;
    int start = 0;
    int end = theVideoDuration;
    if (theMarkIn > -1)
        start = theMarkIn;
    if (theMarkOut > -1)
        end = theMarkOut;
    duration = end - start;
    return duration;
}

/*!
 * \brief TimelineMarks::reset: Reset marks.
 */
void TimelineMarks::Reset(int duration)
{
    theMarkIn = -1;
    theMarkOut = -1;
    theVideoDuration = duration;
}

/*!
 * \brief TimelineMarks::milliseconds_to_timecode: Format given time in ms into a string.
 */
QString TimelineMarks::MillisecondsToTimecode(int time)
{
    QString timecode = "";
    if (time > -1)
    {
        int hours = qFloor(time / 3600000);
        int minutes = qFloor(time / 60000) - hours*60;
        int seconds = qFloor(time / 1000) - hours*3600 - minutes*60;
        int milliseconds = time - hours*3600000 - minutes*60000 - seconds*1000;
        timecode = QString("%1:%2:%3.%4").
                arg(hours, 2, 10, QLatin1Char('0')).
                arg(minutes, 2, 10, QLatin1Char('0')).
                arg(seconds, 2, 10, QLatin1Char('0')).
                arg(milliseconds, 3, 10, QLatin1Char('0'));
    }
    return timecode;
}

/*!
 * \brief TimelineMarks::timecode_start: Returns beginning of range in a timecode format.
 */
QString TimelineMarks::TimecodeStart()
{
    int start = 0;
    if (theMarkIn > -1)
        start = theMarkIn;
    return MillisecondsToTimecode(start);
}

/*!
 * \brief TimelineMarks::timecode_end: Returns end of range in a timecode format.
 */
QString TimelineMarks::TimecodeEnd()
{
    int end = theVideoDuration;
    if (theMarkOut > -1)
        end = theMarkOut;
    return MillisecondsToTimecode(end);
}

/*!
 * \brief TimelineMarks::duration_timecode: Returns duration in a timecode formatted string.
 */
QString TimelineMarks::DurationTimecode()
{
    return MillisecondsToTimecode(Duration());
}

/*!
 * \brief TimelineMarks::current_range: Range in a text format.
 * From mark in to mark out, but if they're not set, show full video duration.
 */
QString TimelineMarks::CurrentRange()
{
    QString video_range = "---";
    if (Duration() > 0)
        video_range = TimecodeStart() + " - " + TimecodeEnd();
    return video_range;
}

/*!
 * \brief TimelineMarks::is_trimmed: Checks if range is not a full video.
 */
bool TimelineMarks::IsTrimmed()
{
    bool trimmed = false;
    if (theMarkIn > -1 || theMarkOut > -1)
        trimmed = true;
    return trimmed;
}

