#include <json/reader.h>
#include <serialflex/json/decoder.h>

namespace serialflex {

static unsigned parse_hex4(const char* str) {
    unsigned h = 0;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    h = h << 4;
    str++;
    if (*str >= '0' && *str <= '9')
        h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F')
        h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f')
        h += 10 + (*str) - 'a';
    else
        return 0;
    return h;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
static bool parse_string(std::string& strValue, const char* str, uint32_t length) {
    if (!length) {
        return false;
    } /* not a string! */
    const char* ptr = str;
    unsigned uc, uc2;
    for (uint32_t i = 0; i < length; ++i) {
        if (ptr[i] != '\\') {
            strValue.append(1, ptr[i]);
        } else {
            ++i;
            switch (ptr[i]) {
                case 'b':
                    strValue.append(1, '\b');
                    break;
                case 'f':
                    strValue.append(1, '\f');
                    break;
                case 'n':
                    strValue.append(1, '\n');
                    break;
                case 'r':
                    strValue.append(1, '\r');
                    break;
                case 't':
                    strValue.append(1, '\t');
                    break;
                case 'u':      /* transcode utf16 to utf8. */
                    uc = parse_hex4(ptr + i + 1);
                    i += 4;    /* get the unicode char. */
                    if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0)
                        break; /* check for invalid.    */
                    /* UTF16 surrogate pairs.    */
                    if (uc >= 0xD800 && uc <= 0xDBFF) {
                        if (ptr[i + 1] != '\\' || ptr[i + 2] != 'u')
                            break; /* missing second-half of surrogate.    */
                        uc2 = parse_hex4(ptr + i + 3);
                        i += 6;
                        if (uc2 < 0xDC00 || uc2 > 0xDFFF)
                            break; /* invalid second-half of surrogate.    */
                        uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
                    }

                    if (uc < 0x80) {
                        strValue.append(1, uc | firstByteMark[1]);
                    } else if (uc < 0x800) {
                        strValue.append(1, '\0');
                        strValue.append(1, (uc | 0x80) & 0xBF);
                        uc >>= 6;
                    } else if (uc < 0x10000) {
                        strValue.append(2, '\0');
                        strValue.append(1, (uc | 0x80) & 0xBF);
                        uc >>= 6;
                    } else {
                        strValue.append(3, '\0');
                        strValue.append(1, (uc | 0x80) & 0xBF);
                        uc >>= 6;
                    }
                    break;
                default:
                    strValue.append(1, ptr[i]);
                    break;
            }
        }
    }
    return true;
}

JSONDecoder::JSONDecoder(const char* str, bool case_insensitive)
    : convert_by_type_(true), case_insensitive_(case_insensitive), current_(NULL) {
    reader_ = new json::Reader();
    current_ = reader_->parse(str);
    assert(current_);
}

JSONDecoder::~JSONDecoder() { delete reader_; }

JSONDecoder& JSONDecoder::setConvertByType(bool convert_by_type) {
    convert_by_type_ = convert_by_type;
    return *this;
}

const char* JSONDecoder::getError() const {
    if (!reader_) {
        return "reader is null";
    }
    return reader_->getError();
}

void JSONDecoder::decodeValue(const char* name, bool& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_BOOL)) {
        value = item2Bool(*item);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, uint32_t& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_NUMBER)) {
        value = (uint32_t)json::Reader::convertUint(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, int32_t& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_NUMBER)) {
        value = (int32_t)json::Reader::convertInt(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, uint64_t& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_NUMBER)) {
        value = json::Reader::convertUint(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, int64_t& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_NUMBER)) {
        value = json::Reader::convertInt(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, float& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_NUMBER)) {
        value = (float)json::Reader::convertDouble(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, double& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_NUMBER)) {
        value = json::Reader::convertDouble(item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
    }
}

void JSONDecoder::decodeValue(const char* name, std::string& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_STRING) && item->value && item->value_size) {
        value.clear();
        bool result = parse_string(value, item->value, item->value_size);
        if (has_value) {
            *has_value = true;
        }
        assert(result);
    }
}

void JSONDecoder::decodeValue(const char* name, std::vector<bool>& value, bool* has_value) {
    const GenericNode* item = JSONDecoder::getObjectItem(current_, name, case_insensitive_);
    if (item && checkItemType(*item, json::VALUE_ARRAY)) {
        for (const GenericNode* child = item->child;
             child && checkItemType(*item, json::VALUE_BOOL); child = child->next) {
            value.push_back(item2Bool(*child));
            if (has_value) {
                *has_value = true;
            }
        }
    }
}

bool JSONDecoder::checkItemType(const GenericNode& item, const int type) const {
    if (!convert_by_type_ && item.type != json::VALUE_NULL) {
        return true;
    }
    return (item.type == type);
}

bool JSONDecoder::item2Bool(const GenericNode& item) const {
    if (item.type == json::VALUE_BOOL) {
        return (item.value_size == 4);
    } else if (!convert_by_type_) {
        if (item.type == json::VALUE_NUMBER) {
            int64_t value = json::Reader::convertInt(item.value, item.value_size);
            return (value != 0);
        } else if (item.type == json::VALUE_STRING) {
            std::string value;
            if (parse_string(value, item.value, item.value_size)) {
                return (atoi(value.c_str()));
            }
        } else {
            ;
        }
    }
    return false;
}

uint32_t JSONDecoder::getObjectSize(const GenericNode* parent) {
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

const GenericNode* JSONDecoder::getObjectItem(const GenericNode* parent, const char* name,
                                              bool case_insensitive) {
    if (!parent || !name) {
        return parent;
    }

    for (GenericNode* child = parent->child; child; child = child->next) {
        if (child->key && (strlen(name) == child->key_size)) {
            if (!case_insensitive) {
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

const GenericNode* JSONDecoder::getChild(const GenericNode* parent) {
    if (parent) {
        return parent->child;
    }
    return NULL;
}

const GenericNode* JSONDecoder::getNext(const GenericNode* parent) {
    if (parent) {
        return parent->next;
    }
    return NULL;
}

const char* JSONDecoder::getKey(const GenericNode* parent) {
    if (parent) {
        return parent->key;
    }
    return NULL;
}

uint32_t JSONDecoder::getKeySize(const GenericNode* parent) {
    if (parent) {
        return parent->key_size;
    }
    return 0;
}

}// namespace serialflex
