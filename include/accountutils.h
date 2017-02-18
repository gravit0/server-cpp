#ifndef ACCOUNTUTILS_H
#define ACCOUNTUTILS_H
#include "boost-server.hpp"
namespace AccountUtils
{
bool auth(boostserver::client::ptr client, std::string login, std::string password, mysqlpp::Connection *db);
}
#endif // ACCOUNTUTILS_H
