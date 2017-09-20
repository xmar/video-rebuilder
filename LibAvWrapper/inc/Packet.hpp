#pragma once
extern "C"
{
    #include <libavformat/avformat.h>
}

namespace IMT {
namespace LibAv
{
    class Packet
    {
        public:
            Packet(void);
            Packet(Packet&& pkt);
            Packet& operator=(Packet&& pkt);
            ~Packet(void);

            int GetNextPacket(AVFormatContext* fmt_ctx);

            const AVPacket& GetAvPacket(void) const {return m_pkt;}
            int64_t GetPos(void) const { return m_pkt.pos;}
        private:
            friend class VideoWriter;
            AVPacket m_pkt;
            bool m_init;
            void Free(void);

            Packet(const Packet& pkt) = delete;
            Packet& operator=(const Packet& pkt) = delete;
    };
}
}
