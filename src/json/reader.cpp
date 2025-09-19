#include <assert.h>
#include "reader.h"

namespace serialflex {

namespace json {

class StringStream {
    const char* src_;

public:
    typedef const char value_type;
    explicit StringStream(const char* src): src_(src) {}
    value_type Peek() const {
        if (isEnd()) {
            return '\0';
        }
        return *src_;
    }
    value_type Second2Last() const { return *(src_ - 1); }
    value_type Take() { return *src_++; }
    value_type* Strart() const { return src_; }
    bool isEnd() const { return (*src_ == '\0'); }
};

/*------------------------------------------------------------------------------*/

Reader::Reader(): cur_value_(NULL), alloc_(values_) {}

Reader::~Reader() {}

int64_t Reader::convertInt(const char* value, uint32_t length) {
    if (!length) {
        return 0;
    }

    int64_t result = 0;
    bool bMinus = false;
    if (value[0] == '-') {
        bMinus = true;
    }
    for (uint32_t idx = bMinus; idx < length; ++idx) {
        result *= 10;
        result += value[idx] - '0';
    }
    if (bMinus) {
        result = 0 - result;
    }
    return result;
}

uint64_t Reader::convertUint(const char* value, uint32_t length) {
    if (!length) {
        return 0;
    }

    uint64_t result = 0;
    for (uint32_t idx = 0; idx < length; ++idx) {
        result *= 10;
        result += value[idx] - '0';
    }
    return result;
}

static inline double decimal(uint8_t n, uint32_t num) {
    double db = n * 1.0f;
    while (num--) {
        db = db / 10;
    }
    return db;
}

double Reader::convertDouble(const char* value, uint32_t length) {
    if (!length) {
        return 0.0f;
    }

    unsigned char* after_end = NULL;
    unsigned char number_c_string[64] = {0};

    for (uint32_t idx = 0; idx < length; ++idx) {
        number_c_string[idx] = value[idx];
    }
    double result = strtod((const char*)number_c_string, (char**)&after_end);
    if (after_end != (number_c_string + length)) {
        return 0.0f; /* parse_error */
    }
    return result;
}

const GenericNode* Reader::parse(const char* src) {
    do {
        ++alloc_;
        StringStream is(src);
        skipWhitespace(is);
        if (is.Peek() != '\0') {
            parseValue(is);
        }
    } while (false);

    //
    alloc_.reSize();

    GenericNode* root = alloc_.allocValue();
    cur_value_ = root;

    StringStream is(src);
    skipWhitespace(is);
    if (is.Peek() != '\0') {
        parseValue(is);
    }
    if (!str_error_.empty()) {
        return NULL;
    }
    return root;
}

void Reader::parseValue(StringStream& is) {
    switch (is.Peek()) {
        case 'n':
            parseNull(is);
            break;
        case 't':
            parseTrue(is);
            break;
        case 'f':
            parseFalse(is);
            break;
        case '"':
            parseString(is);
            break;
        case '{': {
            GenericNode* parent = cur_value_;
            setItemType(VALUE_OBJECT);
            parseObject(is);
            cur_value_ = parent;
        } break;
        case '[': {
            GenericNode* parent = cur_value_;
            setItemType(VALUE_ARRAY);
            parseArray(is);
            cur_value_ = parent;
        } break;
        default:
            parseNumber(is);
            break;
    }
    if (!str_error_.empty()) {
        return;
    }
}

void Reader::parseKey(StringStream& is) {
    assert(is.Peek() == '\"');
    is.Take();// Skip '\"'
    const char* szStart = is.Strart();

    for (; is.Peek() != '\0'; is.Take()) {
        if (is.Peek() == '\"') {
            setItemKey(szStart, (uint32_t)(is.Strart() - szStart));
            is.Take();// Skip '\"'
            return;
        }
    }
    setError("KeyInvalid");
}

void Reader::parseNull(StringStream& is) {
    assert(is.Peek() == 'n');
    is.Take();

    if (consume(is, 'u') && consume(is, 'l') && consume(is, 'l')) {
        setItemValue(VALUE_NULL, "null", 4);
    } else {
        setError("ValueInvalid");
    }
}

void Reader::parseTrue(StringStream& is) {
    assert(is.Peek() == 't');
    const char* szStart = is.Strart();
    is.Take();

    if (consume(is, 'r') && consume(is, 'u') && consume(is, 'e')) {
        setItemValue(VALUE_BOOL, szStart,
                     (uint32_t)(is.Strart() - szStart));
    } else {
        setError("ValueInvalid");
    }
}

void Reader::parseFalse(StringStream& is) {
    assert(is.Peek() == 'f');
    const char* szStart = is.Strart();
    is.Take();

    if (consume(is, 'a') && consume(is, 'l') && consume(is, 's') &&
        consume(is, 'e')) {
        setItemValue(VALUE_BOOL, szStart,
                     (uint32_t)(is.Strart() - szStart));
    } else {
        setError("ValueInvalid");
    }
}

void Reader::parseString(StringStream& is) {
    assert(is.Peek() == '\"');
    is.Take();// Skip '\"'
    const char* szStart = is.Strart();

    for (; is.Peek() != '\0'; is.Take()) {
        if (is.Peek() == '\"' && is.Second2Last() != '\\') {
            setItemValue(VALUE_STRING, szStart,
                         (uint32_t)(is.Strart() - szStart));
            is.Take();// Skip '\"'
            return;
        }
    }
    setError("ValueInvalid");
}

void Reader::parseNumber(StringStream& is) {
    const char* szStart = is.Strart();

    for (; is.Peek() != '\0'; is.Take()) {
        if (is.Peek() == '-' || is.Peek() == '.' ||
            (is.Peek() >= '0' && is.Peek() <= '9')) {
            continue;
        } else {
            setItemValue(VALUE_NUMBER, szStart,
                         (uint32_t)(is.Strart() - szStart));
            return;
        }
    }
    setError("ValueInvalid");
}

void Reader::parseArray(StringStream& is) {
    assert(is.Peek() == '[');
    is.Take();// Skip '['

    skipWhitespace(is);
    if (consume(is, ']')) {
        return;
    }

    for (uint32_t elementIndex = 0;;) {
        getChildItem(elementIndex);
        parseValue(is);
        ++elementIndex;
        skipWhitespace(is);

        if (consume(is, ',')) {
            skipWhitespace(is);
        } else if (consume(is, ']')) {
            return;
        } else {
            setError("ParseErrorArrayMissCommaOrSquareBracket");
            return;
        }
    }
    setError("ValueArrayInvalid");
}

void Reader::parseObject(StringStream& is) {
    assert(is.Peek() == '{');
    //        const char* szStart = is.Strart();
    is.Take();// Skip '{'

    skipWhitespace(is);
    if (is.Peek() == '}') {
        is.Take();
        return;
    }

    for (uint32_t elementIndex = 0; !is.isEnd();) {
        if (is.Peek() != '"') {
            setError("ObjectMissName");
            return;
        }
        getChildItem(elementIndex);
        parseKey(is);

        skipWhitespace(is);
        if (!consume(is, ':')) {
            setError("ObjectMissColon");
            return;
        }

        skipWhitespace(is);
        parseValue(is);
        ++elementIndex;

        skipWhitespace(is);
        switch (is.Peek()) {
            case ',':
                is.Take();
                skipWhitespace(is);
                break;
            case '}':
                is.Take();
                return;
            default:
                setError("ObjectMissCommaOrCurlyBracket");
                break;// This useless break is only for making warning and
                      // coverage happy
        }
    }
}

void Reader::setItemType(const int32_t type) {
    if (cur_value_) {
        assert(VALUE_NULL <= type &&
               type <= VALUE_OBJECT);
        cur_value_->type = type;
    }
}

void Reader::getChildItem(const uint32_t element_index) {
    if (cur_value_) {
        GenericNode* temp = cur_value_;
        cur_value_ = alloc_.allocValue();
        cur_value_->prev = temp;
        if (!element_index) {
            temp->child = cur_value_;
        } else {
            temp->next = cur_value_;
        }
    } else {
        ++alloc_;
    }
}

void Reader::setItemKey(const char* key, const uint32_t key_size) {
    if (cur_value_) {
        assert(key);
        assert(key_size);
        cur_value_->key = key;
        cur_value_->key_size = key_size;
    }
}

void Reader::setItemValue(const int32_t type, const char* value,
                          const uint32_t value_size) {
    if (cur_value_) {
        assert(VALUE_NULL <= type &&
               type <= VALUE_OBJECT);
        assert(value);
        assert(type == VALUE_STRING || value_size);
        cur_value_->type = type;
        cur_value_->value = value;
        cur_value_->value_size = value_size;
    }
}

bool Reader::consume(StringStream& is, const char expect) {
    if (is.Peek() == expect) {
        is.Take();
        return true;
    } else
        return false;
}

void Reader::skipWhitespace(StringStream& is) {
    for (;;) {
        const char c = is.Peek();
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            is.Take();
        } else {
            break;
        }
    }
}

}// namespace json

}// namespace serialflex
