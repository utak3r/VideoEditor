#include <gtest/gtest.h>
#include "../src/VideoSlider.h"

TEST(VideoSlider, General) {
    VideoSlider slider;
    slider.setMinimum(0);
    slider.setMaximum(100);

    EXPECT_EQ(slider.MarkIn(), -1);
    EXPECT_EQ(slider.MarkOut(), -1);

    slider.setMarkIn(20);
    slider.setMarkOut(80);

    EXPECT_EQ(slider.MarkIn(), 20);
    EXPECT_EQ(slider.MarkOut(), 80);

    slider.resetMarks();
    EXPECT_EQ(slider.MarkIn(), -1);
    EXPECT_EQ(slider.MarkOut(), -1);
}
