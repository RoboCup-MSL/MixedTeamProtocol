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
        std::string t("ALL");
        if (command.target().compare(_myteam) == 0)
        {
            t = "US";
        }
        else if (command.target().compare("") != 0)
        {
            t = "THEM";
        }
        cout << "command: " << c << "; target: " << t << endl;
        _rtdb.put("TARGETTEAM", &t);
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
    std::string dbpath;
    std::string dbname;
    std::string network;
    bool simulation;
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("config", po::value<std::string>(&config)->default_value("rtdb2_refbox.xml"), "rtdb configuration file")
        ("host", po::value<std::string>(&host)->default_value("127.0.0.1"), "refbox host")
        ("network", po::value<std::string>(&network)->default_value("refbox"), "refbox network name")
        ("dbpath", po::value<std::string>(&dbpath)->default_value(RTDB2_DEFAULT_PATH), "database storage directory")
        ("dbname", po::value<std::string>(&dbname)->default_value(""), "database name")
        ("simulation", po::value<bool>(&simulation)->default_value(false), "simulation mode: do not start comm")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    
    RtDB2Context::Builder ctxBuilder(0, (simulation ? RtDB2ProcessType::dbclient : RtDB2ProcessType::comm));
    ctxBuilder.withConfigFileName(config);
    ctxBuilder.withNetwork(network);
    if (dbpath.size()) ctxBuilder.withRootPath(dbpath);
    if (dbname.size()) ctxBuilder.withDatabase(dbname);
    RtDB2Context ctx = ctxBuilder.build();

    Comm comm(ctx);
    if (!simulation)
    {
        comm.start();
    }

    RefboxProtocol2020Client c;
    Listener l(ctx);
    c.connect(host, 28097);
    c.listen(&l);

    return 0;
}
