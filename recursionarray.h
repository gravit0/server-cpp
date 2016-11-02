#ifndef RECURSIONARRAY_H
#define RECURSIONARRAY_H
#include <string>
#include <map>
//#include <boost/noncopyable.hpp>
namespace ASLib
{
class RecursionArray;

struct AVariant
{
    void* ptr;
    enum Types
    {
        RecArray,
        String,
        Integer,
        Unknown,
        Not
    };
    Types type;
    bool isPtr;
    void operator=(std::string str);
    void operator=(RecursionArray str);
    void operator=(RecursionArray* str);
    std::string toString(std::string noValue="");
    RecursionArray toMap();
    RecursionArray* toMapEx();
    ~AVariant();
    AVariant();
};
class RecursionArrayCompare
{
  public:
    bool operator() (const std::string x,const AVariant y) const { return true; }
};
class RecursionArray : public std::map<std::string,AVariant>
{
public:
    RecursionArray();
};
}
#endif // RECURSIONARRAY_H
