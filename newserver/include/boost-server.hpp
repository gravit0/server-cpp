#ifndef BOOSTSERVER_HPP
#define BOOSTSERVER_HPP
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include <list>
#include <queue>
#include <string>
#include <functional>
#include <thread>
//#include "database.h"
#include <functional>
#include <set>
#include "recarray.h"
#include "protocol.h"
bool initBaseCmds();
bool initTestCmds();
using namespace std;
extern std::string  config_mysql_login,config_mysql_password,config_mysql_dbname,config_mysql_host;
extern int config_mysql_port;
extern bool config_isdebug;

namespace boostserver
{
namespace fs = boost::filesystem;
namespace asio = boost::asio;
struct event
{
    std::string text;
    bool isReaded;
};
class client : public boost::enable_shared_from_this<client>, boost::noncopyable
{
    asio::ip::tcp::socket mysocket;
    enum { max_msg = 1024 };
    char* read_buffer;
    bool isStarted;
    client();
public:
    char* write_buffer;
    unsigned int write_buffer_writted;
    unsigned int read_buffer_size,write_buffer_size;
    ~client();
    typedef boost::shared_ptr<client> ptr;
    typedef set<client::ptr>::iterator iterator;
    void stop();
    static ptr new_();
    void start();
    void do_read();
    bool do_write(const std::string & msg);
    bool do_write(Protocol::pair msg);
    bool do_write();
    void sync_write(const std::string & msg);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);
    void on_read(const boost::system::error_code & err, size_t bytes);
    void on_write(const boost::system::error_code & err, size_t bytes);
    bool started();
    asio::ip::tcp::endpoint endpoint() const;
    bool newEvent(std::string text);
    asio::ip::tcp::socket & sock();
    //Расширенные опции
    unsigned int permissionsLevel; //Уровень привилегий пользователя
    bool isAuth,isTelnetMode,isPassiveMode;
    std::string login,nickname,email,groups;
    iterator it;
    std::list<event> events;
};
struct ServerConnect
{
    asio::ip::tcp::acceptor* acceptor;
    asio::ip::tcp::endpoint* endpoint;
    ~ServerConnect();
};

struct MyCommand
{
    char* data;
    unsigned int size;
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
    //Database db;
    ~mythread();
    bool isStart() const;
};
int ComandUse(mythread* me,char* data, unsigned int size,boostserver::client::ptr client);
struct Context
{
    client::ptr ptr;
    mythread* thread;
    unsigned int flags;
    char* buffer;
    unsigned int* size;
    unsigned int max_size;
    unsigned long long int session;
};
};
namespace Protocol
{
typedef CmdResult (*CallCell)(boostserver::Context, std::string);
};
namespace boostserver
{
struct Command{
    Protocol::CallCell func;
    unsigned long long int uuid;
    unsigned int minPermissions = 0;
};
class SrvControl
{
public:
    SrvControl();
    vector<mythread*> threads;
    set<client::ptr> clientlist;
    unsigned long long int coutAddThreads=0,coutNewThreads=0;
    void newThread(bool startdb);
    void newThreads(int threadsd, bool startdb);
    void addThread(mythread* thend);
    void autoCommand(MyCommand cmd);
    bool addCommand(Command* cmd);
    bool startdb(bool autofail=false);
    void savelog();
    void cmdsclear();
    void closeclients();
    vector<Command*> cmdlist;
    vector<ServerConnect*> connects;
    bool isCoutMode,isDebug;
    ~SrvControl();
    enum class _status
    {
        preload,
        loading,
        started,
        prestopping,
        stopping,
        close
    };
    _status status;
};
extern SrvControl service;
class Logger
{
public:
    Logger();
    std::stringstream stream;
    std::fstream file;
    bool isPrintStdout,isPrintFileout;
    operator std::string() const;
    template<typename T>
    Logger& operator<< (const T& arg)
    {
       if(isPrintStdout) std::cout << arg;
       if(isPrintStdout)
       {
            stream << arg;
            if(stream.str().size()>1024)
            {
                boostserver::service.savelog();
            }
       }
       return *this;
   }
};

extern ServerConnect thisConnect;
extern Logger logs;
extern boost::asio::io_service ioservice;
void handle_accept(client::ptr client, asio::ip::tcp::acceptor *accep, const boost::system::error_code & err);
}
#endif // BOOSTSERVER_HPP
