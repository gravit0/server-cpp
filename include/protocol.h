#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <string>
#include <vector>
#include <optional>
namespace Protocol
{
struct message_result
{
    unsigned char version;
    unsigned char space;
    signed short flag; //Зарезервировано
    unsigned int size;
    unsigned long long int session;
    unsigned long long int code;
    enum results : unsigned char{
        OK = 0,
        ERROR_FILENOTFOUND = 1,
        ERROR_DEPNOTFOUND = 2,
        ERROR_PKGNOTFOUND = 3,
        ERROR_PKGINCORRECT = 4,
        ERROR_CMDINCORRECT = 5,
        ERROR_PKGALREADYLOADED = 6
    };
};
struct message_error : public message_result
{
    unsigned int errorcode;
};
typedef std::pair<void*,size_t> pair;
typedef std::optional<message_result::results> CmdResult;
struct message_head
{
    unsigned char version;
    unsigned char space; //RESERVED
    unsigned short flag;
    unsigned int cmdflags;
    unsigned int size;
    unsigned long long int cmd;
    unsigned long long int session;
};
};
#endif
