/**********************************
 * Institut Mine-Telecom / Telecom Bretagne
 * Author: Xavier CORBILLON
 *
 * Simulate loss in a HEVC video
 */

 #include "boost/program_options.hpp"
 #include <boost/config.hpp>

 #include <boost/property_tree/ptree.hpp>
 // #include <boost/property_tree/ini_parser.hpp>
 // #include <boost/property_tree/json_parser.hpp>
 // #include <boost/foreach.hpp>

#include "VideoReader.hpp"
#include "VideoWriter.hpp"
#include "resultReader.hpp"
#include "remuxerFunction.hpp"

#include <iostream>
#include <memory>

 using namespace IMT;

 int main( int argc, const char* argv[] )
 {
    namespace po = boost::program_options;
    namespace pt = boost::property_tree;
    po::options_description desc("Options");
    desc.add_options()
       ("help,h", "Produce this help message")
       ("inputVideo,i", po::value<std::string>(), "path to the input video")
       ("config,c", po::value<std::string>(),"Path to the list of received video frame. If not present will keep all frames")
       ("outputVideo,o", po::value<std::string>(), "path to the output video")
       ("format,f", po::value<std::string>(), "video resolution (for instance 1920x1080)")
       ("maxNbFrames,m", po::value<unsigned int>(), "maximum number of frame")
       ;

    po::variables_map vm;
    try
    {
       po::store(po::parse_command_line(argc, argv, desc),
             vm);

       //--help
       if ( vm.count("help") || !(vm.count("inputVideo") && vm.count("outputVideo") && vm.count("format") && vm.count("maxNbFrames")))
       {
          std::cout << "Help: trans -c config"<< std::endl
             <<  desc << std::endl;
          return 0;
       }

       po::notify(vm);

       auto pathToInputVideo = vm["inputVideo"].as<std::string>();
       auto pathToOutputVideo = vm["outputVideo"].as<std::string>();
       auto pathToSolution = vm.count("config") ? vm["config"].as<std::string>() : std::string();
       auto format = vm["format"].as<std::string>();
       unsigned int maxNbFrames = vm["maxNbFrames"].as<unsigned int>();

       std::cout << "Start remuxing for " << pathToSolution << " "
                 << pathToInputVideo << ". Result stored in "
                 << pathToOutputVideo << std::endl;

       std::shared_ptr<Remuxer::ResultReader> rr;
       rr = pathToSolution.size() > 0 ? std::make_shared<Remuxer::ResultReader>(pathToSolution) : std::make_shared<Remuxer::ResultReader>();
       rr->Init();

       IMT::OriginalVideoInfo ovi;
       std::cout << "Generate the corresponding YUV file: " << pathToOutputVideo << std::endl;
       std::cout << "1. Read the original video" << std::endl;
       IMT::OriginalVideoReader(ovi, pathToInputVideo);
       std::cout << "2. Generate the YUV file" << std::endl;
       IMT::RemuxeToYuv(ovi, pathToOutputVideo, format, rr, maxNbFrames);
       std::cout << "Done" << std::endl;
    }
    catch(const po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl
         << desc << std::endl;
      return 1;
    }
    catch(std::exception& e)
    {
      std::cerr << "Uncatched exception: " << e.what() << std::endl
         << desc << std::endl;
      return 1;

    }
    catch(...)
    {
      std::cerr << "Uncatched exception" << std::endl
        << desc << std::endl;
      return 1;

    }

    return 0;
}
