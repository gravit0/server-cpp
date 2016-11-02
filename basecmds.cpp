#include <abstractcommand.h>
list<Command*> _cmds;
bool initBaseCmds()
{
    Command* cmdtest=new Command();
    cmdtest->name="test";
    cmdtest->func=[](Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->do_write("Test\n");
    };
    Command* cmdsu=new Command();
    cmdsu->name="su";
    cmdsu->func=[](Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        if(client->sock().remote_endpoint().address()==boostserver::ep.address())
        {
            client->permissionsLevel=5;
            client->do_write("Superuser Active\n");
        }
    };
    Command* cmdecho=new Command();
    cmdsu->name="echo";
    cmdsu->func=[](Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->do_write(RecArrUtils::printTreeEx(args));
    };
    _cmds.push_back(cmdsu);
    _cmds.push_back(cmdtest);
    _cmds.push_back(cmdecho);
    return true;
}
