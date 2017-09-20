#pragma once

extern "C"
{
    #include <libavformat/avformat.h>
}
#include <string>
#include <cstdint>
#include <stdexcept>
#include <fstream>
#include <memory>
#include "Mat.hpp"

namespace IMT {
namespace LibAv
{
    class Frame
    {
        public:
            enum class FrameType : std::int8_t
            {
                I = 1,
                P = 2,
                B = 3
            };
            //If we create a Frame, we suppose AVFrame* is well allocated and have a frame
            Frame(AVFrame* frame, unsigned int frameId);
            Frame(Frame&& f);
            Frame& operator=(Frame&& f);
            ~Frame(void);

            std::string GetInfo(void) const;
            unsigned int GetCodedId(void) const { return m_frame->coded_picture_number; }
            unsigned int GetFrameId(void) const { return m_frameId; }
            unsigned int GetUniqueId(void) const { return av_frame_get_pkt_pos(m_frame);}
            unsigned int GetFramePktSize(void) const { return m_frame->pkt_size; }
            FrameType GetFrameType(void) const { switch (m_frame->pict_type)
                {
                    case 1:
                        return FrameType::I;
                    case 2:
                        return FrameType::P;
                    case 3:
                        return FrameType::B;
                    default:
                        throw std::invalid_argument("frame type should be a number between 1 and 3");
                }
            }
            unsigned int GetPacketSize(void) { return m_frame->pkt_size; }
            std::string ToReportingString(void) const;
            void WriteYuvToFile(std::ofstream& outputYuvFile, const std::string& format) const; // format = 1920x1080, etc.
            static void WriteBlackYuvToFile(std::ofstream& outputYuvFile, const std::string& format);
            std::shared_ptr<Mat> ToOpenCvMat(const std::string& format) const;
        private:
            AVFrame* m_frame;
            unsigned int m_frameId;
            Frame(const Frame& f) = delete; //no copy constructor
            Frame& operator=(const Frame& f) = delete;

    };

}
}
