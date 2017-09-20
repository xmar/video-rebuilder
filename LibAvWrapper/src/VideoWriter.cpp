#include "VideoReader.hpp"
#include "VideoWriter.hpp"

using namespace IMT::LibAv;

VideoWriter::VideoWriter(const std::string& outputFileName): m_outputFileName(outputFileName),  m_fmt_ctx(NULL), m_isInit(false)
{}

VideoWriter::VideoWriter(VideoWriter&& vw): m_outputFileName(), m_fmt_ctx(NULL), m_isInit(false)
{
    std::swap(m_outputFileName, vw.m_outputFileName);
    std::swap(m_fmt_ctx, vw.m_fmt_ctx);
    std::swap(m_isInit, vw.m_isInit);
}

VideoWriter& VideoWriter::operator=(VideoWriter&& vw)
{
    std::swap(m_outputFileName, vw.m_outputFileName);
    std::swap(m_fmt_ctx, vw.m_fmt_ctx);
    std::swap(m_isInit, vw.m_isInit);
    return *this;
}

VideoWriter::~VideoWriter(void)
{
    if (!m_isInit)
    {
        av_write_trailer(m_fmt_ctx);
        avformat_close_input(&m_fmt_ctx);
        m_fmt_ctx = NULL;
        m_isInit = false;
    }
}

void VideoWriter::Init(const VideoReader& vr)
{
    avformat_alloc_output_context2(&m_fmt_ctx, NULL, NULL, m_outputFileName.c_str());
    if (!m_fmt_ctx)
    {
        throw VideoException("Coulnt allocate output video "+m_outputFileName);
    }

    //now we copy the streams from the vr
    for (unsigned int i = 0; i < vr.m_fmt_ctx->nb_streams; ++i)
    {
        AVStream *in_stream = vr.m_fmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(m_fmt_ctx, in_stream->codec->codec);
        if (!out_stream)
        {
            throw VideoException("Failed to allocate output stream");
        }

        auto ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0)
        {
            throw VideoException("Failed to copy context from input to output stream codec context");
        }

        if (m_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        {
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }

        out_stream->r_frame_rate = in_stream->r_frame_rate;
        out_stream->avg_frame_rate = in_stream->avg_frame_rate;
        out_stream->time_base = av_inv_q( out_stream->r_frame_rate );
        out_stream->codec->time_base = out_stream->time_base;
    }

    av_dump_format(m_fmt_ctx, 0, m_outputFileName.c_str(), 1);
    if (!(m_fmt_ctx->oformat->flags & AVFMT_NOFILE))
    {
        auto ret = avio_open(&m_fmt_ctx->pb, m_outputFileName.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0)
        {
            throw VideoException("Could not open output file "+m_outputFileName);
        }
    }

    auto ret = avformat_write_header(m_fmt_ctx, NULL);
    if (ret < 0)
    {
        throw VideoException("Error occurred when opening output file");
    }
}

void VideoWriter::Write( std::shared_ptr<Packet> pkt)
{
    if(pkt->m_init)
    {
        auto ret = av_interleaved_write_frame(m_fmt_ctx, &pkt->m_pkt);
        if (ret < 0)
        {
            throw VideoException("Error when muxing packet");
        }
    }
    else
    {
        throw VideoException("Error, try to mux a not initialized packet");
    }
}

std::shared_ptr<VideoCodec> VideoWriter::GetCoder(void)
{
    return nullptr;
}
