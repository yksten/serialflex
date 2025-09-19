#ifndef __XML_READER_H__
#define __XML_READER_H__

#include <cstdlib>
#include <ctype.h>
#include <stdio.h>
#include <string>
#include "node.h"

namespace serialflex {

namespace xml {

enum {
    NODE_NULL = 0,
    NODE_DOCUMENT,//!< A document node. Name and value are empty.
    NODE_ELEMENT, //!< An element node. Name contains element name. Value contains text of first
    //!< data node.
    NODE_DATA,    //!< A data node. Name is empty. Value contains data text.
    NODE_CDATA,   //!< A CDATA node. Name is empty. Value contains data text.
    NODE_COMMENT, //!< A comment node. Name is empty. Value contains comment text.
    NODE_DECLARATION,//!< A declaration node. Name and value are empty. Declaration parameters
    //!< (version, encoding and standalone) are in node attributes.
    NODE_DOCTYPE,    //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
    NODE_PI          //!< A PI node. Name contains target. Value contains instructions.
};

class Reader {
    GenericNode* cur_value_;
    std::vector<GenericNode> values_;
    GenericValueAllocator<GenericNode> alloc_;
    std::string str_error_;

public:
    Reader();
    ~Reader();
    const GenericNode* parse(const char* src);
    const char* getError() const;

    static int64_t convertInt(const char* value, uint32_t length);
    static uint64_t convertUint(const char* value, uint32_t length);
    static double convertDouble(const char* value, uint32_t length);
    // in
    // '0'、'1'、'2'、'3'、'4'、'5'、'6'、'7'、'8'、'9'
    // 、'A'、'B'、'C'、'D'、'E'、'F'、'a'、'b'、'c'、'd'、'e'、'f'
    static unsigned char isHexChas(const unsigned char c);

private:
    void setError(const char* error) { str_error_ = error; }

    static GenericNode* getResult(GenericNode* root);

    void parseNode(const char*& src);
    bool parseElement(const char*& src);
    bool parseCdata(const char*& src);
    void parseNodeContents(const char*& src);

    void parseAndAppendData(const char*& src);
    const char* skipAndExpandCharacterRefs(const char*& src);

    void setNodeType(const int32_t type);
    void setNodeKey(const char* key, const uint32_t key_size);
    void setNodeValue(const char* value, const uint32_t value_size);
    void allocNode();

    bool skipXmlDeclaration(const char*& src);
    bool skipPi(const char*& src);
    bool skipComment(const char*& src);
    bool skipCdata(const char*& src);
    bool skipDoctype(const char*& src);
    void skipNodeAttributes(const char*& src);
    static void skipBom(const char*& src);

    // skip
    template <bool (*f)(const char)> static void skip(const char*& src) {
        for (; f(*src);) {
            ++src;
        }
    }
    // '\t'、'\n'、'\r'、' '
    static bool is_9_10_13_32(const char c);
    // is not '\0'、'\t'、'\n'、'\r'、' '、'/'、'>'、'?'
    static bool is_not_0_9_10_13_32_47_62_63(const char c);
    // is not '\0'、'\t'、'\n'、'\r'、' '、'!'、'/'、'<'、'='、'>'、'?'
    static bool is_not_0_9_10_13_32_33_47_60_61_62_63(const char c);
    // is not '\0'、'&'、'<'
    static bool is_not_0_38_60(const char c);
    // is not '\0'、'<'
    static bool is_not_0_60(const char c);

    static bool compare(const char* p1, const uint32_t size1, const char* p2, const uint32_t size2,
                        const bool case_sensitive);
};

}// namespace xml

}// namespace serialflex

#endif
