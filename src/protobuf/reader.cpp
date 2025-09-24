#include "reader.h"

namespace serialflex {
namespace protobuf {

class FieldWrapper {
    GenericNodeAllocator<GenericNode>& alloc_;
    GenericNodeAllocator<GenericNode*>& alloc_numbers_;

    GenericNode* last_node_;
    uint32_t last_field_number_;

public:
    FieldWrapper(GenericNodeAllocator<GenericNode>& alloc,
                 GenericNodeAllocator<GenericNode*>& alloc_numbers)
        : alloc_(alloc), alloc_numbers_(alloc_numbers), last_node_(NULL), last_field_number_(0) {}

    void addField(const uint32_t field_number, const WireType wire_type, const uint32_t value,
                  const uint8_t* data, const uint64_t size) {
        GenericNode* field = createField(field_number, (WireType)wire_type);
        if (field) {
            field->u32 = value;
            field->value = (const char*)data;
            field->value_size = (uint32_t)size;

            updateLastField(*field);
        } else {
            updateLastFieldNumber(field_number);
        }
    }
    void addField(const uint32_t field_number, const WireType wire_type, const uint64_t value,
                  const uint8_t* data, const uint64_t size) {
        GenericNode* field = createField(field_number, (WireType)wire_type);
        if (field) {
            field->u64 = value;
            field->value = (const char*)data;
            field->value_size = (uint32_t)size;

            updateLastField(*field);
        } else {
            updateLastFieldNumber(field_number);
        }
    }
    void addField(const uint32_t field_number, const WireType wire_type, const uint8_t* data,
                  const uint64_t size) {
        GenericNode* field = createField(field_number, (WireType)wire_type);
        if (field) {
            field->value = (const char*)data;
            field->value_size = (uint32_t)size;

            updateLastField(*field);
        } else {
            updateLastFieldNumber(field_number);
        }
    }

private:
    GenericNode* createField(const uint32_t number, const WireType type) {
        GenericNode* new_field = alloc_.allocValue();
        if (!new_field) {
            ++alloc_;
            return NULL;
        }
        new_field->number = number;
        new_field->type = type;
        return new_field;
    }
    void updateLastField(GenericNode& field) {
        if (last_node_ && last_node_->number == field.number) {
            last_node_->next = &field;
        } else {
            GenericNode** number_field = alloc_numbers_.allocValue();
            assert(number_field);
            if (number_field) {
                *number_field = &field;
            }
        }
        last_node_ = &field;
    }

    void updateLastFieldNumber(const uint32_t field_number) {
        if (last_field_number_ != 0 && last_field_number_ == field_number) {
            ;
        } else {
            ++alloc_numbers_;
        }
        last_field_number_ = field_number;
    }
};

/*--------------------------------------------------------------------------------*/

bool Reader::parse(const uint8_t* bytes, const uint32_t size) {
    const uint8_t* binary_bytes = bytes;
    do {
        ++alloc_;
        parseFromBytes(bytes, size);
    } while (false);

    alloc_.reSize();
    alloc_numbers_.reSize();

    if (!parseFromBytes(binary_bytes, size)) {
        return false;
    }
    if (!str_error_.empty()) {
        return false;
    }
    return true;
}

const char* Reader::getError() const {
    if (str_error_.empty()) {
        return NULL;
    }
    return str_error_.c_str();
}

const GenericNode* Reader::getNodeByNumber(const uint32_t field_number) const {
    if (numbers_.empty()) {
        return NULL;
    }
    for (const GenericNode* item : numbers_) {
        if (item->number == field_number) {
            return item;
        }
    }
    return NULL;
}

bool Reader::parseFromBytes(const uint8_t* bytes, const uint32_t size) {
    const uint8_t* current = bytes;
    uint32_t remaining = size;
    FieldWrapper wrapper(alloc_, alloc_numbers_);
    for (; remaining > 0;) {
        uint8_t wire_type = WIRETYPE_NONE;
        uint32_t field_number = 0;
        readWireTypeAndFieldNumber(current, remaining, wire_type, field_number);
        switch (wire_type) {
            case WIRETYPE_VARINT: {
                const uint8_t* data = current;
                uint32_t pos = remaining;
                const uint64_t varint = readVarInt(current, remaining);
                wrapper.addField(field_number, (WireType)wire_type, varint, data, pos - remaining);
            } break;
            case WIRETYPE_FIXED64: {
                const uint8_t* data = current;
                uint32_t pos = remaining;
                const uint64_t value = readFromBytes<uint64_t>(current, remaining);
                wrapper.addField(field_number, (WireType)wire_type, value, data, pos - remaining);
            } break;
            case WIRETYPE_LENGTH_DELIMITED: {
                const uint64_t size = readVarInt(current, remaining);
                const uint8_t* data = current;
                wrapper.addField(field_number, (WireType)wire_type, data, size);
            } break;
            case WIRETYPE_START_GROUP: {
                // setError("GROUPSTART Unhandled wire type encountered");
            } break;
            case WIRETYPE_END_GROUP: {
                // setError("GROUPEND Unhandled wire type encountered");
            } break;
            case WIRETYPE_FIXED32: {
                const uint8_t* data = current;
                uint32_t pos = remaining;
                const uint32_t value = readFromBytes<uint32_t>(current, remaining);
                wrapper.addField(field_number, (WireType)wire_type, value, data, pos - remaining);
            } break;
            default: {
                char error[256] = {0};
                snprintf(error, 256, "Unknown wire type encountered: %d at offset %d",
                         static_cast<int>(wire_type), (size - remaining));
                setError(error);
                return false;
            } break;
        }
    }
    return true;
}

bool Reader::consumeBytes(const uint8_t*& current, uint32_t how_many, uint32_t& remaining) {
    if (how_many > remaining) {
        setError("ReadBytes overrun!");
        return false;
    }
    current += how_many;
    remaining -= how_many;
    return true;
}

uint64_t Reader::readVarInt(const uint8_t*& current, uint32_t& remaining) {
    uint64_t result = 0;
    bool keep_going;
    int shift = 0;
    do {
        const uint8_t next_number = readFromBytes<uint8_t>(current, remaining);
        keep_going = (next_number >= 128);
        result += (uint64_t)(next_number & 0x7f) << shift;
        shift += 7;
    } while (keep_going);
    return result;
}

void Reader::readWireTypeAndFieldNumber(const uint8_t*& current, uint32_t& remaining,
                                        uint8_t& wire_type, uint32_t& field_number) {
    uint64_t wire_type_and_field_number = readVarInt(current, remaining);
    wire_type = wire_type_and_field_number & 0x07;
    field_number = (wire_type_and_field_number & 0xFFFF) >> 3;
}

}// namespace protobuf
}// namespace serialflex
