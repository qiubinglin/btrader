#pragma once

#include "core/types.h"
#include "infra/array.h"

namespace btra::extension {

class DepthCallBoard {
public:
    void set_element_size();

    template <typename T>
    const T &get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                 const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;

    template <typename T>
    void set(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
             const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, const T &data) const;

private:
    size_t element_size_{0};
};

} // namespace btra::extension