#include "databasehelper.h"
#include <mysql/mysql.h>
#include <iostream>
#include <stddef.h>
DatabaseHelper::DatabaseHelper()
{
    mysql_init(nativemysql);

}
bool DatabaseHelper::connect(const char* host,const char* user,const char* password,const char* database,int port)
{
    nativeconnect = mysql_real_connect(nativemysql,host,
                                 user,password,database,port,0,0);
    if (nativeconnect == nullptr) {
        std::cout << mysql_error(nativemysql) << std::endl;
        return false;
    }
    else return true;
}
