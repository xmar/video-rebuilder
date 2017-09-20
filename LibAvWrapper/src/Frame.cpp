#include "Frame.hpp"

extern "C"
{
   #include <libavutil/imgutils.h>
   #include <libavutil/parseutils.h>
   #include <libswscale/swscale.h>
}

using namespace IMT::LibAv;


Frame::Frame(AVFrame* frame, unsigned int frameId): m_frame(frame), m_frameId(frameId)
{}

Frame::Frame(Frame&& f): Frame(f.m_frame, f.m_frameId)
{
    f.m_frame = nullptr;
}

Frame& Frame::operator=(Frame&& f)
{
    std::swap(m_frame, f.m_frame);
    std::swap(m_frameId, f.m_frameId);
    return *this;
}

Frame::~Frame(void)
{
    if(m_frame != nullptr)
    {

        av_frame_unref(m_frame);
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
}


std::string Frame::GetInfo(void) const
{
    return std::to_string(m_frame->coded_picture_number)+";X;"+std::to_string(m_frame->pict_type)+";"
        +std::to_string(m_frame->pkt_pts)+";"+ std::to_string(m_frame->pkt_dts)+";"+std::to_string(m_frame->pkt_size);
}


std::string Frame::ToReportingString(void) const
{
    if (m_frame != nullptr)
    {
        return std::to_string(m_frame->coded_picture_number)+";"+std::to_string(m_frameId)+";"+std::to_string(m_frame->pict_type)+";"+std::to_string(m_frame->pkt_pts)+";"+std::to_string(m_frame->pkt_dts)+";"+std::to_string(m_frame->pkt_size);
    }
    else
    {
        return "0;0;0;0;0;0";
    }
}

void Frame::WriteYuvToFile(std::ofstream& outputYuvFile, const std::string& format) const
{
    uint8_t* dst_data[4];
    int dst_linesize[4];
    int dst_w, dst_h;
    auto dst_pix_fmt = AV_PIX_FMT_YUV420P;
    av_parse_video_size (&dst_w, &dst_h, format.c_str());
    struct SwsContext *sws_ctx;
    int buffSize = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pix_fmt, 1);
    sws_ctx = sws_getContext(m_frame->width, m_frame->height, (enum AVPixelFormat)(m_frame->format), dst_w, dst_h, dst_pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
    sws_scale(sws_ctx, (const uint8_t * const*) m_frame->data, m_frame->linesize, 0, m_frame->height, dst_data, dst_linesize);
    outputYuvFile.write(reinterpret_cast<char*>(dst_data[0]), buffSize);
    av_freep(&dst_data);
    sws_freeContext(sws_ctx);
}

void Frame::WriteBlackYuvToFile(std::ofstream& outputYuvFile, const std::string& format)
{
   uint8_t* dst_data[4];
   int dst_linesize[4];
   int dst_w, dst_h;
   auto dst_pix_fmt = AV_PIX_FMT_YUV420P;
   av_parse_video_size (&dst_w, &dst_h, format.c_str());
   int buffSize = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pix_fmt, 1);
   memset(dst_data[0], 0, buffSize);
   outputYuvFile.write(reinterpret_cast<char*>(dst_data[0]), buffSize);
   av_freep(&dst_data);
}

#include <iostream>
std::shared_ptr<Mat> Frame::ToOpenCvMat(const std::string& format) const
{
    //uint8_t* dst_data[4];
    //int dst_linesize[4];
    //int dst_w, dst_h;
    ////auto dst_pix_fmt = AV_PIX_FMT_YUV420P;
    //auto dst_pix_fmt = PIX_FMT_BGR24;
    //av_parse_video_size (&dst_w, &dst_h, format.c_str());
    //struct SwsContext *sws_ctx;
    //int buffSize = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pix_fmt, 1);
    //sws_ctx = sws_getContext(m_frame->width, m_frame->height, (enum AVPixelFormat)(m_frame->format), dst_w, dst_h, dst_pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
    //sws_scale(sws_ctx, (const uint8_t * const*) m_frame->data, m_frame->linesize, 0, m_frame->height, dst_data, dst_linesize);
    //auto img = std::make_shared<cv::Mat> (m_frame->height,m_frame->width,CV_8UC3, dst_data[0], dst_linesize[0]);
    //av_freep(&dst_data);
    //sws_freeContext(sws_ctx);
    //return img;

   AVFrame dst;
   int w = m_frame->width;
   int h = m_frame->height;
#ifdef PIX_FMT_BGR24
   avpicture_fill( (AVPicture *)&dst, dst.data[0], PIX_FMT_BGR24, w, h);
   auto src_pixfmt = (enum PixelFormat)m_frame->format;
   auto dst_pixfmt = PIX_FMT_BGR24;
#else
   avpicture_fill( (AVPicture *)&dst, dst.data[0], AV_PIX_FMT_BGR24, w, h);
   auto src_pixfmt = (enum AVPixelFormat)m_frame->format;
   auto dst_pixfmt = AV_PIX_FMT_BGR24;
#endif //PIX_FMT_BGR24
   int buffSize = av_image_alloc(dst.data, dst.linesize, w, h, dst_pixfmt, 1);
   auto convert_ctx = sws_getContext(w, h, src_pixfmt, w, h, dst_pixfmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
   auto returnMat = std::make_shared<Mat>(h, w, CV_8UC3, dst.data, dst.linesize[0]);

   if(convert_ctx == NULL)
   {
      std::cout << "Cannot initialize the conversion context!" << std::endl;
   }

   sws_scale(convert_ctx, m_frame->data, m_frame->linesize, 0, h, dst.data, dst.linesize);

   sws_freeContext(convert_ctx);
   return returnMat;
}
