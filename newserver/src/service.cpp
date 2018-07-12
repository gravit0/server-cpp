#include "boost-server.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "config.h"
#include "protocol.h"
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
    threads.push_back(then);
    coutNewThreads++;
}
bool SrvControl::startdb(bool autofail)
{
    for(auto i = threads.begin();i!=threads.end();++i)
    {
        mythread* th=(*i);
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
    std::cout << LOCALTIME << "Stop Listening " << endpoint->address().to_string() << " " << endpoint->port() << "\n";
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
        then->isStart();
        //then->db.connect(config_mysql_host,config_mysql_port,config_mysql_login,config_mysql_password,config_mysql_dbname);
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
    std::cout << LOCALTIME << "All client kicked\n";
}

void SrvControl::autoCommand(MyCommand cmd)
{
    unsigned int maxmessages=0;
    std::cout << (void*)threads[0] << std::endl;
    mythread* thisthread = threads[0];
    maxmessages = threads[0]->commandsarray.size();
    //for(auto& i : threads)
    //{
    //    if(i->commandsarray.size() > maxmessages)
    //    {
    //        maxmessages=i->commandsarray.size();
    //        thisthread=i;
    //        if(maxmessages==0) break;
    //    }
    //}
    thisthread->commandsarray.push(cmd);
    if(maxmessages==0) thisthread->launch(false);
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
        ComandUse(me,thiscmd.data,thiscmd.size,thiscmd.clientptr);
        if(service.status == boostserver::SrvControl::_status::prestopping) {
            boostserver::ioservice.stop();
            break;
        }
        me->commandsarray.pop();
        if(thiscmd.clientptr->started())
            thiscmd.clientptr->do_read();
    }
}
using namespace Protocol;
int ComandUse(mythread* me,char* data, unsigned int size,boostserver::client::ptr client)
{
    std::cout << "Command 1" << std::endl;
    if(size < sizeof(message_head))
    {
        //logger->logg('W', "Command protocol error: 1");
        return -1;
    }
    std::cout << "Command 2" << std::endl;
    message_head* head = (message_head*)data;
    if(size != sizeof(message_head) + head->size)
    {
        //logger->logg('W', "Command protocol error: 2");
        return -1;
    }
    std::cout << "Command 3" << std::endl;
    std::string cmd(data + sizeof(message_head), head->size);
    //printf("Client sent: %s\n", rsock->buf);
    //lpfunc(head,cmd, rsock);
    Command* execcmd = nullptr;
    std::cout << "Command " << std::hex << head->cmd << std::endl;
    for(auto &i : service.cmdlist)
        if(i->uuid == head->cmd)
        {
            execcmd = i;
            break;
        }
    if(execcmd == nullptr)
    {
        //logger->logg('W', "Command protocol error: unsupported command");
        return -1;
    }
    std::cout << "Command 4" << std::endl;
    client->write_buffer_writted = sizeof(message_result);
    Context context{client,me,head->cmdflags,client->write_buffer,&client->write_buffer_writted,0,head->session};
    auto result = execcmd->func(context,cmd);
    if(!result) {
        client->do_write();
    }
    else
    {
        message_result rresult{0,0,0,0,context.session /*session*/,*result};
        client->do_write({&rresult,sizeof(rresult)});
    }
    return 0;
}
SrvControl::~SrvControl()
{
    savelog();
    for(auto& i : connects) delete i;
    for(auto& i : threads) delete i;
    connects.clear();
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
