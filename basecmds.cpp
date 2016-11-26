#include <boost-server.hpp>
#include "config.h"
#include <accountutils.h>
using boostserver::Command;
using boostserver::service;
using boostserver::client;
using boostserver::mythread;
using namespace RecArrUtils;

#define InitCmd(cmdname,varname,funcd) Command* varname=new Command(); \
    varname->name=cmdname; \
    cmdtest->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client) funcd ; \
    service.cmdlist.push_back(varname)
bool initBaseCmds()
{
    Command* cmdclients=new Command();
    cmdclients->name="clients";
    cmdclients->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result;
        result.add("count",std::to_string(service.clientlist.size()));
        result.add("key","1");
        if(client->permissionsLevel>=3)
        {
            int i=0;
            RecursionArray result2;
            for(auto it = service.clientlist.begin();it!=service.clientlist.end();++it)
            {
                result2.add(std::to_string(i),(*it)->sock().remote_endpoint().address().to_string());
            }
            result.add_child("clients",result2);
        }
        client->do_write(toArcan(result));
    };
    Command* cmdsu=new Command();
    cmdsu->name="su";
    cmdsu->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        if(client->sock().remote_endpoint().address()==boostserver::thisConnect.endpoint->address())
        {
            client->permissionsLevel=5;
            RecursionArray result;
            result.add("key","1");
            client->do_write(toArcan(result));
            //client->do_write("Superuser Active\n");
        }
    };
    Command* cmddisconnect=new Command();
    cmddisconnect->name="disconnect";
    cmddisconnect->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->stop();
    };
    Command* cmdgetevent=new Command();
    cmdgetevent->name="getevent";
    cmdgetevent->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result;
        result.add("key","1");
        int i=0;
        for(auto it = client->events.begin();it!=client->events.end();++it)
        {
            RecursionArray result2;
            result2.add("text",(*it).text);
            if((*it).isReaded)
            {
                result2.add("status","1");

            }
            else
            {
                result2.add("status","0");
                (*it).isReaded=true;
            }
            result.add_child(std::to_string(i),result2);
            ++i;
        }
        if(args.get<std::string>("clean","true")!="false")
        {
            client->events.clear();
        }
        client->do_write(toArcan(result));
    };
    Command* cmdseteventmode=new Command();
    cmdseteventmode->name="seteventmode";
    cmdseteventmode->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        client->isPassiveMode=!client->isPassiveMode;
        ReturnCode(OK);
    };
    Command* cmdversion=new Command();
    cmdversion->name="version";
    cmdversion->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result;
        result.add("server.version",SERVER_VERSION);
        result.add("server.info",SERVER_TYPE);
        if(client->permissionsLevel>=5)
        {
            result.add("server.threads",service.threads.size());
            result.add("server.clients",service.clientlist.size());
        }
        client->do_write(RecArrUtils::toArcan(result));
    };
    Command* cmdauth=new Command();
    cmdauth->name="auth";
    cmdauth->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        std::string login=args.get<std::string>("login","");
        for(int i=login.find("'");i>0 && i< login.size();i=login.find("'",i+1))
        {
            login.replace(i,1,"\\'");
            i++;
        }
        std::string passwd=args.get<std::string>("pass","");
        for(int i=passwd.find("'");i>0 && i< passwd.size();i=passwd.find("'",i+1))
        {
            passwd.replace(i,1,"\\'");
            i++;
        }
        if(AccountUtils::auth(client,login,passwd,&(me->db)))
        {
            ReturnCode(OK);
        }
        else
        {
            ReturnCode(Fail);
        }
    };
    Command* cmdplugins=new Command();
    cmdplugins->name="plugins";
    cmdplugins->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client)
    {
        RecursionArray result,result2;
        result.add("key","1");
        result.add("count",std::to_string(service.cmdlist.size()));
        int i=0;
        for(auto it = service.cmdlist.begin();it!=service.cmdlist.end();++it)
        {
            result2.add(std::to_string(i),(*it)->name);
            ++i;
        }
        result.add_child("plugins",result2);
        client->do_write(toArcan(result));
    };
    service.cmdlist.push_back(cmdsu);
    service.cmdlist.push_back(cmdauth);
    service.cmdlist.push_back(cmdclients);
    service.cmdlist.push_back(cmddisconnect);
    service.cmdlist.push_back(cmdgetevent);
    service.cmdlist.push_back(cmdseteventmode);
    service.cmdlist.push_back(cmdplugins);
    service.cmdlist.push_back(cmdversion);
    return true;
}
#undef InitCmd
