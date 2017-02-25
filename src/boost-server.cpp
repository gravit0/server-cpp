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
#ifndef NOLOGTIME
#define LOCALTIME "[" << boost::posix_time::microsec_clock::local_time() << "] "
#else
#define LOCALTIME " "
#endif
ServerConnect thisConnect;
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
    if(service.isDebug)
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
asio::ip::tcp::endpoint client::endpoint() const
{
    return mysocket.remote_endpoint();
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
    //bool found = std::find(read_buffer, read_buffer + bytes, '\n') < read_buffer + bytes;
//        bool found=false;
//        if(bytes==0) found=false;
//        else if(read_buffer[bytes-1]=='\n') found=true;
//        return found ? 0 : 1;
        if(bytes==0) return 1;
        if(read_buffer[bytes-1]=='\n') return 0;
        //return found ? 0 : 1;
//    if(bytes>0)
//        return 0;
//    else
//        return 1;
}
Logger::Logger()
{
    isPrintStdout=true;
    isPrintFileout=false;
}
//template<typename T>
//Logger& Logger::operator<< (const T& arg) {
//    stream << arg;
//    cout << arg;
//    if(stream.str().size()>1024)
//    {
//        boostserver::service.savelog();
//    }
//    return *this;
//}
Logger::operator std::string() const {
    return stream.str();
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

void handle_accept(client::ptr client, boost::asio::ip::tcp::acceptor* accep, const boost::system::error_code & err)
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
    accep->async_accept(new_client->sock(), boost::bind(handle_accept,new_client,accep,_1));
}
}
