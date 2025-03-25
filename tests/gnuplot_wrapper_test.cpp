#include <gtest/gtest.h>

//#include <gnuplot_wrapper.h>
//#include <gnuplot/gnuplot_wrapper.h>
//#include <gnuplot/data/data.h>
//#include <gnuplot/gnuplot_1d.h>

/*TEST(TestGnuplot, BasicTest)
{
    {
        const size_t size = 100;

        gnuplot::Gnuplot1d gnuplot1d;
        std::vector<double> arg, func;

        gnuplot::png::PngProperties pngProperties(640, 480);

        gnuplot::png::PngFile pngFile("testPic");

        pngFile.setPngProperties(pngProperties);

        // gnuplot1d.setPngFile(pngFile);

        {
            arg.clear();
            func.clear();

            arg.reserve(size);
            func.reserve(size);

            for (size_t i = 0; i < size; i++) // todo wrap to esasy to use way
            {
                arg.push_back(i * 0.1);
                func.push_back(10 * arg.back());
            }

            gnuplot::DataFunction dataFunction(arg, func);

            dataFunction.setTitle("10*x");
            dataFunction.setLineColor(gnuplot::RED);
            dataFunction.setLineType(gnuplot::LINES);

            gnuplot1d.addDataToPlot(dataFunction);
        }

        {
            arg.clear();
            func.clear();

            arg.reserve(size);
            func.reserve(size);

            for (size_t i = 0; i < size; i++)
            {
                arg.push_back(i * 0.1);
                func.push_back(arg.back() * arg.back() + arg.back());
            }

            gnuplot::DataFunction dataFunction(arg, func);

            dataFunction.setTitle("x**2+x");
            dataFunction.setLineColor(gnuplot::BLACK);
            dataFunction.setLineType(gnuplot::LINES_POINTS);

            gnuplot1d.addDataToPlot(dataFunction);
        }

        {
            std::string data = "100*sin(x)";
            gnuplot::DataGnuplotFunction dataGnuplotFunction(data);

            // dataGnuplotFunction.setTitle("100*sin(x)");

            gnuplot1d.addDataToPlot(dataGnuplotFunction);
        }

        gnuplot1d.setPlotTitle("testTitle");
        gnuplot1d.act1dVector();
    }
}*/