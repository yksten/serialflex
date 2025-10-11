#include <protobuf/reader.h>
#include <serialflex/protobuf/decoder.h>

namespace serialflex {

ProtobufDecoder::ProtobufDecoder(const uint8_t* data, const uint32_t size): reader_(NULL) {
    reader_ = new protobuf::Reader();
    bool status = reader_->parse(data, size);
    assert(status);
}

ProtobufDecoder::~ProtobufDecoder() {
    if (reader_) {
        delete reader_;
    }
}

void ProtobufDecoder::readValue(const GenericNode& node, int32_t& value,
                                const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_INT32 || field_type == protobuf::FIELDTYPE_BOOL ||
        field_type == protobuf::FIELDTYPE_ENUM) {
        value = (int32_t)node.u32;
    } else if (field_type == protobuf::FIELDTYPE_SFIXED32) {
        value = (int32_t)node.u32;
    } else if (field_type == protobuf::FIELDTYPE_SINT32) {
        uint32_t n = node.u32;
        value = (n >> 1) ^ -static_cast<int32_t>(n & 1);
    } else {
        assert(false);
    }
}

void ProtobufDecoder::readValue(const GenericNode& node, int64_t& value,
                                const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_INT64) {
        value = (int64_t)node.u64;
    } else if (field_type == protobuf::FIELDTYPE_SFIXED64) {
        value = (int64_t)node.u64;
    } else if (field_type == protobuf::FIELDTYPE_SINT64) {
        uint64_t n = node.u64;
        value = (n >> 1) ^ -static_cast<int64_t>(n & 1);
    } else {
        assert(false);
    }
}

void ProtobufDecoder::readValue(const GenericNode& node, uint32_t& value,
                                const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_UINT32) {
        value = node.u32;
    } else if (field_type == protobuf::FIELDTYPE_FIXED32) {
        value = node.u32;
    } else {
        assert(false);
        return;
    }
}

void ProtobufDecoder::readValue(const GenericNode& node, uint64_t& value,
                                const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_UINT64) {
        value = node.u64;
    } else if (field_type == protobuf::FIELDTYPE_FIXED64) {
        value = node.u64;
    } else {
        assert(false);
        return;
    }
}

void ProtobufDecoder::readValue(const GenericNode& node, float& value,
                                const protobuf::FieldType field_type) {
    assert(field_type == protobuf::FIELDTYPE_FIXED32);
    union {
        uint32_t u32;
        float f;
    };
    u32 = node.u32;
    value = f;
}

void ProtobufDecoder::readValue(const GenericNode& node, double& value,
                                const protobuf::FieldType field_type) {
    assert(field_type == protobuf::FIELDTYPE_FIXED64);
    union {
        uint64_t u64;
        double db;
    };
    u64 = node.u64;
    value = db;
}

void ProtobufDecoder::readValue(const GenericNode& node, std::string& value,
                                const protobuf::FieldType field_type) {
    assert(field_type == protobuf::FIELDTYPE_BYTES || field_type == protobuf::FIELDTYPE_STRING);
    value.append((const char*)node.value, node.value_size);
}

const GenericNode* ProtobufDecoder::getNodeByNumber(const uint32_t field_number) const {
    if (!reader_) {
        return NULL;
    }
    return reader_->getNodeByNumber(field_number);
}

protobuf::WireType ProtobufDecoder::getWireType(const GenericNode* node) {
    if (!node) {
        return protobuf::WIRETYPE_NONE;
    }
    return (protobuf::WireType)node->type;
}

const GenericNode* ProtobufDecoder::getNextNode(const GenericNode* node) {
    if (!node) {
        return NULL;
    }
    return node->next;
}

const uint8_t* ProtobufDecoder::getData(const GenericNode* node) {
    if (!node) {
        return NULL;
    }
    return (const uint8_t*)node->value;
}

const uint32_t ProtobufDecoder::getDataSize(const GenericNode* node) {
    if (!node) {
        return 0;
    }
    return node->value_size;
}

}// namespace serialflex
