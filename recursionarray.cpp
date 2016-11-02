#include "recursionarray.h"
using namespace ASLib;
RecursionArray::RecursionArray()
{
}
void AVariant::operator =(std::string str)
{
    std::string* result = new std::string();
    *result=str;
    ptr=result;
    type=String;
    isPtr=false;
}
void AVariant::operator =(RecursionArray str)
{
    RecursionArray* result = new RecursionArray();
    *result=str;
    ptr=result;
    type=RecArray;
    isPtr=false;
}
void AVariant::operator =(RecursionArray* str)
{
    ptr=str;
    type=RecArray;
    isPtr=true;
}
AVariant::~AVariant()
{
    if(!isPtr)
    {
        if(type==String)
        {
            delete (std::string*)ptr;
        }
        else if(type==RecArray)
        {
            delete (RecursionArray*)ptr;
        }
    }
}
AVariant::AVariant()
{
    type=Unknown;
    ptr=nullptr;
    isPtr=true;
}
std::string AVariant::toString(std::string noValue)
{
    if(type==String)
    {
        std::string result;
        result=*((std::string*)ptr);
        return result;
    }
    else return noValue;
}
RecursionArray AVariant::toMap()
{
    if(type==RecArray)
    {
        RecursionArray result;
        result=*((RecursionArray*)ptr);
        return result;
    }
    else return RecursionArray();
}
RecursionArray* AVariant::toMapEx()
{
    if(type==RecArray)
    {
        return (RecursionArray*)ptr;
    }
    else return nullptr;
}
