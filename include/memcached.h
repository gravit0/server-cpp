#ifndef MEMCACHED_H
#define MEMCACHED_H
#include <tcpclient.h>
#include <string>
class MemcachedConnect
{
public:
    MemcachedConnect();
    TcpClient::ptr sock;
    void connect(std::string ip, int port)
    {
        sock = TcpClient::start(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip),port));
        sock->do_write("version");
    }
};

#endif // MEMCACHED_H
