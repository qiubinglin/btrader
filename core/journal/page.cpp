#include "page.h"

#include "exceptions.h"
#include "infra/log.h"
#include "version.h"

namespace btra::journal {

PageUnit::PageUnit(JLocationSPtr location, uint32_t dest_id, const uint32_t page_id, const size_t size, const bool lazy,
                   uintptr_t address)
    : location_(std::move(location)),
      dest_id_(dest_id),
      page_id_(page_id),
      lazy_(lazy),
      size_(size),
      header_(reinterpret_cast<PageHeader *>(address)) {
    assert(address > 0);
}

PageUnit::~PageUnit() {
    if (not infra::release_mmap_buffer(address(), size_, lazy_)) {
        INFRA_LOG_ERROR("can not release page {}/{:08x}.{}.journal", location_->uname, dest_id_, page_id_);
    }
}

void PageUnit::set_last_frame_position(uint64_t position) {
    const_cast<PageHeader *>(header_)->last_frame_position = position;
}

PageUnitSPtr PageUnit::load(const JLocationSPtr &location, uint32_t dest_id, uint32_t page_id, bool is_writing,
                            bool lazy) {
    uint32_t page_size = find_page_size(location, dest_id);
    std::string path = get_page_path(location, dest_id, page_id);
    uintptr_t address = infra::load_mmap_buffer(path, page_size, is_writing, lazy);

    INFRA_LOG_DEBUG("Loading page from {}", path);
    INFRA_LOG_DEBUG("page_size 0x{0:x}, address 0x{0:x}", page_size, address);

    if (address == 0) {
        throw JournalError("unable to load page for " + path);
    }

    PageHeader *header = reinterpret_cast<PageHeader *>(address);
    if (header->last_frame_position == 0) {
        header->version = __JOURNAL_VERSION__;
        header->page_header_length = sizeof(PageHeader);
        header->page_size = page_size;
        header->frame_header_length = sizeof(FrameHeader);
        header->last_frame_position = header->page_header_length;
    }

    if (header->version != __JOURNAL_VERSION__) {
        uint32_t v = header->version;
        throw JournalError(fmt::format("{} version mismatch, required {}, found {}", path, __JOURNAL_VERSION__, v));
    }
    if (header->page_header_length != sizeof(PageHeader)) {
        uint32_t l = header->page_header_length;
        throw JournalError(
            fmt::format("{} header length mismatch, required {}, found {}", path, sizeof(PageHeader), l));
    }
    if (header->page_size != page_size) {
        uint32_t s = header->page_size;
        throw JournalError(
            fmt::format("page size mismatch, required {}, found {}, location {}, path {}, dest_id {}, page_id {}",
                        page_size, s, location->uname, path, dest_id, page_id));
    }

    return std::shared_ptr<PageUnit>(new PageUnit(location, dest_id, page_id, page_size, lazy, address));
}

std::string PageUnit::get_page_path(const JLocationSPtr &location, uint32_t dest_id, uint32_t page_id) {
    auto page_name = fmt::format("{:08x}.{}", dest_id, page_id);
    return location->locator->layout_file(location, enums::layout::JOURNAL, page_name);
}

uint32_t PageUnit::find_page_id(const JLocationSPtr &location, uint32_t dest_id, int64_t time) {
    std::vector<uint32_t> page_ids = location->locator->list_page_id(location, dest_id);
    if (page_ids.empty()) {
        return 1;
    }
    if (time == 0) {
        return page_ids.front();
    }
    for (int i = static_cast<int>(page_ids.size()) - 1; i >= 0; i--) {
        if (PageUnit::load(location, dest_id, page_ids[i], false, true)->begin_time() < time) {
            return page_ids[i];
        }
    }
    return page_ids.front();
}

} // namespace btra::journal