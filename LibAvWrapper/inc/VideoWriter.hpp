/**
 * Author: Xavier Corbillon
 * Wrapper for libav to write a video file
 */
#pragma once

extern "C"
{
    #include <libavformat/avformat.h>
}

#include "Packet.hpp"
#include "VideoCodec.hpp"

#include <string>
#include <exception>
#include <memory>

namespace IMT {
namespace LibAv
{
    class VideoException;

    class VideoReader;

    class VideoWriter
    {
        public:
            VideoWriter(const std::string& outputFileName);
            VideoWriter(VideoWriter&& vw);
            VideoWriter& operator=(VideoWriter&& vw);
            ~VideoWriter(void);

            //To init the video writter with the same parameters as the VideoReader
            void Init(const VideoReader& vr);

            void Write(std::shared_ptr<Packet> pkt);

            std::shared_ptr<VideoCodec> GetCoder(void);
        private:
            std::string m_outputFileName;
            AVFormatContext* m_fmt_ctx;
            bool m_isInit;

            VideoWriter(const VideoWriter& vw) = delete;
            VideoWriter& operator=(const VideoWriter& vw) = delete;
    };
}
}
