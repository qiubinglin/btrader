#include "writer.h"

#include <cstring>

#include "exceptions.h"
#include "core/fds_map.h"

namespace btra::journal {

constexpr uint32_t PAGE_ID_TRANC = 0xFFFF0000;
constexpr uint32_t FRAME_ID_TRANC = 0x0000FFFF;

Writer::Writer(const JLocationSPtr &location, uint32_t dest_id, bool lazy)
    : frame_id_base_(uint64_t(location->uid xor dest_id) << 32u),
      journal_(location, dest_id, true, lazy),
      size_to_write_(0),
      writer_start_time_32int_(infra::time::nano_hashed(infra::time::now_time())) {
  journal_.seek_to_time(infra::time::now_time());

  const auto &fds_map = FdsMap::get_fds_map();
  std::string key = std::to_string(location->uid) + "_" + std::to_string(dest_id);
  if (fds_map.count(key)) {
    jour_ind_.set_fd(fds_map.at(key));
  }
}

uint64_t Writer::current_frame_uid() {
  uint32_t page_part = (journal_.page_->page_id_ << 16u) & PAGE_ID_TRANC;
  uint32_t frame_part = journal_.page_frame_nb_ & FRAME_ID_TRANC;
  // frame_id_base is used for get account id while canceling order
  return frame_id_base_ | ((page_part | frame_part) xor writer_start_time_32int_);
}

FrameUnitSPtr Writer::open_frame(int64_t trigger_time, int32_t msg_type, uint32_t data_length) {
  assert(sizeof(FrameHeader) + data_length + sizeof(FrameHeader) <= journal_.page_->get_page_size());
  int64_t start_time = infra::time::now_in_nano();
  while (not writer_mtx_.try_lock()) {
    if (infra::time::now_in_nano() - start_time > 30 * infra::time_unit::NANOSECONDS_PER_SECOND) {
      throw JournalError("Can not lock writer for " + journal_.location_->uname);
    }
  }
  if (journal_.current_frame()->address() + sizeof(FrameHeader) + data_length >= journal_.page_->address_border()) {
    close_page(trigger_time);
  }
  auto frame = journal_.current_frame();
  frame->set_header_length();
  frame->set_trigger_time(trigger_time);
  frame->set_msg_type(msg_type);
  frame->set_source(journal_.location_->uid);
  frame->set_dest(journal_.dest_id_);
  size_to_write_ = data_length;
  return frame;
}

void Writer::close_frame(size_t data_length, int64_t gen_time) {
  assert(size_to_write_ >= data_length);
  auto frame = journal_.current_frame();
  auto next_frame_address = frame->address() + frame->header_length() + data_length;
  assert(next_frame_address < journal_.page_->address_border());
  memset(reinterpret_cast<void *>(next_frame_address), 0, sizeof(FrameHeader));
  frame->set_gen_time(gen_time);
  frame->set_data_length(data_length);
  size_to_write_ = 0;
  journal_.page_->set_last_frame_position(frame->address() - journal_.page_->address());
  journal_.next();
  writer_mtx_.unlock();
  jour_ind_.post();
}

void Writer::copy_frame(const FrameUnitSPtr &source) {
  assert(source->frame_length() + sizeof(FrameHeader) <= journal_.page_->get_page_size());
  if (journal_.current_frame()->address() + source->frame_length() >= journal_.page_->address_border()) {
    close_page(infra::time::now_in_nano());
  }

  auto frame = journal_.current_frame();
  frame->copy(*source);

  auto next_frame_address = frame->address() + frame->header_length() + frame->data_length();
  memset(reinterpret_cast<void *>(next_frame_address), 0, sizeof(FrameHeader));
  journal_.page_->set_last_frame_position(frame->address() - journal_.page_->address());
  journal_.next();
}

void Writer::mark(int64_t trigger_time, int32_t msg_type) {
  open_frame(trigger_time, msg_type, 0);
  close_frame(0);
}

[[maybe_unused]] void Writer::mark_at(int64_t gen_time, int64_t trigger_time, int32_t msg_type) {
  open_frame(trigger_time, msg_type, 0);
  close_frame(0, gen_time);
}

[[maybe_unused]] void Writer::write_raw(int64_t trigger_time, int32_t msg_type, uintptr_t data, uint32_t length) {
  auto frame = open_frame(trigger_time, msg_type, length);
  memcpy(const_cast<void *>(frame->data_address()), reinterpret_cast<void *>(data), length);
  close_frame(length);
}

[[maybe_unused]] void Writer::write_bytes(int64_t trigger_time, int32_t msg_type, const std::vector<uint8_t> &data,
                                          uint32_t length) {
  auto frame = open_frame(trigger_time, msg_type, length);
  memcpy(const_cast<void *>(frame->data_address()), data.data(), length);
  close_frame(length);
}

void Writer::close_data() { close_frame(size_to_write_); }

void Writer::close_page(int64_t trigger_time) {
  PageUnitSPtr last_page = journal_.page_;
  journal_.load_next_page();

  FrameUnit last_page_frame;
  last_page_frame.set_address(last_page->last_frame_address());
  last_page_frame.move_to_next();
  last_page_frame.set_header_length();
  last_page_frame.set_trigger_time(trigger_time);
  last_page_frame.set_msg_type(MsgTag::PageEnd);
  last_page_frame.set_source(journal_.location_->uid);
  last_page_frame.set_dest(journal_.dest_id_);
  last_page_frame.set_gen_time(infra::time::now_in_nano());
  last_page_frame.set_data_length(0);
  last_page->set_last_frame_position(last_page_frame.address() - last_page->address());
}

}  // namespace btra::journal