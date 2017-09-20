#pragma once

#include "VideoReader.hpp"
#include "Packet.hpp"
#include "resultReader.hpp"
#include <iostream>
#include <string>
#include <vector>


namespace IMT {

   struct OriginalVideoInfo {

      OriginalVideoInfo(void): m_nbFrames(0), m_lastI(0), m_maxDistanceBetweenTwoI(0), m_decoder(nullptr) {}
      ~OriginalVideoInfo(void) = default;

      void AddNextPacket(std::shared_ptr<LibAv::Packet> pkt_ptr) { m_packetVec.push_back(pkt_ptr);}
      void AddNextFrame(std::shared_ptr<LibAv::Frame> frame)
      {
         auto codingId =  frame->GetCodedId();
         auto frameId = frame->GetFrameId();
         auto uniqueId = frame->GetUniqueId();

         m_uniqueIdToFrameId[uniqueId] = frameId-1;
         m_uniqueIdToCodingId[uniqueId] = codingId;
         m_codingIdToUniqueId[codingId] = uniqueId;
         if (frame->GetFrameType() == LibAv::Frame::FrameType::B)
         {
            m_uniqueIdToFrameType[uniqueId] = 3;
         }
         else if (frame->GetFrameType() == LibAv::Frame::FrameType::P)
         {
            m_uniqueIdToFrameType[uniqueId] = 2;
         }
         else
         {
            m_uniqueIdToFrameType[uniqueId] = 1;
            m_storeIcodingId.push_back(codingId);
            long distance = codingId - m_lastI;
            m_maxDistanceBetweenTwoI = std::max(distance, m_maxDistanceBetweenTwoI);
            m_lastI = codingId;
         }
         m_uniqueIdToFrameSize[uniqueId] = frame->GetFramePktSize();

         ++m_nbFrames;
      }

      unsigned int GetRealCodingId(std::shared_ptr<LibAv::Frame> frame) { return m_uniqueIdToCodingId[frame->GetUniqueId()]; }
      unsigned int GetRealFrameId(std::shared_ptr<LibAv::Frame> frame)  { return m_uniqueIdToFrameId[frame->GetUniqueId()]; }
      unsigned int CodingIdToFrameId(unsigned int i) { return m_uniqueIdToFrameId[m_codingIdToUniqueId[i]];}
      unsigned int CodingIdToFrameType(unsigned int i) { return m_uniqueIdToFrameType[m_codingIdToUniqueId[i]];}
      unsigned int CodingIdToFrameSize(unsigned int i) { return m_uniqueIdToFrameSize[m_codingIdToUniqueId[i]];}

      std::vector<std::shared_ptr<LibAv::Packet>> m_packetVec;
      std::map<unsigned int,unsigned int> m_uniqueIdToFrameId;
      std::map<unsigned int,unsigned int> m_codingIdToUniqueId;
      std::map<unsigned int,unsigned int> m_uniqueIdToCodingId;
      std::map<unsigned int,unsigned int> m_uniqueIdToFrameType;
      std::map<unsigned int,unsigned int> m_uniqueIdToFrameSize;
      std::vector<unsigned int> m_storeIcodingId;
      unsigned int m_nbFrames;
      unsigned int m_lastI;
      long m_maxDistanceBetweenTwoI;
      std::shared_ptr<LibAv::VideoCodec> m_decoder;
      std::shared_ptr<LibAv::VideoReader> m_vr;
   };

   void OriginalVideoReader(OriginalVideoInfo& ovi, std::string videoPath);
   void RemuxeToYuv(OriginalVideoInfo& ovi, std::string videoOutputPath, std::string format, std::shared_ptr<Remuxer::ResultReader> rr, unsigned int maxNbFrame);
}
