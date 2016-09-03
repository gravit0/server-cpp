#include <boost-server.hpp>
namespace boostserver
{
thread_control threadcontrol;
boost::asio::io_service ioserv;
#define MEM_FN(x) boost::bind(&client::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&client::x, shared_from_this(),y)
#define MEM_FN2(x,y,z) boost::bind(&client::x, shared_from_this(),y,z)
asio::ip::tcp::endpoint ep( asio::ip::address::from_string("127.0.0.1"), 8001);
asio::ip::tcp::acceptor acceptor(ioserv, ep);
client::client() : sock_(ioserv), started_(false),isreal(true)
{
    read_buffer_ = new char[max_msg];
    write_buffer_ = new char[max_msg];
    read_buffer_size = max_msg;
    write_buffer_size = max_msg;
    permissionsLevel=0;
    isAuth=false;
}
void client::stop()
{
    if ( !started_) return;
    started_ = false;
    sock_.close();
}
client::ptr client::new_()
{
    ptr new_(new client);
    return new_;
}
void client::start()
{
    started_ = true;
    do_read();
}
void client::do_read()
{
    async_read(sock_, asio::buffer(read_buffer_,read_buffer_size),
               MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
}
void client::do_write(const std::string & msg)
{
    if ( !started() ) return;
    std::copy(msg.begin(), msg.end(), write_buffer_);
    sock_.async_write_some( asio::buffer(write_buffer_, msg.size()),
                            MEM_FN2(on_write,_1,_2));
}
void client::sync_write(const std::string & msg)
{
    if ( !started() ) return;
    std::copy(msg.begin(), msg.end(), write_buffer_);
    sock_.write_some( asio::buffer(write_buffer_, msg.size()));
}
size_t client::read_complete(const boost::system::error_code & err, size_t bytes)
{
    if ( err) return 0;
    bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;

    return found ? 0 : 1;
}
void client::on_read(const boost::system::error_code & err, size_t bytes)
{
    if ( !err)
    {
        std::string msg(read_buffer_, bytes);
        MyCommand cmd;
        cmd.clientptr=shared_from_this();
        cmd.cmd=msg;
        threadcontrol.autoCommand(cmd);
        //do_read();
    }
    else
    {
        if(err!=asio::error::eof)
            cerr << err.message() << endl;
    }
}
void thread_control::newThread()
{
    mythread* then = new mythread();
    threads.push_back(then);
    newThreads++;
}
void thread_control::addThread(mythread* thend)
{
    threads.push_back(thend);
    addThreads++;
}
void thread_control::autoCommand(MyCommand cmd)
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
bool client::isReal()
{
    return isreal;
}

client::~client()
{
    cout << "Дестркутор!" << endl;
    cout << flush;
    delete[] read_buffer_;
    delete[] write_buffer_;
}

bool client::started() {return started_;}
asio::ip::tcp::socket & client::sock() { return sock_;}
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
        ComandUse(me,&thiscmd);
        me->commandsarray.pop();
        if(thiscmd.clientptr->started())
            thiscmd.clientptr->do_read();
    }
}
void ComandUse(mythread* me,MyCommand* thiscmd)
{
    if(thiscmd->cmd=="stop\r\n")
    {
        thiscmd->clientptr->sync_write("test!");
        thiscmd->clientptr->stop();
    }
    else if(thiscmd->cmd=="test\r\n")
    {
        thiscmd->clientptr->do_write("test!");
    }

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
    cout << "New Client!" << endl;
    client::ptr new_client = client::new_();
    acceptor.async_accept(new_client->sock(), boost::bind(handle_accept,new_client,_1));
}
}
