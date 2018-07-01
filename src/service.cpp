#include "boost-server.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "config.h"
#ifndef NOLOGTIME
#define LOCALTIME "[" << boost::posix_time::microsec_clock::local_time() << "] "
#else
#define LOCALTIME " "
#endif
namespace boostserver
{
void SrvControl::newThread(bool startdb)
{
    mythread* then = new mythread();
    //then->db.connect(config_mysql_host,config_mysql_port,config_mysql_login,config_mysql_password,config_mysql_dbname);
    if(startdb) then->db.connect(config_mysql_dbname.c_str(),config_mysql_host.c_str(),config_mysql_login.c_str(),config_mysql_password.c_str(),config_mysql_port);
    threads.push_back(then);
    coutNewThreads++;
}
bool SrvControl::startdb(bool autofail)
{
    for(auto i = threads.begin();i!=threads.end();++i)
    {
        mythread* th=(*i);
        if(th->db.connected() && autofail) return false;
        th->db.connect(config_mysql_dbname.c_str(),config_mysql_host.c_str(),config_mysql_login.c_str(),config_mysql_password.c_str(),config_mysql_port);
    }
    return true;
}
void SrvControl::savelog()
{
    logs.file << logs.stream.str();
    logs.file.flush();
    logs.stream.clear();
}
ServerConnect::~ServerConnect()
{
    logs << LOCALTIME << "Stop Listening " << endpoint->address().to_string() << " " << endpoint->port() << "\n";
    delete endpoint;
    delete acceptor;
}
SrvControl::SrvControl()
{
    isCoutMode=false;
    isDebug=false;
}

void SrvControl::newThreads(int threadsd,bool startdb)
{
    for(int  i=0;i<threadsd;++i)
    {
        mythread* then = new mythread();
        //then->db.connect(config_mysql_host,config_mysql_port,config_mysql_login,config_mysql_password,config_mysql_dbname);
        if(startdb) then->db.connect(config_mysql_dbname.c_str(),config_mysql_host.c_str(),config_mysql_login.c_str(),config_mysql_password.c_str(),config_mysql_port);
        threads.push_back(then);
    }
    coutNewThreads+=threadsd;
}
void SrvControl::addThread(mythread* thend)
{
    threads.push_back(thend);
    coutAddThreads++;
}
void SrvControl::cmdsclear()
{
    for(auto i =cmdlist.begin();i!=cmdlist.end();++i)
    {
        Command* ptr =(*i);
        delete ptr;
    }
    cmdlist.clear();
}
void SrvControl::closeclients()
{
    for(auto i =clientlist.begin();i!=clientlist.end();++i)
    {
        (*i)->stop();
    }
    logs << LOCALTIME << "All client kicked\n";
}

void SrvControl::autoCommand(MyCommand cmd)
{
    unsigned int maxmessages=0;
    auto i = threads.begin(), thisthread = threads.begin();
    maxmessages=(*thisthread)->commandsarray.size();
    for(++i;i!=threads.end();++i)
    {
        if((*i)->commandsarray.size() > maxmessages)
        {
            maxmessages=(*i)->commandsarray.size();
            thisthread=i;
            if(maxmessages==0) break;
        }
    }
    (*thisthread)->commandsarray.push(cmd);
    if(maxmessages==0) (*thisthread)->launch(false);
}
bool mythread::isStart() const
{
    return _isStart;
}

void mythread::launch(bool isJoin)
{
    _isStart=true;
    threadptr=new std::thread(run,this);
    if(isJoin)
        threadptr->join();
    else
        threadptr->detach();
}
void mythread::run(mythread* me)
{
    while(me->commandsarray.size()>0)
    {
        MyCommand thiscmd=me->commandsarray.front();
        ComandUse(me,thiscmd.cmd,thiscmd.clientptr);
        me->commandsarray.pop();
        if(thiscmd.clientptr->started())
            thiscmd.clientptr->do_read();
    }
}
void ComandUse(mythread* me,std::string thiscmd,boostserver::client::ptr client)
{
    RecursionArray arr=RecArrUtils::fromArcan(thiscmd);
    if(!thiscmd.empty())
    {
        std::string cmdname=arr.get<std::string>("type","");
        if(cmdname.empty())
        {
            int cmdsize=thiscmd.size();
            if(cmdsize>2)
            {
                if(thiscmd.at(cmdsize-1)=='\n' && thiscmd.at(cmdsize-2)=='\r')
                    cmdname=thiscmd.substr(0,cmdsize-2);
                else
                    cmdname=thiscmd;
            }
            else return;
        }
        for(auto i = boostserver::service.cmdlist.begin();i!=boostserver::service.cmdlist.end();++i)
        {
            if((*i)->uuid==0) //TODO: FIX
            {
                if(client->permissionsLevel>=(*i)->minPermissions)
                    (*i)->func(me,(*i),arr,client);
                else
                {
                    if(client->isTelnetMode) client->do_write("Not Permissions");
                    else
                    {
                        RecursionArray result;
                        result.add("key",replys.NotPermissions);
                        client->do_write(RecArrUtils::toArcan(result));
                    }
                }
                if(client->isTelnetMode)
                {
                    if(client->permissionsLevel==5)client->do_write("\n#");
                    else client->do_write("\n$");
                }
                return;
            }
        }
    }
    if(client->isTelnetMode)
    {
        client->do_write("Command not found");
        if(client->permissionsLevel==5)client->do_write("\n#");
        else client->do_write("\n$");
    }
    else
    {
        RecursionArray result;
        result.add("key",replys.NotCommand);
        client->do_write(RecArrUtils::toArcan(result));
    }
}
SrvControl::~SrvControl()
{
    savelog();
}
bool SrvControl::addCommand(Command* cmd)
{
    cmdlist.push_back(cmd);
    return true;
}

mythread::~mythread()
{
    if(_isStart)
        delete threadptr;
}
}
