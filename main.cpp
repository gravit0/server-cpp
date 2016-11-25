#include <boost-server.hpp>
#include <boost/any.hpp>
#include "boost-server.hpp"
#include "database.h"
//#include "recursionarray.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <recarray.h>
#include <ctime>
#include "config.h"
#define LOCALTIME "[" << microsec_clock::local_time() << "] "
std::string  config_mysql_login,config_mysql_password,config_mysql_dbname,config_mysql_host;
int config_mysql_port;
using boostserver::service;
using boostserver::SrvControl;
using boostserver::logs;
using namespace boost::posix_time;
_replys replys;
RecursionArray configarray;
boostserver::Logger boostserver::logs;
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
//class MakeLog {
//public:
//    MakeLog()
//    {
//    }

//    template<class T>
//    MakeLog& operator<< (const T& arg) {
//        m_stream << arg;
//        return *this;
//    }
//    operator std::string() const {
//        return m_stream.str();
//    }
//    ~MakeLog()
//    {

//        logs.push_back(m_stream.str());
//        if(logs.size()>300)
//        {
//            boostserver::service.savelog();
//        }
//        cout << m_stream.str();
//    }

//protected:
//    std::stringstream m_stream;
//};
int main(int argc, char *argv[])
{
    service.thisstatus=SrvControl::status::preload;
    cout << LOCALTIME << "Load configs ";
    std::fstream file;
    file.open("config.json",ios_base::in | ios_base::out);
    try
    {
        if(!file.is_open())
        {
            RecursionArray arr;
            arr.add("version","1");
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
            arr.add("server.debug.print","false");
//            arr.add("server.writelist.file","writelist.json");
//            arr.add("server.writelist.enable","false");
//            arr.add("server.writelist.type","file");
//            arr.add("server.backlist.file","backlist.json");
//            arr.add("server.backlist.enable","false");
//            arr.add("server.backlist.type","file");
            arr.add("server.logs.file","server.log");
            arr.add("server.logs.stdout","true");
            arr.add("server.http.enable","false");
            arr.add("server.http.type","headers");
            //arr.add("command.allowSU","true");
            //arr.add("command.allowAuth","true");
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
        cout << "fail\n" << ex.what() << "\n";
        file.close();
        return 1;
    }
    file.close();
    logs.file.open(configarray.get<std::string>("server.logs.file","server.log"),ios_base::app);
    if(configarray.get<std::string>("server.logs.stdout","true")=="true")
    {
        logs.isPrintStdout=true;
    }
    else
    {
        logs.isPrintStdout=false;
    }
    //ptime timer = microsec_clock::local_time();
//    config_mysql_host="localhost";
//    config_mysql_login="chat";
//    config_mysql_dbname="chat";
//    config_mysql_password="FJS8CFhuumsERQbp!";
//    config_mysql_port=3306;
    boostserver::endpoint= new boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string(
                    configarray.get<std::string>("server.host","127.0.0.1")),
                configarray.get<int>("server.port",8001));
    cout << "OK\n";
    if(configarray.get<std::string>("server.baseCommands","true")=="true")
    {
        logs << LOCALTIME << "Load base command ";
        if(initBaseCmds())
            logs << "OK\n";
        else
            logs << "Fail\n";
    }
    if(configarray.get<std::string>("server.testCommands","true")=="true")
    {
        logs << LOCALTIME << "Load test command ";
        if(initTestCmds())
            logs << "OK\n";
        else
            logs << "Fail\n";
    }
    service.thisstatus=SrvControl::status::loading;
    logs << LOCALTIME << "Start threads ";
    boostserver::service.newThreads(configarray.get<int>("server.threads",2),false);
    logs << "OK\n";
    logs << LOCALTIME << "Start local thread ";
    std::thread localcmdthread(localcmd_thread);
    localcmdthread.detach();
    logs << "OK\n";
    logs << LOCALTIME << "Start database connection ";
    try
    {
        if(boostserver::service.startdb(false))
            logs << "OK\n";
        else
            logs << "fail\n";
    }
    catch(mysqlpp::Exception ex)
    {
        logs << "fail\n";
        logs << ex.what() << "\n";
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
//    boost::property_tree::ptree test1,test2;
//    test1.add("test","test1");
//    test1.add("test","test2");
//    test2.add("test2","test1");
//    test1.add_child("test_child",test2);
//    RecArrUtils::printTree(test1);
//    RecArrUtils::printTree(RecArrUtils::fromArcan("test1[test1\\[\\]_\\\\s\\\\]test2[test2_s]test3[test1[1]test2[2]]3[\\[\\[\\\\]testX[]Zen[1]"));
    //for(auto &v=test1.begin();v!=test1.end();++v)
//    logs << LOCALTIME << "Test RecursionArray1\n";
//    RecursionArray test1;
//    for(int i=0;i<1000000;++i)
//    {
//        test1.push_back(RecursionArray::value_type(std::to_string(i), RecursionArray("AAAAAAAAAAAAAAAA")));
//    }
//    logs << LOCALTIME << "Test RecursionArray2\n";
//    std::string test2=RecArrUtils::toArcan(test1);
//    logs << LOCALTIME << "Test RecursionArray3\n";
//    RecArrUtils::fromArcan(test2);
//    logs << LOCALTIME << "Start server socket ";
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
    logs << "OK\n";
    service.thisstatus=SrvControl::status::started;
    logs << LOCALTIME << "Server started\n ";
    service.savelog();
    boostserver::ioservice.run();
    return 0;
}
