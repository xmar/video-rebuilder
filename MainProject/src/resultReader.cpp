/************************************
 * Institut Mine Telecom / Telecom Bretagne
 * Author: Xavier CORBILLON
 *
 * Class used to read the result file from the solver and store the id of frames that have been received
*/

#include "resultReader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace IMT::Remuxer;

ResultReader::ResultReader(const std::string& inputFileName): m_inputFileName(inputFileName), m_rxFrameIds(), m_keepAll(false)
{}

ResultReader::ResultReader(void): m_inputFileName(), m_rxFrameIds(), m_keepAll(true)
{}

void ResultReader::Init(void)
{
    if (m_rxFrameIds.empty() && !m_keepAll)
    {
        std::ifstream inputFile(m_inputFileName);
        std::string line;
        //skipe first line
        std::getline(inputFile, line);
        int id;
        while(std::getline(inputFile, line))
        {
            std::stringstream ss(line);
            ss >> id; //hypothesis -> first element is an integer
            m_rxFrameIds.push(id);
        }
        std::cout << "There are " << m_rxFrameIds.size() << " received frames."<< std::endl;
    }
}

bool ResultReader::IsFrameReceived(const unsigned int id)
{
    if (!m_rxFrameIds.empty() && id == m_rxFrameIds.front())
    {
        m_rxFrameIds.pop();
        return true;
    }
    else if (id == 0 || m_keepAll)//we always keep the frameid 0
    {
       return true;
    }
    else
    {
        return false;
    }
}
