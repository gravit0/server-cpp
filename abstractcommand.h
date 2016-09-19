#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H
#include <string>
#include <list>
#include <boost-server.hpp>
#include <functional>
class Command{
public:
    virtual void operator ()(std::list<std::string>* args,boostserver::client::ptr client) = 0;
    std::string name;
    unsigned int minPermissions = 0;
    virtual ~Command() {}
};
bool initBaseCmds();
extern list<Command*> _cmds;
#endif // ABSTRACTCOMMAND_H
