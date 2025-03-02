# serialflex
### 一、简介
C++98序列化库，C++类与JSON快速超高效互转库。快速、简洁的 API 设计，大大降低直接使用JSON解析库来实现此类功能的代码复杂度。也没有外部依赖。  

***注：*** 该库旨在C++和json互转（后续可能会支持xml、protobuf）。 

支持结构体类型：
* 支持`bool`、`uint32_t`、`int32_t`、`uint64_t`、`int64_t`、`float`、`double`、`enum`、`std::string`、`std::vector`和`std::map`等。（不支持`std::list`）
* `std::map`的key只能是基础类型，且不能是`float`、`double`。
* 不支持指针和固定数组。（以后也不考虑支持指针和数组）
* 数组内需要统一类型，不支持`[1,2,{"key":"value"}]`。
* 解析时支持has功能。


### 二、性能(benchmark)
```
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
Encode                  42850 ns        42812 ns        15775
Decode                  13109 ns        13085 ns        56355
cJSON Encode           130396 ns       130237 ns         5382
cJSON Decode            78503 ns        78451 ns         8670
```

### 三、编译
```
$ git clone https://github.com/yksten/serialflex.git
$ cd serialflex
$ mkdir -p cmake/build
$ pushd cmake/build
$ cmake -DEXAMPLE=ON ../..
$ make -j
$ popd
```

### 四、如何使用

#### 1.声明结构体：

##### a.常规方式实现serialize函数：

```c++
/* ------头文件 h、hpp------ */
class Data {
  int32_t a;
  bool has_a;
  uint32_t b;
  bool has_b;
  float c;
  bool has_c;
  double d;
  bool has_d;
  StructItem e; // StructItem里需要实现serialize模板函数
  bool has_e;
public:
  Data() : a(0), has_a(false), b(0), has_b(false), c(0.0f), has_c(false), d(0.0f), has_d(false),, has_e(false) {}

  void set_a(const int32_t& value) { a = value; has_a = true; }
  const int32_t& get_a() const { return a; }
  void set_b(const uint32_t& value) { b = value; has_b = true; }
  const int32_t& get_b() const { return b; }
  void set_c(const float& value) { c = value; has_c = true; }
  const int32_t& get_c() const { return c; }
  void set_d(const double& value) { d = value; has_d = true; }
  const int32_t& get_d() const { return d; }
  void set_e(const StructItem& value) { e = value; has_e = true; }
  const int32_t& get_e() const { return e; }

  template <typename Archive>
  void serialize(Archive& ar) {
    ar.convert("a", a, &has_a).convert("b", b, &has_b).convert("c", c, &has_c);
    ar.convert("d", d, &has_d).convert("e", e, , &has_e);
  }
};
```

##### b.通过宏方式实现serialize函数：

*   SERIALIZE宏暂时没有has功能。

```c++
/* ------头文件 h、hpp------ */
#include <serialflex/serialflex.h>
class Data {
  int32_t a;
  bool has_a;
  uint32_t b;
  bool has_b;
  float c;
  bool has_c;
  double d;
  bool has_d;
  StructItem e; // StructItem里需要实现serialize模板函数
  bool has_e;
public:
  Data() : a(0), has_a(false), b(0), has_b(false), c(0.0f), has_c(false), d(0.0f), has_d(false),, has_e(false) {}

  void set_a(const int32_t& value) { a = value; has_a = true; }
  const int32_t& get_a() const { return a; }
  void set_b(const uint32_t& value) { b = value; has_b = true; }
  const int32_t& get_b() const { return b; }
  void set_c(const float& value) { c = value; has_c = true; }
  const int32_t& get_c() const { return c; }
  void set_d(const double& value) { d = value; has_d = true; }
  const int32_t& get_d() const { return d; }
  void set_e(const StructItem& value) { e = value; has_e = true; }
  const int32_t& get_e() const { return e; }

  template <typename Archive>
  void serialize(Archive& ar) {
    // SERIALIZE参数限制63个
    SERIALIZE(ar, a, b, c, d, e);
    // SERIALIZE(ar, f); /* 超过63个参数后建议换行后继续使用SERIALIZE */
  }
};
```

#### 2.常规使用方式：

```c++
/* ------源文件 cpp、cc------ */
// 编码
Data data; /* 假设data已赋值 */
std::string json;
bool result = serialflex::JSONEncoder(json) << data;
// 解码
Data data;
std::string json; /* 假设json已被赋值 */
bool result = serialflex::JSONDecoder(json.c_str()) >> data;
```

#### 3.非侵入式序列化（不建议使用）：

*   原理：C++模板 **SFINAE**
    
*   适用范围：非侵入式serialize函数，仅在极端情况下使用，如外部的结构体。
    
*   非侵入式serialize函数声明定义在cpp/cc文件里时，那么也仅在当前cpp/cc文件里有效。部分情况需要考虑命名空间问题，实现困难。
    

```c++
/* ------源文件 cpp、cc------ */
// 非侵入式序列化，和侵入式序列化同时存在时，会优先有这里
template <typename Archive>
void serialize(Archive& ar, Data& data) {
  ar.convert("a", data.a).convert("b", data.b).convert("c", data.c);
  ar.convert("d", data.d).convert("e", data.e);
  // 如果包含了 #include <serialflex/serialflex.h> 头文件可以这样实现serialize函数：
  // NISERIALIZE(ar, data, a, b, c, d, e); /* NISERIALIZE也是限制63个参数 */
}
// 编码
Data data; /* 假设data已赋值 */
std::string json;
bool result = serialflex::JSONEncoder(json) << data;
// 解码
Data data;
std::string json; /* 假设json已被赋值 */
bool result = serialflex::JSONDecoder(json.c_str()) >> data;
```

