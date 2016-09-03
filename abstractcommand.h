#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H
#include <string>
#include <list>
#include <boost-server.hpp>
class AbstractCommand{
public:
    virtual void init() = 0;
    virtual void exec(std::list<std::string>* args,boostserver::client::ptr client, unsigned int permissionLevel) = 0;
    virtual std::string getName() = 0;
    unsigned int minPermissions = 0;
};

#endif // ABSTRACTCOMMAND_H
