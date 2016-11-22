#include "logger.h"
#include "boost-server.hpp"
using namespace boostserver;
Logger::Logger()
{

}
//template<typename T>
//Logger& Logger::operator<< (const T& arg) {
//    stream << arg;
//    cout << arg;
//    if(stream.str().size()>1024)
//    {
//        boostserver::service.savelog();
//    }
//    return *this;
//}
Logger::operator std::string() const {
    return stream.str();
}
