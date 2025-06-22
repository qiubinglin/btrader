#pragma once

#include "journal.h"

namespace btra::journal {

class Reader {
public:
    explicit Reader(bool lazy) : lazy_(lazy), current_(nullptr){};

    ~Reader();

    /**
     * @brief Join journal at given unique id
     * @param location
     * @param dest_id
     * @param from_time subscribe events after this time, 0 means from start
     */
    void join(const JLocationSPtr &location, uint32_t dest_id, int64_t from_time);

    void disjoin(uint32_t location_uid);

    void disjoin_channel(uint32_t location_uid, uint32_t dest_id);

    [[nodiscard]] FrameUnitSPtr current_frame() const { return current_->current_frame(); }

    [[nodiscard]] PageUnitSPtr current_page() const { return current_->current_page(); }

    [[maybe_unused]] [[nodiscard]] const std::unordered_map<uint64_t, Journal> &journals() const { return journals_; }

    bool data_available();

    /** seek journal to time */
    void seek_to_time(int64_t time);

    /** seek next frame */
    void next();

    /**
     * @brief Sort to get earliest frame
     * 
     */
    void sort();

private:
    const bool lazy_;
    Journal *current_;
    std::unordered_map<uint64_t, Journal> journals_;
};
DECLARE_UPTR(Reader)

} // namespace btra::journal