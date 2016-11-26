#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <recarray.h>
#define SERVER_VERSION "0.0.1"
#define SERVER_TYPE "cluServer(boost)"

class _replys
{
public:
    std::string OK="1";
    std::string Fail="";
    std::string NotCommand="33";
    std::string NotPermissions="31";
    std::string IntegralServerError="10";
    std::string SQLError="11";
};
extern _replys replys;
extern RecursionArray configarray;
#define ReturnCode(code) \
    RecursionArray coderes; \
    coderes.add("key",replys.code); \
    client->do_write(toArcan(coderes))
#define ReturnCodeEx(code,result) \
    result.add("key",replys.code); \
    client->do_write(toArcan(result))
#endif // CONFIG_H
