#pragma once

#include "infra/infra.h"
#include "jid.h"
#include "jlocation.h"

namespace btra {

class MainCfg {
public:
    MainCfg() {}
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

private:
    Json::json cfg_;

    enums::RunMode run_mode_;
    std::string root_;

    std::vector<uint32_t> md_dests_;
    std::vector<std::string> md_institutions_;
    std::vector<uint32_t> td_dests_;
    std::vector<std::string> td_institutions_;
};

} // namespace btra