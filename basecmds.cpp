#include <abstractcommand.h>
list<Command*> _cmds;
class cmdTest : public Command
{
public:
    void operator ()(std::list<std::string>* args,boostserver::client::ptr client)
    {
        client->do_write("Test\n");
    }
};

bool initBaseCmds()
{
    cmdTest* cmdtest = new cmdTest();
    cmdtest->name="test\r\n";
    _cmds.push_back(cmdtest);
    return true;
}
