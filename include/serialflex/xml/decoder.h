#ifndef __XML_DECODER_H__
#define __XML_DECODER_H__

#include <map>
#include <serialflex/field.h>
#include <serialflex/traits.h>

namespace serialflex {

namespace xml {
class Reader;
}// namespace xml

struct GenericNode;
class EXPORTAPI XMLDecoder {
    bool convert_by_type_; // convert by field type
    bool case_insensitive_;// key case insensitive
    xml::Reader* reader_;
    const GenericNode* current_;

    XMLDecoder(const XMLDecoder&);
    XMLDecoder& operator=(const XMLDecoder&);

public:
    XMLDecoder(const char* str, bool case_insensitive = false);
    ~XMLDecoder();
    
    const char* getError() const;

    // convert by field type
    XMLDecoder& setConvertByType(bool convert_by_type);

    template <typename T>
    XMLDecoder& operator&(const Field<T>& field) {
        Field<T>& remove_const_field = *const_cast<Field<T>*>(&field);
        return convert(field.getName(), remove_const_field.value(), remove_const_field.has());
    }

    template <typename T>
    XMLDecoder& convert(const char* name, T& value, bool* has_value = NULL) {
        decodeValue(name, *(typename internal::TypeTraits<T>::Type*)(&value), has_value);
        return *this;
    }

    template <typename T>
    bool operator>>(T& value) {
        if (!current_) {
            return false;
        }
        const GenericNode* parent = current_;
        internal::serializeWrapper(*this, value);

        if (parent != current_) {
            return false;
        }
        if (getError()) {
            return false;
        }
        return true;
    }

    template <typename T>
    bool operator>>(std::vector<T>& value) {
        if (!current_) {
            return false;
        }
        const GenericNode* parent = current_;
        // uint32_t size = XMLDecoder::getObjectSize(current_);
        // if (size) {
        //     value.resize(size);
        // }
        const GenericNode* parent_temp = current_;
        current_ = XMLDecoder::getChild(current_);
        value.clear();
        for (uint32_t idx = 0; current_; (current_ = XMLDecoder::getNext(current_)), ++idx) {
            T item = T();
            decodeValue("value", *(typename internal::TypeTraits<T>::Type*)(&item), NULL);
            value.push_back(item);
        }
        current_ = parent_temp;

        if (parent != current_) {
            return false;
        }
        if (getError()) {
            return false;
        }
        return true;
    }

    template <typename K, typename V>
    bool operator>>(std::map<K, V>& value) {
        if (!current_) {
            return false;
        }
        const GenericNode* parent = current_;
        value.clear();
        const GenericNode* parent_temp = current_;
        for (const GenericNode* child = XMLDecoder::getChild(parent); child;
             child = XMLDecoder::getNext(child)) {
            K key = K();
            V item = V();
            decodeValue("key", *(typename internal::TypeTraits<K>::Type*)(&key), NULL);
            decodeValue("value", *(typename internal::TypeTraits<V>::Type*)(&item), NULL);
            value.insert(std::pair<K, V>(key, item));
        }
        current_ = parent_temp;

        if (parent != current_) {
            return false;
        }
        if (getError()) {
            return false;
        }
        return true;
    }

private:
    template <typename T>
    void decodeValue(const char* name, T& value, bool* has_value) {
        const GenericNode* parent = current_;
        current_ = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
        if (current_) {
            internal::serializeWrapper(*this, value);
            if (has_value) *has_value = true;
        }
        current_ = parent;
    }

    template <typename T>
    void decodeValue(const char* name, std::vector<T>& value, bool* has_value) {
        const GenericNode* parent = current_;
        current_ = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
        if (current_) {
            value.clear();
            // uint32_t size = XMLDecoder::getObjectSize(current_);
            // if (size) {
            //     value.resize(size);
            // }
            const GenericNode* parent_temp = current_;
            current_ = XMLDecoder::getChild(current_);
            for (uint32_t idx = 0; current_; (current_ = XMLDecoder::getNext(current_)), ++idx) {
                T item = T();
                decodeValue(NULL, *(typename internal::TypeTraits<T>::Type*)(&item), NULL);
                value.push_back(item);
            }
            current_ = parent_temp;
            if (has_value) {
                *has_value = true;
            }
        }
        current_ = parent;
    }

    template <typename K, typename V>
    void decodeValue(const char* name, std::map<K, V>& value, bool* has_value) {
        const GenericNode* parent = current_;
        current_ = XMLDecoder::getObjectItem(current_, name, case_insensitive_);
        if (current_) {
            value.clear();

            const GenericNode* parent_temp = current_;
            for (current_ = XMLDecoder::getChild(current_); current_;
                 current_ = XMLDecoder::getNext(current_)) {
                K key = K();
                V item = V();
                decodeValue("key", *(typename internal::TypeTraits<K>::Type*)(&key), NULL);
                decodeValue("value", *(typename internal::TypeTraits<V>::Type*)(&item), NULL);
                value.insert(std::pair<K, V>(key, item));
            }
            current_ = parent_temp;
            if (has_value) {
                *has_value = true;
            }
        }
        current_ = parent;
    }

    void decodeValue(const char* name, bool& value, bool* has_value);
    void decodeValue(const char* name, uint32_t& value, bool* has_value);
    void decodeValue(const char* name, int32_t& value, bool* has_value);
    void decodeValue(const char* name, uint64_t& value, bool* has_value);
    void decodeValue(const char* name, int64_t& value, bool* has_value);
    void decodeValue(const char* name, float& value, bool* has_value);
    void decodeValue(const char* name, double& value, bool* has_value);
    void decodeValue(const char* name, std::string& value, bool* has_value);
    void decodeValue(const char* name, std::vector<bool>& value, bool* has_value);
    bool item2Bool(const GenericNode& item) const;

    // for value
    static uint32_t getObjectSize(const GenericNode* parent);
    static const GenericNode* getObjectItem(const GenericNode* parent, const char* name,
                                            bool case_insensitive);
    static const GenericNode* getChild(const GenericNode* parent);
    static const GenericNode* getNext(const GenericNode* parent);

    static void dealWithString(std::string& str);
    static void insert_coded_character(std::string& str, unsigned long code);
};

}// namespace serialflex

#endif
