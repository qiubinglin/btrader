#include "journal_comm_data.h"

#include "main_cfg.h"

#include "fds_map.h"

namespace btra {

void JourCommData::init(const Json::json &json) {
    MainCfg main_cfg(json);
    FdsMap::set_fds_file(main_cfg.get_fds_file()); /* Set fds file path first */

    auto begin_time = infra::time::now_time();

    /* Initialize just like CP engine */
    reader = std::make_unique<journal::Reader>(false);
    const auto &md_dests = main_cfg.md_dests();
    for (auto dest : md_dests) {
        reader->join(main_cfg.md_location(), dest, begin_time);
    }
    md_account_count = md_dests.size();

    reader->join(main_cfg.td_reponse_location(), journal::JIDUtil::build(journal::JIDUtil::TD_RESPONSE), begin_time);

    const auto &td_dests = main_cfg.td_dests();
    for (auto dest : td_dests) {
        writers[dest] = std::make_unique<journal::Writer>(main_cfg.td_location(), dest, false);
    }

    auto md_req_dest = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
    writers[md_req_dest] = std::make_unique<journal::Writer>(main_cfg.md_req_location(), md_req_dest, false);

    interrupt_sender.init();
    observe_helper.add_target(interrupt_sender.get_fd());

    /* Intialize ObserveHelper */
    observe_helper.add_customer(reader);
}

} // namespace btra