#ifndef __NODE_H__
#define __NODE_H__

#include <stdint.h>
#include <vector>
#include <cassert>

namespace serialflex {

struct GenericNode {
    int32_t type;

    const char* key;
    uint32_t key_size;

    const char* value;
    uint32_t value_size;

    GenericNode* prev;
    GenericNode* next;
    GenericNode* child;

    GenericNode()
        : type(-1), key(NULL), key_size(0), value(NULL), value_size(0),
          prev(NULL), next(NULL), child(NULL) {}
};

template <class T>
class GenericValueAllocator {
    uint32_t cur_ndex_;
    uint32_t capacity_;
    std::vector<T>& array_;

public:
    explicit GenericValueAllocator(std::vector<T>& vec)
        : cur_ndex_(0), capacity_(0), array_(vec) {}
    void operator++() { ++capacity_; }
    void reSize() {
        assert(array_.empty());
        array_.resize(capacity_);
    }
    T* allocValue() {
        assert(cur_ndex_ < array_.size());
        return &array_.at(cur_ndex_++);
    }
};

}// namespace serialflex

#endif
