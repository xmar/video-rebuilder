/**
 * Author: Xavier Corbillon
 * Wrapper for libav to read a video file
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
    class VideoException : public std::exception
    {
        public:
            VideoException(std::string info): m_info(info) {}
            const char* what(void) const throw()
            {
                return m_info.c_str();
            }
        private:
            std::string m_info;
    };

    class VideoReader
    {
        public:
            VideoReader(const std::string& inputFileName);
            VideoReader(VideoReader&& vr);
            VideoReader& operator=(VideoReader&& vr);
            ~VideoReader();

            void Init(void);

            std::shared_ptr<Packet> GetNextPacket(void);

            std::shared_ptr<VideoCodec> GetDecoder(bool useMultithread = true);
        private:
            friend class VideoWriter;
            std::string m_inputFileName;
            AVFormatContext* m_fmt_ctx;
            bool m_isInit;

            VideoReader(const VideoReader& vr) = delete;
            VideoReader& operator=(const VideoReader& vr) = delete;
    };
}
}
