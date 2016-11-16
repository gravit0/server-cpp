#include "accountutils.h"
#include "recarray.h"
#include <boost/foreach.hpp>
#include <iostream>
#include <strstream>
#include "database.h"
namespace AccountUtils
{
void auth(boostserver::client::ptr client, std::string login,std::string password,Database* db)
{

}
}
namespace RecArrUtils
{
void printTree(const RecursionArray& tree)
{
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
            std::cout << v.first << " " << v.second.get<std::string>("") << std::endl;
        else
        {
            std::cout << "Map " << v.first << std::endl;
            printTree(tmp);
            std::cout << "End" << std::endl;
        }
    }
}
std::string printTreeEx(const RecursionArray& tree)
{
    std::stringstream steam;
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
            steam << v.first << " " << v.second.get<std::string>("") << std::endl;
        else
        {
            steam << "Map " << v.first << std::endl;
            steam << printTreeEx(tmp);
            steam << "End" << std::endl;
        }
    }
    return steam.str();
}
std::string toArcan(const RecursionArray& tree)
{
    std::stringstream steam;
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
        {
            std::string second = v.second.get<std::string>("");
            int tPos=0;
            while(true)
            {
                tPos=second.find("\\[");
                if(tPos<0) break;
                second.replace(tPos,2,"[");
            }
            tPos=0;
            while(true)
            {
                tPos=second.find("\\]");
                if(tPos<0) break;
                second.replace(tPos,2,"]");
            }
            tPos=0;
            while(true)
            {
                tPos=second.find("\\\\");
                if(tPos<0) break;
                second.replace(tPos,2,"\\");
            }
            steam << v.first << "[" << v.second.get<std::string>("") << "]";
        }
        else
        {
            steam << v.first << "[" << toArcan(tmp) << "]";
        }
    }
    return steam.str();
}
std::string toCfg(const RecursionArray& tree, const std::string& prefix)
{
    std::stringstream steam;
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
            steam << "\n" << prefix << v.first << ": " << v.second.get<std::string>("");
        else
        {
            steam << "\n" << prefix << v.first << "\n" << prefix << "{" << toCfg(tmp,prefix+"\t") << "\n" << prefix << "}";
        }
    }
    return steam.str();
}
int findNoSlash(const std::string& str,const char ch, const unsigned int frist_pos,bool* isReplaceT,bool* isReplaceS)
{
    bool NoAdept=false;
    unsigned int size=str.size();
    for(unsigned int i=frist_pos;i<size;++i)
    {
        char thch=str[i];
        if(thch=='\\')
        {
            if(NoAdept)
            {
                *isReplaceS=true;
            }
            NoAdept=!NoAdept;
        }
        else
        {
            if(!NoAdept)
            {
                if(thch==ch)
                {
                    return (int)i;
                }
            }
            else
            {
                NoAdept=false;
                *isReplaceT=true;
            }
        }
    }
    return -1;
}
void SlashReplace(std::string* str, const unsigned int frist_pos)
{
    bool NoAdept=false;
    unsigned int size=str->size();
    for(unsigned int i=frist_pos;i<size;++i)
    {
        char thch=(*str)[i];
        if(thch=='\\')
        {
            NoAdept=!NoAdept;
            if(i+1<size)
            {
                char nextch=(*str)[i+1];
                str->replace(i,2,1,nextch);
            }
        }
        else
        {
            if(NoAdept)
            {
                NoAdept=false;
            }
        }
    }
}
RecursionArray fromArcan(const std::string &str)
{
    RecursionArray arr;
    int i=0,Shet=0;
    while(i<str.size())
    {

        bool isReplaseA=false,isReplaseB=false,isReplaceC=false,isReplaceD=false,isReplaceE=false;
        int first_pos=findNoSlash(str,'[',i,&isReplaceD,&isReplaceE);
        if(first_pos<0) break;
        std::string first;
        if(first_pos==i)
        {
            first=std::to_string(Shet);
            Shet++;
        }
        else first=str.substr(i,first_pos-i);
        /*int zPos=0;
        while(true)
        {
            zPos=first.find("\\]",zPos);
            if(zPos<0) break;
            first.replace(zPos,2,"]");
        }
        if(isReplaceD)
        {
            zPos=0;
            while(true)
            {
                zPos=first.find("\\[",zPos);
                if(zPos<0) break;
                first.replace(zPos,2,"[");
            }
        }
        if(isReplaceE)
        {
            zPos=0;
            while(true)
            {
                zPos=first.find("\\\\",zPos);
                if(zPos<0) break;
                first.replace(zPos,2,"\\");
            }
        }*/
        SlashReplace(&first,0);
        //int second_pos=str.find(']',first_pos);
        int second_pos=findNoSlash(str,']',first_pos,&isReplaseB,&isReplaceC);
        if(second_pos<0) break;
        /*if(second_pos-1==first_pos)
        {
            i=second_pos+1;
            continue;
        }*/
        int recursion_pos=findNoSlash(str,'[',first_pos+1,&isReplaseA,&isReplaceC);
        if(recursion_pos>0)
        {
            if(recursion_pos<second_pos)
            {

                int recursions=0;
                while(recursion_pos<second_pos)
                {
                    recursion_pos=findNoSlash(str,'[',recursion_pos+1,&isReplaseA,&isReplaceC);

                    recursions++;
                    if(recursion_pos<0) break;
                }
                int last_second=second_pos;
                while(recursions>0)
                {
                    last_second=findNoSlash(str,']',last_second+1,&isReplaseB,&isReplaceC);
                    if(last_second<0)
                    {
                        cout << "WARNING: Arcan Decode failed: 1" << endl << flush;
                        goto norecursion;
                    }
                    recursions--;
                }
                if(last_second<0) break;
                std::string second=str.substr(first_pos+1,last_second-first_pos-1);
                /*if(isReplaseA)
                {
                    int tPos=0;
                    while(true)
                    {
                        tPos=second.find("\\[");
                        if(tPos<0) break;
                        second.replace(tPos,2,"[");
                    }
                }
                if(isReplaseB)
                {
                    int tPos=0;
                    while(true)
                    {
                        tPos=second.find("\\]");
                        if(tPos<0) break;
                        second.replace(tPos,2,"]");
                    }
                }
                if(isReplaceC)
                {
                    int tPos=0;
                    while(true)
                    {
                        tPos=second.find("\\\\");
                        if(tPos<0) break;
                        second.replace(tPos,2,"\\");
                    }
                }*/
                arr.add_child(first,fromArcan(second));
                i=last_second+1;
                continue;
            }
        }
        norecursion:
        std::string second=str.substr(first_pos+1,second_pos-first_pos-1);
        /*if(isReplaseA)
        {
            int tPos=0;
            while(true)
            {
                tPos=second.find("\\[",tPos);
                if(tPos<0) break;
                second.replace(tPos,2,"[");
            }
        }
        if(isReplaseB)
        {
            int tPos=0;
            while(true)
            {
                tPos=second.find("\\]",tPos);
                if(tPos<0) break;
                second.replace(tPos,2,"]");
            }
        }
        if(isReplaceC)
        {
            int tPos=0;
            while(true)
            {
                tPos=second.find("\\\\",tPos);
                if(tPos<0) break;
                second.replace(tPos,2,"\\");
            }
        }*/
        SlashReplace(&second,0);
        arr.add(first,second);
        i=second_pos+1;
    }
    return arr;
}
}
