#include "main_cfg.h"

#include <filesystem>

#include "extension/globalparams.h"
#include "infra/singleton.h"
#include "jid.h"

namespace btra {

using namespace btra::journal;

MainCfg::MainCfg(const std::string &filepath) {
    std::ifstream ifs(filepath);
    Json::json cfg = Json::json::parse(ifs);
    *this = MainCfg(cfg);
}

MainCfg::MainCfg(const Json::json &cfg) : cfg_(cfg) {
    run_mode_ = enums::get_mode_by_name(cfg_["system"]["mode"].get<std::string>());
    root_ = cfg_["system"]["output_root_path"].get<std::string>();
    root_ = std::filesystem::absolute(root_);

    for (auto &elm : cfg_["md"]) {
        std::string institution = elm["institution"];
        std::string account = elm["account"];
        md_dests_.push_back(JIDUtil::build(institution, account));
        md_institutions_.push_back(institution);
    }

    for (auto &elm : cfg_["td"]) {
        std::string institution = elm["institution"];
        std::string account = elm["account"];
        td_dests_.push_back(JIDUtil::build(institution, account));
        td_institutions_.push_back(institution);
    }

    if (cfg_["system"].contains("initial_book")) {
        initial_book_.asset.avail = cfg_["system"]["initial_book"]["asset"].get<double>();
    }

    if (run_mode_ == enums::RunMode::USER_APP) {
        fds_file_ = std::filesystem::absolute(cfg_["user-app"]["fds_file"].get<std::string>());
    }

    page_rollback_size_ = cfg_["system"]["page_rollback_size"].get<uint32_t>();

    if (cfg_["system"].contains("time_unit")) {
        std::string unitstr = cfg_["system"]["time_unit"].get<std::string>();
        if (unitstr == "sec") {
            time_unit_ = infra::TimeUnit::SEC;
        } else if (unitstr == "nano") {
            time_unit_ = infra::TimeUnit::NANO;
        } else {
            time_unit_ = infra::TimeUnit::MILLI;
        }
    }

    /* Setup global parameters */
    Singleton<GlobalParams>::instance().root_dir = root_;
    if (cfg_["system"].contains("statistics")) {
        auto stat_mode = cfg_["system"]["statistics"]["mode"].get<std::string>();
        INSTANCE(GlobalParams).stat_params.set_mode(stat_mode);
    }

    if (cfg_["system"].contains("simulation")) {
        INSTANCE(GlobalParams).is_simulation = cfg_["system"]["simulation"].get<bool>();
    }

    if (cfg_["system"].contains("backtest")) {
        INSTANCE(GlobalParams).is_backtest = cfg_["system"]["backtest"].get<bool>();
    }
}

JLocationSPtr MainCfg::md_location() const {
    JLocatorSPtr locator = std::make_shared<JLocator>(root_, run_mode_);
    JLocationSPtr res = std::make_shared<JLocation>(run_mode_, enums::Module::MD, "", "", locator);
    return res;
}

const std::vector<uint32_t> &MainCfg::md_dests() const { return md_dests_; }

uint32_t MainCfg::get_md_location_uid() const { return md_location()->location_uid; }

JLocationSPtr MainCfg::td_location() const {
    JLocatorSPtr locator = std::make_shared<JLocator>(root_, run_mode_);
    JLocationSPtr res = std::make_shared<JLocation>(run_mode_, enums::Module::TD, "", "", locator);
    return res;
}

const std::vector<uint32_t> &MainCfg::td_dests() const { return td_dests_; }

uint32_t MainCfg::get_td_location_uid() const { return td_location()->location_uid; }

JLocationSPtr MainCfg::md_req_location() const {
    JLocatorSPtr locator = std::make_shared<JLocator>(root_, run_mode_);
    JLocationSPtr res = std::make_shared<JLocation>(run_mode_, enums::Module::STRATEGY, "", "", locator);
    return res;
}

JLocationSPtr MainCfg::td_reponse_location() const {
    JLocatorSPtr locator = std::make_shared<JLocator>(root_, run_mode_);
    JLocationSPtr res = std::make_shared<JLocation>(run_mode_, enums::Module::STRATEGY, "", "", locator);
    return res;
}

std::vector<std::string> MainCfg::get_journal_names() const {
    std::vector<std::string> res;
    /* md */
    {
        auto md_l = md_location();
        for (const auto dest : md_dests_) {
            std::string key = std::to_string(md_l->uid) + "_" + std::to_string(dest);
            res.push_back(key);
        }

        auto md_req_l = md_req_location();
        {
            std::string key =
                std::to_string(md_req_l->uid) + "_" + std::to_string(journal::JIDUtil::build(journal::JIDUtil::MD_REQ));
            res.push_back(key);
        }
        {
            std::string key = std::to_string(md_req_l->uid) + "_" +
                              std::to_string(journal::JIDUtil::build(journal::JIDUtil::MD_RESPONSE));
            res.push_back(key);
        }
    }

    /* td */
    {
        auto td_l = td_location();
        for (const auto dest : td_dests_) {
            std::string key = std::to_string(td_l->uid) + "_" + std::to_string(dest);
            res.push_back(key);
        }

        auto td_resp_l = td_reponse_location();
        {
            std::string key = std::to_string(td_resp_l->uid) + "_" +
                              std::to_string(journal::JIDUtil::build(journal::JIDUtil::TD_RESPONSE));
            res.push_back(key);
        }
        {
            std::string key = std::to_string(td_resp_l->uid) + "_" +
                              std::to_string(journal::JIDUtil::build(journal::JIDUtil::TD_REQ));
            res.push_back(key);
        }
    }
    return res;
}

const Book &MainCfg::get_initail_book() const { return initial_book_; }

} // namespace btra