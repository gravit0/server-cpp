#include <boost-server.hpp>
#include <boost/any.hpp>
#include "boost-server.hpp"
#include "database.h"
//#include "recursionarray.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <recarray.h>
#include "config.h"
std::string  config_mysql_login,config_mysql_password,config_mysql_dbname,config_mysql_host;
int config_mysql_port;
using boostserver::service;
using boostserver::SrvControl;
_replys replys;
RecursionArray configarray;
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
    std::fstream file;
    file.open("config.json",ios_base::in | ios_base::out);
    try
    {
        if(!file.is_open())
        {
            RecursionArray arr;
            arr.add("mysql.active","true");
            arr.add("mysql.user","root");
            arr.add("mysql.host","localhost");
            arr.add("mysql.password","");
            arr.add("mysql.db","server");
            arr.add("mysql.port","3306");
            arr.add("server.host","127.0.0.1");
            arr.add("server.port","8001");
            arr.add("server.threads","2");
            arr.add("server.baseCommands","true");
            arr.add("server.testCommands","true");
            arr.add("command.allowSU","true");
            arr.add("command.allowAuth","true");
            file.close();
            boost::property_tree::json_parser::write_json("config.json",arr);

            configarray=arr;
        }
        else
        {
            boost::property_tree::json_parser::read_json(file,configarray);
            config_mysql_host=configarray.get<std::string>("server.host","127.0.0.1");
            config_mysql_login=configarray.get<std::string>("server.login","root");
            config_mysql_dbname=configarray.get<std::string>("server.dbname","server");
            config_mysql_password=configarray.get<std::string>("server.password","");
            config_mysql_port=configarray.get<int>("server.port",3306);
            RecArrUtils::printTree(configarray);
        }
    }
    catch(boost::property_tree::json_parser_error ex)
    {
        cout << "fail" << endl << ex.what() << endl;
        file.close();
        return 1;
    }
    file.close();
//    config_mysql_host="localhost";
//    config_mysql_login="chat";
//    config_mysql_dbname="chat";
//    config_mysql_password="FJS8CFhuumsERQbp!";
//    config_mysql_port=3306;
    boostserver::endpoint= new boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string(
                    configarray.get<std::string>("server.host","127.0.0.1")),
                configarray.get<int>("server.port",8001));
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
    boostserver::service.newThreads(2,false);
    cout << "OK" << endl;
    cout << "Start local thread ";
    std::thread localcmdthread(localcmd_thread);
    localcmdthread.detach();
    cout << "OK" << endl;
    cout << "Start database connection ";
    try
    {
        if(boostserver::service.startdb(false))
            cout << "OK" << endl;
        else
            cout << "fail" << endl;
    }
    catch(mysqlpp::Exception ex)
    {
        cout << "fail" << endl;
        cout << ex.what() << endl;
    }
    //Database db;
    //db.connect("localhost",3306,"chat","FJS8CFhuumsERQbp!","chat");
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
