#include <iostream>
#include "RefboxCommand.h"
#include "RefboxProtocol2020Client.h"
#include "RtDB2.h"
#include "comm.hpp"
#include "boost/program_options.hpp"

using namespace std;
using namespace rbc;

namespace po = boost::program_options;

class Listener : public RefBoxCallback
{

    virtual void handleCommand(RefboxCommand &command)
    {
        std::string c(command.command());
        if (_myteam.compare("undefined") == 0 && c.compare("WELCOME") == 0)
        {
            _myteam = command.target();
        }
        std::string t("all");
        if (command.target().compare(_myteam) == 0)
        {
            t = "us";
        }
        else if (command.target().compare("") != 0)
        {
            t = "them";
        }
        cout << "command: " << c << "; target: " << t << endl;
        _rtdb.put("TARGET", &t);
        _rtdb.put("COMMAND", &c);
    }

public:
    Listener(RtDB2Context &ctx) : _rtdb(ctx), _myteam("undefined")
    {
    }

private:
    RtDB2 _rtdb;
    std::string _myteam;
};

int main(int argc, char **argv)
{
    std::string host;
    std::string config;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")("config", po::value<std::string>(&config)->default_value("rtdb2_refbox.xml"), "rtdb configuration file")("host", po::value<std::string>(&host)->default_value("127.0.0.1"), "refbox host");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    RtDB2Context ctx = RtDB2Context::Builder(0, RtDB2ProcessType::comm)
                           .withConfigFileName(config)
                           .withNetwork("refbox")
                           .build();

    Comm comm(ctx);
    comm.start();

    RefboxProtocol2020Client c;
    Listener l(ctx);
    c.connect(host, 28097);
    c.listen(&l);

    return 0;
}
