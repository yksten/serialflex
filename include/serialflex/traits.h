#ifndef __SERIALFLEX_TRAITS_H__
#define __SERIALFLEX_TRAITS_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>

#ifdef _MSC_VER
#ifdef EXPORTAPI
#define EXPORTAPI _declspec(dllimport)
#else
#define EXPORTAPI _declspec(dllexport)
#endif
#else
#define EXPORTAPI
#endif

namespace serialflex {

namespace internal {

template <class T, class C> void serialize(T& t, C& c) { c.serialize(t); }

template <class T, class C> void serializeWrapper(T& t, C& c) {
    serialize(t, c);
}
#if __cplusplus >= 201103L
template <typename T, bool is_enum = std::is_enum<T>::value> struct TypeTraits {
    typedef T Type;
};
#else
template <typename From, typename To> class is_convertible {
    typedef char one;
    typedef int two;

    template <typename To1> static To1& create();

    template <typename To1> static one test(To1);

    template <typename> static two test(...);

public:
    static const bool value = (sizeof(test<To>(create<From>())) == sizeof(one));
};

template <class T> struct isIntegral {
    static const bool value = false;
};
template <> struct isIntegral<bool> {
    static const bool value = true;
};
template <> struct isIntegral<int8_t> {
    static const bool value = true;
};
template <> struct isIntegral<uint8_t> {
    static const bool value = true;
};
template <> struct isIntegral<int16_t> {
    static const bool value = true;
};
template <> struct isIntegral<uint16_t> {
    static const bool value = true;
};
template <> struct isIntegral<int32_t> {
    static const bool value = true;
};
template <> struct isIntegral<uint32_t> {
    static const bool value = true;
};
template <> struct isIntegral<int64_t> {
    static const bool value = true;
};
template <> struct isIntegral<uint64_t> {
    static const bool value = true;
};
template <> struct isIntegral<float> {
    static const bool value = true;
};
template <> struct isIntegral<double> {
    static const bool value = true;
};
template <> struct isIntegral<long> {
    static const bool value = true;
};
template <> struct isIntegral<unsigned long> {
    static const bool value = true;
};

template <class T> struct is_enum {
    static const bool value =
        is_convertible<T, int32_t>::value & !isIntegral<T>::value;
};

template <typename T, bool is_enum = is_enum<T>::value> struct TypeTraits {
    typedef T Type;
};
#endif
template <typename T, bool is_enum> struct TypeTraits<std::vector<T>, is_enum> {
    typedef std::vector<T> Type;
};
template <typename T> struct TypeTraits<T, true> {
    typedef int32_t Type;
};

namespace STOT {
enum { BUFSIZE = 128 };
template <typename T> struct type {};
// bool
template <> struct type<bool> {
    static inline bool strto(const char* str) { return atoi(str) != 0; }
    static inline std::string tostr(bool v) { return v ? "1" : "0"; }
};
// int32_t
template <> struct type<int32_t> {
    static inline int32_t strto(const char* str) { return (int32_t)atoi(str); }
    static inline std::string tostr(int32_t v) {
        char trans_buffer[BUFSIZE] = {0};
        snprintf(trans_buffer, BUFSIZE, "%d", v);
        return std::string(trans_buffer);
    }
};
// uint32_t
template <> struct type<uint32_t> {
    static inline uint32_t strto(const char* str) {
        return (uint32_t)atoi(str);
    }
    static inline std::string tostr(uint32_t v) {
        char trans_buffer[BUFSIZE] = {0};
        snprintf(trans_buffer, BUFSIZE, "%u", v);
        return std::string(trans_buffer);
    }
};
// int64_t
template <> struct type<int64_t> {
    static inline int64_t strto(const char* str) { return atoll(str); }
    static inline std::string tostr(int64_t v) {
        char trans_buffer[BUFSIZE] = {0};
        snprintf(trans_buffer, BUFSIZE, "%lld", (long long)v);
        return std::string(trans_buffer);
    }
};
// uint64_t
template <> struct type<uint64_t> {
    static inline uint64_t strto(const char* str) {
        return (uint64_t)atoll(str);
    }
    static inline std::string tostr(uint64_t v) {
        char trans_buffer[BUFSIZE] = {0};
        snprintf(trans_buffer, BUFSIZE, "%llu", (long long)v);
        return std::string(trans_buffer);
    }
};
// std::string
template <> struct type<std::string> {
    static inline const std::string& strto(const std::string& str) {
        return str;
    }
    static inline const std::string& tostr(const std::string& v) { return v; }
};

}// namespace STOT

}// namespace internal

}// namespace serialflex

#endif
