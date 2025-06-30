#include "md_engine.h"

namespace btra {

void MDEngine::react() {
    events_.filter(is<MsgTag::Termination>).subscribe([this](const EventSPtr &event) { this->stop(); });

    events_.filter(is<MsgTag::TradingStart>).subscribe(ON_MEM_FUNC(on_trading_start));
    events_.filter(is<MsgTag::MDSubscribe>).subscribe(ON_MEM_FUNC(on_subscribe_data));
}

void MDEngine::on_active() {}

void MDEngine::on_setup() {
    main_cfg_ = MainCfg(cfg_);

    reader_ = std::make_unique<journal::Reader>(false);
    reader_->join(main_cfg_.md_req_location(), journal::JIDUtil::build(journal::JIDUtil::MD_REQ), begin_time_);

    const auto &md_dests = main_cfg_.md_dests();
    const auto &md_institutions = main_cfg_.md_institutions();
    for (size_t i = 0; i < md_dests.size(); ++i) {
        auto dest = md_dests[i];
        const auto &institution = md_institutions[i];
        writers_[dest] = std::make_unique<journal::Writer>(main_cfg_.md_location(), dest, false);
        data_services_[dest] = broker::DataService::create(institution, main_cfg_.get_backtest_data_type());
        data_services_[dest]->setup(cfg_["md"][i]);
        /* tmp, to be removed in future. */
        if (main_cfg_.get_backtest_data_type() != enums::BacktestDataType::None) {
            data_services_[dest]->setup(cfg_["system"]);
        }
        data_services_[dest]->set_customer(writers_[dest].get());
    }
}

void MDEngine::on_trading_start(const EventSPtr &event) {
    const auto trading_start = event->data<TradingStart>();
    begin_time_ = trading_start.sync_time;
    INFRA_LOG_INFO("md Trading started at: {}", begin_time_);

    for (auto &[key, service] : data_services_) {
        service->start();
    }
}

void MDEngine::on_subscribe_data(const EventSPtr &event) {
    auto md_subscribe = event->data<MDSubscribe>();
    data_services_[md_subscribe.id]->subscribe(md_subscribe.instrument_keys);
}

} // namespace btra