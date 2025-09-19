#ifndef __XML_WRITER_H__
#define __XML_WRITER_H__

#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

namespace serialflex {

namespace xml {

class Writer {
    std::string& str_;
    bool formatted_;
    int32_t layer_;

public:
    explicit Writer(std::string& str, bool formatted = false);
    // key and value
    Writer& startKey(const char* key);
    Writer& endKey(const char* key);
    Writer& value(bool b);
    Writer& value(int64_t i64);
    Writer& value(uint64_t u64);
    Writer& value(double d);
    Writer& value(const std::string& value);
    //
    void startObject(const char* name);
    void endObject(const char* name);

    bool result() const { return (layer_ == 0); }

private:
    // \t
    void tab(std::string& str, int32_t layer) const;
};

}// namespace xml

}// namespace serialflex

#endif
