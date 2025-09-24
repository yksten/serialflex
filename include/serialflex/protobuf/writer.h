#ifndef __PROTOBUF_WRITER_H__
#define __PROTOBUF_WRITER_H__
#include <map>
#include <string>
#include <serialflex/traits.h>
#include <serialflex/field.h>

namespace serialflex {

namespace protobuf {

class EXPORTAPI MessageByteSize {
    uint32_t& size_;

public:
    explicit MessageByteSize(uint32_t& size): size_(size) {}

    void operator&(const Field<std::string>& value);

    template <typename T>
    void operator&(const Field<T>& field) {
        if (field.getHas() && field.getNumber() != 0) {
            fieldSize(*(const Field<typename internal::TypeTraits<T>::Type>*)(&field));
        }
    }

    template <typename T>
    static uint32_t valueSize(const T& value, const FieldType field_type) {
        assert(field_type == FIELDTYPE_MESSAGE);
        uint32_t size = 0;
        MessageByteSize mb(size);
        internal::serializeWrapper(mb, value);
        return size;
    }
    static uint32_t valueSize(const int32_t& value, const FieldType field_type);
    static uint32_t valueSize(const int64_t& value, const FieldType field_type);
    static uint32_t valueSize(const uint32_t& value, const FieldType field_type);
    static uint32_t valueSize(const uint64_t& value, const FieldType field_type);
    static uint32_t valueSize(const float& value, const FieldType field_type = FIELDTYPE_FIXED32);
    static uint32_t valueSize(const double& value, const FieldType field_type = FIELDTYPE_FIXED64);
    static uint32_t valueSize(const std::string& value, const FieldType field_type);
    static uint32_t varintSize(const uint64_t value);
    static uint32_t zigZagEncode(const int32_t value);
    static uint64_t zigZagEncode(const int64_t value);

private:
    template <typename T>
    void fieldSize(const Field<T>& field) {
        // tag - value
        // string/bytes/message : tag - length - value | tag - length - value | ......
        const FieldType field_type = field.getType();
        size_ += varintSize((field.getNumber() << 3) | field.getWireType());// tag
        const uint64_t length = valueSize(field.getValue(), field_type);
        if (field_type == FIELDTYPE_STRING || field_type == FIELDTYPE_MESSAGE || field_type == FIELDTYPE_BYTES) {
            size_ += varintSize(length);                                    // length
        }
        size_ += length;                                                    // value
    }

    template <typename T>
    void fieldSize(const Field<std::vector<T> >& field) {
        // repeated
        const std::vector<T>& value = field.getValue();
        if (value.empty()) {
            return;
        }
        const uint32_t field_number = field.getNumber();
        const FieldType field_type = field.getType();
        const uint32_t size = (uint32_t)value.size();
        if (field.getPacked()) {
            // tag - length - value - value ......
            const uint64_t tag = (field_number << 3) | WIRETYPE_LENGTH_DELIMITED;
            size_ += 0;
            varintSize(tag);
            uint64_t length = 0;
            for (uint32_t idx = 0; idx < size; ++idx) {
                const typename internal::TypeTraits<T>::Type& traits_item = value.at(idx);
                length += varintSize(valueSize(traits_item, field_type));
            }
            size_ += (varintSize(length) + length);
        } else {
            // tag - value | tag - value | tag - value | ......
            // string/bytes/message: tag - length - value | tag - length - value | ......
            for (uint32_t idx = 0; idx < size; ++idx) {
                const typename internal::TypeTraits<T>::Type& traits_item = value.at(idx);
                size_ += varintSize((field_number << 3) | field.getWireType());// tag
                const uint64_t length = valueSize(traits_item, field_type);
                if (field_type == FIELDTYPE_STRING || field_type == FIELDTYPE_MESSAGE ||
                    field_type == FIELDTYPE_BYTES) {
                    size_ += varintSize(length);// length
                }
                size_ += length;                // value
            }
        }
    }

    template <typename K, typename V>
    void fieldSize(const Field<std::map<K, V> >& field) {
        // map(repeated message)
        const std::map<K, V>& value = field.getValue();
        if (value.empty()) {
            return;
        }
        // tag - length - value
        const uint32_t field_number = field.getNumber();
        const FieldType field_type = field.getType();
        const FieldType field_type2 = field.getType2();
        for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
            size_ += varintSize((field_number << 3) | WIRETYPE_LENGTH_DELIMITED);// tag
            
            const typename internal::TypeTraits<K>::Type& first = it->first;
            const typename internal::TypeTraits<V>::Type& second = it->second;
            uint64_t length = 0;
            length += 1;// field number is 1(one byte)
            const uint64_t key_length = valueSize(first, field_type);
            if (field_type == FIELDTYPE_STRING || field_type == FIELDTYPE_MESSAGE ||
                field_type == FIELDTYPE_BYTES) {
                length += varintSize(key_length);// length key
            }
            length += key_length;

            length += 1;// field number is 2(one byte)
            const uint64_t value_length = valueSize(second, value.getType2());
            if (field_type2 == FIELDTYPE_STRING || field_type2 == FIELDTYPE_MESSAGE ||
                field_type2 == FIELDTYPE_BYTES) {
                length += varintSize(value_length);// length value
            }
            length += value_length;

            size_ += varintSize(length);// length
            size_ += length;            // value
        }
    }

    static uint32_t sintSize(const int32_t value);
    static uint32_t sintSize(const int64_t value);
};

}// namespace protobuf

}// namespace serialflex

#endif
