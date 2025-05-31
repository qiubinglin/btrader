#pragma once

#include <array>
#include <cstddef>
#include <sstream>
#include <type_traits>
#include <vector>

//------------------------------------------------------------------------
// workaround for using c++20 with hana-1.7.0@conan-center
#if defined(_WINDOWS) && (_MSVC_LANG > 201704L)
namespace std {
template <typename T> struct is_literal_type {};
} // namespace std
#endif
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// workaround for using hana string literals
#ifdef __linux__
#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL
#include <boost/hana.hpp>
using namespace boost::hana::literals;
#define HANA_STR(STR) STR##_s
#else
#include <boost/hana.hpp>
#define HANA_STR(STR) BOOST_HANA_STRING(STR)
#endif
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// pack struct for fixing data length in journal
#ifdef _WINDOWS
#define strcpy(dest, src) strcpy_s(dest, sizeof(dest), src)
#define strncpy(dest, src, max_length) strncpy_s(dest, sizeof(dest), src, max_length)
#define KF_PACK_TYPE_BEGIN __pragma(pack(push, 1))
#define KF_PACK_TYPE_END                                                                                               \
    ;                                                                                                                  \
    __pragma(pack(pop))
#else
#define KF_PACK_TYPE_BEGIN
#define KF_PACK_TYPE_END __attribute__((packed));
#endif
//------------------------------------------------------------------------

template <typename V, size_t N, typename = void> struct array_to_string;

template <typename V, size_t N> struct array_to_string<V, N, std::enable_if_t<std::is_same_v<V, char>>> {
    std::string operator()(const V *v) { return std::string(v); };
};

template <typename V, size_t N> struct array_to_string<V, N, std::enable_if_t<not std::is_same_v<V, char>>> {
    std::string operator()(const V *v) {
        std::stringstream ss;
        ss << "[";
        for (int i = 0; i < N; i++) {
            ss << (i > 0 ? "," : "") << v[i];
        }
        ss << "]";
        return ss.str();
    };
};

template <typename T> struct is_array : public std::false_type {};

template <typename T> static constexpr bool is_array_v = is_array<T>::value;

template <typename V, typename T> struct is_array_of : public std::false_type {};

template <typename V, typename T> static constexpr bool is_array_of_v = is_array_of<V, T>::value;

template <typename V, typename T>
static constexpr bool is_array_of_others_v = is_array_v<V> and not is_array_of_v<V, T>;

template <typename T> struct is_vector : public std::false_type {};

template <typename T, typename A> struct is_vector<std::vector<T, A>> : public std::true_type {};

template <typename DataType> static constexpr bool is_vector_v = is_vector<DataType>::value;

template <typename T, typename = void> struct size_fixed : public std::false_type {};

template <typename DataType>
struct size_fixed<DataType, std::enable_if_t<std::is_fundamental_v<DataType> or std::is_enum_v<DataType>>>
    : public std::true_type {};

template <typename DataType>
struct size_fixed<DataType, std::enable_if_t<std::is_class_v<DataType> and DataType::fixed>> {
    static constexpr bool value = true;
};

template <typename DataType> static constexpr bool size_fixed_v = size_fixed<DataType>::value;
template <typename DataType> static constexpr bool size_unfixed_v = not size_fixed<DataType>::value;

template <typename ValueType>
static constexpr bool is_signed_int_v = std::is_integral_v<ValueType> and (sizeof(ValueType) <= 4) and
                                        not std::is_same_v<ValueType, bool> and std::is_signed_v<ValueType>;

template <typename ValueType>
static constexpr bool is_unsigned_int_v = std::is_integral_v<ValueType> and (sizeof(ValueType) <= 4) and
                                          not std::is_same_v<ValueType, bool> and std::is_unsigned_v<ValueType>;

template <typename ValueType>
static constexpr bool is_signed_bigint_v = //
    std::is_integral_v<ValueType> and (sizeof(ValueType) > 4) and std::is_signed_v<ValueType>;

template <typename ValueType>
static constexpr bool is_unsigned_bigint_v = //
    std::is_integral_v<ValueType> and (sizeof(ValueType) > 4) and std::is_unsigned_v<ValueType>;

template <typename ValueType>
static constexpr bool is_numeric_v = std::is_arithmetic_v<ValueType> or std::is_enum_v<ValueType>;

template <typename ValueType>
static constexpr bool is_enum_class_v = std::is_enum_v<ValueType> and not std::is_convertible_v<ValueType, int>;