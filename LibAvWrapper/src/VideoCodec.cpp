#include "VideoCodec.hpp"

using namespace IMT::LibAv;

VideoCodec::VideoCodec(void): m_video_dec_ctx(nullptr), m_codec_open(false), m_frameId(0), m_packetId(0), m_posToPacketId(), m_streamId(0)
{}

VideoCodec::VideoCodec(VideoCodec&& vc): VideoCodec()
{
    std::swap(m_video_dec_ctx, vc.m_video_dec_ctx);
    std::swap(m_codec_open, vc.m_codec_open);
    std::swap(m_frameId, vc.m_frameId);
}

VideoCodec& VideoCodec::operator=(VideoCodec&& vc)
{
    std::swap(m_video_dec_ctx, vc.m_video_dec_ctx);
    std::swap(m_codec_open, vc.m_codec_open);
    std::swap(m_frameId, vc.m_frameId);
    return *this;
}

VideoCodec::~VideoCodec(void)
{
    if (m_codec_open)
    {
        avcodec_close(m_video_dec_ctx);
    }
}

bool VideoCodec::Init(AVFormatContext* fmt_ctx, unsigned int streamId, bool useMultithread)
{
    AVStream* st = fmt_ctx->streams[streamId];
    AVDictionary* opts(nullptr);
    m_video_dec_ctx = st->codec;
    auto* dec = avcodec_find_decoder(m_video_dec_ctx->codec_id);

    if(!dec)
    {
        //ERROR
        return false;
    }

    av_dict_set(&opts, "refcounted_frames", "1", 0);
    if (!useMultithread)
    {
       av_dict_set(&opts, "threads", "1", 0);
    }
    int ret = -1;
    if ((ret = avcodec_open2(m_video_dec_ctx, dec, &opts)) < 0)
    {
        m_codec_open = false;
        m_video_dec_ctx = nullptr;
        //ERROR
        return false;
    }
    else
    {
        m_codec_open = true;
        m_streamId = streamId;
        return true;
    }
}

std::shared_ptr<Frame> VideoCodec::Decode(std::shared_ptr<Packet> pkt_ptr)
{
    if (m_codec_open)
    {
        if ((pkt_ptr && pkt_ptr->GetAvPacket().stream_index != m_streamId) || !pkt_ptr)
        {
           return nullptr;
        }
        int got_a_frame = 0;
        AVFrame* frame_ptr = av_frame_alloc();
        m_posToPacketId[pkt_ptr->GetPos()] = m_packetId++;
        const AVPacket* const packet_ptr = &pkt_ptr->GetAvPacket();
        int ret = avcodec_decode_video2(m_video_dec_ctx, frame_ptr, &got_a_frame, packet_ptr);
        if (ret > 0 && got_a_frame)
        {
            auto packetIdRef = m_posToPacketId.find(av_frame_get_pkt_pos(frame_ptr));
            if (packetIdRef != m_posToPacketId.end())
            {
               frame_ptr->coded_picture_number = packetIdRef->second;
               m_posToPacketId.erase(packetIdRef);
            }
            std::shared_ptr<Frame> s_f_ptr = std::make_shared<Frame>(frame_ptr, ++m_frameId);
            return s_f_ptr;
        }
        else
        {
            av_frame_free(&frame_ptr);
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

#include <iostream>
std::shared_ptr<Frame> VideoCodec::Flush(void)
{
   if (m_codec_open)
   {
      int got_a_frame = 0;
      AVPacket pkt;
      av_init_packet(&pkt);
      pkt.data = nullptr;
      pkt.size = 0;
      pkt.stream_index = m_streamId;
      AVFrame* frame_ptr = av_frame_alloc();
      int ret = avcodec_decode_video2(m_video_dec_ctx, frame_ptr, &got_a_frame, &pkt);
      av_free_packet(&pkt);
      if (got_a_frame)
      {
         auto packetIdRef = m_posToPacketId.find(av_frame_get_pkt_pos(frame_ptr));
         if (packetIdRef != m_posToPacketId.end())
         {
            frame_ptr->coded_picture_number = packetIdRef->second;
            m_posToPacketId.erase(packetIdRef);
         }
         std::shared_ptr<Frame> s_f_ptr = std::make_shared<Frame>(frame_ptr, ++m_frameId);
         return s_f_ptr;
      }
      else
      {
         av_frame_free(&frame_ptr);
         return nullptr;
      }
   }
   else
   {
      return nullptr;
   }
}

void VideoCodec::PrintSliceInfo(void) const
{
    if (m_codec_open)
    {
        std::cout << "Nb slices: " << m_video_dec_ctx->slice_count << "; " << m_video_dec_ctx->slices << std::endl;
    }
}
