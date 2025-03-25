#include <iostream>

#include <string>
#include <sstream>
#include <vector>

namespace function_generator
{

    class FunctionProbeData
    {
    public:
        FunctionProbeData() : m_function(),
                              m_time0(0),
                              m_length(0),
                              m_dt(0)
        {
            auto function = [](double argument, std::vector<double> coeff)
            {
                return 0.0;
            };

            m_function = function;
        }

        FunctionProbeData(double (*function)(double, std::vector<double>),
                          double time0,
                          size_t length,
                          double dt,
                          std::vector<double> coeff = std::vector<double>()) : m_function(function),
                                                                               m_time0(time0),
                                                                               m_length(length),
                                                                               m_dt(dt),
                                                                               m_coeff(coeff)

        {
        }

        ~FunctionProbeData()
        {
            m_functionDescription.clear();
        }

        std::string getProcInfo() const
        {
            return m_procInfo;
        }

        void setFileName(const std::string &fileName)
        {
            m_fileName = fileName;
        }

        void setFunction(double (*function)(double, std::vector<double>))
        {
            m_function = function;
        }

        void setFunctionDescription(const std::string descr)
        {
            m_functionDescription = descr;
        }

        void setFunction(double (*function)(double, std::vector<double>), const std::string descr)
        {
            m_function = function;
            m_functionDescription = descr;
        }

        void setData(double (*function)(double, std::vector<double>),
                            const std::string& functionDescription, 
                            double time0,
                            size_t length,
                            double dt,
                            std::vector<double> coeff = std::vector<double>())
        {
            this->setFunctionDescription(functionDescription);
            this->setData(function, time0, length, dt, coeff);
        }

        void setData(double (*function)(double, std::vector<double>),
                     double time0,
                     size_t length,
                     double dt,
                     std::vector<double> coeff = std::vector<double>())
        {           

            m_function = function;
            m_time0 = time0;
            m_length = length;
            m_dt = dt;
            m_coeff = coeff;
        }

        void setData(double time0,
                     size_t length,
                     double dt,
                     std::vector<double> coeff = std::vector<double>())
        {
            m_time0 = time0;
            m_length = length;
            m_dt = dt;
            m_coeff = coeff;
        }

        void setData(const std::string &fileName,
                     double time0,
                     size_t length,
                     double dt,
                     std::vector<double> coeff = std::vector<double>())
        {
            m_fileName = fileName;
            m_time0 = time0;
            m_length = length;
            m_dt = dt;
            m_coeff = coeff;
        }

        void setDataCoeff(std::vector<double> coeff)
        {
            m_coeff = coeff;
        }

        void setDataCoeff(const std::string &fileName, std::vector<double> &coeff)
        {
            m_fileName = fileName;
            m_coeff = coeff;
        }

        void setDataTime0(const std::string &fileName, double time0)
        {
            m_fileName = fileName;
            m_time0 = time0;
        }

        void setDataTime0(double time0)
        {
            m_time0 = time0;
        }

        std::string m_fileName;
        std::string m_functionDescription;

        double (*m_function)(double, std::vector<double>);

        double m_time0;
        size_t m_length;
        double m_dt;
        std::vector<double> m_coeff;
        std::string m_procInfo;
    };

} // namespace function_generator
