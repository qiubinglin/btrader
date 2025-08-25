#pragma once

/**
 * @file array.h
 * @brief Custom fixed-size array template with enhanced functionality
 * 
 * This header provides a custom Array template class that wraps C-style arrays
 * with additional features like string conversion, JSON serialization, and
 * implicit conversions. It's designed for use in journal files and other
 * scenarios where fixed-size arrays with rich functionality are needed.
 */

#include <cstring>

#include "common.h"
#include "ext_type_traits.h"

namespace infra {

/**
 * @brief Fixed-size array template with enhanced functionality
 * 
 * This template class provides a wrapper around C-style arrays with additional
 * features beyond std::array:
 * - Automatic zero-initialization
 * - String conversion capabilities
 * - JSON serialization support
 * - Implicit conversions to various pointer types
 * - Custom assignment operators with type-specific behavior
 * 
 * @tparam T The element type of the array
 * @tparam N The fixed size of the array
 */
template <typename T, size_t N> struct Array {
    static constexpr size_t length = N;    ///< Compile-time array length
    using element_type = T;                 ///< Type of array elements
    using type = T[N];                     ///< C-style array type
    type value;                            ///< The underlying array data

    /**
     * @brief Default constructor with zero-initialization
     * 
     * Initializes all elements to zero. For char arrays, uses null terminators
     * to ensure proper string behavior.
     */
    Array() {
        if constexpr (std::is_same_v<T, char>) {
            // For char arrays, initialize with null terminators for string safety
            memset(value, '\0', sizeof(value));
        } else {
            // For other types, initialize with zeros
            memset(value, 0, sizeof(value));
        }
    }

    /**
     * @brief Copy constructor
     * 
     * Uses default copy constructor for efficient copying.
     */
    Array(const Array<T, N> &other) = default;

    /**
     * @brief Constructor from pointer to data
     * 
     * Copies data from a pointer into the array. Assumes the source
     * has at least N elements.
     * 
     * @param t Pointer to source data
     */
    explicit Array(const T *t) { memcpy(value, t, sizeof(value)); }

    /**
     * @brief Constructor from unsigned char pointer
     * 
     * Allows construction from unsigned char data, useful for binary data
     * or when working with raw memory buffers.
     * 
     * @param t Pointer to unsigned char source data
     */
    explicit Array(const unsigned char *t) { memcpy(value, t, sizeof(value)); }

    /**
     * @brief Get the size of the array
     * 
     * @return The fixed size N of the array
     */
    [[nodiscard]] size_t size() const { return N; }

    /**
     * @brief Convert array to string representation
     * 
     * Uses the array_to_string utility from ext_type_traits.h to create
     * a human-readable string representation of the array contents.
     * 
     * @return String representation of the array
     */
    [[nodiscard]] std::string to_string() const { return array_to_string<T, N>{}(value); }

    /**
     * @brief Implicit conversion to T pointer
     * 
     * Allows the array to be used anywhere a T* is expected.
     * 
     * @return Pointer to the first element
     */
    operator T *() { return static_cast<T *>(value); }

    /**
     * @brief Implicit conversion to const T pointer
     * 
     * Allows the array to be used anywhere a const T* is expected.
     * 
     * @return Const pointer to the first element
     */
    operator const T *() const { return static_cast<const T *>(value); }

    /**
     * @brief Implicit conversion to const void pointer
     * 
     * Useful for passing to functions that expect void* (e.g., memcpy).
     * 
     * @return Const void pointer to the array data
     */
    operator const void *() const { return static_cast<const void *>(value); }

    /**
     * @brief Implicit conversion to std::string
     * 
     * Automatically converts the array to its string representation.
     * 
     * @return String representation of the array
     */
    operator std::string() const { return to_string(); }

    /**
     * @brief Array element access operator
     * 
     * Provides indexed access to array elements. Note that this operator
     * is const but returns a non-const reference, allowing modification
     * of const arrays (similar to std::array behavior).
     * 
     * @param i Index of the element to access
     * @return Reference to the element at index i
     */
    T &operator[](int i) const { return const_cast<T &>(value[i]); }

    /**
     * @brief Assignment operator from pointer data
     * 
     * Assigns data from a pointer to the array. For char arrays, only
     * copies up to the null terminator to avoid buffer overruns.
     * For other types, copies the full array size.
     * 
     * @param data Pointer to source data
     * @return Reference to this array
     */
    Array<T, N> &operator=(const T *data) {
        // Self-assignment check for efficiency
        if (value == data) {
            return *this;
        }
        
        if constexpr (std::is_same_v<T, char>) {
            // For char arrays, copy only up to null terminator
            memcpy(value, data, strlen(data));
        } else {
            // For other types, copy the full array size
            memcpy(value, data, sizeof(value));
        }
        return *this;
    }

    /**
     * @brief Assignment operator from another Array
     * 
     * Assigns the contents of another Array by delegating to the
     * pointer assignment operator.
     * 
     * @param other The source Array to copy from
     * @return Reference to this array
     */
    Array<T, N> &operator=(const Array<T, N> &other) { return operator=(other.value); }
};

//------------------------------------------------------------------------
// JSON serialization support
//------------------------------------------------------------------------

/**
 * @brief JSON serialization for Array types
 * 
 * Converts an Array to JSON format. The entire array is serialized
 * as a JSON array containing all elements.
 * 
 * @tparam T The element type of the array
 * @tparam N The size of the array
 * @param j The JSON object to serialize to
 * @param value The Array to serialize
 */
template <typename T, size_t N> [[maybe_unused]] void to_json(Json::json &j, const Array<T, N> &value) {
    j = value.value;
}

/**
 * @brief JSON deserialization for Array types
 * 
 * Reconstructs an Array from JSON format. Expects a JSON array with
 * exactly N elements, each convertible to type T.
 * 
 * @tparam T The element type of the array
 * @tparam N The size of the array
 * @param j The JSON object to deserialize from
 * @param value The Array to populate
 */
template <typename T, size_t N> [[maybe_unused]] void from_json(const Json::json &j, Array<T, N> &value) {
    for (int i = 0; i < N; i++) {
        value.value[i] = j[i].get<T>();
    }
}

/**
 * @brief Stream output operator for Array types
 * 
 * Allows Array objects to be written to output streams (e.g., std::cout).
 * Uses the to_string() method for human-readable output.
 * 
 * @tparam T The element type of the array
 * @tparam N The size of the array
 * @param os The output stream
 * @param t The Array to output
 * @return Reference to the output stream
 */
template <typename T, size_t N> std::ostream &operator<<(std::ostream &os, Array<T, N> t) {
    return os << t.to_string();
}

} // namespace infra

//------------------------------------------------------------------------
// Type trait specializations for Array
//------------------------------------------------------------------------

/**
 * @brief Specialization of is_array for infra::Array types
 * 
 * Makes infra::Array types compatible with the is_array type trait
 * from ext_type_traits.h, allowing them to be detected as arrays
 * in generic code.
 */
template <typename T, size_t N> struct is_array<infra::Array<T, N>> : public std::true_type {};

/**
 * @brief Specialization of is_array_of for infra::Array types
 * 
 * Allows checking if an infra::Array is an array of a specific type.
 * Useful for SFINAE and compile-time type checking in generic code.
 * 
 * @tparam V The value type of the array
 * @tparam N The size of the array
 * @tparam T The type to check against
 */
template <typename V, size_t N, typename T> struct is_array_of<infra::Array<V, N>, T> {
    static constexpr bool value = std::is_same_v<V, T>;
};