#include "remuxerFunction.hpp"
#include "VideoWriter.hpp"


void IMT::OriginalVideoReader(OriginalVideoInfo& ovi, std::string videoPath)
{
      auto vr = std::make_shared<LibAv::VideoReader>(videoPath);

      vr->Init();

      auto decoder = vr->GetDecoder();
      if (decoder == nullptr)
      {
         std::cout << "Fatal error" << std::endl;
         exit(1);
      }
      ovi.m_decoder = decoder;
      ovi.m_vr = vr;

      auto pkt_ptr = vr->GetNextPacket();
      while(pkt_ptr)
      {
         ovi.AddNextPacket(pkt_ptr);
         auto frame_ptr = decoder->Decode(pkt_ptr);
         if (frame_ptr != nullptr)
         {
            ovi.AddNextFrame(frame_ptr);
         }

         pkt_ptr = vr->GetNextPacket();
      }
      //Flush last frames
      auto frame_ptr = decoder->Flush();
      while(frame_ptr)
      {
         ovi.AddNextFrame(frame_ptr);
         frame_ptr = decoder->Flush();
      }
      if (ovi.m_maxDistanceBetweenTwoI == 0)
      {
         ovi.m_maxDistanceBetweenTwoI = ovi.m_nbFrames;
      }
}




void IMT::RemuxeToYuv(OriginalVideoInfo& ovi, std::string videoOutputPath, std::string format,  std::shared_ptr<Remuxer::ResultReader> rr, unsigned int maxNbFrame)
{
   std::ofstream yuvOutput(videoOutputPath, std::ofstream::binary | std::ofstream::out);

   std::shared_ptr<LibAv::Frame> lastFrame(nullptr);
   unsigned int lastFrameId(0);

   ovi.m_decoder->Reset();
   if (maxNbFrame == 0)
   {
      maxNbFrame = ovi.m_nbFrames;
   }
//   ->WriteYuvToFile(yuvOutput, format);
   for (unsigned int codingId = 0; codingId < maxNbFrame; ++codingId)
   {
      if ( rr == nullptr || rr->IsFrameReceived(codingId)) //check if coding id is not in the lost vector
      {
         auto frame_ptr = ovi.m_decoder->Decode(ovi.m_packetVec[codingId]);
         if (frame_ptr != nullptr)
         {
            auto currFrameId = ovi.GetRealFrameId(frame_ptr);
            while (lastFrameId < currFrameId) //then their is missing frames
            {
               if (lastFrame == nullptr)
               {
                  std::cerr << "Debug: Coding id = " << codingId << "; lastFrame = " << lastFrameId << "; currFrame = " << currFrameId  << std::endl;
                  std::cerr << "Fatal error: missing firt frame ..." << std::endl;
                 // exit(1);
                 LibAv::Frame::WriteBlackYuvToFile(yuvOutput, format);
               }
               else
               {
                  lastFrame->WriteYuvToFile(yuvOutput, format);
               }
               ++lastFrameId;
            }
            frame_ptr->WriteYuvToFile(yuvOutput, format);
            lastFrame = frame_ptr;
            ++lastFrameId;
         }
      }
   }
   //Flush last frames
   auto frame_ptr = ovi.m_decoder->Flush();
   while(frame_ptr)
   {
      auto currFrameId = ovi.GetRealFrameId(frame_ptr);
      while (lastFrameId < currFrameId) //then their is missing frames
      {
         if (lastFrame == nullptr)
         {
            std::cerr << "Fatal error: missing firt frame (2) ..." << std::endl;
            //exit(1);
            LibAv::Frame::WriteBlackYuvToFile(yuvOutput, format);
         }
         else
         {
            lastFrame->WriteYuvToFile(yuvOutput, format);
         }
         ++lastFrameId;
      }
      frame_ptr->WriteYuvToFile(yuvOutput, format);
      lastFrame = frame_ptr;
      ++lastFrameId;
      frame_ptr = ovi.m_decoder->Flush();
   }
   //Write last frames if not there:
   while (lastFrameId < ovi.m_nbFrames)
   {
       if (lastFrame == nullptr)
       {
           std::cerr << "Fatal error: missing firt frame ..." << std::endl;
           exit(1);
       }
       lastFrame->WriteYuvToFile(yuvOutput, format);
       ++lastFrameId;
   }
}
