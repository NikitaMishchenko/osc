#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <string>

#include <boost/filesystem.hpp>

#include "flow/wt_flow.h"
#include "flow/ptl_flow.h"

void parseAndSaveFlow(const boost::filesystem::path& ptlFlowFile)
{
    std::vector<wt_flow::Flow> flowVector = parsePtlFile(ptlFlowFile.string() + ".ptl");

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
    
    for (int i = 4461; i < 4475; i++)
    {
        std::string coreName = std::to_string(i); //"4474";

        boost::filesystem::path ptlFlowFile = basePath / coreName;

        parseAndSaveFlow(ptlFlowFile);
    }

}
