#ifndef BOOSTSERVER_HPP
#define BOOSTSERVER_HPP
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include <list>
#include <queue>
#include <functional>
#include <thread>
using namespace std;

namespace boostserver
{
namespace fs = boost::filesystem;
namespace asio = boost::asio;
class basic_client
{
public:
    basic_client() {}
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void do_write(const std::string & msg) = 0;
    virtual void sync_write(const std::string & msg) = 0;
    virtual bool started() = 0;
    virtual ~basic_client() {}
};

class client : public boost::enable_shared_from_this<client>, boost::noncopyable,basic_client
{
    asio::ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char* read_buffer_;
    char* write_buffer_;
    bool started_,isreal;
    int read_buffer_size,write_buffer_size;
    client();
public:
    virtual ~client();
    typedef boost::shared_ptr<client> ptr;
    void stop();
    static ptr new_();
    void start();
    void do_read();
    void do_write(const std::string & msg);
    void sync_write(const std::string & msg);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);
    void on_read(const boost::system::error_code & err, size_t bytes);
    void on_write(const boost::system::error_code & err, size_t bytes);
    bool started();
    bool isReal();
    asio::ip::tcp::socket & sock();
    //Расширенные опции
    unsigned int permissionsLevel; //Уровень привилегий пользователя
    bool isAuth; //Авторизирован ли он

};

struct MyCommand
{
    std::string cmd;
    client::ptr clientptr;
};
class mythread
{
    std::thread* threadptr;
    bool _isStart=false;
public:
    std::queue<MyCommand> commandsarray;
    void launch(bool isJoin);
    static void run(mythread *me);
    ~mythread();
    bool isStart() const;
};
void ComandUse(mythread* me, MyCommand *thiscmd);
class thread_control
{
    list<mythread*> threads;
    unsigned long long int addThreads=0,newThreadsd=0;
public:
    void newThread();
    void newThreads(unsigned int threadsd);
    void addThread(mythread* thend);
    void autoCommand(MyCommand cmd);
};
extern thread_control threadcontrol;
extern asio::ip::tcp::acceptor acceptor;
extern asio::ip::tcp::endpoint ep;
extern boost::asio::io_service ioserv;
void handle_accept(client::ptr client, const boost::system::error_code & err);
}
#endif // BOOSTSERVER_HPP
