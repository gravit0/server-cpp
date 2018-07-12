#include <boost-server.hpp>
#include <boost/any.hpp>
#include "boost-server.hpp"
//#include "database.h"
//#include "recursionarray.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <recarray.h>
#include <ctime>
#include "config.h"
#ifndef NOLOGTIME
#define LOCALTIME "[" << microsec_clock::local_time() << "] "
#else
#define LOCALTIME " "
#endif
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
            for(auto it=service.connects.begin();it!=service.connects.end();++it)
            {
                delete (*it);
            }
            service.connects.clear();
            boostserver::ioservice.stop();
        }
        cout << flush;
    }
}
RecursionArray getStdConfig()
{
            RecursionArray arr;
            arr.add("version",CONFIG_VERSION);
            arr.add("mysql.active","false");
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
            arr.add("reserve.active","false");
//            arr.add("server.writelist.file","writelist.json");
//            arr.add("server.writelist.enable","false");
//            arr.add("server.writelist.type","file");
//            arr.add("server.backlist.file","backlist.json");
//            arr.add("server.backlist.enable","false");
//            arr.add("server.backlist.type","file");
            arr.add("server.logs.file","server.log");
            arr.add("server.logs.stdout","true");
            arr.add("server.logs.fileout","true");
//            arr.add("server.http.enable","false");
//            arr.add("server.http.type","headers");
//            arr.add("command.allowSU","true");
//            arr.add("command.allowAuth","true");
            return arr;
}
int main(int argc, char *argv[])
{
    service.status=SrvControl::_status::preload;
    cout << LOCALTIME << "Load configs ";
    std::fstream file;
    bool configetc=false;
    if(argc>=2)
    {
        if(std::string(argv[1])=="--system") configetc=true;
        if(std::string(argv[1])=="--user") configetc=false;
    }
    if(configetc) file.open("/etc/cluserver.cfg",ios_base::in);
    else file.open("config.json",ios_base::in | ios_base::out);
    try
    {
        if(!file.is_open())
        {
            RecursionArray arr = getStdConfig();
            file.close();
            if(configetc) boost::property_tree::json_parser::write_json("/etc/cluserver.cfg",arr);
            else boost::property_tree::json_parser::write_json("config.json",arr);
            configarray=arr;
        }
        else
        {
            boost::property_tree::json_parser::read_json(file,configarray);
            int cfgver=configarray.get<int>("version",CONFIG_VERSION);
            if(cfgver!=CONFIG_VERSION) std::cout << "Warn\nWARNING! Config version does not match the version of the program. Config version:" << cfgver << ". Standart config version:" <<CONFIG_VERSION;
            config_mysql_host=configarray.get<std::string>("server.host","127.0.0.1");
            config_mysql_login=configarray.get<std::string>("server.login","root");
            config_mysql_dbname=configarray.get<std::string>("server.dbname","server");
            config_mysql_password=configarray.get<std::string>("server.password","");
            config_mysql_port=configarray.get<int>("server.port",3306);
            //RecArrUtils::printTree(configarray);
        }
    }
    catch(boost::property_tree::json_parser_error* ex)
    {
        cout << "fail\n" << ex->what() << "\n";
        file.close();
        return 1;
    }
    file.close();
    if(configarray.get<std::string>("server.logs.fileout","true")=="true")
    {
        logs.isPrintFileout=true;
        if(configetc) logs.file.open(configarray.get<std::string>("server.logs.file","/var/log/server.log"),ios_base::app);
        else logs.file.open(configarray.get<std::string>("server.logs.file","server.log"),ios_base::app);
    }
    else
    {
        logs.isPrintFileout=false;
    }

    if(configarray.get<std::string>("server.logs.stdout","true")=="true")
    {
        logs.isPrintStdout=true;
    }
    else
    {
        logs.isPrintStdout=false;
    }
    boostserver::thisConnect.endpoint= new boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string(
                    configarray.get<std::string>("server.host","127.0.0.1")),
                configarray.get<int>("server.port",8001));
    cout << "OK\n";
    if(configarray.get<std::string>("server.debug.print","false")=="true")
    {
        service.isDebug=true;
    }
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
    service.status=SrvControl::_status::loading;
    logs << LOCALTIME << "Start " << configarray.get<int>("server.threads",2) << "threads ";
    boostserver::service.newThreads(configarray.get<int>("server.threads",2),false);
    logs << "OK\n";
    logs << LOCALTIME << "Start local thread ";
    std::thread localcmdthread(localcmd_thread);
    localcmdthread.detach();
    logs << "OK\n";
    logs << LOCALTIME << "Start server socket ";
    try
    {
        boostserver::thisConnect.acceptor=new boost::asio::ip::tcp::acceptor(boostserver::ioservice,*boostserver::thisConnect.endpoint);
        boostserver::client::ptr cliente = boostserver::client::new_();
        boostserver::thisConnect.acceptor->async_accept(cliente->sock(), boost::bind(boostserver::handle_accept,cliente,boostserver::thisConnect.acceptor,_1));
        logs << "OK\n";
        logs << LOCALTIME << "Listen " << boostserver::thisConnect.endpoint->address().to_string() << " " << boostserver::thisConnect.endpoint->port() << "\n";
    }
    catch(boost::system::system_error* a)
    {
        cout << "fail" << endl;
        cout << a->what() << endl;
        return 1;
    }


    RecursionArray connectsarr = configarray.get_child("connects",{});
    if(service.isDebug) RecArrUtils::printTree(connectsarr);
    if(!connectsarr.empty())
    {
        BOOST_FOREACH(auto &v, connectsarr)
        {
            boost::property_tree::ptree tmp = v.second.get_child("");
            if(!tmp.empty())
            {
                try
                {
                    boost::asio::ip::tcp::endpoint* endpo= new boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::address::from_string(
                                    tmp.get<std::string>("host","127.0.0.1")),
                                tmp.get<int>("port",8001));
                    boostserver::ServerConnect* conn = new boostserver::ServerConnect();
                    conn->endpoint=endpo;
                    conn->acceptor=new boost::asio::ip::tcp::acceptor(boostserver::ioservice,*endpo);
                    boostserver::client::ptr cliente = boostserver::client::new_();
                    conn->acceptor->async_accept(cliente->sock(), boost::bind(boostserver::handle_accept,cliente,conn->acceptor,_1));
                    service.connects.push_back(conn);
                    logs << LOCALTIME << "Listen " << conn->endpoint->address().to_string() << " " << conn->endpoint->port() << "\n";
                }
                catch(boost::system::system_error* a)
                {
                    cout << a->what() << endl;
                    return 1;
                }
            }
            else
            {
                logs << LOCALTIME << "Failed parse connect\n ";
                service.savelog();
                return 1;
            }
        }
    }


    service.status=SrvControl::_status::started;
    logs << LOCALTIME << "Server started\n";
    service.savelog();
    boostserver::ioservice.run();
    logs << LOCALTIME << "Server stoped\n";
    service.status=SrvControl::_status::stopping;
    service.savelog();
    return 0;
}
