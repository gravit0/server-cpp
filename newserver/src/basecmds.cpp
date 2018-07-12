#include <boost-server.hpp>
#include "config.h"
using boostserver::Command;
using boostserver::service;
using boostserver::client;
using boostserver::mythread;
using namespace RecArrUtils;

#define InitCmd(cmdname,varname,funcd) Command* varname=new Command(); \
    varname->name=cmdname; \
    cmdtest->func=[](boostserver::mythread* me,Command* cmd,const RecursionArray&  args,boostserver::client::ptr client) funcd ; \
    service.cmdlist.push_back(varname)
bool initBaseCmds()
{
    return true;
}
#undef InitCmd
