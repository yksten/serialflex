#include <serialflex/xml/decoder.h>
#include <xml/reader.h>

namespace serialflex {

XMLDecoder::XMLDecoder(const char* str, bool case_insensitive)
    : convert_by_type_(true), case_insensitive_(case_insensitive), current_(NULL) {
    reader_ = new xml::Reader();
    current_ = reader_->parse(str);
    assert(current_);
}

XMLDecoder::~XMLDecoder() {
    if (reader_) {
        delete reader_;
    }
}

XMLDecoder& XMLDecoder::setConvertByType(bool convert_by_type) {
    convert_by_type_ = convert_by_type;
    return *this;
}

void XMLDecoder::decodeValue(const char* name, bool& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = item2Bool(*item);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, uint32_t& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = (uint32_t)xml::Reader::convertUint(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, int32_t& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = (int32_t)xml::Reader::convertInt(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, uint64_t& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = xml::Reader::convertUint(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, int64_t& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = xml::Reader::convertInt(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, float& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = (float)xml::Reader::convertDouble(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, double& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        value = xml::Reader::convertDouble(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, std::string& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (!item) {
        return;
    }
    if (item->value && item->value_size) {
        value.clear();
        value.append(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
        XMLDecoder::dealWithString(value);
    } else if (const GenericNode* data = item->child) {
        value.clear();
        value.append(data->value, data->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void XMLDecoder::decodeValue(const char* name, std::vector<bool>& value, bool* has_value) {
    const GenericNode* item = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item) {
        for (const GenericNode* child = item->child; child; child = child->next) {
            value.push_back(item2Bool(*child));
            if (has_value) {
                *has_value = true;
            }
        }
    }
}

bool XMLDecoder::item2Bool(const GenericNode& item) const {
    if (item.type == xml::NODE_ELEMENT) {
        if (item.value_size == 5 && strncmp("false", item.value, item.value_size) == 0) {
            return false;
        } else if (item.value_size == 4 && strncmp("true", item.value, item.value_size) == 0) {
            return true;
        }
        return (atoi(std::string(item.value, item.value_size).c_str()));
    }
    return false;
}

uint32_t XMLDecoder::getObjectSize(const GenericNode* parent) {
    uint32_t size = 0;
    if (parent) {
        for (const GenericNode* child = parent->child; child; child = child->next) {
            ++size;
        }
    }
    return size;
}

static int32_t strcasecmp(const char* s1, const char* s2) {
    if (!s1) return (s1 == s2) ? 0 : 1;
    if (!s2) return 1;
    for (; tolower(*s1) == tolower(*s2); ++s1, ++s2)
        if (*s1 == 0) return 0;
    return tolower(*(const unsigned char*)s1) - tolower(*(const unsigned char*)s2);
}

const GenericNode* XMLDecoder::getObjectItem(const GenericNode* parent,
                                                   const char* name, bool caseInsensitive) {
    if (!parent || !name) {
        return parent;
    }

    for (GenericNode* child = parent->child; child; child = child->next) {
        if (child->key && (strlen(name) == child->key_size)) {
            if (!caseInsensitive) {
                if (strncmp(name, child->key, child->key_size) == 0) {
                    return child;
                }
            } else {
                if (strcasecmp(name, std::string(child->key, child->key_size).c_str()) == 0) {
                    return child;
                }
            }
        }
    }

    return NULL;
}

const GenericNode* XMLDecoder::getChild(const GenericNode* parent) {
    if (parent) {
        return parent->child;
    }
    return NULL;
}

const GenericNode* XMLDecoder::getNext(const GenericNode* parent) {
    if (parent) {
        return parent->next;
    }
    return NULL;
}

void XMLDecoder::dealWithString(std::string& str) {
    std::string result;
    result.reserve(str.length());
    const int32_t size = int32_t(str.size());
    for (int32_t idx = 0; idx < size;) {
        const char c = str.at(idx);
        if (c == '&') {
            if ((idx <= size - 4) && str.at(idx + 1) == 'l' && str.at(idx + 2) == 't' &&
                str.at(idx + 3) == ';') {
                idx += 4;
                result.append(1, '<');
                continue;
            } else if ((idx <= size - 4) && str.at(idx + 1) == 'g' && str.at(idx + 2) == 't' &&
                       str.at(idx + 3) == ';') {
                idx += 4;
                result.append(1, '>');
                continue;
            } else if ((idx <= size - 5) && str.at(idx + 1) == 'a' && str.at(idx + 2) == 'm' &&
                       str.at(idx + 3) == 'p' && str.at(idx + 4) == ';') {
                idx += 5;
                result.append(1, '&');
                continue;
            } else if ((idx <= size - 6) && str.at(idx + 1) == 'a' && str.at(idx + 2) == 'p' &&
                       str.at(idx + 3) == 'o' && str.at(idx + 4) == 's' && str.at(idx + 5) == ';') {
                idx += 6;
                result.append(1, '\'');
                continue;
            } else if ((idx <= size - 6) && str.at(idx + 1) == 'q' && str.at(idx + 2) == 'u' &&
                       str.at(idx + 3) == 'o' && str.at(idx + 4) == 't' && str.at(idx + 5) == ';') {
                idx += 6;
                result.append(1, '\"');
                continue;
            } else if ((idx <= size - 1) && str.at(idx + 1) == '#') {
                if ((idx <= size - 2) && str.at(idx + 2) == 'x') {
                    idx += 3;// Skip &#x
                    unsigned long code = 0;
                    while (1) {
                        unsigned char digit =
                            xml::Reader::isHexChas(static_cast<unsigned char>(str.at(idx)));
                        if (digit == 0xFF) {
                            break;
                        }
                        code = code * 16 + digit;
                        ++idx;
                        insert_coded_character(str, code);
                    }
                } else {
                    idx += 2;// Skip &#
                    unsigned long code = 0;
                    while (1) {
                        unsigned char digit =
                            xml::Reader::isHexChas(static_cast<unsigned char>(str.at(idx)));
                        if (digit == 0xFF) {
                            break;
                        }
                        code = code * 10 + digit;
                        ++idx;
                        insert_coded_character(str, code);
                    }
                }
                ++idx;// Skip ;
                continue;
            }
        }
        result.append(1, c);
        ++idx;
    }
    result.swap(str);
}

void XMLDecoder::insert_coded_character(std::string& str, unsigned long code) {
    // Insert UTF8 sequence
    if (code < 0x80) {
        str.append(1, static_cast<unsigned char>(code));
    } else if (code < 0x800) {
        char text[2] = {0};
        text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[0] = static_cast<unsigned char>(code | 0xC0);
        str.append(text);
    } else if (code < 0x10000) {
        char text[3] = {0};
        text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[0] = static_cast<unsigned char>(code | 0xE0);
        str.append(text);
    } else if (code < 0x110000) {
        char text[4] = {0};
        text[3] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[0] = static_cast<unsigned char>(code | 0xF0);
        str.append(text);
    } else {
        // Invalid, only codes up to 0x10FFFF are allowed in Unicode
    }
}

}// namespace serialflex
