#include <iostream>
#include "RefboxCommand.h"
#include "RefboxProtocol2020Client.h"

using namespace std;
using namespace rbc;

class Listener : public RefBoxCallback
{
    virtual void handleCommand(RefboxCommand& command) {
        cout << "command: " << command.command() << "; target: " << command.target() << endl;
    }
};

int main()
{
    RefboxProtocol2020Client c;
    Listener l;
    c.connect("127.0.0.1", 28097);
    c.listen(&l);

    return 0;
}
