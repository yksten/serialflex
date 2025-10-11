#include <serialflex/protobuf/encoder.h>

namespace serialflex {

void ProtobufEncoder::writeField(const Field<std::string>& field) {
    const std::string& str = field.getValue();
    if (str.empty()) {
        return;
    }
    // tag - length - value
    writeTag(field.getNumber(), field.getWireType());
    writeValue(str, field.getType());
}

void ProtobufEncoder::writeValue(const int32_t& value, const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_INT32 || field_type == protobuf::FIELDTYPE_BOOL ||
        field_type == protobuf::FIELDTYPE_ENUM) {
        writeVarint((uint32_t)value);
    } else if (field_type == protobuf::FIELDTYPE_SFIXED32) {
        writeFixed32(*reinterpret_cast<const uint32_t*>(&value));
    } else if (field_type == protobuf::FIELDTYPE_SINT32) {
        writeVarint(protobuf::MessageByteSize::zigZagEncode(value));
    } else {
        assert(false);
    }
}

void ProtobufEncoder::writeValue(const int64_t& value, const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_INT64) {
        writeVarint((uint32_t)value);
    } else if (field_type == protobuf::FIELDTYPE_SFIXED64) {
        writeFixed32(*reinterpret_cast<const uint32_t*>(&value));
    } else if (field_type == protobuf::FIELDTYPE_SINT64) {
        writeVarint(protobuf::MessageByteSize::zigZagEncode(value));
    } else {
        assert(false);
    }
}

void ProtobufEncoder::writeValue(const uint32_t& value, const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_UINT32) {
        return writeVarint(value);
    } else if (field_type == protobuf::FIELDTYPE_FIXED32) {
        writeFixed32(value);
    } else {
        assert(false);
        return;
    }
}

void ProtobufEncoder::writeValue(const uint64_t& value, const protobuf::FieldType field_type) {
    if (field_type == protobuf::FIELDTYPE_UINT64) {
        return writeVarint(value);
    } else if (field_type == protobuf::FIELDTYPE_FIXED64) {
        writeFixed64(value);
    } else {
        assert(false);
        return;
    }
}

void ProtobufEncoder::writeValue(const float& value, const protobuf::FieldType field_type) {
    assert(field_type == protobuf::FIELDTYPE_FIXED32);
    writeFixed32(*reinterpret_cast<const uint32_t*>(&value));
}

void ProtobufEncoder::writeValue(const double& value, const protobuf::FieldType field_type) {
    assert(field_type == protobuf::FIELDTYPE_FIXED64);
    writeFixed64(*reinterpret_cast<const uint64_t*>(&value));
}

void ProtobufEncoder::writeValue(const std::string& value, const protobuf::FieldType field_type) {
    const uint64_t length = value.size();
    writeVarint(length);
    str_.append(value.data(), length);
}

void ProtobufEncoder::writeTag(const uint32_t field_number, const protobuf::WireType wire_type) {
    const uint64_t tag = ((field_number << 3) | wire_type);
    writeVarint(tag);
}

void ProtobufEncoder::writeVarint(const uint64_t value) {
    uint64_t v = value;
    while (v >= 0x80) {
        const uint8_t c = static_cast<uint8_t>(v | 0x80);
        str_.append(1, c);
        v >>= 7;
    }
    str_.append(1, static_cast<uint8_t>(v));
}

void ProtobufEncoder::writeFixed32(const uint32_t value) {
    str_.append(1, (char)(value & 0xFF));
    str_.append(1, (char)((value >> 8) & 0xFF));
    str_.append(1, (char)((value >> 16) & 0xFF));
    str_.append(1, (char)((value >> 24) & 0xFF));
}

void ProtobufEncoder::writeFixed64(const uint64_t value) {
    str_.append(1, (char)(value & 0xFF));
    str_.append(1, (char)((value >> 8) & 0xFF));
    str_.append(1, (char)((value >> 16) & 0xFF));
    str_.append(1, (char)((value >> 24) & 0xFF));
    str_.append(1, (char)((value >> 32) & 0xFF));
    str_.append(1, (char)((value >> 40) & 0xFF));
    str_.append(1, (char)((value >> 48) & 0xFF));
    str_.append(1, (char)((value >> 56) & 0xFF));
}

}// namespace serialflex
