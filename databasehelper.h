#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include <mysql/mysql.h>
#include <string>
#include <map>
#include <list>
namespace DatabaseHelper
{
bool connect(const char* host,const char* user,const char* password,const char* database,int port);
bool init();
std::list<std::map<std::string, std::string> > query(std::string query);
}

#endif // DATABASEHELPER_H
