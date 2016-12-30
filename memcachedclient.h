#ifndef MEMCACHEDCLIENT_H
#define MEMCACHEDCLIENT_H
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <iostream>
class MemcachedClient: public boost::enable_shared_from_this<MemcachedClient>,boost::noncopyable
{
    typedef MemcachedClient self_type;
    MemcachedClient(const std::string & message) : sock_(*ioservice), started_(true),message_(message) {}
    unsigned int read_buffer_size,write_buffer_size;
    void start(boost::asio::ip::tcp::endpoint ep);
public:
    typedef boost::system::error_code error_code;
    boost::asio::io_service* ioservice;
    typedef boost::shared_ptr<MemcachedClient> ptr;
    boost::asio::ip::tcp::endpoint endpoint;
    static ptr start(boost::asio::ip::tcp::endpoint ep, const std::string & message);
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
