#pragma once

#include "core/book.h"
#include "core/journal/jid.h"
#include "core/journal/jlocation.h"
#include "infra/infra.h"

namespace btra {

class MainCfg {
public:
    MainCfg() {}
    MainCfg(const std::string &filepath);
    MainCfg(const Json::json &cfg);
    journal::JLocationSPtr md_location() const;
    const std::vector<uint32_t> &md_dests() const;
    const std::vector<std::string> &md_institutions() const { return md_institutions_; }
    uint32_t get_md_location_uid() const;
    journal::JLocationSPtr td_location() const;
    const std::vector<uint32_t> &td_dests() const;
    const std::vector<std::string> &td_institutions() const { return td_institutions_; }
    uint32_t get_td_location_uid() const;

    journal::JLocationSPtr md_req_location() const;
    journal::JLocationSPtr td_reponse_location() const;

    enums::RunMode run_mode() const { return run_mode_; }

    std::vector<std::string> get_journal_names() const;

    const Book &get_initail_book() const;

    const std::string &root_path() const { return root_; }

    const std::string &get_fds_file() const { return fds_file_; }

    uint32_t get_page_rollback_size() const { return page_rollback_size_; }

    infra::TimeUnit get_time_unit() const { return time_unit_; }

private:
    Json::json cfg_;

    enums::RunMode run_mode_;
    std::string root_;

    std::vector<uint32_t> md_dests_;
    std::vector<std::string> md_institutions_;
    std::vector<uint32_t> td_dests_;
    std::vector<std::string> td_institutions_;

    Book initial_book_;

    std::string fds_file_;

    uint32_t page_rollback_size_{0};

    infra::TimeUnit time_unit_{infra::TimeUnit::MILLI};
};

} // namespace btra