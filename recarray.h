#ifndef RECARRAY_H
#define RECARRAY_H
#include <boost/property_tree/ptree.hpp>
#include <strstream>
typedef boost::property_tree::ptree RecursionArray;
namespace RecArrUtils
{
void printTree(const RecursionArray& tree);
RecursionArray fromArcan(const std::string& str);
std::string toArcan(const RecursionArray& tree);
std::string getString(const RecursionArray& tree,std::string key);
std::string printTreeEx(const RecursionArray& tree);
}
#endif // RECARRAY_H
