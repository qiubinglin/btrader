#pragma once

#include "jid.h"
#include "jlocation.h"
#include "page.h"

namespace btra::journal {

/**
 * Journal class, the abstraction of continuous memory access
 */
class Journal {
 public:
  Journal(JLocationSPtr location, uint32_t dest_id, bool is_writing, bool lazy)
      : location_(std::move(location)),
        dest_id_(dest_id),
        is_writing_(is_writing),
        lazy_(lazy),
        frame_(std::shared_ptr<FrameUnit>(new FrameUnit())),
        page_frame_nb_(0u) {}

  ~Journal();

  [[nodiscard]] FrameUnitSPtr &current_frame() { return frame_; }

  [[nodiscard]] PageUnitSPtr &current_page() { return page_; }

  [[nodiscard]] const JLocationSPtr &get_location() const { return location_; }

  [[maybe_unused]] [[nodiscard]] uint32_t get_source() const { return location_->location_uid; }

  [[maybe_unused]] [[nodiscard]] uint32_t get_dest() const { return dest_id_; }

  /**
   * move current frame to the next available one
   */
  void next();

  /**
   * makes sure after this call, next time user calls current_frame() gets the right available frame
   * (gen_time() > nanotime or writable)
   * @param nanotime
   */
  void seek_to_time(int64_t nanotime);

 private:
  const JLocationSPtr location_;
  const uint32_t dest_id_;
  const bool is_writing_;
  const bool lazy_;
  PageUnitSPtr page_;
  FrameUnitSPtr frame_;
  uint64_t page_frame_nb_;

  void load_page(int page_id);

  /** load next page, current page will be released if not empty */
  void load_next_page();

  friend class Reader;

  friend class Writer;
};

}  // namespace btra::journal