#pragma once

#include <mutex>

#include "jlocation.h"
#include "journal.h"

namespace btra::journal {

class Writer {
public:
    Writer(const JLocationSPtr &location, uint32_t dest_id, bool lazy);

    [[nodiscard]] const JLocationSPtr &get_location() const { return journal_.location_; }

    [[maybe_unused]] [[nodiscard]] uint32_t get_dest() const { return journal_.dest_id_; }

    [[nodiscard]] const Journal &get_journal() const { return journal_; }

    [[nodiscard]] const PageUnitSPtr get_current_page() const { return journal_.page_; }

    uint64_t current_frame_uid();

    /**
     * @brief Opens a new frame for writing.
     * 
     * @param trigger_time 
     * @param msg_type 
     * @param length 
     * @return FrameUnitSPtr 
     */
    FrameUnitSPtr open_frame(int64_t trigger_time, int32_t msg_type, uint32_t length);

    void close_frame(size_t data_length, int64_t gen_time = infra::time::now_time());

    void copy_frame(const FrameUnitSPtr &source);

    void mark(int64_t trigger_time, int32_t msg_type);

    [[maybe_unused]] void mark_at(int64_t gen_time, int64_t trigger_time, int32_t msg_type);

    [[maybe_unused]] void write_raw(int64_t trigger_time, int32_t msg_type, uintptr_t data, uint32_t length);

    [[maybe_unused]] void write_bytes(int64_t trigger_time, int32_t msg_type, const std::vector<uint8_t> &data,
                                      uint32_t length);

    /**
     * Using auto with the return mess up the reference with the undlerying memory address, DO NOT USE it.
     * @tparam T
     * @param trigger_time
     * @param msg_type
     * @return a casted reference to the underlying memory address in mmap file
     */
    template <typename T> std::enable_if_t<size_fixed_v<T>, T &> open_data(int64_t trigger_time = 0) {
        auto frame = open_frame(trigger_time, T::tag, sizeof(T));
        return const_cast<T &>(frame->template data<T>());
    }

    void close_data();

    template <typename T>
    std::enable_if_t<size_fixed_v<T>> write(int64_t trigger_time, const T &data, int32_t msg_type = T::tag) {
        auto frame = open_frame(trigger_time, msg_type, sizeof(T));
        auto size = frame->copy_data(data);
        close_frame(size);
    }

    template <typename T>
    std::enable_if_t<size_unfixed_v<T>> write(int64_t trigger_time, const T &data, int32_t msg_type = T::tag) {
        auto s = data.to_string();
        auto size = s.length();
        auto frame = open_frame(trigger_time, msg_type, size);
        memcpy(const_cast<void *>(frame->data_address()), s.c_str(), size);
        close_frame(size);
    }

    template <typename T>
    std::enable_if_t<size_fixed_v<T>> write_as(int64_t trigger_time, const T &data, uint32_t source, uint32_t dest) {
        auto frame = open_frame(trigger_time, T::tag, sizeof(T));
        auto size = frame->copy_data(data);
        frame->set_source(source);
        frame->set_dest(dest);
        close_frame(size);
    }

    template <typename T>
    std::enable_if_t<size_unfixed_v<T>> write_as(int64_t trigger_time, const T &data, uint32_t source, uint32_t dest) {
        auto s = data.to_string();
        auto size = s.length();
        auto frame = open_frame(trigger_time, T::tag, size);
        memcpy(const_cast<void *>(frame->data_address()), s.c_str(), size);
        frame->set_source(source);
        frame->set_dest(dest);
        close_frame(size);
    }

    template <typename T>
    [[maybe_unused]] std::enable_if_t<size_fixed_v<T>> write_at(int64_t gen_time, int64_t trigger_time, const T &data) {
        auto frame = open_frame(trigger_time, T::tag, sizeof(T));
        auto size = frame->copy_data(data);
        close_frame(size, gen_time);
    }

    template <typename T>
    [[maybe_unused]] std::enable_if_t<size_unfixed_v<T>> write_at(int64_t gen_time, int64_t trigger_time,
                                                                  const T &data) {
        auto s = data.to_string();
        auto size = s.length();
        auto frame = open_frame(trigger_time, T::tag, size);
        memcpy(const_cast<void *>(frame->data_address()), s.c_str(), size);
        close_frame(size, gen_time);
    }

private:
    const uint64_t frame_id_base_;
    Journal journal_;
    size_t size_to_write_;
    uint32_t writer_start_time_32int_;
    std::mutex writer_mtx_ = {};

    JourIndicator jour_ind_;

    void close_page(int64_t trigger_time);
};
DECLARE_UPTR(Writer)

}; // namespace btra::journal