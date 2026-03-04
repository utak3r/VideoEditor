
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/TimelineMarks.h"

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
    EXPECT_EQ(marks.MarkIn(), -1);
    EXPECT_EQ(marks.MarkOut(), -1);
    EXPECT_EQ(marks.Duration(), 0);
    EXPECT_EQ(marks.IsTrimmed(), false);
}

TEST(TimelineMarks, Timecode)
{
    TimelineMarks marks;
    EXPECT_EQ(marks.CurrentRange(), QString(QLatin1String("---")));
    marks.setMarkIn(2000); // 2 secs
    marks.setMarkOut(5297000); // 1h 28m 17s
    EXPECT_EQ(marks.TimecodeStart(), QString(QLatin1String("00:00:02.000")));
    EXPECT_EQ(marks.TimecodeEnd(), QString(QLatin1String("01:28:17.000")));
    EXPECT_EQ(marks.DurationTimecode(), QString(QLatin1String("01:28:15.000")));
    EXPECT_EQ(marks.CurrentRange(), QString(QLatin1String("00:00:02.000 - 01:28:17.000")));
}

TEST(TimelineMarks, OneWayTrimming)
{
    TimelineMarks marks;
    EXPECT_EQ(marks.IsTrimmed(), false);
    marks.setMarkOut(10000);
    EXPECT_EQ(marks.IsTrimmed(), true);
    EXPECT_EQ(marks.Duration(), 10000);
    EXPECT_EQ(marks.CurrentRange(), QString(QLatin1String("00:00:00.000 - 00:00:10.000")));
    marks.Reset(30000);
    EXPECT_EQ(marks.IsTrimmed(), false);
    marks.setMarkIn(5000);
    EXPECT_EQ(marks.IsTrimmed(), true);
    EXPECT_EQ(marks.Duration(), 25000);
    EXPECT_EQ(marks.CurrentRange(), QString(QLatin1String("00:00:05.000 - 00:00:30.000")));
}

TEST(TimelineMarks, EdgeCases)
{
    TimelineMarks marks;
    marks.Reset(60000);
    EXPECT_EQ(marks.IsTrimmed(), false);
    EXPECT_EQ(marks.Duration(), 60000);
    marks.setMarkIn(20000);
    marks.setMarkOut(15000);
    EXPECT_EQ(marks.IsTrimmed(), true);
    EXPECT_EQ(marks.Duration(), 40000); // setting the mark out should fail and stay on the video's end
    EXPECT_EQ(marks.CurrentRange(), QString(QLatin1String("00:00:20.000 - 00:01:00.000")));
}

TEST(TimelineMarks, Extended)
{
    TimelineMarks marks;
    marks.Reset(60000);
    EXPECT_EQ(marks.IsFullVideo(), true);
    EXPECT_EQ(marks.MillisecondsStart(), 0);
    EXPECT_EQ(marks.MillisecondsEnd(), 60000);

    marks.setMarkIn(10000);
    marks.setMarkOut(50000);
    EXPECT_EQ(marks.IsFullVideo(), false);
    EXPECT_EQ(marks.MillisecondsStart(), 10000);
    EXPECT_EQ(marks.MillisecondsEnd(), 50000);

    TimelineMarks marks2;
    marks2.Reset(60000);
    marks2.setMarkIn(10000);
    marks2.setMarkOut(50000);
    EXPECT_TRUE(marks == marks2);

    marks2.setMarkIn(20000);
    EXPECT_TRUE(marks != marks2);

    TimelineMarks marks3(marks2);
    EXPECT_TRUE(marks3 == marks2);
    EXPECT_EQ(marks3.MillisecondsStart(), 20000);
    EXPECT_EQ(marks3.MillisecondsEnd(), 50000);

    TimelineMarks marks4;
    marks4 = marks3;
    EXPECT_TRUE(marks4 == marks3);
    EXPECT_EQ(marks4.AsString(), QString(QLatin1String("00:00:20.000 - 00:00:50.000")));
}
