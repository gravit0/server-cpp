#ifndef BASEFUNCIONS_H
#define BASEFUNCIONS_H
#include <string>
#include <sstream>
long long int byteToLong(std::string buffer);
int byteToInt(std::string buffer);
std::string IntToByte(int integer);
void SlashReplace(std::string* str, const unsigned int frist_pos);
int findNoSlash(const std::string& str,const char ch, const unsigned int frist_pos,bool* isReplace);
#endif // BASEFUNCIONS_H
