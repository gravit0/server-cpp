#include "tcpclient.h"
#define MEM_FN(x) boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z) boost::bind(&self_type::x, shared_from_this(),y,z)
void TcpClient::stop()
{
    if ( !started_) return;
    started_ = false;
    sock_.close();
}
void TcpClient::do_read()
{
    async_read(sock_, boost::asio::buffer(read_buffer_),
               MEM_FN2(read_complete,_1,_2),
               MEM_FN2(on_read,_1,_2));
}
void TcpClient::connect(boost::asio::ip::tcp::endpoint ep)
{
    sock_.async_connect(ep, MEM_FN1(on_connect,_1));
}
void TcpClient::do_write(const std::string & msg)
{
    if ( !started() ) return;
    if(msg.size()>write_buffer_size)
    {
        std::cout << "WARNING: write_buffer > msg.size()" << std::endl;
        return;
    }
    std::copy(msg.begin(), msg.end(), write_buffer_);
    sock_.async_write_some( boost::asio::buffer(write_buffer_, msg.size()),
                            MEM_FN2(on_write,_1,_2));
}
void TcpClient::on_write(const error_code & err, size_t bytes)
{
    do_read();
}
TcpClient::ptr TcpClient::start(boost::asio::ip::tcp::endpoint ep)
{
    ptr new_(new TcpClient());
    new_->start(ep);
    return new_;
}
size_t TcpClient::read_complete(const error_code & err, size_t bytes)
{
    if ( err) return 0;
    if(bytes==0) return 1;
    if(read_buffer_[bytes-1]=='\n') return 0;
}
void TcpClient::on_connect(const error_code & err)
{
    if ( !err) do_write(message_ + "\n");
    else stop();
}
void TcpClient::on_read(const error_code & err, size_t bytes)
{
    if ( !err)
    {
        std::string copy(read_buffer_, bytes - 1);
        std::cout << "server echoed our " << message_ << ": "<< (copy == message_ ? "OK" : "FAIL") << std::endl;
    }
    stop();
}
