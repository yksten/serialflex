#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "writer.h"

namespace serialflex {

namespace json {

Writer& Writer::key(const char* key) {
    if (key) {
        value_type& vt = stack_.back();
        if (vt.second) {
            comma(str_);
        }
        tab(str_, (int32_t)stack_.size());
        appendString(str_, key);
        vt.second++;
        vt.first = KEY_TYPE;
    }
    return *this;
}

void Writer::value(bool b) {
    value_type& vt = stack_.back();
    if (vt.first == KEY_TYPE) {
        colon(str_);
    } else if (vt.first == VALUE_TYPE) {
        comma(str_);
        tab(str_, (int32_t)stack_.size());
    } else {
        tab(str_, (int32_t)stack_.size());
    }
    if (b) {
        str_.append("true");
    } else {
        str_.append("false");
    }
    vt.first = VALUE_TYPE;
}

void Writer::value(int64_t i64) {
    value_type& vt = stack_.back();
    if (vt.first == KEY_TYPE) {
        colon(str_);
    } else if (vt.first == VALUE_TYPE) {
        comma(str_);
        tab(str_, (int32_t)stack_.size());
    } else {
        tab(str_, (int32_t)stack_.size());
    }
    char buffer[32] = {0};
    snprintf(buffer, 32, "%lld", i64);
    str_.append(buffer);
    vt.first = VALUE_TYPE;
}

void Writer::value(uint64_t u64) {
    value_type& vt = stack_.back();
    if (vt.first == KEY_TYPE) {
        colon(str_);
    } else if (vt.first == VALUE_TYPE) {
        comma(str_);
        tab(str_, (int32_t)stack_.size());
    } else {
        tab(str_, (int32_t)stack_.size());
    }
    char buffer[32] = {0};
    snprintf(buffer, 32, "%llu", u64);
    str_.append(buffer);
    vt.first = VALUE_TYPE;
}

void Writer::value(double d) {
    value_type& vt = stack_.back();
    if (vt.first == KEY_TYPE) {
        colon(str_);
    } else if (vt.first == VALUE_TYPE) {
        comma(str_);
        tab(str_, (int32_t)stack_.size());
    } else {
        tab(str_, (int32_t)stack_.size());
    }
    char buffer[64] = {0};
    snprintf(buffer, 64, "%lf", d);
    str_.append(buffer);
    vt.first = VALUE_TYPE;
}

void Writer::value(const char* value) {
    value_type& vt = stack_.back();
    if (vt.first == KEY_TYPE) {
        colon(str_);
    } else if (vt.first == VALUE_TYPE) {
        comma(str_);
        tab(str_, (int32_t)stack_.size());
    } else {
        tab(str_, (int32_t)stack_.size());
    }
    appendString(str_, value);
    vt.first = VALUE_TYPE;
}

void Writer::startObject() {
    if (!stack_.empty()) {
        if (stack_.back().first == KEY_TYPE) {
            colon(str_);
        } else if (stack_.back().first == VALUE_TYPE) {
            comma(str_);
        } else {
            tab(str_, (int32_t)stack_.size());
        }
    }
    stack_.push_back(value_type(NULL_TYPE, 0));
    leftBrace(str_);
}

void Writer::endObject() {
    rightBrace(str_, (int32_t)stack_.size());
    stack_.pop_back();
}

void Writer::startArray() {
    if (!stack_.empty()) {
        if (stack_.back().first == KEY_TYPE) {
            colon(str_);
        } else if (stack_.back().first == VALUE_TYPE) {
            comma(str_);
        } else {
            tab(str_, (int32_t)stack_.size());
        }
    }
    leftBracket(str_);
    stack_.push_back(value_type(NULL_TYPE, 0));
}

void Writer::endArray() {
    rightBracket(str_, (int32_t)stack_.size());
    stack_.pop_back();
}

void Writer::colon(std::string& str) const {
    str.append(1, ':');
    if (formatted_) {
        str.append(1, ' ');
    }
}

void Writer::comma(std::string& str) const {
    str.append(1, ',');
    if (formatted_) {
        str.append(1, '\n');
    }
}

void Writer::leftBrace(std::string& str) const {
    str.append(1, '{');
    if (formatted_) {
        str.append(1, '\n');
    }
}

void Writer::rightBrace(std::string& str, int32_t layer) const {
    if (formatted_) {
        str.append(1, '\n');
        tab(str, layer - 1);
    }
    str.append(1, '}');
}

void Writer::leftBracket(std::string& str) const {
    str.append(1, '[');
    if (formatted_) {
        str.append(1, '\n');
    }
}

void Writer::rightBracket(std::string& str, int32_t layer) const {
    if (formatted_) {
        str.append(1, '\n');
        tab(str, layer - 1);
    }
    str.append(1, ']');
}

void Writer::tab(std::string& str, int32_t layer) const {
    if (formatted_) {
        for (int32_t idx = 0; idx < layer; ++idx) {
            str.append(1, '\t');
        }
    }
}

void Writer::appendString(std::string& str, const char* value) {
    str.append(1, '\"');
    for (const char* ptr = value; *ptr; ++ptr) {
        if ((unsigned char)*ptr > 31 && *ptr != '\"' && *ptr != '\\') {
            str.append(1, *ptr);
        } else {
            str.append(1, '\\');
            switch (*ptr) {
                case '\\':
                    str.append(1, '\\');
                    break;
                case '\"':
                    str.append(1, '\"');
                    break;
                case '\b':
                    str.append(1, 'b');
                    break;
                case '\f':
                    str.append(1, 'f');
                    break;
                case '\n':
                    str.append(1, 'n');
                    break;
                case '\r':
                    str.append(1, 'r');
                    break;
                case '\t':
                    str.append(1, 't');
                    break;
                default: {
                    size_t nSize = str.size();
                    str.resize(nSize + 5, '\0');
                    sprintf(&str.at(nSize), "u%04x", *ptr);
                    break;
                }
            }
        }
    }
    str.append(1, '\"');
}

}// namespace json

}// namespace serialflex
