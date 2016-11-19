#include <boost-server.hpp>
#include <boost/any.hpp>
#include "boost-server.hpp"
#include "database.h"
//#include "recursionarray.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/foreach.hpp>
#include <recarray.h>
#include "config.h"
std::string  config_mysql_login,config_mysql_password,config_mysql_dbname,config_mysql_host;
int config_mysql_port;
using boostserver::service;
using boostserver::SrvControl;
void localcmd_thread()
{
    bool isStarted=true;
    while(isStarted)
    {
        std::string cmd;
        cin >> cmd;
        if(cmd=="info")
        {
            cout << "info OK";
        }
        else if(cmd=="stop")
        {
            service.cmdsclear();
            service.closeclients();
            delete boostserver::endpoint;
            std::terminate();
        }
        cout << flush;
    }
}

int main(int argc, char *argv[])
{
    service.thisstatus=SrvControl::status::preload;
    cout << "Load configs ";
    config_mysql_host="localhost";
    config_mysql_login="chat";
    config_mysql_dbname="chat";
    config_mysql_password="FJS8CFhuumsERQbp!";
    config_mysql_port=3306;
    boostserver::endpoint= new boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string(CONFIG_SERVER_HOST), CONFIG_SERVER_PORT);
    cout << "OK" << endl;
    cout << "Load base command ";
    if(initBaseCmds())
        cout << "OK" << endl;
    else
        cout << "Fail" << endl;
    cout << "Load test command ";
    if(initTestCmds())
        cout << "OK" << endl;
    else
        cout << "Fail" << endl;
    service.thisstatus=SrvControl::status::loading;
    cout << "Start threads ";
    boostserver::service.newThreads(2);
    cout << "OK" << endl;
    cout << "Start local thread ";
    std::thread localcmdthread(localcmd_thread);
    localcmdthread.detach();
    cout << "OK" << endl;
    cout << "Start database connection ";
    //Database db;
    //db.connect("localhost",3306,"chat","FJS8CFhuumsERQbp!","chat");
    cout << "OK" << endl;
    //MySqlResult ind = db.query("SELECT * FROM rooms;");
    //ind.print();

    //cout << ind.value("id") << endl;
    /*std::string testerd="mymy",saha="AAAAA!";
    ASLib::AVariant t2,t4,t7; //saha = "AAAAA!"
    t2=testerd; //saha = непонятно что
    ASLib::RecursionArray t3,t5,t6;
    t3["test1"]=t2;
    t5["testZ"]=saha;
    t3["test2"]=&t5;
    t4=t3["test1"];
    t6=t3["test2"].toMap();
    t7=t6["testZ"];
    cout << t4.toString() << t7.toString();*/
    boost::property_tree::ptree test1,test2;
    test1.add("test","test1");
    test1.add("test","test2");
    test2.add("test2","test1");
    test1.add_child("test_child",test2);
    RecArrUtils::printTree(test1);
    RecArrUtils::printTree(RecArrUtils::fromArcan("test1[test1\\[\\]_\\\\s\\\\]test2[test2_s]test3[test1[1]test2[2]]3[\\[\\[\\\\]testX[]Zen[1]"));
    //for(auto &v=test1.begin();v!=test1.end();++v)
    cout << "Start server socket " << flush;
    try
    {
        boostserver::acceptor=new boost::asio::ip::tcp::acceptor(boostserver::ioservice,*boostserver::endpoint);
        boostserver::client::ptr cliente = boostserver::client::new_();
        boostserver::acceptor->async_accept(cliente->sock(), boost::bind(boostserver::handle_accept,cliente,_1));
    }
    catch(boost::system::system_error a)
    {
        cout << "fail" << endl;
        cout << a.what() << endl;
        return 1;
    }
    cout << "OK" << endl;
    service.thisstatus=SrvControl::status::started;
    cout << "Server started " << endl;
    boostserver::ioservice.run();
    return 0;
}
