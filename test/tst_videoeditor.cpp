
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "TimelineMarks.h"

QT_BEGIN_NAMESPACE
inline void PrintTo(const QString &qString, ::std::ostream *os)
{
    *os << qUtf8Printable(qString);
}
QT_END_NAMESPACE

using namespace testing;

TEST(TimelineMarks, General)
{
    TimelineMarks marks;
    EXPECT_EQ(marks.MarkIn, -1);
    EXPECT_EQ(marks.MarkOut, -1);
    EXPECT_EQ(marks.VideoDuration, 0);
}

TEST(TimelineMarks, Timecode)
{
    TimelineMarks marks;
    marks.MarkIn = 2000; // 2 secs
    marks.MarkOut = 17000; // 17 secs
    EXPECT_EQ(marks.TimecodeStart(), QString(QLatin1String("00:00:02.000")));
    EXPECT_EQ(marks.TimecodeEnd(), QString(QLatin1String("00:00:17.000")));
    EXPECT_EQ(marks.DurationTimecode(), QString(QLatin1String("00:00:15.000")));
}
