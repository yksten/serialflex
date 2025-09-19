#include <assert.h>
#include <ctype.h>
#include "reader.h"

namespace serialflex {

namespace xml {

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

unsigned char Reader::isHexChas(const unsigned char c) {
    if ('0' <= c && c <= '9') {
        return (c - '0');
    }
    if ('A' <= c && c <= 'F') {
        return (c - 'A' + 10);
    }
    if ('a' <= c && c <= 'f') {
        return (c - 'a' + 10);
    }
    return 0xFF;
}

const GenericNode* Reader::parse(const char* src) {
    assert(src);
    // Parse BOM, if any
    Reader::skipBom(src);
    // Skip whitespace before node
    skip<Reader::is_9_10_13_32>(src);

    const char* text = src;
    do {
        ++alloc_;
        // Parse children
        for (;;) {
            // Skip whitespace before node
            skip<Reader::is_9_10_13_32>(src);
            if (*src == 0) {
                break;
            }
            // Parse
            if (*src == '<') {
                ++src;// Skip '<'
                parseNode(src);
            } else {
                setError("expected <");
                break;
            }
        }
    } while (false);

    //
    alloc_.reSize();
    GenericNode* root = alloc_.allocValue();
    root->type = NODE_DOCUMENT;
    cur_value_ = root;
    // Parse children
    for (int32_t idx = 0;; ++idx) {
        // Skip whitespace before node
        skip<Reader::is_9_10_13_32>(text);
        if (*text == 0) {
            break;
        }
        // Parse
        if (*text == '<') {
            ++text;// Skip '<'
            GenericNode* temp = cur_value_;
            parseNode(text);
            cur_value_ = temp;
        } else {
            setError("expected <");
            break;
        }
    }

    if (!str_error_.empty()) {
        return NULL;
    }
    return Reader::getResult(root);
}

const char* Reader::getError() const {
    if (str_error_.empty()) {
        return NULL;
    }
    return str_error_.c_str();
}

GenericNode* Reader::getResult(GenericNode* root) {
    if (!root) {
        return NULL;
    }
    for (GenericNode* item = root->child; item; item = item->next) {
        if (item->type == NODE_ELEMENT && item->key_size == strlen("serialflex") &&
            strncmp("serialflex", item->key, item->key_size) == 0) {
            return item;
        }
    }
    return NULL;
}

void Reader::parseNode(const char*& src) {
    allocNode();
    // Parse proper node type
    switch (src[0]) {
        // <...
        default: {
            // Parse and append element node
            parseElement(src);
        } break;
        // <?...
        case ('?'): {
            ++src;// Skip ?
            if ((src[0] == 'x' || src[0] == 'X') && (src[1] == 'm' || src[1] == 'M') &&
                (src[2] == 'l' || src[2] == 'L') && Reader::is_9_10_13_32(src[3])) {
                // '<?xml ' - xml declaration
                src += 4;// Skip 'xml '
                skipXmlDeclaration(src);
                return;
            } else {
                // Parse PI
                skipPi(src);
                return;
            }
        } break;
        // <!...
        case ('!'): {
            // Parse proper subset of <! node
            switch (src[1]) {
                // <!-
                case ('-'):
                    if (src[2] == ('-')) {
                        // '<!--' - xml comment
                        src += 3;// Skip '!--'
                        skipComment(src);
                        return;
                    }
                    break;
                    // <![
                case ('['):
                    if (src[2] == 'C' && src[3] == 'D' && src[4] == 'A' && src[5] == 'T' &&
                        src[6] == 'A' && src[7] == '[') {
                        // '<![CDATA[' - cdata
                        parseCdata(src);
                        return;
                    }
                    break;
                    // <!D
                case ('D'):
                    if (src[2] == 'O' && src[3] == 'C' && src[4] == 'T' && src[5] == 'Y' &&
                        src[6] == 'P' && src[7] == 'E' && Reader::is_9_10_13_32(src[8])) {
                        // '<!DOCTYPE ' - doctype
                        src += 9;// skip '!DOCTYPE '
                        skipDoctype(src);
                        return;
                    }
                    break;
            }// switch

            // Attempt to skip other, unrecognized node types starting with <!
            ++src;// Skip !
            while (*src != '>') {
                if (*src == 0) {
                    setError("unexpected end of data");
                    break;
                }
                ++src;
            }
            ++src; // Skip '>'
            return;// No node recognized
        } break;
    }
}

bool Reader::parseElement(const char*& src) {
    setNodeType(NODE_ELEMENT);

    // Extract element name
    const char* name = src;
    skip<Reader::is_not_0_9_10_13_32_47_62_63>(src);
    if (src == name) {
        setError("expected element name");
        return false;
    }
    setNodeKey(name, uint32_t(src - name));

    // Skip whitespace between element name and attributes or >
    skip<Reader::is_9_10_13_32>(src);

    // Parse attributes, if any
    skipNodeAttributes(src);

    // Determine ending type
    if (*src == '>') {
        ++src;
        parseNodeContents(src);
    } else if (*src == '/') {
        ++src;
        if (*src != '>') {
            setError("expected >");
            return false;
        }
        ++src;
    } else {
        setError("expected >");
        return false;
    }
    return true;
}

void Reader::parseNodeContents(const char*& src) {
    // For all children and text
    for (;;) {
        if (getError()) {
            return;
        }
        // Skip whitespace between > and node contents
        skip<Reader::is_9_10_13_32>(src);
        const char next_char = *src;

        // After data nodes, instead of continuing the loop, control jumps here.
        // This is because zero termination inside parse_and_append_data() function
        // would wreak havoc with the above code.
        // Also, skipping whitespace after data nodes is unnecessary.

        // Determine what comes next: node closing, child node, data node, or 0?
        if (next_char == '\0') {
            // End of data - error
            setError("unexpected end of data");
            return;
        }
        if (next_char == '<') {
            // Node closing or child node
            if (src[1] == '/') {
                // Node closing
                src += 2;// Skip '</'
                // Skip and validate closing tag name
                const char* closing_name = src;
                skip<Reader::is_not_0_9_10_13_32_47_62_63>(src);
                if (cur_value_ &&
                    !Reader::compare(cur_value_->key, cur_value_->key_size, closing_name,
                                     uint32_t(src - closing_name), true)) {
                    setError("invalid closing tag name");
                    return;
                }
                // Skip remaining whitespace after node name
                skip<Reader::is_9_10_13_32>(src);
                if (*src != '>') {
                    setError("expected >");
                    return;
                }
                ++src; // Skip '>'
                return;// Node closed, finished parsing contents
            } else {
                // Child node
                ++src;// Skip '<'
                GenericNode* temp = cur_value_;
                parseNode(src);
                cur_value_ = temp;
            }
        } else {
            // Data node
            parseAndAppendData(src);
            // Bypass regular processing after data nodes
        }
    }
}

void Reader::parseAndAppendData(const char*& src) {
    // Skip until end of data
    const char* start = src;
    const char* end = NULL;
    end = skipAndExpandCharacterRefs(src);

    // If characters are still left between end and value (this test is only necessary if
    // normalization is enabled) Create new data node
    if (end) {
        // setNodeType(GenericValue::NODE_DATA);
        setNodeValue(start, uint32_t(end - start));
    }
}

const char* Reader::skipAndExpandCharacterRefs(const char*& src) {
    // Use simple skip until first modification is detected
    skip<Reader::is_not_0_38_60>(src);

    // Use translation skip
    for (; Reader::is_not_0_60(*src);) {
        // Test if replacement is needed
        if (src[0] == '&') {
            if (src[1] == 'a') {
                // &amp; &apos;
                if (src[2] == 'm' && src[3] == 'p' && src[4] == ';') {
                    src += 5;
                    continue;
                }
                if (src[2] == 'p' && src[3] == 'o' && src[4] == 's' && src[5] == ';') {
                    src += 6;
                    continue;
                }
            } else if (src[1] == 'q') {
                // &quot;
                if (src[2] == 'u' && src[3] == 'o' && src[4] == 't' && src[5] == ';') {
                    src += 6;
                    continue;
                }
            } else if (src[1] == 'g') {
                // &gt;
                if (src[2] == 't' && src[3] == ';') {
                    src += 4;
                    continue;
                }
            } else if (src[1] == 'l') {
                // &lt;
                if (src[2] == 't' && src[3] == ';') {
                    src += 4;
                    continue;
                }
            } else if (src[1] == '#') {
                // &#...; - assumes ASCII
                if (src[2] == 'x') {
                    unsigned long code = 0;
                    src += 3;// Skip &#x
                    while (1) {
                        unsigned char digit = Reader::isHexChas(static_cast<unsigned char>(*src));
                        if (digit == 0xFF) {
                            break;
                        }
                        code = code * 16 + digit;
                        ++src;
                    }
                    if (code >= 0x110000) {
                        // Invalid, only codes up to 0x10FFFF are allowed in Unicode
                        setError("invalid numeric character entity");
                        return NULL;
                    }
                } else {
                    unsigned long code = 0;
                    src += 2;// Skip &#
                    while (1) {
                        unsigned char digit = Reader::isHexChas(static_cast<unsigned char>(*src));
                        if (digit == 0xFF) {
                            break;
                        }
                        code = code * 10 + digit;
                        ++src;
                    }
                    if (code >= 0x110000) {
                        // Invalid, only codes up to 0x10FFFF are allowed in Unicode
                        setError("invalid numeric character entity");
                        return NULL;
                    }
                }
                if (*src == ';') {
                    ++src;
                } else {
                    setError("expected ;");
                    return NULL;
                }
                continue;
            }
        }
        ++src;
    }

    // Return new end
    return src;
}

void Reader::setNodeType(const int32_t type) {
    if (cur_value_) {
        cur_value_->type = type;
    }
}

void Reader::setNodeKey(const char* key, const uint32_t key_size) {
    if (cur_value_) {
        cur_value_->key = key;
        cur_value_->key_size = key_size;
    }
}

void Reader::setNodeValue(const char* value, const uint32_t value_size) {
    if (cur_value_) {
        cur_value_->value = value;
        cur_value_->value_size = value_size;
    }
}

void Reader::allocNode() {
    if (cur_value_) {
        GenericNode* temp = cur_value_;
        cur_value_ = alloc_.allocValue();
        cur_value_->prev = temp;
        for (GenericNode **child = &temp->child, *prev = temp;;
             prev = *child, child = &(*child)->next) {
            if (!*child) {
                *child = cur_value_;
                cur_value_->prev = prev;
                break;
            }
        }
    } else {
        ++alloc_;
    }
}

bool Reader::skipXmlDeclaration(const char*& src) {
    // parsing of declaration is disabled
    // Skip until end of declaration
    while (src[0] != '?' || src[1] != '>') {
        if (!src[0]) {
            setError("unexpected end of data");
            return false;
        }
        ++src;
    }
    src += 2;// Skip '?>'

    setNodeType(NODE_DECLARATION);
    return true;
}

bool Reader::skipPi(const char*& src) {
    // parsing of pi is disabled
    // Skip to '?>'
    while (src[0] != '?' || src[1] != '>') {
        if (*src == '\0') {
            setError("unexpected end of data");
            return false;
        }
        ++src;
    }
    src += 2;// Skip '?>'

    setNodeType(NODE_PI);
    return true;
}

bool Reader::skipComment(const char*& src) {
    // parsing of comments is disabled
    // Skip until end of comment
    for (; (src[0] != '-' || src[1] != '-' || src[2] != '>');) {
        if (!src[0]) {
            setError("unexpected end of data");
            return false;
        }
        ++src;
    }
    src += 3;// Skip '-->'
    // Do not produce comment node

    setNodeType(NODE_COMMENT);
    return true;
}

bool Reader::parseCdata(const char*& src) {
    // Skip until end of cdata
    const char* value = src;
    src += 8;// Skip '![CDATA['
    while (src[0] != ']' || src[1] != ']' || src[2] != '>') {
        if (!src[0]) {
            setError("unexpected end of data");
            return false;
        }
        ++src;
    }
    src += 2;// Skip ]]

    setNodeType(NODE_CDATA);
    setNodeValue(value, uint32_t(src - value));
    src += 1;// Skip >

    return true;
}

bool Reader::skipCdata(const char*& src) {
    // Skip until end of cdata
    const char* value = src;
    src += 8;// Skip '![CDATA['
    while (src[0] != ']' || src[1] != ']' || src[2] != '>') {
        if (!src[0]) {
            setError("unexpected end of data");
            return false;
        }
        ++src;
    }
    src += 3;// Skip ]]>

    setNodeType(NODE_CDATA);
    return true;
}

bool Reader::skipDoctype(const char*& src) {
    // parsing of doctype is disabled
    // Remember value start
    const char* value = src;

    // Skip to >
    for (; *src != '>';) {
        // Error on end of text
        if (*src == '\0') {
            setError("unexpected end of data");
            return false;
        }
        // Determine character type
        // If '[' encountered, scan for matching ending ']' using naive algorithm with depth
        // This works for all W3C test files except for 2 most wicked
        if (*src == '[') {
            ++src;// Skip '['
            for (int depth = 1; depth > 0; ++src) {
                if (*src == 0) {
                    setError("unexpected end of data");
                    return false;
                }

                if (*src == '[') {
                    ++depth;
                } else if (*src == ']') {
                    --depth;
                }
            }
            continue;
        }
        // Other character, skip it
        ++src;
    }

    // Create a new doctype node
    // setNodeType(GenericValue::NODE_DOCTYPE);
    // setNodeValue(value, uint32_t(src - value));

    src += 1;// skip '>'

    setNodeType(NODE_DOCTYPE);
    return true;
}

void Reader::skipNodeAttributes(const char*& src) {
    // parsing of attributes is disabled
    skip<Reader::is_not_0_9_10_13_32_33_47_60_61_62_63>(src);
}

void Reader::skipBom(const char*& src) {
    if (static_cast<unsigned char>(src[0]) == 0xEF && static_cast<unsigned char>(src[1]) == 0xBB &&
        static_cast<unsigned char>(src[2]) == 0xBF) {
        src += 3;
    }
}

bool Reader::is_9_10_13_32(const char c) {
    return (c == '\t' || c == '\n' || c == '\r' || c == ' ');
}

bool Reader::is_not_0_9_10_13_32_47_62_63(const char c) {
    return (c != '\0' && c != '\t' && c != '\n' && c != '\r' && c != ' ' && c != '/' && c != '>' &&
            c != '?');
}

bool Reader::is_not_0_9_10_13_32_33_47_60_61_62_63(const char c) {
    return (c != '\0' && c != '\t' && c != '\n' && c != '\r' && c != ' ' && c != '!' && c != '/' &&
            c != '<' && c != '=' && c != '>' && c != '?');
}

bool Reader::is_not_0_38_60(const char c) { return (c != '\0' && c != '&' && c != '<'); }

bool Reader::is_not_0_60(const char c) { return (c != '\0' && c != '<'); }

bool Reader::compare(const char* p1, const uint32_t size1, const char* p2, const uint32_t size2,
                     const bool case_sensitive) {
    if (size1 != size2) {
        return false;
    }
    if (case_sensitive) {
        for (const char* end = p1 + size1; p1 < end; ++p1, ++p2) {
            if (*p1 != *p2) {
                return false;
            }
        }
    } else {
        for (const char* end = p1 + size1; p1 < end; ++p1, ++p2) {
            if (tolower(static_cast<unsigned char>(*p1)) !=
                tolower(static_cast<unsigned char>(*p2))) {
                return false;
            }
        }
    }
    return true;
}

}// namespace xml

}// namespace serialflex
