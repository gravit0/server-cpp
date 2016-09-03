#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H
#include <mysql/mysql.h>
class DatabaseHelper
{
private:
    MYSQL* nativemysql,*nativeconnect;
public:
    DatabaseHelper();
    bool connect(const char* host,const char* user,const char* password,const char* database,int port);
};

#endif // DATABASEHELPER_H
