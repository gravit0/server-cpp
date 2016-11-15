#include <boost-server.hpp>
using boostserver::Command;
using boostserver::service;
using boostserver::client;
using boostserver::mythread;
using namespace RecArrUtils;
#define InitCmd(cmdname,varname,funcd) Command* varname=new Command(); \
    varname->name=cmdname; \
    cmdtest->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client) funcd ; \
    service.cmdlist.push_back(varname)
bool initTestCmds()
{
    Command* cmdtelnet=new Command();
    cmdtelnet->name="telnet";
    cmdtelnet->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->isTelnetMode=!client->isTelnetMode;
        RecursionArray result;
        result.add("key","1");
        client->do_write(toArcan(result));
    };
    Command* cmdtestevent=new Command();
    cmdtestevent->name="testevent";
    cmdtestevent->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->newEvent("test event");
        RecursionArray result;
        result.add("key","1");
        client->do_write(toArcan(result));
    };
    Command* cmdecho=new Command();
    cmdecho->name="echo";
    cmdecho->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->do_write(RecArrUtils::printTreeEx(args));
    };

    Command* cmdtest=new Command();
    cmdtest->name="test";
    cmdtest->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result;
        result.add("key","1");
        client->do_write(toArcan(result));
    };
    service.cmdlist.push_back(cmdtest);
    service.cmdlist.push_back(cmdtelnet);
    service.cmdlist.push_back(cmdecho);
    service.cmdlist.push_back(cmdtestevent);
    return true;
}
#undef InitCmd