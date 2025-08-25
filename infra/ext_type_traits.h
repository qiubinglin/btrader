#pragma once

/**
 * @file ext_type_traits.h
 * @brief Extended type traits and utility functions for type checking and manipulation
 * 
 * This header provides additional type traits beyond the standard library, including:
 * - Array and vector type detection
 * - Fixed-size type identification
 * - Numeric type categorization
 * - Platform-specific workarounds for Boost.Hana and MSVC compatibility
 * - String conversion utilities for arrays
 */

#include <array>
#include <cstddef>
#include <sstream>
#include <type_traits>
#include <vector>
#include <sstream>

//------------------------------------------------------------------------
// Platform-specific workarounds and compatibility fixes
//------------------------------------------------------------------------

/**
 * @brief Workaround for using C++20 with hana-1.7.0@conan-center on Windows
 * 
 * The `is_literal_type` trait was deprecated in C++17 and removed in C++20.
 * This workaround provides a compatibility layer for older Boost.Hana versions
 * that still reference this trait.
 */
#if defined(_WINDOWS) && (_MSVC_LANG > 201704L)
namespace std {
template <typename T> struct is_literal_type {};
} // namespace std
#endif

/**
 * @brief Boost.Hana string literal configuration for different platforms
 * 
 * Configures Boost.Hana string literals with platform-specific optimizations:
 * - Linux: Enables string UDL and uses `_s` suffix for better performance
 * - Other platforms: Uses standard BOOST_HANA_STRING macro
 */
#ifdef __linux__
#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL
#include <boost/hana.hpp>
using namespace boost::hana::literals;
#define HANA_STR(STR) STR##_s
#else
#include <boost/hana.hpp>
#define HANA_STR(STR) BOOST_HANA_STRING(STR)
#endif

/**
 * @brief Cross-platform packed struct definitions for journal data
 * 
 * Provides macros for creating packed structs to ensure consistent data layout
 * across different platforms and compilers. This is crucial for journal files
 * that need to maintain exact byte layouts.
 * 
 * - Windows: Uses MSVC-specific `#pragma pack(push/pop)`
 * - Unix/Linux: Uses GCC/Clang `__attribute__((packed))`
 * 
 * Also includes safe string copy macros for Windows compatibility.
 */
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
// Array and container type traits
//------------------------------------------------------------------------

/**
 * @brief Template for converting arrays to string representation
 * 
 * Specialized for different types:
 * - char arrays: Convert directly to std::string
 * - Other types: Convert to formatted string with brackets and commas
 * 
 * @tparam V The value type of the array
 * @tparam N The size of the array
 * @tparam Enable SFINAE enable_if parameter
 */
template <typename V, size_t N, typename = void> struct array_to_string;

/**
 * @brief Specialization for char arrays - direct string conversion
 */
template <typename V, size_t N> struct array_to_string<V, N, std::enable_if_t<std::is_same_v<V, char>>> {
    std::string operator()(const V *v) { return std::string(v); };
};

/**
 * @brief Specialization for non-char arrays - formatted string conversion
 * 
 * Converts arrays to string format like "[1,2,3,4]" for easy debugging
 * and logging purposes.
 */
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

/**
 * @brief Type trait to detect if a type is an array
 * 
 * Base template returns false, specialized for actual array types.
 * Useful for SFINAE and compile-time type checking.
 */
template <typename T> struct is_array : public std::false_type {};

/** @brief Convenience variable template for is_array */
template <typename T> static constexpr bool is_array_v = is_array<T>::value;

/**
 * @brief Type trait to detect if a type is an array of a specific value type
 * 
 * Base template returns false, specialized for specific array types.
 * Allows checking if a type is an array of a particular element type.
 */
template <typename V, typename T> struct is_array_of : public std::false_type {};

/** @brief Convenience variable template for is_array_of */
template <typename V, typename T> static constexpr bool is_array_of_v = is_array_of<V, T>::value;

/**
 * @brief Check if a type is an array but not of a specific type
 * 
 * Useful for excluding specific array types from generic operations.
 */
template <typename V, typename T>
static constexpr bool is_array_of_others_v = is_array_v<V> and not is_array_of_v<V, T>;

/**
 * @brief Type trait to detect std::vector types
 * 
 * Base template returns false, specialized for std::vector with any allocator.
 */