#### 4.编码和解码函数分开（不建议使用）：

*   缺点：新增成员变量需要在两个函数实现，容易遗漏。
    

```c++
/* ------头文件 h、hpp------ */
#include <serialflex/json/encoder.h>
#include <serialflex/json/decoder.h>

class Data {
public:
  int32_t a;
  uint32_t b;
  float c;
  double d;
  StructItem e;
  // #include <serialflex/json/encoder.h>
  void serialize(serialflex::JSONEncoder& ar) {
    if (/*老版本*/) {
      a = 666;
    }
    ar.convert("a", a).convert("b", b).convert("c", c);
    ar.convert("d", d).convert("e", e);
  }
  // #include <serialflex/json/decoder.h>
  void serialize(serialflex::JSONDecoder& ar) {
    ar.convert("a", a).convert("b", b).convert("c", c);
    ar.convert("d", d).convert("e", e);
  }
};
```

### 五、CRTP技术实现encode、decode：

*   为了达到最小依赖、降低产物大小，源码里未实现这些功能：
    

```c++
#include <serialflex/json/encoder.h>
#include <serialflex/json/decoder.h>

template <typename Derived>
class ConvertWrapper {
public:
  explicit ConvertWrapper(Derived& d) : _d(d) {
  }

  inline bool operator>>(std::string& json) {
    return serialflex::JSONEncoder().operator<<(*((Derived*)this)).toString(json);
  }

  inline bool operator<<(const std::string& json) {
    return serialflex::JSONDecoder(json.c_str()) >> *((Derived*)this);
  }  
};
```
```c++
// 编码
Data data; /* 假设data已赋值 */
std::string json;
bool result = ConvertWrapper(data) >> json;
// 解码
Data data;
std::string json; /* 假设json已被赋值 */
bool result = ConvertWrapper(data) << json;
```
```c++
/* ------可使用宏代替------ */
#define ENCODE_TO_JSON(data, json) serialflex::JSONEncoder().operator<<(data).toString(json);
#define DECODE_FROM_JSON(data, json) serialflex::JSONDecoder(json.c_str()) >> data;
```

### 六、黑科技

#### 1.枚举是实现如何实现编码、解码的？

*   原理：**模板类型萃取**。
    
*   缺点：不能判断json里的值是否在枚举范围内。
    
*   为什么不使用std::is\_enum？
    
    *   std::is\_enum是C++11提供的，serialflex实现之初是为了支持C++98语法。
        

```c++
template <class T> struct isIntegral { static const bool value = false; };
template <> struct isIntegral<bool> { static const bool value = true; };
template <> struct isIntegral<int8_t> { static const bool value = true; };
template <> struct isIntegral<uint8_t> { static const bool value = true; };
template <> struct isIntegral<int16_t> { static const bool value = true; };
template <> struct isIntegral<uint16_t> { static const bool value = true; };
template <> struct isIntegral<int32_t> { static const bool value = true; };
template <> struct isIntegral<uint32_t> { static const bool value = true; };
template <> struct isIntegral<int64_t> { static const bool value = true; };
template <> struct isIntegral<uint64_t> { static const bool value = true; };
template <> struct isIntegral<float> { static const bool value = true; };
template <> struct isIntegral<double> { static const bool value = true; };

template <typename From, typename To> struct is_convertible { static const bool value = sizeof(ConvertHelper::Test<To>(ConvertHelper::Create<From>())) == sizeof(ConvertHelper::YesType); };
// 判断类型是否是枚举enum
template <class T> struct is_enum { static const bool value = is_convertible<T, int>::value && !isIntegral<T>::value; };
// 是枚举enum的情况下萃取类型为int
template <typename T, bool flag = is_enum<T>::value> struct UnwrapTraits { typedef T Type; };
template <typename T> struct UnwrapTraits<T, true> { typedef int32_t Type; };
```
```c++
/* ---编码---*/
template <typename T>
JSONEncoder& convert(const char* name, const T& value, bool* has_value = NULL) {
  setValue(name, *(typename internal::UnwrapTraits<T>::Type*)(&value));
  return *this;
}
/* ---解码---*/
template <typename T>
JSONDecoder& convert(const char* name, T& value, bool* has_value = NULL) {
  getValue(name, *(typename internal::UnwrapTraits<T>::Type*)(&value), has_value);
  return *this;
}
```

### 七、工具

#### `tool\json2cpp`是辅助工具，可以将json字符串转换为C++ class。
*   json字符串：
```json
{
    "a": 1,
    "b": 1.5,
    "c": {
        "d": [
            1,
            2,
            3
        ]
    }
}
```
*   生成的c++源码：
```c++
// Generated according to JSON, for reference only!
#include <stdint.h>
#include <vector>


class CXX_C {
    std::vector<int32_t> d;
    bool has_d;
public:
    CXX_C()  {}

    void set_d(const std::vector<int32_t>& value) { d = value; has_d = true; }
    const std::vector<int32_t>& get_d() const { return d; }
    template<typename Archive>
    void serialize(Archive& ar) {
        ar.convert("d", d, &has_d);
    }
};

class CXX_Result {
    int32_t a;
    bool has_a;
    float b;
    bool has_b;
    CXX_C c;
    bool has_c;
public:
    CXX_Result() : a(0), b(0) {}

    void set_a(const int32_t& value) { a = value; has_a = true; }
    const int32_t& get_a() const { return a; }
    void set_b(const float& value) { b = value; has_b = true; }
    const float& get_b() const { return b; }
    void set_c(const CXX_C& value) { c = value; has_c = true; }
    const CXX_C& get_c() const { return c; }
    template<typename Archive>
    void serialize(Archive& ar) {
        ar.convert("a", a, &has_a).convert("b", b, &has_b).convert("c", c, &has_c);
    }
};
```