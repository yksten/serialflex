#ifndef __PROTOBUF_FIELD_H__
#define __PROTOBUF_FIELD_H__
#include <cstdint>
#include <cassert>

namespace serialflex {
namespace protobuf {

enum WireType {
    WIRETYPE_NONE = -1,
    WIRETYPE_VARINT = 0,          // int32, int64, uint32, uint64, sint32, sint64, bool, enum
    WIRETYPE_FIXED64 = 1,         // fixed64, sfixed64, double
    WIRETYPE_LENGTH_DELIMITED = 2,// string, bytes, embedded messages, packed repeated fields
    WIRETYPE_START_GROUP = 3,     // group start (deprecated)
    WIRETYPE_END_GROUP = 4,       // group end (deprecated)
    WIRETYPE_FIXED32 = 5,         // fixed32, sfixed32, float
};

enum FieldType {
    FIELDTYPE_NONE = -1,
    FIELDTYPE_DOUBLE = 1,
    FIELDTYPE_FLOAT = 2,
    FIELDTYPE_INT64 = 3,
    FIELDTYPE_UINT64 = 4,
    FIELDTYPE_INT32 = 5,
    FIELDTYPE_FIXED64 = 6,
    FIELDTYPE_FIXED32 = 7,
    FIELDTYPE_BOOL = 8,
    FIELDTYPE_STRING = 9,
    FIELDTYPE_GROUP = 10,
    FIELDTYPE_MESSAGE = 11,
    FIELDTYPE_BYTES = 12,
    FIELDTYPE_UINT32 = 13,
    FIELDTYPE_ENUM = 14,
    FIELDTYPE_SFIXED32 = 15,
    FIELDTYPE_SFIXED64 = 16,
    FIELDTYPE_SINT32 = 17,
    FIELDTYPE_SINT64 = 18,
};

inline WireType fieldType2WireType(const FieldType field_type) {
    if (field_type == FIELDTYPE_DOUBLE || field_type == FIELDTYPE_FIXED64 || field_type == FIELDTYPE_SFIXED64) {
        return WIRETYPE_FIXED64;
    } else if (field_type == FIELDTYPE_FLOAT || field_type == FIELDTYPE_FIXED32 ||
               field_type == FIELDTYPE_SFIXED32) {
        return WIRETYPE_FIXED32;
    } else if (field_type == FIELDTYPE_INT64 || field_type == FIELDTYPE_UINT64 || field_type == FIELDTYPE_INT32 ||
               field_type == FIELDTYPE_BOOL || field_type == FIELDTYPE_UINT32 || field_type == FIELDTYPE_ENUM ||
               field_type == FIELDTYPE_SINT32 || field_type == FIELDTYPE_SINT64) {
        return WIRETYPE_VARINT;
    } else if (field_type == FIELDTYPE_STRING || field_type == FIELDTYPE_MESSAGE ||
               field_type == FIELDTYPE_BYTES) {
        return WIRETYPE_LENGTH_DELIMITED;
    } else {
        assert(false);
        return WIRETYPE_NONE;
    }
}

}// namespace protobuf

template <typename T>
class Field {
    const char* name_;
    const uint32_t number_;
    const protobuf::FieldType type_;
    T& value_;
    bool* has_;
    const bool packed_;    // for repeated
    const protobuf::FieldType type2_;// for map

    Field();
    Field& operator=(const Field&);
public:
    Field(const char* name, const uint32_t number, const protobuf::FieldType type, T& value, bool* has,
          const bool packed = false, const protobuf::FieldType type2 = protobuf::FIELDTYPE_NONE)
        : name_(name), number_(number), type_(type), value_(value), has_(has), packed_(packed),
          type2_(type2) {}

    const char* getName() const { return name_; }
    uint32_t getNumber() const { return number_; }
    protobuf::FieldType getType() const { return type_; }
    T& value() { return value_; }
    const T& getValue() const { return value_; }
    const bool* has() const { return has_; }
    bool* has() { return has_; }
    bool getHas() const { if (!has_) { return true; } return *has_; }
    void setHas(const bool has) { if (has_) { *has_ = has; } }
    protobuf::WireType getWireType() const { return fieldType2WireType(type_);}

    bool getPacked() const { return packed_; }
    protobuf::FieldType getType2() const { return type2_; }
    protobuf::WireType getWireType2() const { return fieldType2WireType(type2_);}
};

// name、value
template <class T>
inline Field<T> makeField(const char* name, T& value) {
    return Field<T>(name, 0, protobuf::FIELDTYPE_NONE, value, NULL);
}

// name、value、has
template <class T>
inline Field<T> makeField(const char* name, T& value, bool* has) {
    return Field<T>(name, 0, protobuf::FIELDTYPE_NONE, value, has);
}
// name、number、type、value、has
template <class T>
inline Field<T> makeField(const char* name, const uint32_t number, const protobuf::FieldType type,
                          T& value, bool* has) {
    return Field<T>(name, number, type, value, has);
}
// name、number、type、value、has、packed（repeated）
template <class T>
inline Field<T> makeField(const char* name, const uint32_t number, const protobuf::FieldType type,
                          T& value, bool* has, const bool packed) {
    return Field<T>(name, number, type, value, NULL, packed);
}
// name、number、type、value、has、type2（map）
template <class T>
inline Field<T> makeField(const char* name, const uint32_t number, const protobuf::FieldType type,
                          T& value, bool* has, const protobuf::FieldType type2) {
    return Field<T>(name, number, type, value, has, false, type2);
}

}// namespace serialflex

#define MAKE_FIELD serialflex::makeField

#endif
