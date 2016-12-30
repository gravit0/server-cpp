#ifndef MEMCACHEDCLIENT_H
#define MEMCACHEDCLIENT_H
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <iostream>
#include <functional>
class TcpClient: public boost::enable_shared_from_this<TcpClient>,boost::noncopyable
{
    typedef TcpClient self_type;
    TcpClient() : sock_(*ioservice), started_(true) {}
    unsigned int read_buffer_size,write_buffer_size;
    void connect(boost::asio::ip::tcp::endpoint ep);
public:
    typedef boost::system::error_code error_code;
    boost::asio::io_service* ioservice;
    typedef boost::shared_ptr<TcpClient> ptr;
    boost::asio::ip::tcp::endpoint endpoint;
    std::function<void(std::string react)> onread;
    static ptr start(boost::asio::ip::tcp::endpoint ep);
    void stop();
    bool started() { return started_; }
    void do_read();
    void do_write(const std::string & msg);
    size_t read_complete(const error_code & err, size_t bytes);
    void on_connect(const error_code & err);
    void on_read(const error_code & err, size_t bytes);
    void on_write(const error_code & err, size_t bytes);
    boost::asio::ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
    std::string message_;
};

#endif // MEMCACHEDCLIENT_H
