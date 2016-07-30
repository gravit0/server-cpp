#ifndef MAIN_H
#define MAIN_H
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/filesystem.hpp>
#include <list>
#include <queue>
#include <thread>
using namespace std;
namespace fs = boost::filesystem;
namespace asio = boost::asio;
class client : public boost::enable_shared_from_this<client>, boost::noncopyable
{
    asio::ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;

    client();
public:
    typedef boost::shared_ptr<client> ptr;
    void stop();
    static ptr new_();
    void start();
    void do_read();
    void do_write(const std::string & msg);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);
    void on_read(const boost::system::error_code & err, size_t bytes);
    void on_write(const boost::system::error_code & err, size_t bytes);
    bool started();
    asio::ip::tcp::socket & sock();
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
class thread_control
{
    list<mythread*> threads;
    unsigned long long int addThreads=0,newThreads=0;
public:
    void newThread();
    void addThread(mythread* thend);
    void autoCommand(MyCommand cmd);
};
extern thread_control threadcontrol;

#endif // MAIN_H
