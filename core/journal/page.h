#pragma once

#include <cstddef>

#include "constants.h"
#include "enums.h"
#include "frame.h"
#include "jlocation.h"

namespace btra::journal {

struct PageHeader {
    uint32_t version;
    uint32_t page_header_length;
    uint32_t page_size;
    uint32_t frame_header_length;
    uint64_t last_frame_position;
};

struct Page {
    PageHeader header;
    char body[PAGE_SIZE - sizeof(header)];
};

FORWARD_DECLARE_SPTR(PageUnit)
class PageUnit {
public:
    ~PageUnit();

    [[nodiscard]] uint32_t get_page_size() const { return header_->page_size; }

    [[nodiscard]] JLocationSPtr get_location() const { return location_; }

    [[nodiscard]] uint32_t get_dest_id() const { return dest_id_; }

    [[nodiscard]] uint32_t get_page_id() const { return page_id_; }

    [[nodiscard]] int64_t begin_time() const {
        return reinterpret_cast<FrameHeader *>(first_frame_address())->gen_time;
    }

    [[nodiscard]] int64_t end_time() const { return reinterpret_cast<FrameHeader *>(last_frame_address())->gen_time; }

    [[nodiscard]] uintptr_t address() const { return reinterpret_cast<uintptr_t>(header_); }

    [[nodiscard]] uintptr_t address_border() const { return address() + header_->page_size - sizeof(FrameHeader); }

    [[nodiscard]] uintptr_t first_frame_address() const { return address() + header_->page_header_length; }

    [[nodiscard]] uintptr_t last_frame_address() const { return address() + header_->last_frame_position; }

    [[nodiscard]] bool is_full() const {
        return last_frame_address() + reinterpret_cast<FrameHeader *>(last_frame_address())->length > address_border();
    }

    static PageUnitSPtr load(const JLocationSPtr &location, uint32_t dest_id, uint32_t page_id, bool is_writing,
                             bool lazy);

    static std::string get_page_path(const JLocationSPtr &location, uint32_t dest_id, uint32_t page_id);

    static uint32_t find_page_id(const JLocationSPtr &location, uint32_t dest_id, int64_t time);

private:
    const JLocationSPtr location_;
    const uint32_t dest_id_;
    const uint32_t page_id_;
    const bool lazy_;
    const size_t size_;
    const PageHeader *header_;

    PageUnit(JLocationSPtr location, uint32_t dest_id, uint32_t page_id, size_t size, bool lazy, uintptr_t address);

    /**
     * update page header when new frame added
     */
    void set_last_frame_position(uint64_t position);

    friend class Journal;

    friend class Writer;

    friend class Reader;
};

inline static uint32_t find_page_size(const JLocationSPtr &location, uint32_t dest_id) {
    if (location->category == enums::Module::MD) {
        return 128 * MB;
    }
    if (location->category == enums::Module::TD || location->category == enums::Module::STRATEGY) {
        return 16 * MB;
    }
    return MB;
}

} // namespace btra::journal