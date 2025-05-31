#pragma once

#include <cstring>

#include "common.h"
#include "ext_type_traits.h"

namespace infra {

template <typename T, size_t N> struct Array {
    static constexpr size_t length = N;
    using element_type = T;
    using type = T[N];
    type value;

    Array() {
        if constexpr (std::is_same_v<T, char>) {
            memset(value, '\0', sizeof(value));
        } else {
            memset(value, 0, sizeof(value));
        }
    }

    explicit Array(const T *t) { memcpy(value, t, sizeof(value)); }

    explicit Array(const unsigned char *t) { memcpy(value, t, sizeof(value)); }

    [[nodiscard]] size_t size() const { return N; }

    [[nodiscard]] std::string to_string() const { return array_to_string<T, N>{}(value); }

    operator T *() { return static_cast<T *>(value); }

    operator const T *() const { return static_cast<const T *>(value); }

    operator const void *() const { return static_cast<const void *>(value); }

    operator std::string() const { return to_string(); }

    T &operator[](int i) const { return const_cast<T &>(value[i]); }

    Array<T, N> &operator=(const T *data) {
        if (value == data) {
            return *this;
        }
        if constexpr (std::is_same_v<T, char>) {
            memcpy(value, data, strlen(data));
        } else {
            memcpy(value, data, sizeof(value));
        }
        return *this;
    }

    Array<T, N> &operator=(const Array<T, N> &other) { return operator=(other.value); }
};

template <typename T, size_t N> [[maybe_unused]] void to_json(Json::json &j, const Array<T, N> &value) {
    j = value.value;
}

template <typename T, size_t N> [[maybe_unused]] void from_json(const Json::json &j, Array<T, N> &value) {
    for (int i = 0; i < N; i++) {
        value.value[i] = j[i].get<T>();
    }
}

template <typename T, size_t N> std::ostream &operator<<(std::ostream &os, Array<T, N> t) {
    return os << t.to_string();
}

} // namespace infra

template <typename T, size_t N> struct is_array<infra::Array<T, N>> : public std::true_type {};

template <typename V, size_t N, typename T> struct is_array_of<infra::Array<V, N>, T> {
    static constexpr bool value = std::is_same_v<V, T>;
};