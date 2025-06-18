#pragma once

#include <cstdint>
#include <string>

#include "infra/ext_type_traits.h"
#include "infra/infra.h"

namespace btra {

struct Event {
    virtual ~Event() = default;

    /**
     * @brief Get the generation time of the event. Generation time is the time when the event was created completely.
     *
     * @return The generation time.
     */
    [[nodiscard]] virtual int64_t gen_time() const = 0;

    /**
     * @brief Get the trigger time of the event. Trigger time is the time when the event is intended to create.
     *
     * @return The trigger time.
     */
    [[nodiscard]] virtual int64_t trigger_time() const = 0;

    [[nodiscard]] virtual int32_t msg_type() const = 0;

    [[nodiscard]] virtual uint32_t source() const = 0;

    [[nodiscard]] virtual uint32_t dest() const = 0;

    [[nodiscard]] virtual uint32_t data_length() const = 0;

    [[nodiscard]] virtual const void *data_address() const = 0;

    [[nodiscard]] virtual const char *data_as_bytes() const = 0;

    [[nodiscard]] virtual std::string data_as_string() const = 0;

    [[nodiscard]] virtual std::string to_string() const = 0;

    /**
     * Using auto with the return mess up the reference with the undlerying memory address, DO NOT USE it.
     * @brief Get the data of the event as a specific type. This is for fixed size data.
     * @tparam T
     * @return a casted reference to the underlying memory address
     */
    template <typename T> std::enable_if_t<size_fixed_v<T>, const T &> data() const {
        return *(reinterpret_cast<const T *>(data_address()));
    }

    /**
     * @brief Get the data of the event as a specific type. This is for variable size data.
     * @tparam T
     * @return a new created object of type T.
     */
    template <typename T> std::enable_if_t<not size_fixed_v<T>, T> data() const {
        return T(data_as_bytes(), data_length());
    }
};
DECLARE_SPTR(Event)

} // namespace btra