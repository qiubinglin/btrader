#include "journal.h"

#include "enums.h"

namespace btra::journal {

Journal::~Journal() {
  if (page_.get() != nullptr) {
    page_.reset();
  }
}

void Journal::next() {
  assert(page_.get() != nullptr);
  if (frame_->msg_type() == MsgTag::PageEnd) {
    load_next_page();
  } else {
    frame_->move_to_next();
    page_frame_nb_++;
  }
}

void Journal::seek_to_time(int64_t time) {
  int page_id = PageUnit::find_page_id(location_, dest_id_, time);
  load_page(page_id);
  while (page_->is_full() && page_->end_time() <= time) {
    load_next_page();
  }
  while (frame_->has_data() && frame_->gen_time() <= time) {
    next();
  }
}

void Journal::load_page(int page_id) {
  if (page_.get() == nullptr or page_->get_page_id() != page_id) {
    page_ = PageUnit::load(location_, dest_id_, page_id, is_writing_, lazy_);
  }
  frame_->set_address(page_->first_frame_address());
  page_frame_nb_ = 0u;
}

void Journal::load_next_page() { load_page(page_->get_page_id() + 1); }

}  // namespace btra