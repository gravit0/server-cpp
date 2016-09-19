#include "databasehelper.h"
#include <mysql/mysql.h>
#include <iostream>
namespace DatabaseHelper{
MYSQL *nativemysql,*nativeconnect;
bool init()
{
    mysql_init(nativemysql);
}
bool connect(const char* host,const char* user,const char* password,const char* database,int port)
{
    nativeconnect = mysql_real_connect(nativemysql,host,
                                 user,password,database,port,0,0);
    if (nativeconnect == nullptr) {
        std::cout << mysql_error(nativemysql) << std::endl;
        return false;
    }
    else return true;
}

std::list<std::map<std::string,std::string>> query(std::string query)
{
    if (mysql_query(nativeconnect, query.data()) > 0) // запорс. Если ошибок нет, то продолжаем работу
    {

        // Если была ошибка, ...
        std::cout << mysql_error(nativeconnect) << std::endl;
        return std::list<std::map<std::string,std::string>>(); // и завершим работу
    }
    MYSQL_RES *res = mysql_store_result(nativeconnect); // Берем результат,
    int num_fields = mysql_num_fields(res); // количество полей
    int num_rows = mysql_num_rows(res); // и количество строк.

    for (int i = 0; i < num_fields; i++) // Выводим названия полей
    {
        MYSQL_FIELD* field = mysql_fetch_field_direct(res, i); // Получение названия текущего поля
        printf("| %s |", field->name);
    }
    for (int i = 0; i < num_rows; i++) // Вывод таблицы
    {
        MYSQL_ROW row = mysql_fetch_row(res); // получаем строку
        std::map<std::string,std::string> me;
        for (int l = 0; l < num_fields; l++)
        {
            MYSQL_FIELD* field = mysql_fetch_field_direct(res, i);
        }

        printf("\n");
    }

    printf("Count records = %d", num_rows); // Вывод информации о количестве записей
    mysql_free_result(res); // Очищаем результаты
}
}
