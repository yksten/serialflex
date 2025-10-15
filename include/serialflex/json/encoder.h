#ifndef __JSON_ENCODER_H__
#define __JSON_ENCODER_H__

#include <map>
#include <serialflex/field.h>
#include <serialflex/traits.h>

namespace serialflex {

namespace json {
class Writer;
}// namespace json

class EXPORTAPI JSONEncoder {
    json::Writer* writer_;

public:
    explicit JSONEncoder(std::string& str, bool formatted = false);
    ~JSONEncoder();

    template <typename T>
    JSONEncoder& operator&(const Field<T>& field) {
        return convert(field.getName(), field.getValue(), field.has());
    }

    template <typename T>
    JSONEncoder& convert(const char* name, const T& value, const bool* has_value = NULL) {
        if (!has_value || (*has_value == true)) {
            encodeValue(name, *(const typename internal::TypeTraits<T>::Type*)(&value));
        }
        return *this;
    }

    template <typename T>
    bool operator<<(const T& value) {
        startObject(NULL);
        const typename internal::TypeTraits<T>::Type& tratis_value =
            *(const typename internal::TypeTraits<T>::Type*)&value;
        internal::serializeWrapper(
            *this, *const_cast<typename internal::TypeTraits<T>::Type*>(&tratis_value));
        endObject();
        return writerResult();
    }

    template <typename T>
    bool operator<<(const std::vector<T>& value) {
        if (value.empty()) {
            return false;
        }
        startArray(NULL);
        int32_t size = (int32_t)value.size();
        for (int32_t i = 0; i < size; ++i) {
            encodeValue(NULL, *(const typename internal::TypeTraits<T>::Type*)(&value.at(i)));
        }
        endArray();
        return writerResult();
    }

    template <typename K, typename V>
    bool operator<<(const std::map<K, V>& value) {
        if (value.empty()) {
            return false;
        }
        startObject(NULL);
        for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
            const typename internal::TypeTraits<K>::Type& key =
                *(const typename internal::TypeTraits<K>::Type*)(&it->first);
            const typename internal::TypeTraits<V>::Type& item =
                *(const typename internal::TypeTraits<V>::Type*)(&it->second);
            convert(internal::STOT::type<K>::tostr(key).c_str(), item);
        }
        endObject();
        return writerResult();
    }

private:
    template <typename T>
    void encodeValue(const char* name, const T& value) {
        startObject(name);
        const typename internal::TypeTraits<T>::Type& tratis_value =
            *(const typename internal::TypeTraits<T>::Type*)&value;
        internal::serializeWrapper(
            *this, *const_cast<typename internal::TypeTraits<T>::Type*>(&tratis_value));
        endObject();
    }

    template <typename T>
    void encodeValue(const char* name, const std::vector<T>& value) {
        startArray(name);
        int32_t size = (int32_t)value.size();
        for (int32_t i = 0; i < size; ++i) {
            if (i) {
                writerSeparation();
            }
            encodeValue(NULL, *(const typename internal::TypeTraits<T>::Type*)(&value.at(i)));
        }
        endArray();
    }

    template <typename K, typename V>
    void encodeValue(const char* name, const std::map<K, V>& value) {
        startObject(name);
        for (typename std::map<K, V>::const_iterator it = value.begin(); it != value.end(); ++it) {
            const typename internal::TypeTraits<K>::Type& key =
                *(const typename internal::TypeTraits<K>::Type*)(&it->first);
            const typename internal::TypeTraits<V>::Type& item =
                *(const typename internal::TypeTraits<V>::Type*)(&it->second);
            convert(internal::STOT::type<K>::tostr(key).c_str(), item);
        }
        endObject();
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
    void endObject();
    void startArray(const char* name);
    void endArray();

    // for Writer
    bool writerResult() const;
    void writerSeparation();
};

}// namespace serialflex

#endif
