#pragma once

#include <cstdint>
#include <unordered_map>

#include "core/hashid.h"
#include "core/types.h"
#include "infra/array.h"

namespace btra::extension {

class DepthCallBoard {
public:
    void init(const std::string &dir, size_t size, size_t elm_size, bool is_writing);

    template <size_t N>
    bool get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
             const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, InstrumentDepth<N> &output) {
        using DataType = InstrumentDepth<N>;
        auto id = hash_instrument(exchange_id, instrument_id);
        char *ptr = nullptr;
        if (data_positions_.contains(id)) {
            ptr = data_positions_.at(id);
        } else {
            ptr = (char *)header_ + header_->header_length;
            auto end = (char *)header_ + header_->used_length;
            while (ptr + sizeof(DataType) <= end) {
                const DataType *data = reinterpret_cast<const DataType *>(ptr);
                auto id_cmp = hash_instrument(data->exchange_id, data->instrument_id);
                if (id == id_cmp) {
                    data_positions_[id] = ptr;
                    break;
                }
                ptr += sizeof(DataType);
            }
            if (ptr + sizeof(DataType) > end) {
                return false;
            }
        }
        output = *reinterpret_cast<DataType *>(ptr);
        return true;
    }

    template <size_t N>
    void set(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
             const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, const InstrumentDepth<N> &input) {
        if (not is_writing_) {
            return;
        }
        using DataType = InstrumentDepth<N>;
        auto id = hash_instrument(exchange_id, instrument_id);
        char *ptr = nullptr;
        if (data_positions_.contains(id)) {
            ptr = data_positions_.at(id);
        } else {
            ptr = (char *)header_ + header_->used_length;
            header_->used_length += sizeof(DataType);
            data_positions_[id] = ptr;
        }
        *reinterpret_cast<DataType *>(ptr) = input;
    }

private:
    bool is_writing_{false};
    std::unordered_map<uint32_t, char *> data_positions_;
    struct Header {
        /** total frame length (including header and data body) */
        volatile uint32_t length;
        /** header length */
        uint32_t header_length;
        uint32_t used_length;
        uint32_t elm_size;
    } *header_;
};

} // namespace btra::extension