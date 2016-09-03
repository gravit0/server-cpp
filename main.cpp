#include <main.hpp>
#include <boost/any.hpp>
#include "abstractcommand.h"
void localcmd_thread()
{
    std::string cmd;
    cin >> cmd;
    if(cmd=="info")
    {
        cout << "info OK";
    }
    cout << flush;
}
int main(int argc, char *argv[])
{
    boostserver::threadcontrol.newThread();
    boostserver::threadcontrol.newThread();
    cout << "Hello World!" << endl;
    std::thread localcmdthread(localcmd_thread);
    localcmdthread.detach();
    boostserver::client::ptr cliente = boostserver::client::new_();
    boostserver::acceptor.async_accept(cliente->sock(), boost::bind(boostserver::handle_accept,cliente,_1));
    boostserver::ioserv.run();
    return 0;
}
