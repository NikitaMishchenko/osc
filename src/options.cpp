#include "options.h"

namespace options
{
    bool Options::parse_options(int ac, char* av[])
    {
        po::store(po::command_line_parser(ac, av).options(*descr).run(), m_vm);
        po::notify(m_vm);

        if(exist("help"))
        {
            //todo
            //show_help();
            return true;
        }

        return true;
    }
}
