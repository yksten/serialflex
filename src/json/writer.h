#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__

#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

namespace serialflex {

namespace json {

class Writer {
    enum Type {
        NULL_TYPE = 0, //!< null
        KEY_TYPE = 1,  //!< key
        VALUE_TYPE = 2,//!< value
    };
    typedef std::pair<int32_t, uint32_t> value_type;
    std::vector<value_type> stack_;
    std::string& str_;
    bool formatted_;

public:
    explicit Writer(std::string& str, bool formatted = false): str_(str), formatted_(formatted) {}
    // key and value
    Writer& key(const char* key);
    void value(bool b);
    void value(int64_t i64);
    void value(uint64_t u64);
    void value(double d);
    void value(const char* value);
    //
    void startObject();
    void endObject();
    void startArray();
    void endArray();

    void separation() {
        if (!stack_.empty() && stack_.back().first == NULL_TYPE) {
            comma(str_);
        }
    }
    // :
    void colon(std::string& str) const;
    // ,
    void comma(std::string& str) const;
    // {
    void leftBrace(std::string& str) const;
    // }
    void rightBrace(std::string& str, int32_t layer) const;
    // [
    void leftBracket(std::string& str) const;
    // ]
    void rightBracket(std::string& str, int32_t layer) const;
    // \t
    void tab(std::string& str, int32_t layer) const;

    bool result() const { return stack_.empty(); }

private:
    void appendString(std::string& str, const char* value);
};

}// namespace json

}// namespace serialflex

#endif
