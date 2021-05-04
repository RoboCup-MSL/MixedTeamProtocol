#include <iostream>
#include "RefboxCommand.h"
#include "RefboxProtocol2020Client.h"
#include "RtDB2.h"
#include "comm.hpp"

using namespace std;
using namespace rbc;

class Listener : public RefBoxCallback
{
    virtual void handleCommand(RefboxCommand& command) {
        std::string c(command.command());
        cout << "command: " << command.command() << "; target: " << command.target() << endl;
        _rtdb.put("COMMAND", &c);
    }

    public:
        Listener(RtDB2Context& ctx) : _rtdb(ctx) {
        }

    private:
        RtDB2 _rtdb;
};

int main()
{
    RtDB2Context ctx = RtDB2Context::Builder(0, RtDB2ProcessType::comm)
                        .withConfigFileName("rtdb2_refbox.xml")
                        .withNetwork("refbox")
                        .build();

    Comm comm(ctx);
    comm.start();

    RefboxProtocol2020Client c;
    Listener l(ctx);
    c.connect("127.0.0.1", 28097);
    c.listen(&l);

    return 0;
}
