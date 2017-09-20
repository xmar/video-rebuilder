#include "VideoReader.hpp"

extern "C"
{
    #include <libavutil/imgutils.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
}

using namespace IMT::LibAv;

VideoReader::VideoReader(const std::string& inputFileName): m_inputFileName(inputFileName), m_fmt_ctx(NULL), m_isInit(false)
{}

VideoReader::VideoReader(VideoReader&& vr): m_inputFileName(), m_fmt_ctx(NULL), m_isInit(false)
{
    std::swap(m_inputFileName, vr.m_inputFileName);
    std::swap(m_isInit, vr.m_isInit);
    std::swap(m_fmt_ctx, vr.m_fmt_ctx);
}

VideoReader& VideoReader::operator=(VideoReader&& vr)
{
    std::swap(m_inputFileName, vr.m_inputFileName);
    std::swap(m_isInit, vr.m_isInit);
    std::swap(m_fmt_ctx, vr.m_fmt_ctx);
    return *this;
}

VideoReader::~VideoReader(void)
{
    if (m_isInit)
    {
        avformat_close_input(&m_fmt_ctx);
        m_fmt_ctx = NULL;
        m_isInit = false;
    }
}

void VideoReader::Init(void)
{
    av_register_all();
    if (avformat_open_input(&m_fmt_ctx, m_inputFileName.c_str(), NULL, NULL) < 0)
    {
        throw VideoException("Could not open source file "+m_inputFileName);
    }

    if (avformat_find_stream_info(m_fmt_ctx, NULL) < 0)
    {
        throw VideoException("Could not find stream information in "+m_inputFileName);
    }

    av_dump_format(m_fmt_ctx, 0, m_inputFileName.c_str(), 0);
}

std::shared_ptr<Packet> VideoReader::GetNextPacket(void)
{
    if (m_fmt_ctx == nullptr) return nullptr;
    auto pkt_ptr = std::make_shared<Packet>();
    if (pkt_ptr->GetNextPacket(m_fmt_ctx) >= 0)
    {
        return pkt_ptr;
    }
    return nullptr;
}

#include <iostream>
std::shared_ptr<VideoCodec> VideoReader::GetDecoder(bool useMultithread)
{
    int video_stream_idx = av_find_best_stream(m_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

    if (video_stream_idx < 0)
    {
        std::cout << "Error: no best video stream found" << std::endl;
        return nullptr;
    }

    std::shared_ptr<VideoCodec> vc(new VideoCodec);
    if (vc->Init(m_fmt_ctx, video_stream_idx, useMultithread))
    {
        return vc;
    }
    else
    {
        std::cout << "Error: failed to unit the video decoder for stream " << video_stream_idx << std::endl;
        return nullptr;
    }
}
