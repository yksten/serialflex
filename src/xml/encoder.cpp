#include <serialflex/xml/encoder.h>
#include <xml/writer.h>

namespace serialflex {

XMLEncoder::XMLEncoder(std::string& str, bool formatted) {
    writer_ = new xml::Writer(str, formatted);
}

XMLEncoder::~XMLEncoder() {
    if (writer_) {
        delete writer_;
    }
}

void XMLEncoder::encodeValue(const char* name, const bool& value) {
    if (writer_) {
        writer_->startKey(name).value(value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const uint32_t& value) {
    if (writer_) {
        writer_->startKey(name).value((uint64_t)value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const int32_t& value) {
    if (writer_) {
        writer_->startKey(name).value((int64_t)value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const uint64_t& value) {
    if (writer_) {
        writer_->startKey(name).value(value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const int64_t& value) {
    if (writer_) {
        writer_->startKey(name).value(value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const float& value) {
    if (writer_) {
        writer_->startKey(name).value(value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const double& value) {
    if (writer_) {
        writer_->startKey(name).value(value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const std::string& value) {
    if (writer_) {
        writer_->startKey(name).value(value).endKey(name);
    }
}

void XMLEncoder::encodeValue(const char* name, const std::vector<bool>& value) {
    startObject(name);
    int32_t size = (int32_t)value.size();
    for (int32_t i = 0; i < size; ++i) {
        const bool item = value.at(i);
        encodeValue("value", item);
    }
    endObject(name);
}

void XMLEncoder::startObject(const char* name) {
    if (writer_) {
        writer_->startObject(name);
    }
}

void XMLEncoder::endObject(const char* name) {
    if (writer_) {
        writer_->endObject(name);
    }
}

bool XMLEncoder::writerResult() const {
    if (writer_) {
        return writer_->result();
    }
    return false;
}

}// namespace serialflex
