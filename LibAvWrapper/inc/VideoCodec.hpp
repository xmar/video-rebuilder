
#pragma once
extern "C"
{
    #include <libavformat/avformat.h>
}

#include <memory>
#include <map>
#include "Frame.hpp"
#include "Packet.hpp"

namespace IMT {
namespace LibAv
{
    class VideoCodec
    {
        public:
            VideoCodec(void);
            VideoCodec(VideoCodec&& vc);
            VideoCodec& operator=(VideoCodec&& vc);
            ~VideoCodec(void);

            bool Init(AVFormatContext* fmt_ctx, unsigned int streamId, bool useMultithread);

            std::shared_ptr<Frame> Decode(std::shared_ptr<Packet> pkt_ptr);
            std::shared_ptr<Frame> Flush(void);
            void Reset(void) { avcodec_flush_buffers(m_video_dec_ctx); }
            void PrintSliceInfo(void) const;
            int GetSkipCount(void) const {return m_codec_open ? m_video_dec_ctx->frame_number : -1;}
        private:
            AVCodecContext* m_video_dec_ctx;
            bool m_codec_open;
            unsigned int m_frameId;
            unsigned int m_packetId;
            std::map<int64_t, int64_t> m_posToPacketId;
            unsigned int m_streamId;

            VideoCodec(const VideoCodec& vc) = delete;
            VideoCodec& operator=(const VideoCodec& vc) = delete;
    };

}
}
