#include "accountutils.h"
#include "recarray.h"
#include <boost/foreach.hpp>
#include <iostream>
namespace AccountUtils
{
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
            printTree(tmp);
            steam << "End" << std::endl;
        }
    }
    return steam.str();
}
std::string getString(const RecursionArray& tree,std::string key)
{
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
            if(v.first==key) return v.second.get<std::string>("");
    }
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
RecursionArray fromArcan(const std::string &str)
{
    RecursionArray arr;
    int i=0;
    while(i<str.size())
    {
        int first_pos=str.find('[',i);
        if(first_pos<0 || first_pos==i) break;
        std::string first=str.substr(i,first_pos-i);
        int second_pos=str.find(']',first_pos);
        if(second_pos<0 || second_pos-1==first_pos) break;
        int recursion_pos=str.find('[',first_pos+1);
        if(recursion_pos>0 && recursion_pos<second_pos)
        {
            int recursions=1;
            while(recursion_pos<second_pos)
            {
                recursion_pos=str.find('[',recursion_pos+1);
                recursions++;
            }
            int last_second=second_pos;
            while(recursions>0)
            {
                last_second=str.find(']',last_second+1);
                recursions--;
            }
            if(last_second<0) break;
            std::string second=str.substr(first_pos+1,last_second-first_pos-1);
            arr.add_child(first,fromArcan(second));
            i=last_second+1;
        }
        else
        {
            std::string second=str.substr(first_pos+1,second_pos-first_pos-1);
            arr.add(first,second);
            i=second_pos+1;
        }
    }
    return arr;
}
}
