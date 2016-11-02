#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H
#include <string>
#include <list>
#include <boost-server.hpp>
#include <functional>
#include "recarray.h"
class Command{
public:
    std::function<void(Command* cmd,const RecursionArray& args,boostserver::client::ptr client)> func;
    std::string name;
    unsigned int minPermissions = 0;
    ~Command() {}
};
bool initBaseCmds();
extern list<Command*> _cmds;
#endif // ABSTRACTCOMMAND_H
