#ifndef __JSON_READER_H__
#define __JSON_READER_H__

#include <cstdlib>
#include <ctype.h>
#include <stdio.h>
#include <string>
#include "node.h"

namespace serialflex {

namespace json {

enum {
    VALUE_NULL = 0,
    VALUE_BOOL = 1,
    VALUE_NUMBER = 2,
    VALUE_STRING = 3,
    VALUE_ARRAY = 4,
    VALUE_OBJECT = 5
};

class StringStream;
class Reader {
    GenericNode* cur_value_;
    std::vector<GenericNode> values_;
    GenericValueAllocator<GenericNode> alloc_;
    std::string str_error_;

public:
    Reader();
    ~Reader();
    const GenericNode* parse(const char* src);
    const char* getError() const { return str_error_.c_str(); }

    static int64_t convertInt(const char* value, uint32_t length);
    static uint64_t convertUint(const char* value, uint32_t length);
    static double convertDouble(const char* value, uint32_t length);

private:
    void setError(const char* error) { str_error_ = error; }
    void parseValue(StringStream& is);
    void parseKey(StringStream& is);
    void parseNull(StringStream& is);
    void parseTrue(StringStream& is);
    void parseFalse(StringStream& is);
    void parseString(StringStream& is);
    void parseNumber(StringStream& is);
    void parseArray(StringStream& is);
    void parseObject(StringStream& is);

    void setItemType(const int32_t type);
    void getChildItem(const uint32_t element_index);
    void setItemKey(const char* key, const uint32_t key_size);
    void setItemValue(const int32_t type, const char* value,
                      const uint32_t value_size);

    static bool consume(StringStream& is, const char expect);
    static void skipWhitespace(StringStream& is);
};

}// namespace json

}// namespace serialflex

#endif
