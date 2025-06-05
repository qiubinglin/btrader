#include "core/types.h"
/* Main header above */

namespace btra {

PositionBook::PositionBook(const char *data, uint32_t len) {
    if (len < sizeof(uint32_t) * 2) {
        throw std::runtime_error("PositionBook data too short");
    }
    const char *ptr = data;
    uint32_t long_size = *reinterpret_cast<const uint32_t *>(ptr);
    ptr += sizeof(uint32_t);
    uint32_t short_size = *reinterpret_cast<const uint32_t *>(ptr);
    ptr += sizeof(uint32_t);

    for (uint32_t i = 0; i < long_size; ++i) {
        uint32_t key = *reinterpret_cast<const uint32_t *>(ptr);
        ptr += sizeof(uint32_t);
        Position position = *reinterpret_cast<const Position *>(ptr);
        ptr += sizeof(Position);
        long_positions[key] = position;
    }

    for (uint32_t i = 0; i < short_size; ++i) {
        uint32_t key = *reinterpret_cast<const uint32_t *>(ptr);
        ptr += sizeof(uint32_t);
        Position position = *reinterpret_cast<const Position *>(ptr);
        ptr += sizeof(Position);
        short_positions[key] = position;
    }
}

std::string PositionBook::to_string() const {
    std::string buf;
    size_t long_size = long_positions.size() * (sizeof(uint32_t) + sizeof(Position));
    size_t short_size = short_positions.size() * (sizeof(uint32_t) + sizeof(Position));
    size_t total_size = sizeof(uint32_t) * 2 + long_size + short_size;
    buf.resize(total_size);
    char *ptr = buf.data();
    uint32_t *size_ptr = reinterpret_cast<uint32_t *>(ptr);
    *size_ptr = static_cast<uint32_t>(long_positions.size());
    ptr += sizeof(uint32_t);
    size_ptr = reinterpret_cast<uint32_t *>(ptr);
    *size_ptr = static_cast<uint32_t>(short_positions.size());
    ptr += sizeof(uint32_t);
    for (const auto &[key, val] : long_positions) {
        *reinterpret_cast<uint32_t *>(ptr) = key;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<Position *>(ptr) = val;
        ptr += sizeof(Position);
    }
    for (const auto &[key, val] : short_positions) {
        *reinterpret_cast<uint32_t *>(ptr) = key;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<Position *>(ptr) = val;
        ptr += sizeof(Position);
    }
    return buf;
}

} // namespace btra