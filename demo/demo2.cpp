#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "flow/wt_flow.h"
#include "flow/ptl_flow.h"

void parseAndSaveFlow(const boost::filesystem::path &ptlFlowFile)
{
    std::cout << "parsing " << ptlFlowFile << std::endl;

    std::vector<wt_flow::Flow> flowVector;

    try
    {
        flowVector = parsePtlFile(ptlFlowFile.string() + ".ptl");
    }
    catch(...)
    {
        std::cerr << "failed to parse " << ptlFlowFile << " skipping\n";
        return;
    }

    std::cout << "parsed size: " << flowVector.size() << std::endl;

    {
        std::ofstream fout(ptlFlowFile.string() + "_parsed_flow");

        for (const auto &flow : flowVector)
        {
            fout << flow.getDensity() << " "
                 << flow.getDynamicPressure() << " "
                 << flow.getVelocity() << " "
                 << flow.getTemperature() << " "
                 << flow.getMach() << " "
                 << flow.getReynolds() << "\n";
        }
    }

    wt_flow::Flow flowCalculated(averageFLowData(flowVector));

    {
        flowCalculated.saveFile(ptlFlowFile.string() + ".flow");
    }
}

int main(int argc, char **argv)
{
    boost::filesystem::path basePath = "/home/mishnic/data/data_proc/sphere_cone_M1.75/flow_ptl/";

    std::vector<boost::filesystem::path> ptlFiles;
    boost::regex regexPtl("(.*\\.ptl)");

    // for (int i = 4461; i < 4475; i++)
    for (boost::filesystem::directory_iterator it(basePath); it != boost::filesystem::directory_iterator(); ++it)
    {
        if (boost::regex_search(it->path().string(), regexPtl))
        {
            std::cout << *it << "\n";
            ptlFiles.push_back(it->path());
        }
    }

    for (const auto& ptlFile : ptlFiles)
    {
        // std::string coreName = std::to_string(i); //"4474";

        // boost::filesystem::path ptlFlowFile = basePath / coreName;
        std::string fileName(ptlFile.string());
        fileName = std::string(fileName.begin(), fileName.end() - std::string(".ptl").size());
        parseAndSaveFlow(fileName);
    }

    std::cout << "Finished\n";
}
