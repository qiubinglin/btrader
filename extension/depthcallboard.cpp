#include "depthcallboard.h"

#include "infra/mmap.h"

namespace btra::extension {

void DepthCallBoard::init(const std::string &dir, size_t size, size_t elm_size, bool is_writing) {
    is_writing_ = is_writing;
    header_ = reinterpret_cast<Header *>(infra::load_mmap_buffer(dir + "/depthcallboard", size, is_writing, false));
    header_->length = size;
    header_->header_length = sizeof(Header);
    header_->used_length = header_->header_length;
    header_->elm_size = elm_size;
}

} // namespace btra::extension