#ifndef __GENERIC_READER_H__
#define __GENERIC_READER_H__

#include <cassert>
#include <cstdlib>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace custom {

struct GenericValue {
    enum {
        VALUE_NULL = 0,
        VALUE_BOOL = 1,
        VALUE_NUMBER = 2,
        VALUE_STRING = 3,
        VALUE_ARRAY = 4,
        VALUE_OBJECT = 5
    };

    int32_t type;

    const char* key;
    uint32_t key_size;

    const char* value;
    uint32_t value_size;

    struct GenericValue* prev;
    struct GenericValue* next;
    struct GenericValue* child;

    GenericValue()
        : type(VALUE_NULL), key(NULL), key_size(0), value(NULL), value_size(0), prev(NULL),
          next(NULL), child(NULL) {}
};

class StringStream;
class Reader {
    class GenericValueAllocator {
        uint32_t cur_ndex_;
        uint32_t capacity_;
        std::vector<GenericValue>& vec_;

    public:
        explicit GenericValueAllocator(std::vector<GenericValue>& vec)
            : cur_ndex_(0), capacity_(0), vec_(vec) {}
        void operator++() { ++capacity_; }
        void reSize() {
            assert(vec_.empty());
            vec_.resize(capacity_);
        }
        GenericValue* allocValue() {
            assert(cur_ndex_ < vec_.size());
            return &vec_.at(cur_ndex_++);
        }
    };
    GenericValue* cur_value_;
    std::vector<GenericValue> values_;
    GenericValueAllocator alloc_;
    std::string str_error_;

public:
    Reader();
    ~Reader();
    const GenericValue* parse(const char* src);
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
    void setItemValue(const int32_t type, const char* value, const uint32_t value_size);

    static bool consume(StringStream& is, const char expect);
    static void skipWhitespace(StringStream& is);
};

}// namespace custom

#endif
