#pragma once

#include <cstdint>

#include "event.h"

namespace btra::journal {

struct FrameHeader {
    /** total frame length (including header and data body) */
    volatile uint32_t length;
    /** header length */
    uint32_t header_length;
    /** generate time of the frame data */
    int64_t gen_time;
    /** trigger time for this frame, use for latency stats */
    int64_t trigger_time;
    /** msg type of the data in frame */
    volatile int32_t msg_type;
    /** source of this frame */
    uint32_t source;
    /** dest of this frame */
    uint32_t dest;
};

/**
 * Basic memory unit,
 * holds header / data / errorMsg (if needs)
 */
struct FrameUnit : Event {
    ~FrameUnit() override = default;

    [[nodiscard]] bool has_data() const { return header_->length > 0 && header_->msg_type > 0; }

    [[nodiscard]] uintptr_t address() const { return reinterpret_cast<uintptr_t>(header_); }

    [[nodiscard]] uint32_t frame_length() const { return header_->length; }

    [[nodiscard]] uint32_t header_length() const { return header_->header_length; }

    [[nodiscard]] uint32_t data_length() const override { return frame_length() - header_length(); }

    [[nodiscard]] int64_t gen_time() const override { return header_->gen_time; }

    [[nodiscard]] int64_t trigger_time() const override { return header_->trigger_time; }

    [[nodiscard]] int32_t msg_type() const override { return header_->msg_type; }

    [[nodiscard]] uint32_t source() const override { return header_->source; }

    [[nodiscard]] uint32_t dest() const override { return header_->dest; }

    [[nodiscard]] const void *data_address() const override {
        return reinterpret_cast<void *>(address() + header_length());
    }

    [[nodiscard]] const char *data_as_bytes() const override {
        return reinterpret_cast<char *>(address() + header_length());
    }

    [[nodiscard]] std::string data_as_string() const override { return std::string(data_as_bytes()); }

    [[nodiscard]] std::string to_string() const override { return std::string(reinterpret_cast<char *>(address())); }

    template <typename T> size_t copy_data(const T &data) {
        size_t length = sizeof(T);
        memcpy(const_cast<void *>(data_address()), &data, length);
        return length;
    }

private:
    /** address with type,
     * will keep moving forward until change page */
    FrameHeader *header_ = nullptr;

    FrameUnit() = default;

    void set_address(uintptr_t address) { header_ = reinterpret_cast<FrameHeader *>(address); }

    void move_to_next() { set_address(address() + frame_length()); }

    void set_header_length() { header_->header_length = sizeof(FrameHeader); }

    void set_data_length(uint32_t length) { header_->length = header_length() + length; }

    void set_gen_time(int64_t gen_time) { header_->gen_time = gen_time; }

    void set_trigger_time(int64_t trigger_time) { header_->trigger_time = trigger_time; }

    void set_msg_type(int32_t msg_type) { header_->msg_type = msg_type; }

    void set_source(uint32_t source) { header_->source = source; }

    void set_dest(uint32_t dest) { header_->dest = dest; }

    void copy(FrameUnit &source) { memcpy(header_, source.header_, source.frame_length()); }

    friend class Journal;

    friend class Writer;
};

DECLARE_SPTR(FrameUnit)

} // namespace btra::journal