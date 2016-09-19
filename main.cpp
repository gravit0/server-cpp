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
    cout << "Load base command ";
    if(initBaseCmds())
        cout << "OK" << endl;
    else
        cout << "Fail" << endl;
    cout << "Start threads ";
    boostserver::threadcontrol.newThreads(2);
    cout << "OK" << endl;
    cout << "Start local thread ";
    std::thread localcmdthread(localcmd_thread);
    localcmdthread.detach();
    cout << "OK" << endl;
    cout << "Start server socket ";
    boostserver::client::ptr cliente = boostserver::client::new_();
    boostserver::acceptor.async_accept(cliente->sock(), boost::bind(boostserver::handle_accept,cliente,_1));
    cout << "OK" << endl;
    cout << "Server started " << endl;
    boostserver::ioserv.run();
    return 0;
}
