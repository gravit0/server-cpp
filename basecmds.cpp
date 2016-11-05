#include <boost-server.hpp>
using boostserver::Command;
using boostserver::service;
using namespace RecArrUtils;
bool initBaseCmds()
{
    Command* cmdtest=new Command();
    cmdtest->name="test";
    cmdtest->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result;
        result.add("key","1");
        client->do_write(toArcan(result));
    };
    Command* cmdsu=new Command();
    cmdsu->name="su";
    cmdsu->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        if(client->sock().remote_endpoint().address()==boostserver::endpoint->address())
        {
            client->permissionsLevel=5;
            RecursionArray result;
            result.add("key","1");
            client->do_write(toArcan(result));
            //client->do_write("Superuser Active\n");
        }
    };
    Command* cmdecho=new Command();
    cmdecho->name="echo";
    cmdecho->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->do_write(RecArrUtils::printTreeEx(args));
    };
    Command* cmdtelnet=new Command();
    cmdtelnet->name="telnet";
    cmdtelnet->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->isTelnetMode=!client->isTelnetMode;
        RecursionArray result;
        result.add("key","1");
        client->do_write(toArcan(result));
    };
    Command* cmdclients=new Command();
    cmdclients->name="clients";
    cmdclients->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result;
        result.add("result",std::to_string(service.clientlist.size()));
        result.add("key","1");
        client->do_write(toArcan(result));
    };
    Command* cmddisconnect=new Command();
    cmddisconnect->name="disconnect";
    cmddisconnect->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->stop();
    };
    service.cmdlist.push_back(cmdsu);
    service.cmdlist.push_back(cmdtest);
    service.cmdlist.push_back(cmdecho);
    service.cmdlist.push_back(cmdtelnet);
    service.cmdlist.push_back(cmdclients);
    service.cmdlist.push_back(cmddisconnect);
    return true;
}
