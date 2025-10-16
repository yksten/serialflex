#ifndef __NODE_H__
#define __NODE_H__

#include <cassert>
#include <stdint.h>
#include <vector>

namespace serialflex {

struct GenericNode {
    GenericNode()
        : type(-1), key(NULL), key_size(0), value(NULL), value_size(0), u64(0), prev(NULL), 
          next(NULL), child(NULL), number(0) {}

    int32_t type;
    const char* key;
    uint32_t key_size;
    const char* value;
    uint32_t value_size;
    union {
        uint64_t u64;
        uint32_t u32;
    };

    GenericNode* prev;
    GenericNode* next;
    GenericNode* child;

    // for protobuf
    uint32_t number;
};

template <class T>
class GenericNodeAllocator {
    uint32_t cur_ndex_;
    uint32_t capacity_;
    std::vector<T>& array_;

public:
    explicit GenericNodeAllocator(std::vector<T>& vec): cur_ndex_(0), capacity_(0), array_(vec) {}
    void operator++() { ++capacity_; }
    void reSize() {
        assert(array_.empty());
        array_.resize(capacity_);
    }
    T* allocValue() {
        if (array_.empty()) {
            return NULL;
        }
        assert(cur_ndex_ < array_.size());
        return &array_.at(cur_ndex_++);
    }
};

}// namespace serialflex

#endif
