#ifndef __PROTOBUF_DECODER_H__
#define __PROTOBUF_DECODER_H__

#include <map>
#include <serialflex/field.h>
#include <serialflex/traits.h>

namespace serialflex {

struct GenericNode;
namespace protobuf {
class Reader;
}// namespace protobuf
class EXPORTAPI ProtobufDecoder {
    protobuf::Reader* reader_;

    ProtobufDecoder(const ProtobufDecoder&);
    ProtobufDecoder& operator=(const ProtobufDecoder&);

public:
    ProtobufDecoder(const uint8_t* data, const uint32_t size);
    ~ProtobufDecoder();
    
    const char* getError() const;

    template <typename T>
    bool operator>>(T& value) {
        internal::serializeWrapper(*this, value);
        if (getError()) {
            return false;
        }
        return true;
    }

    template <typename T>
    ProtobufDecoder& operator&(const Field<T>& field) {
        Field<T>& remove_const_field = *const_cast<Field<T>*>(&field);
        readField(remove_const_field);
        return *this;
    }

public:
    template <typename T>
    void readField(Field<T>& field) {
        const GenericNode* node = getNodeByNumber(field.getNumber());
        if (!node || getWireType(node) != field.getWireType()) {
            return;
        }
        assert(!getNextNode(node));
        field.setHas(true);
        readValue(*node, field.value(), field.getType());
    }

    template <typename T>
    void readField(Field<std::vector<T> >& field) {
        // repeated
        const GenericNode* node = getNodeByNumber(field.getNumber());
        if (!node) {
            return;
        }
        const protobuf::WireType wire_type = getWireType(node);
        if (wire_type != field.getWireType() && wire_type != protobuf::WIRETYPE_LENGTH_DELIMITED) {
            return;
        }
        field.setHas(true);
        std::vector<T>& value = field.value();
        value.clear();
        for (const GenericNode* cur_node = node; cur_node; cur_node = getNextNode(cur_node)) {
            T item = T();
            readValue(*cur_node, *(typename internal::TypeTraits<T>::Type*)(&item),
                      field.getType());
            value.push_back(item);
        }
    }

    template <typename K, typename V>
    void readField(Field<std::map<K, V> >& field) {
        // map(repeated message)
        const GenericNode* node = getNodeByNumber(field.getNumber());
        if (!node || getWireType(node) != protobuf::WIRETYPE_LENGTH_DELIMITED) {
            return;
        }
        field.setHas(true);
        std::map<K, V>& value = field.getValue();
        value.clear();
        for (const GenericNode* cur_node = node; cur_node; cur_node = getNextNode(cur_node)) {
            ProtobufDecoder decoder(getData(cur_node), getDataSize(cur_node));
            const GenericNode* first_node = decoder.getNodeByNumber(1);
            const GenericNode* second_node = decoder.getNodeByNumber(2);
            if (!first_node || !second_node) {
                assert(false);
                continue;
            }
            K key = K();
            V item = V();
            readValue(*first_node, *(typename internal::TypeTraits<K>::Type*)(&key),
                      field.getType());
            readValue(*second_node, *(typename internal::TypeTraits<V>::Type*)(&item),
                      field.getType2());
            value.insert(std::pair<K, V>(key, item));
        }
    }

private:
    template <typename T>
    void readValue(const GenericNode& node, T& value, const protobuf::FieldType field_type) {
        assert(field_type == protobuf::FIELDTYPE_MESSAGE);
        ProtobufDecoder decoder(getData(&node), getDataSize(&node));
        decoder >> value;
    }
    void readValue(const GenericNode& node, int32_t& value, const protobuf::FieldType field_type);
    void readValue(const GenericNode& node, int64_t& value, const protobuf::FieldType field_type);
    void readValue(const GenericNode& node, uint32_t& value, const protobuf::FieldType field_type);
    void readValue(const GenericNode& node, uint64_t& value, const protobuf::FieldType field_type);
    void readValue(const GenericNode& node, float& value, const protobuf::FieldType field_type);
    void readValue(const GenericNode& node, double& value, const protobuf::FieldType field_type);
    void readValue(const GenericNode& node, std::string& value,
                   const protobuf::FieldType field_type);

    const GenericNode* getNodeByNumber(const uint32_t field_number) const;
    protobuf::WireType getWireType(const GenericNode* node);
    static const GenericNode* getNextNode(const GenericNode* node);
    static const uint8_t* getData(const GenericNode* node);
    static const uint32_t getDataSize(const GenericNode* node);
};

}// namespace serialflex

#endif
