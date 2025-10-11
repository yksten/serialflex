#ifndef __PROTOBUF_READER_H__
#define __PROTOBUF_READER_H__
#include <assert.h>
#include <serialflex/field.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "node.h"

namespace serialflex {
namespace protobuf {

class Reader {
    std::vector<GenericNode> nodes_;
    GenericNodeAllocator<GenericNode> alloc_;
    std::vector<GenericNode*> numbers_;// protobuf use
    GenericNodeAllocator<GenericNode*> alloc_numbers_;
    std::string str_error_;

    Reader(const Reader&);
    Reader& operator==(const Reader&);

public:
    Reader(): alloc_(nodes_), alloc_numbers_(numbers_) {}
    ~Reader() {}
    bool parse(const uint8_t* bytes, const uint32_t size);
    const char* getError() const;
    const GenericNode* getNodeByNumber(const uint32_t field_number) const;

private:
    void setError(const char* error) { str_error_ = error; }
    bool parseFromBytes(const uint8_t* bytes, const uint32_t size);

    template <class Dest, class Source>
    inline Dest bit_cast(const Source& source) {
        static_assert(sizeof(Dest) == sizeof(Source), "Sizes do not match");
        Dest dest;
        memcpy(&dest, &source, sizeof(dest));
        return dest;
    }
    template <class T>
    T readFromBytes(const uint8_t*& current, uint32_t& remaining) {
        consumeBytes(current, sizeof(T), remaining);
        const T result = *(bit_cast<T*>(current - sizeof(T)));
        return result;
    }
    bool consumeBytes(const uint8_t*& current, uint32_t how_many, uint32_t& remaining);
    uint64_t readVarInt(const uint8_t*& current, uint32_t& remaining);
    void readWireTypeAndFieldNumber(const uint8_t*& current, uint32_t& remaining,
                                    uint8_t& wire_type, uint32_t& field_number);
};

}// namespace protobuf
}// namespace serialflex

#endif