template <typename T> struct is_vector : public std::false_type {};

/**
 * @brief Specialization for std::vector types
 * 
 * @tparam T The element type of the vector
 * @tparam A The allocator type (defaults to std::allocator<T>)
 */
template <typename T, typename A> struct is_vector<std::vector<T, A>> : public std::true_type {};

/** @brief Convenience variable template for is_vector */
template <typename DataType> static constexpr bool is_vector_v = is_vector<DataType>::value;

//------------------------------------------------------------------------
// Fixed-size type detection
//------------------------------------------------------------------------

/**
 * @brief Type trait to detect if a type has a fixed size
 * 
 * A type is considered fixed-size if:
 * - It's a fundamental type (int, float, etc.)
 * - It's an enum type
 * - It's a class with a static `fixed` member set to true
 * 
 * Fixed-size types are important for journal serialization and
 * network protocols where exact byte layouts must be maintained.
 */
template <typename T, typename = void> struct size_fixed : public std::false_type {};

/**
 * @brief Specialization for fundamental and enum types
 * 
 * These types have known, fixed sizes at compile time.
 */
template <typename DataType>
struct size_fixed<DataType, std::enable_if_t<std::is_fundamental_v<DataType> or std::is_enum_v<DataType>>>
    : public std::true_type {};

/**
 * @brief Specialization for classes with explicit fixed size declaration
 * 
 * Classes can declare themselves as fixed-size by defining a static `fixed` member.
 * This allows custom types to participate in fixed-size type checking.
 */
template <typename DataType>
struct size_fixed<DataType, std::enable_if_t<std::is_class_v<DataType> and DataType::fixed>> {
    static constexpr bool value = true;
};

/** @brief Convenience variable template for size_fixed */
template <typename DataType> static constexpr bool size_fixed_v = size_fixed<DataType>::value;

/** @brief Convenience variable template for size_unfixed (negation of size_fixed) */
template <typename DataType> static constexpr bool size_unfixed_v = not size_fixed<DataType>::value;

//------------------------------------------------------------------------
// Numeric type categorization
//------------------------------------------------------------------------

/**
 * @brief Check if a type is a signed integer with size <= 4 bytes
 * 
 * Excludes bool type and requires signed integers.
 * Examples: int8_t, int16_t, int32_t, int
 */
template <typename ValueType>
static constexpr bool is_signed_int_v = std::is_integral_v<ValueType> and (sizeof(ValueType) <= 4) and
                                        not std::is_same_v<ValueType, bool> and std::is_signed_v<ValueType>;

/**
 * @brief Check if a type is an unsigned integer with size <= 4 bytes
 * 
 * Excludes bool type and requires unsigned integers.
 * Examples: uint8_t, uint16_t, uint32_t, unsigned int
 */
template <typename ValueType>
static constexpr bool is_unsigned_int_v = std::is_integral_v<ValueType> and (sizeof(ValueType) <= 4) and
                                          not std::is_same_v<ValueType, bool> and std::is_unsigned_v<ValueType>;

/**
 * @brief Check if a type is a signed integer with size > 4 bytes
 * 
 * Examples: int64_t, long long
 */
template <typename ValueType>
static constexpr bool is_signed_bigint_v = //
    std::is_integral_v<ValueType> and (sizeof(ValueType) > 4) and std::is_signed_v<ValueType>;

/**
 * @brief Check if a type is an unsigned integer with size > 4 bytes
 * 
 * Examples: uint64_t, unsigned long long
 */
template <typename ValueType>
static constexpr bool is_unsigned_bigint_v = //
    std::is_integral_v<ValueType> and (sizeof(ValueType) > 4) and std::is_unsigned_v<ValueType>;

/**
 * @brief Check if a type is numeric (arithmetic or enum)
 * 
 * Useful for generic operations that should work with any numeric type.
 */
template <typename ValueType>
static constexpr bool is_numeric_v = std::is_arithmetic_v<ValueType> or std::is_enum_v<ValueType>;

/**
 * @brief Check if a type is a scoped enum (enum class)
 * 
 * Scoped enums are safer than traditional enums as they don't implicitly
 * convert to integers and have their own scope.
 */
template <typename ValueType>
static constexpr bool is_enum_class_v = std::is_enum_v<ValueType> and not std::is_convertible_v<ValueType, int>;