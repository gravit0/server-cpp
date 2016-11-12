#ifndef RECARRAY_H
#define RECARRAY_H
#include <boost/property_tree/ptree.hpp>

typedef boost::property_tree::ptree RecursionArray;
namespace RecArrUtils
{
void printTree(const RecursionArray& tree);
RecursionArray fromArcan(const std::string& str);
std::string toArcan(const RecursionArray& tree);
std::string toCfg(const RecursionArray& tree, const std::string& prefix="");
std::string printTreeEx(const RecursionArray& tree);
}
#endif // RECARRAY_H
