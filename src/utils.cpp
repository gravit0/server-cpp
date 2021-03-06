#include "accountutils.h"
#include "recarray.h"
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>
//#include "database.h"
#include "basefuncions.h"
namespace AccountUtils
{
bool auth(boostserver::client::ptr client, std::string login,std::string password,mysqlpp::Connection* db)
{
    if(login.empty() || password.empty() || !db->connected()) return false;
    mysqlpp::Query query = db->query( "SELECT * FROM users WHERE `login`='"+login+"' AND `pass` = MD5('"+password+"');" );
    mysqlpp::StoreQueryResult res = query.store();
    RecursionArray resultr;
    if(res.num_rows()>0)
    {
        unsigned int num_fields=res.num_fields();
        for(unsigned int i = 0; i < num_fields; i++)
        {
            std::string first(res.field(i).name());
            std::string second(res[0][i]);
            resultr.push_back(RecursionArray::value_type(first, RecursionArray(second)));
        }
        RecArrUtils::printTree( resultr );
        client->login=login;
        client->isAuth=true;
        client->permissionsLevel=1;
        client->nickname=resultr.get<std::string>("nickname","");
        client->email=resultr.get<std::string>("email","");
        return true;
    }
    else return false;
    /*MySqlResult res = db->query("SELECT users.login,users.pass,users.nickname FROM users WHERE `login`='"+login+"' AND `pass` = MD5('"+password+"');");
    RecursionArray result=res.get_all();
    if(!result.empty())
    {
        client->login=login;
        client->isAuth=true;
        client->permissionsLevel=1;
        client->nickname=result.get<std::string>("nickname","");
        return true;
    }
    else return false;*/

}
}
namespace RecArrUtils
{


void printTree(const RecursionArray& tree, const std::string& prefix)
{
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
            std::cout << prefix << v.first << ": \"" << v.second.get<std::string>("") << "\"\n" ;
        else
        {
            std::cout<< prefix << v.first << "\n" << prefix << "{";
            printTree(tmp,prefix+"\t");
            std::cout<< "\n" << prefix << "}\n" ;
        }
    }
}
std::string printTreeEx(const RecursionArray& tree, const std::string& prefix)
{
    std::stringstream steam;
    BOOST_FOREACH(auto &v, tree)
    {
        boost::property_tree::ptree tmp = v.second.get_child("");
        if(tmp.empty())
            steam << "\n" << prefix << v.first << ": \"" << v.second.get<std::string>("") << "\"";
        else
        {
            steam << "\n" << prefix << v.first << "\n" << prefix << "{" << printTreeEx(tmp,prefix+"\t") << "\n" << prefix << "}";
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
/*std::string toCfg(const RecursionArray& tree, const std::string& prefix)
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
}*/

char SlashReplaceEx(std::string* str, const unsigned int frist_pos)
{
    bool NoAdept=false;
    unsigned int size=str->size();
    char returnchar='b';
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
            else if(thch=='@')
            {
                if(i+1<size)
                returnchar=(*str)[i+1];
            }
        }
    }
    return returnchar;
}
RecursionArray fromArcan(const std::string &str)
{
    RecursionArray arr;
    int i=0,Shet=0;
    while(i<(int)str.size())
    {

        bool isReplaceA=false,isReplaceB=false;
        int first_pos=findNoSlash(str,'[',i,&isReplaceA);
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
        char typed = SlashReplaceEx(&first,0);
        //int second_pos=str.find(']',first_pos);
        int second_pos=findNoSlash(str,']',first_pos,&isReplaceB);
        if(second_pos<0) break;
        /*if(second_pos-1==first_pos)
        {
            i=second_pos+1;
            continue;
        }*/
        int recursion_pos=findNoSlash(str,'[',first_pos+1,&isReplaceB);
        if(recursion_pos>0)
        {
            if(recursion_pos<second_pos)
            {

                int recursions=0;
                while(recursion_pos<second_pos)
                {
                    recursion_pos=findNoSlash(str,'[',recursion_pos+1,&isReplaceB);

                    recursions++;
                    if(recursion_pos<0) break;
                }
                int last_second=second_pos;
                while(recursions>0)
                {
                    last_second=findNoSlash(str,']',last_second+1,&isReplaceB);
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
        if(typed=='0')
        {
            //arr.add(first,false);
            arr.push_back(RecursionArray::value_type(first, RecursionArray("false")));
        }
        else if(typed=='1')
        {
            //arr.add(first,true);
            arr.push_back(RecursionArray::value_type(first, RecursionArray("true")));
        }
        else if(typed=='i')
        {
            //arr.add(first,true);
            std::string second=str.substr(first_pos+1,second_pos-first_pos-1);
            if(isReplaceB)
                SlashReplace(&second,0);
            //arr.add(first,second);
            arr.push_back(RecursionArray::value_type(first, RecursionArray(std::to_string(byteToInt(second)))));

        }
        else if(typed=='l')
        {
            //arr.add(first,true);
            std::string second=str.substr(first_pos+1,second_pos-first_pos-1);
            if(isReplaceB)
                SlashReplace(&second,0);
            //arr.add(first,second);
            arr.push_back(RecursionArray::value_type(first, RecursionArray(std::to_string(byteToLong(second)))));

        }
        else
        {
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
            if(isReplaceB)
                SlashReplace(&second,0);
            //arr.add(first,second);
            arr.push_back(RecursionArray::value_type(first, RecursionArray(second)));
        }
        i=second_pos+1;
    }
    return arr;
}
}
