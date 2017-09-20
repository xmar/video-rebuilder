#include "Packet.hpp"

#include <utility>

using namespace IMT::LibAv;

Packet::Packet(void): m_pkt(), m_init(false)
{
    av_init_packet(&m_pkt);
    m_pkt.data = NULL;
    m_pkt.size = 0;
    m_pkt.stream_index = 0;
}

Packet::Packet(Packet&& pkt): Packet()
{
    std::swap(m_pkt, pkt.m_pkt);
    std::swap(m_init, pkt.m_init);
}

Packet& Packet::operator=(Packet&& pkt)
{
    std::swap(m_pkt, pkt.m_pkt);
    std::swap(m_init, pkt.m_init);
    return *this;
}

Packet::~Packet(void)
{
    Free();
}

void Packet::Free(void)
{
    if(m_init)
    {
        av_free_packet(&m_pkt);
        m_init = false;
    }
}

int Packet::GetNextPacket(AVFormatContext* fmt_ctx)
{

    if (fmt_ctx == nullptr) return -1;
    Free();
    int ret = -1;
    if ((ret = av_read_frame(fmt_ctx, &m_pkt)) >= 0 )
    {
        m_init = true;
    }
    return ret;
}
