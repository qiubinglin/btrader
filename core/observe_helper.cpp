#include "observe_helper.h"

#include <fstream>

#include "core/fds_map.h"
#include "infra/json.h"

namespace btra {

ObserveHelper::ObserveHelper() {
#ifndef HP
    jour_observer_.init();
#endif
}

void ObserveHelper::add_customer(journal::ReaderUPtr &reader) {
    if (reader_ != nullptr) {
        return;
    }
    reader_ = reader.get();
#ifndef HP
    const auto &fds_map = FdsMap::get_fds_map();
    const auto &journals = reader->journals();
    std::string key;
    for (const auto &[_, jour] : journals) {
        key = std::to_string(jour.get_location()->uid) + "_" + std::to_string(jour.get_dest());
        if (fds_map.count(key)) {
            INFRA_LOG_DEBUG("add_customer {}, {}", key, fds_map.at(key));
            jour_observer_.add_target(fds_map.at(key));
        }
    }
#endif
}

bool ObserveHelper::data_available() {
#ifndef HP
    bool retval = false;
    if (jour_observer_.wait() > 0) {
        reader_->sort();
        retval = true;
    }
    return retval;
#else
    return reader_->data_available();
#endif
}

void ObserveHelper::add_target(int efd) {
#ifndef HP
    jour_observer_.add_target(efd);
#else
    (void)efd;
#endif
}

} // namespace btra