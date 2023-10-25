#include <iostream>

#include <vector>
#include <numeric>

#include <boost/filesystem.hpp>

#include "flow/wt_flow.h"
#include "flow/parse_ptl.h"

int main(int argc, char **argv)
{
    boost::filesystem::path basePath = "/home/mishnic/data/data_proc/shpere_cone_0";
    std::string coreName = "4462";

    boost::filesystem::path workDir = basePath/coreName;

    wt_flow::Flow flow;
    {
        std::string ptlFleName = coreName + ".ptl";
        boost::filesystem::path flowPtlFile = workDir/ptlFleName;

        //wt_flow::parsePTLfile(flowPtlFile.string(), flow, 6, 3);
        flow.setDynamicPressure(4098 * 9.80665);
        flow.setReynolds(16251398);
        flow.setT0(55.1560);
        flow.setMach(1.14210);
        
        flow.calculateVelocity();
        flow.calculateDensity();
    }
    
    
    {
        std::string flowFleName = coreName + ".flow";
        
        flow.saveFile(boost::filesystem::path(workDir/flowFleName).string());   
    }
}
