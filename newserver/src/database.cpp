#include "database.h"
#include <iostream>
Database::Database()
{
    mysql_init(&mysql);
}
void Database::connect(std::string dbhost, int dbport,std::string dbuser,std::string dbpass,std::string dbname)
{
    connection = mysql_real_connect(&mysql,dbhost.data(),
                                    dbuser.data(),dbpass.data(),dbname.data(),dbport,0,0);
    if (connection == NULL) {
        std::cout << mysql_error(&mysql) << std::endl;
    }
}
MySqlResult Database::query(std::string sql)
{
    MYSQL_RES *result;
    //MYSQL_ROW row;
    int query_state;
    query_state = mysql_query(connection, sql.data());
    if (query_state !=0) {
        std::cout << mysql_error(connection) << std::endl;
    }
    result = mysql_store_result(connection);
    //while (( row = mysql_fetch_row(result)) != NULL) {
    //    std::cout << "Number of active users : " << row[0] << std::endl;
    //}
    MySqlResult result2(result);
    return result2;
    //mysql_free_result(result);
}
MySqlResult::~MySqlResult()
{
    mysql_free_result(result);
}
MySqlResult::MySqlResult(MYSQL_RES* res)
{
    result=res;
}
void Database::close()
{

}
void MySqlResult::print()
{
    MYSQL_FIELD* fd ;
    MYSQL_ROW row ;
    unsigned int i,num_fields;
    num_fields = mysql_num_fields(result);
    fd = mysql_fetch_fields(result);
    for(unsigned int j = 0; j < num_fields; j++)
    {
        std::cout << fd[j].name << " | ";
    }
    std::cout << std::endl;
    //Последовательно выводим все полученные данные, пока в структуре еще есть записи.
    while ((row = mysql_fetch_row(result)))
    {
    for(i = 0; i < num_fields; i++)
    {
    std::cout << row[i] << " | ";
    }
    std::cout << std::endl;
    }
    mysql_data_seek(result,0);
}
std::string MySqlResult::value(std::string pole)
{
    MYSQL_FIELD* fd ;
    MYSQL_ROW row ;
    unsigned int num_fields;
    num_fields = mysql_num_fields(result);
    fd = mysql_fetch_fields(result);
    for(unsigned int i = 0; i < num_fields; i++)
    {
        if(std::string(fd[i].name,fd[i].name_length)==pole)
        {
            row = mysql_fetch_row(result);
            std::string rmR(row[i]);
            return rmR;
        }
    }
    return "";
}
RecursionArray MySqlResult::get_all()
{
    RecursionArray resultr;
    MYSQL_FIELD* fd ;
    MYSQL_ROW row ;
    unsigned int num_fields;
    num_fields = mysql_num_fields(result);
    fd = mysql_fetch_fields(result);
    for(unsigned int i = 0; i < num_fields; i++)
    {
        std::string first(fd[i].name,fd[i].name_length);
        std::string second(row[i]);
        resultr.push_back(RecursionArray::value_type(first, RecursionArray(second)));
    }
    return resultr;
}
void MySqlResult::seek(int i)
{
    mysql_data_seek(result,i);
}
