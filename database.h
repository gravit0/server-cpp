#ifndef DATABASE_H
#define DATABASE_H
#include "mysql/mysql.h"
#include <string>
#include <recarray.h>
struct MySqlResult
{
    MYSQL_RES *result;
    ~MySqlResult();
    MySqlResult(MYSQL_RES *res);
    void print();
    void seek(int i);
    RecursionArray get_all();
    std::string value(std::string pole);
};

class Database
{
public:
    Database();
    void connect(std::string dbhost, int dbport,std::string dbuser,std::string dbpass,std::string dbname);
    MySqlResult query(std::string sql);
    void close();
protected:
    MYSQL *connection, mysql;
};

#endif // DATABASE_H
