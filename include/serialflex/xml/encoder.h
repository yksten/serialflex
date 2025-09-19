#ifndef __XML_ENCODER_H__
#define __XML_ENCODER_H__

#include <map>
#include <serialflex/traits.h>

namespace serialflex {

namespace xml {
class Writer;
}// namespace xml

class EXPORTAPI XMLEncoder {
    xml::Writer* writer_;

    XMLEncoder(const XMLEncoder&);
    XMLEncoder& operator=(const XMLEncoder&);

public:
    explicit XMLEncoder(std::string& str, bool formatted = false);
    ~XMLEncoder();

    template <typename T>
    XMLEncoder& convert(const char* name, const T& value, bool* has_value = NULL) {
        if (!has_value || (*has_value == true)) {
            encodeValue(name, *(const typename internal::TypeTraits<T>::Type*)(&value));
        }
        return *this;
    }

    template <typename T> bool operator<<(const T& value) {
        startObject("serialflex");
        internal::serializeWrapper(*this,
                                   *const_cast<typename internal::TypeTraits<T>::Type*>(
                                       (const typename internal::TypeTraits<T>::Type*)&value));
        endObject("serialflex");
        return writerResult();
    }

    template <typename T> bool operator<<(const std::vector<T>& value) {
        startObject("serialflex");
        int32_t size = (int32_t)value.size();
        for (int32_t i = 0; i < size; ++i) {
            const typename internal::TypeTraits<T>::Type& item = value.at(i);
            encodeValue("value", item);
        }
        endObject("serialflex");
        return writerResult();
    }

    template <typename K, typename V> bool operator<<(const std::map<K, V>& value) {
        startObject("serialflex");
        for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
            typename internal::TypeTraits<K>::Type* pKey =
                const_cast<typename internal::TypeTraits<K>::Type*>(&(it->first));
            typename internal::TypeTraits<V>::Type* pValue =
                const_cast<typename internal::TypeTraits<V>::Type*>(&(it->second));
            startObject("value");
            encodeValue("key", *pKey);
            encodeValue("value", *pKey);
            endObject("value");
        }
        endObject("serialflex");
        return writerResult();
    }

private:
    template <typename T> void encodeValue(const char* name, const T& value) {
        startObject(name);
        internal::serializeWrapper(*this,
                                   *const_cast<typename internal::TypeTraits<T>::Type*>(
                                       (const typename internal::TypeTraits<T>::Type*)(&value)));
        endObject(name);
    }

    template <typename T> void encodeValue(const char* name, const std::vector<T>& value) {
        startObject(name);
        int32_t size = (int32_t)value.size();
        for (int32_t i = 0; i < size; ++i) {
            const typename internal::TypeTraits<T>::Type& item = value.at(i);
            encodeValue("value", item);
        }
        endObject(name);
    }

    template <typename K, typename V>
    void encodeValue(const char* name, const std::map<K, V>& value) {
        startObject(name);
        for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
            const V& item = it->second;
            startObject("value");
            encodeValue("key", it->first);
            encodeValue("value", item);
            endObject("value");
        }
        endObject(name);
    }

    void encodeValue(const char* name, const bool& value);
    void encodeValue(const char* name, const uint32_t& value);
    void encodeValue(const char* name, const int32_t& value);
    void encodeValue(const char* name, const uint64_t& value);
    void encodeValue(const char* name, const int64_t& value);
    void encodeValue(const char* name, const float& value);
    void encodeValue(const char* name, const double& value);
    void encodeValue(const char* name, const std::string& value);
    void encodeValue(const char* name, const std::vector<bool>& value);
    //
    void startObject(const char* name);
    void endObject(const char* name);

    // for Writer
    bool writerResult() const;
};

}// namespace serialflex

#endif
