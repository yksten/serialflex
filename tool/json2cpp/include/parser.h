#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>

#ifdef _MSC_VER
#ifdef EXPORTAPI
#define EXPORTAPI _declspec(dllimport)
#else
#define EXPORTAPI _declspec(dllexport)
#endif
#else
#define EXPORTAPI
#endif

namespace json2cpp {

EXPORTAPI std::string buildStruct(const char* json);

}// namespace json2cpp

#endif
