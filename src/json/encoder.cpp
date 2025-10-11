#include <json/writer.h>
#include <serialflex/json/encoder.h>

namespace serialflex {

JSONEncoder::JSONEncoder(std::string& str, bool formatted) {
    writer_ = new json::Writer(str, formatted);
}

JSONEncoder::~JSONEncoder() { delete writer_; }

void JSONEncoder::encodeValue(const char* name, const bool& value) {
    if (writer_) {
        writer_->key(name).value(value);
    }
}

void JSONEncoder::encodeValue(const char* name, const uint32_t& value) {
    if (writer_) {
        writer_->key(name).value((uint64_t)value);
    }
}

void JSONEncoder::encodeValue(const char* name, const int32_t& value) {
    if (writer_) {
        writer_->key(name).value((int64_t)value);
    }
}

void JSONEncoder::encodeValue(const char* name, const uint64_t& value) {
    if (writer_) {
        writer_->key(name).value(value);
    }
}

void JSONEncoder::encodeValue(const char* name, const int64_t& value) {
    if (writer_) {
        writer_->key(name).value(value);
    }
}

void JSONEncoder::encodeValue(const char* name, const float& value) {
    if (writer_) {
        writer_->key(name).value(value);
    }
}

void JSONEncoder::encodeValue(const char* name, const double& value) {
    if (writer_) {
        writer_->key(name).value(value);
    }
}

void JSONEncoder::encodeValue(const char* name, const std::string& value) {
    if (writer_) {
        writer_->key(name).value(value.c_str());
    }
}

void JSONEncoder::encodeValue(const char* name, const std::vector<bool>& value) {
    startArray(name);
    int32_t size = (int32_t)value.size();
    for (int32_t i = 0; i < size; ++i) {
        const bool item = value.at(i);
        encodeValue(NULL, item);
    }
    endArray();
}

void JSONEncoder::startObject(const char* name) {
    if (writer_) {
        writer_->key(name).startObject();
    }
}

void JSONEncoder::endObject() {
    if (writer_) {
        writer_->endObject();
    }
}

void JSONEncoder::startArray(const char* name) {
    if (writer_) {
        writer_->key(name).startArray();
    }
}

void JSONEncoder::endArray() {
    if (writer_) {
        writer_->endArray();
    }
}

bool JSONEncoder::writerResult() const {
    if (writer_) {
        return writer_->result();
    }
    return false;
}

void JSONEncoder::writerSeparation() {
    if (writer_) {
        writer_->separation();
    }
}

}// namespace serialflex
