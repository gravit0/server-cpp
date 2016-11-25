#include <boost-server.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "config.h"
namespace boostserver
{
SrvControl service;
boost::asio::io_service ioservice;
#define MEM_FN(x) boost::bind(&client::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&client::x, shared_from_this(),y)
#define MEM_FN2(x,y,z) boost::bind(&client::x, shared_from_this(),y,z)
#define LOCALTIME "[" << boost::posix_time::microsec_clock::local_time() << "] "
asio::ip::tcp::endpoint* endpoint;
asio::ip::tcp::acceptor* acceptor;
client::client() : mysocket(ioservice), isStarted(false)
{
    read_buffer = new char[max_msg];
    write_buffer = new char[max_msg];
    read_buffer_size = max_msg;
    write_buffer_size = max_msg;
    permissionsLevel=0;
    isAuth=false;
    isTelnetMode=false;
    isPassiveMode=false;
}
Command::~Command()
{
    std::cout << "Комманда " << name << " выгружена" << std::endl << std::flush;
}
void client::stop()
{
    if ( !isStarted) return;
    service.clientlist.erase(it);
    isStarted = false;
    mysocket.close();
}
client::ptr client::new_()
{
    ptr new_(new client);
    return new_;
}
bool client::newEvent(std::string text)
{
    event result;
    result.text=text;
    if(isPassiveMode)
    {
        result.isReaded=true;
        events.push_back(result);
        RecursionArray rec;
        rec.add("key","20");
        rec.add("text",text);
        do_write(RecArrUtils::toArcan(rec));
        return false;
    }
    else
    {
        result.isReaded=false;
        events.push_back(result);
        return true;
    }
}

void client::start()
{
    isStarted = true;
    it=service.clientlist.insert(shared_from_this()).first;
    do_read();
}
void client::do_read()
{
    async_read(mysocket, asio::buffer(read_buffer,read_buffer_size),
               MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
}
bool client::do_write(const std::string & msg)
{
    if ( !started() ) return false;
    if(msg.size()>write_buffer_size)
    {
        cout << "WARNING: write_buffer > msg.size()" << endl;
        return false;
    }
    std::copy(msg.begin(), msg.end(), write_buffer);
    mysocket.async_write_some( asio::buffer(write_buffer, msg.size()),
                            MEM_FN2(on_write,_1,_2));
    return true;
}
void client::sync_write(const std::string & msg)
{
    if ( !started() ) return;
    if(msg.size()>write_buffer_size)
    {
        cout << "WARNING: write_buffer > msg.size()" << endl;
        return;
    }
    std::copy(msg.begin(), msg.end(), write_buffer);
    mysocket.write_some( asio::buffer(write_buffer, msg.size()));
}
size_t client::read_complete(const boost::system::error_code & err, size_t bytes)
{
    if ( err) return 0;
    bool found = std::find(read_buffer, read_buffer + bytes, '\n') < read_buffer + bytes;

    return found ? 0 : 1;
}
void client::on_read(const boost::system::error_code & err, size_t bytes)
{
    if ( !err)
    {
        std::string msg(read_buffer, bytes);
        if(!service.isCoutMode)
        {
            MyCommand cmd;
            cmd.clientptr=shared_from_this();
            cmd.cmd=msg;
            service.autoCommand(cmd);
        }
        else cout << msg << endl << flush;
        //do_read();
    }
    else
    {
        if(err!=asio::error::eof)
            cerr << err.message() << endl;
        else stop();
    }
}
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

void client::on_write(const boost::system::error_code & err, size_t bytes)
{
}

client::~client()
{
    cout << LOCALTIME << "Дестркутор!\n";
    cout << flush;
    if(isStarted) service.clientlist.erase(it);
    delete[] read_buffer;
    delete[] write_buffer;
}

bool client::started() {return isStarted;}
asio::ip::tcp::socket & client::sock() { return mysocket;}
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
    if(thiscmd.empty()) return;
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
        if((*i)->name==cmdname)
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

void handle_accept(client::ptr client, const boost::system::error_code & err)
{
    if(err)
    {
        cerr << err.message() << endl;
        return;
    }
    client->start();
    //cout << "New Client!" << endl;
    logs << LOCALTIME << "New Connect: " << client->sock().remote_endpoint().address().to_string() << ":" << client->sock().remote_endpoint().port() << "\n";
    client::ptr new_client = client::new_();
    acceptor->async_accept(new_client->sock(), boost::bind(handle_accept,new_client,_1));
}
}
