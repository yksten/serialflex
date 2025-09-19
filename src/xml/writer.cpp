#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "writer.h"

namespace serialflex {

namespace xml {

Writer::Writer(std::string& str, bool formatted): str_(str), formatted_(formatted), layer_(0) {
    str_.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    if (formatted_) {
        str_.append(1, '\n');
    }
}

Writer& Writer::startKey(const char* key) {
    if (key) {
        tab(str_, layer_);
        str_.append(1, '<').append(key).append(1, '>');
    }
    return *this;
}

Writer& Writer::endKey(const char* key) {
    if (key) {
        str_.append(1, '<').append(1, '/').append(key).append(1, '>');
        if (formatted_) {
            str_.append(1, '\n');
        }
    }
    return *this;
}

Writer& Writer::value(bool b) {
    if (b) {
        str_.append("true");
    } else {
        str_.append("false");
    }

    return *this;
}

Writer& Writer::value(int64_t i64) {
    char buffer[32] = {0};
    snprintf(buffer, 32, "%lld", i64);
    str_.append(buffer);

    return *this;
}

Writer& Writer::value(uint64_t u64) {
    char buffer[32] = {0};
    snprintf(buffer, 32, "%llu", u64);
    str_.append(buffer);

    return *this;
}

Writer& Writer::value(double d) {
    char buffer[64] = {0};
    snprintf(buffer, 64, "%lf", d);
    str_.append(buffer);

    return *this;
}

Writer& Writer::value(const std::string& value) {
    if ((value.find("![CDATA[") == 0) && (value.find("]]") == value.size() - 2)) {
        str_.append(1, '<').append(value).append(1, '>');
    } else {
        for (const char& c : value) {
            if (c == '<') {
                str_.append("&lt;");
            } else if (c == '>') {
                str_.append("&gt;");
            } else if (c == '\'') {
                str_.append("&apos;");
            } else if (c == '"') {
                str_.append("&quot;");
            } else if (c == '&') {
                str_.append("&amp;");
            } else {
                str_.append(1, c);
            }
        }
    }

    return *this;
}

void Writer::startObject(const char* name) {
    tab(str_, layer_);
    str_.append(1, '<').append(name).append(1, '>');
    if (formatted_) {
        str_.append(1, '\n');
    }

    layer_++;
}

void Writer::endObject(const char* name) {
    assert(layer_ > 0);
    layer_--;

    tab(str_, layer_);
    str_.append(1, '<').append(1, '/').append(name).append(1, '>');
    if (formatted_) {
        str_.append(1, '\n');
    }
}

void Writer::tab(std::string& str, int32_t layer) const {
    if (formatted_) {
        for (int32_t idx = 0; idx < layer; ++idx) {
            str.append(1, '\t');
        }
    }
}

}// namespace xml

}// namespace serialflex
