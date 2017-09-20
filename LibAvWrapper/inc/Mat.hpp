//A wrapper to the Opencv Mat class to desallocat Libav memory
#pragma once

extern "C"
{
   #include <libavformat/avformat.h>
   #include <libavutil/imgutils.h>
//   #include <libavutil/parseutils.h>
//   #include <libswscale/swscale.h>
}
#include <opencv2/core/core.hpp>
#include <cstring>
#include <iostream>

namespace IMT {
namespace LibAv
{

class Mat: public cv::Mat
{
   public:
      Mat(int rows, int cols, int type, uint8_t* libAvData[4], size_t step): cv::Mat(rows, cols, type, libAvData[0], step)
       {
          for (auto i = 0; i < 4; ++i) { m_libAvDstData[i] = libAvData[i]; }
       }
      ~Mat(void)
      {
         av_freep(&m_libAvDstData);
      }

//      operator cv::Mat*(void) {return &m_mat;}
//      operator cv::Mat&(void) {return m_mat;}
//      cv::Mat* operator->(void) {return &m_mat;}
   private:
      Mat(const Mat&) = delete;
      Mat(Mat&&) = delete;

//      cv::Mat m_mat;
      uint8_t* m_libAvDstData[4];
};

}
}
