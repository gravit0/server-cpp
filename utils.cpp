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
            printTreeEx(tmp);
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
            steam << v.first << "[" << v.second.get<std::string>("") << "]";
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
RecursionArray fromArcan(const std::string &str)
{
    RecursionArray arr;
    int i=0;
    while(i<str.size())
    {
        bool isReplaseA=false,isReplaseB=false;
        int first_pos=str.find('[',i);
        if(first_pos<0 || first_pos==i) break;
        std::string first=str.substr(i,first_pos-i);
        int second_pos=str.find(']',first_pos);
        if(second_pos<0 || second_pos-1==first_pos) break;
        while(str.at(second_pos-1)=='\\')
        {
            second_pos=str.find(']',second_pos+1);
            if(second_pos<0) break;
            isReplaseB=true;
        }
        int recursion_pos=str.find('[',first_pos+1);
        if(recursion_pos>0)
        {
            while(str.at(recursion_pos-1)=='\\')
            {
                recursion_pos=str.find('[',recursion_pos+1);
                if(recursion_pos<0) goto norecursion;
                isReplaseA=true;
            }
            if(recursion_pos<second_pos)
            {

                int recursions=1;
                while(recursion_pos<second_pos)
                {
                    recursion_pos=str.find('[',recursion_pos+1);
                    while(str.at(recursion_pos-1)=='\\')
                    {
                        recursion_pos=str.find('[',recursion_pos+1);
                        if(recursion_pos<0) return arr;
                        isReplaseA=true;

                    }
                    recursions++;
                }
                int last_second=second_pos;
                while(recursions>0)
                {
                    last_second=str.find(']',last_second+1);
                    if(last_second<0) return arr;
                    while(str.at(last_second-1)=='\\')
                    {
                        last_second=str.find(']',last_second+1);
                        if(last_second<0) return arr;
                        isReplaseB=true;
                    }
                    recursions--;
                }
                if(last_second<0) break;
                std::string second=str.substr(first_pos+1,last_second-first_pos-1);
                if(isReplaseA)
                {
                    int tPos=0;
                    while(tPos=second.find("\\["))
                    {
                        if(tPos<0) break;
                        second.replace(tPos,2,"[");
                    }
                }
                if(isReplaseB)
                {
                    int tPos=0;
                    while(tPos=second.find("\\]"))
                    {
                        if(tPos<0) break;
                        second.replace(tPos,2,"]");
                    }
                }
                arr.add_child(first,fromArcan(second));
                i=last_second+1;
                continue;
            }
        }
        norecursion:
        std::string second=str.substr(first_pos+1,second_pos-first_pos-1);
        if(isReplaseA)
        {
            int tPos=0;
            while(tPos=second.find("\\["))
            {
                if(tPos<0) break;
                second.replace(tPos,2,"[");
            }
        }
        if(isReplaseB)
        {
            int tPos=0;
            while(tPos=second.find("\\]"))
            {
                if(tPos<0) break;
                second.replace(tPos,2,"]");
            }
        }
        arr.add(first,second);
        i=second_pos+1;
    }
    return arr;
}
}
