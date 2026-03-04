#include <gtest/gtest.h>
#include "../src/EncodingPreset.h"
#include "../src/VideoPreset.h"
#include "../src/Codec.h"
#include "codec_x264.h"
#include "codec_aac.h"
#include "ffmpegMock.h"

using ::testing::_;
using ::testing::Return;

class EncodingPresetTest : public ::testing::Test {
protected:
    FFmpegMock ff;
};

TEST_F(EncodingPresetTest, General) {
    // Ensure codecs are registered
    static CodecRegistrar<CodecX264> registrarX264("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
    static CodecRegistrar<CodecAAC> registrarAAC("AAC (Advanced Audio Coding)");

    EncodingPreset dialog(ff);

    VideoPreset preset("TestPreset", ".mp4", "libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10", "fast", "AAC (Advanced Audio Coding)", "high");
    dialog.setPreset(preset);

    VideoPreset retrieved = dialog.getPreset();
    EXPECT_EQ(retrieved.Name, "TestPreset");
    EXPECT_EQ(retrieved.Extension, ".mp4");
    EXPECT_EQ(retrieved.VideoCodec, "libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
    EXPECT_EQ(retrieved.VideoCodecPreset, "fast");
    EXPECT_EQ(retrieved.AudioCodec, "AAC (Advanced Audio Coding)");
    EXPECT_EQ(retrieved.AudioCodecPreset, "high");
}
