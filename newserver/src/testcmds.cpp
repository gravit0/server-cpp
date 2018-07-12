#include <boost-server.hpp>
#include "basefuncions.h"
#include "config.h"
using boostserver::Command;
using boostserver::service;
using boostserver::client;
using boostserver::mythread;
using namespace RecArrUtils;
#define InitCmd(cmdname,varname,funcd) Command* varname=new Command(); \
    varname->name=cmdname; \
    varname->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client) funcd ; \
    service.cmdlist.push_back(varname)
Protocol::CmdResult cmd_telnet(boostserver::Context context, std::string)
{
    context.ptr->isTelnetMode=!context.ptr->isTelnetMode;
    return Protocol::message_result::OK;
}

Protocol::CmdResult cmd_stop(boostserver::Context context, std::string)
{
    service.closeclients();
    service.status = boostserver::SrvControl::_status::prestopping;
    return Protocol::message_result::OK;
}

Command testcmds[] = {
    {&cmd_telnet,0x5c04d270af35bf79,0},
    {&cmd_stop,0x4f6e0b2184918d89,0}
    
};
bool initTestCmds()
{
    for(auto &i : testcmds) service.cmdlist.push_back(&i);
    return true;
}
#undef InitCmd
