#pragma once

#include <cstdint>
#include <string>

#include "infra/infra.h"
#include "infra/ext_type_traits.h"

namespace btra {

struct Event {
  virtual ~Event() = default;

  [[nodiscard]] virtual int64_t gen_time() const = 0;

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
   * @tparam T
   * @return a casted reference to the underlying memory address
   */
  template <typename T>
  std::enable_if_t<size_fixed_v<T>, const T &> data() const {
    return *(reinterpret_cast<const T *>(data_address()));
  }

  template <typename T>
  std::enable_if_t<not size_fixed_v<T>, const T> data() const {
    return T(data_as_bytes(), data_length());
  }
};
DECLARE_SPTR(Event)

}  // namespace btra