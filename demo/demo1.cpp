#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "oscillation/wt_oscillation.h"

int main(int argc, char** argv)
{
    boost::filesystem::path root     = "/home/mishnic/data/phd";
    

    boost::filesystem::path fileName = "sphere_conde_M1.75";
    
    boost::filesystem::path fileToProceed = root.string() + std::string("/sphere_cone_M1.75");
    
    std::cout << fileToProceed << "\n";

    AngleHistory angleHistory(fileToProceed.string() + "/4463");

    Oscillation oscillation;

    
}