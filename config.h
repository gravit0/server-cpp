#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <recarray.h>
//Базовые параметры
#define CONFIG_SERVER_HOST "127.0.0.1"
#define CONFIG_SERVER_PORT 8001

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
