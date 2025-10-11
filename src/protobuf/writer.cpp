#include <serialflex/protobuf/writer.h>

namespace serialflex {

namespace protobuf {

void MessageByteSize::operator&(const Field<std::string>& value) {
    const std::string& str = value.getValue();
    if (str.empty()) {
        return;
    }
    // tag - length - value
    fieldSize(value);
}

uint32_t MessageByteSize::valueSize(const int32_t& value, const FieldType field_type) {
    if (field_type == FIELDTYPE_INT32 || field_type == FIELDTYPE_BOOL ||
        field_type == FIELDTYPE_ENUM) {
        return varintSize((uint32_t)value);
    } else if (field_type == FIELDTYPE_SFIXED32) {
        return 4;
    } else if (field_type == FIELDTYPE_SINT32) {
        return varintSize(zigZagEncode(value));
    } else {
        assert(false);
        return 0;
    }
}

uint32_t MessageByteSize::valueSize(const int64_t& value, const FieldType field_type) {
    if (field_type == FIELDTYPE_INT64) {
        return varintSize((uint64_t)value);
    } else if (field_type == FIELDTYPE_SFIXED64) {
        return 8;
    } else if (field_type == FIELDTYPE_SINT64) {
        return varintSize(zigZagEncode(value));
    } else {
        assert(false);
        return 0;
    }
}

uint32_t MessageByteSize::valueSize(const uint32_t& value, const FieldType field_type) {
    if (field_type == FIELDTYPE_UINT32) {
        return varintSize(value);
    } else if (field_type == FIELDTYPE_FIXED32) {
        return 4;
    } else {
        assert(false);
        return 0;
    }
}

uint32_t MessageByteSize::valueSize(const uint64_t& value, const FieldType field_type) {
    if (field_type == FIELDTYPE_UINT64) {
        return varintSize(value);
    } else if (field_type == FIELDTYPE_FIXED64) {
        return 8;
    } else {
        assert(false);
        return 0;
    }
}

uint32_t MessageByteSize::valueSize(const float& value, const FieldType field_type) {
    assert(field_type == FIELDTYPE_FIXED32);
    return 4;
}

uint32_t MessageByteSize::valueSize(const double& value, const FieldType field_type) {
    assert(field_type == FIELDTYPE_FIXED64);
    return 8;
}

uint32_t MessageByteSize::valueSize(const std::string& value, const FieldType field_type) {
    assert(field_type == FIELDTYPE_STRING || field_type == FIELDTYPE_BYTES);
    return (uint32_t)value.size();
}

uint32_t MessageByteSize::varintSize(const uint64_t value) {
    if (value < (1ull << 35)) {
        if (value < (1ull << 7)) {
            return 1;
        } else if (value < (1ull << 14)) {
            return 2;
        } else if (value < (1ull << 21)) {
            return 3;
        } else if (value < (1ull << 28)) {
            return 4;
        } else {
            return 5;
        }
    } else {
        if (value < (1ull << 42)) {
            return 6;
        } else if (value < (1ull << 49)) {
            return 7;
        } else if (value < (1ull << 56)) {
            return 8;
        } else if (value < (1ull << 63)) {
            return 9;
        } else {
            return 10;
        }
    }
}

uint32_t MessageByteSize::zigZagEncode(const int32_t value) {
    return (static_cast<uint32_t>(value) << 1) ^ (value >> 31);
}

uint64_t MessageByteSize::zigZagEncode(const int64_t value) {
    return (static_cast<uint64_t>(value) << 1) ^ (value >> 63);
}

uint32_t MessageByteSize::sintSize(const int32_t value) { return varintSize(zigZagEncode(value)); }

uint32_t MessageByteSize::sintSize(const int64_t value) { return varintSize(zigZagEncode(value)); }

}// namespace protobuf

}// namespace serialflex
