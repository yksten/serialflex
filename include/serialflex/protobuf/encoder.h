#ifndef __PROTOBUF_ENCODER_H__
#define __PROTOBUF_ENCODER_H__

#include <map>
#include <serialflex/field.h>
#include <serialflex/protobuf/writer.h>
#include <serialflex/traits.h>

namespace serialflex {

class EXPORTAPI ProtobufEncoder {
    std::string& str_;

public:
    explicit ProtobufEncoder(std::string& str): str_(str) {}

    template <typename T>
    bool operator<<(const T& value) {
        uint32_t capacity = 0;
        protobuf::MessageByteSize mb(capacity);
        internal::serializeWrapper(mb,
                                   *const_cast<typename internal::TypeTraits<T>::Type*>(&value));
        str_.reserve(capacity);

        internal::serializeWrapper(*this,
                                   *const_cast<typename internal::TypeTraits<T>::Type*>(&value));
        return (!str_.empty());
    }

    template <typename T>
    ProtobufEncoder& operator&(const Field<T>& field) {
        if (field.getHas() && field.getNumber() != 0) {
            writeField(*(const Field<typename internal::TypeTraits<T>::Type>*)(&field));
        }
        return *this;
    }

private:
    void writeField(const Field<std::string>& field);

    template <typename T>
    void writeField(const Field<T>& field) {
        // message : tag - length - value
        writeTag(field.getNumber(), field.getWireType());
        writeValue(field.getValue(), field.getType());
    }

    template <typename T>
    void writeField(const Field<std::vector<T> >& field) {
        // repeated
        const std::vector<T>& value = field.getValue();
        if (value.empty()) {
            return;
        }
        const uint32_t field_number = field.getNumber();
        const protobuf::FieldType field_type = field.getType();
        const uint32_t size = (uint32_t)value.size();
        if (field.getPacked()) {
            // tag - length - value - value ......
            writeTag(field_number, protobuf::WIRETYPE_LENGTH_DELIMITED);// tag

            uint64_t length = 0;
            for (uint32_t idx = 0; idx < size; ++idx) {
                const typename internal::TypeTraits<T>::Type& item = value.at(idx);
                length += protobuf::MessageByteSize::varintSize(
                    protobuf::MessageByteSize::valueSize(item, field_type));
            }
            writeVarint(length);// length

            for (uint32_t idx = 0; idx < size; ++idx) {
                const typename internal::TypeTraits<T>::Type& item = value.at(idx);
                writeValue(item, field_type);// value
            }
        } else {
            // tag - value | tag - value | tag - value | ......
            // string/bytes/message: tag - length - value | tag - length - value | ......
            for (uint32_t idx = 0; idx < size; ++idx) {
                const typename internal::TypeTraits<T>::Type& item = value.at(idx);
                writeTag(field_number, field.getWireType());// tag
                writeValue(item, field_type);               // value
            }
        }
    }

    template <typename K, typename V>
    void writeField(const Field<std::map<K, V> >& field) {
        // map(repeated message)
        const std::map<K, V>& value = field.getValue();
        if (value.empty()) {
            return;
        }
        // tag - length - value
        const uint32_t field_number = field.getNumber();
        const protobuf::FieldType field_type = field.getType();
        const protobuf::FieldType field_type2 = field.getType2();
        for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
            // 1.tag
            writeTag(field_number, protobuf::WIRETYPE_LENGTH_DELIMITED);

            uint64_t length = 0;
            length += 1;// field number is 1(one byte)
            const uint64_t key_length = protobuf::MessageByteSize::valueSize(
                *((const typename internal::TypeTraits<K>::Type*)(&it->first)), field_type);
            if (field_type == protobuf::FIELDTYPE_STRING ||
                field_type == protobuf::FIELDTYPE_MESSAGE ||
                field_type == protobuf::FIELDTYPE_BYTES) {
                length += protobuf::MessageByteSize::varintSize(key_length);// length key
            }
            length += key_length;

            length += 1;// field number is 2(one byte)
            const uint64_t value_length = protobuf::MessageByteSize::valueSize(
                *((const typename internal::TypeTraits<V>::Type*)(&it->second)), field.getType2());
            if (field_type2 == protobuf::FIELDTYPE_STRING ||
                field_type2 == protobuf::FIELDTYPE_MESSAGE ||
                field_type2 == protobuf::FIELDTYPE_BYTES) {
                length += protobuf::MessageByteSize::varintSize(value_length);// length value
            }
            length += value_length;
            // 2.length
            writeVarint(length);

            // 3.value
            writeTag(1, field.getWireType());
            writeValue(it->first, field_type);
            writeTag(2, field.getWireType2());
            writeValue(it->second, field_type2);
        }
    }

    template <typename T>
    void writeValue(const T& value, const protobuf::FieldType field_type) {
        assert(field_type == protobuf::FIELDTYPE_MESSAGE);

        uint32_t size = 0;
        protobuf::MessageByteSize mb(size);
        internal::serializeWrapper(mb,
                                   *const_cast<typename internal::TypeTraits<T>::Type*>(&value));

        writeVarint(size);             // length
        ProtobufEncoder(str_) << value;// value
    }
    void writeValue(const int32_t& value, const protobuf::FieldType field_type);
    void writeValue(const int64_t& value, const protobuf::FieldType field_type);
    void writeValue(const uint32_t& value, const protobuf::FieldType field_type);
    void writeValue(const uint64_t& value, const protobuf::FieldType field_type);
    void writeValue(const float& value,
                    const protobuf::FieldType field_type = protobuf::FIELDTYPE_FIXED32);
    void writeValue(const double& value,
                    const protobuf::FieldType field_type = protobuf::FIELDTYPE_FIXED64);
    void writeValue(const std::string& value, const protobuf::FieldType field_type);

    void writeTag(const uint32_t field_number, const protobuf::WireType wire_type);
    void writeVarint(const uint64_t value);
    void writeFixed32(const uint32_t value);
    void writeFixed64(const uint64_t value);
};

}// namespace serialflex

#endif
