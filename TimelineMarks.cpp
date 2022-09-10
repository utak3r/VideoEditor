#include "TimelineMarks.h"
#include <QtMath>

/*!
 * \brief TimelineMarks::TimelineMarks: default constructor
 */
TimelineMarks::TimelineMarks()
    : MarkIn(-1)
    , MarkOut(-1)
    , VideoDuration(0)
{

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
    int end = VideoDuration;
    if (MarkIn > -1)
        start = MarkIn;
    if (MarkOut > -1)
        end = MarkOut;
    duration = end - start;
    return duration;
}

/*!
 * \brief TimelineMarks::reset: Reset marks.
 */
void TimelineMarks::Reset(int duration)
{
    MarkIn = -1;
    MarkOut = -1;
    VideoDuration = duration;
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
    if (MarkIn > -1)
        start = MarkIn;
    return MillisecondsToTimecode(start);
}

/*!
 * \brief TimelineMarks::timecode_end: Returns end of range in a timecode format.
 */
QString TimelineMarks::TimecodeEnd()
{
    int end = VideoDuration;
    if (MarkOut > -1)
        end = MarkOut;
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
    if (MarkIn > -1 || MarkOut > -1)
        trimmed = true;
    return trimmed;
}

