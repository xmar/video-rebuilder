/************************************
 * Institut Mine Telecom / Telecom Bretagne
 * Author: Xavier CORBILLON
 *
 * Class used to read the result file from the solver and store the id of frames that have been received
 */
#pragma once

#include <string>
#include <queue>

namespace IMT {
namespace Remuxer {

class ResultReader
{
    public:
        ResultReader (const std::string& inputFileName);
        ResultReader (void);//to keep all

        void Init(void);

        bool IsFrameReceived(const unsigned int id);
    private:
        const std::string m_inputFileName;
        std::queue<int> m_rxFrameIds;
        bool m_keepAll;
};

}
}
