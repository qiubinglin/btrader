#include "td_engine.h"

namespace btra {

void TDEngine::react() {
    events_.filter(is<MsgTag::Termination>).subscribe([this](const EventSPtr &event) { this->stop(); });

    events_.filter(is<MsgTag::TradingStart>).subscribe(ON_MEM_FUNC(on_trading_start));
    events_.filter(is<MsgTag::OrderInput>).subscribe(ON_MEM_FUNC(insert_order));
    events_.filter(is<MsgTag::OrderAction>).subscribe(ON_MEM_FUNC(cancel_order));
    events_.filter(is<MsgTag::AccountReq>).subscribe(ON_MEM_FUNC(on_account_req));
}

void TDEngine::on_setup() {
    main_cfg_ = MainCfg(cfg_);

    reader_ = std::make_unique<journal::Reader>(false);

    const auto &td_dests = main_cfg_.td_dests();
    const auto &td_institutions = main_cfg_.td_institutions();
    for (size_t i = 0; i < td_dests.size(); ++i) {
        auto dest = td_dests[i];
        const auto &institution = td_institutions[i];
        reader_->join(main_cfg_.td_location(), dest, begin_time_);

        trade_services_[dest] = broker::TradeService::create(institution, main_cfg_.get_backtest_data_type());
        trade_services_[dest]->setup(cfg_["td"][i]);
    }

    auto response_td_id = journal::JIDUtil::build(journal::JIDUtil::TD_RESPONSE);
    writers_[response_td_id] =
        std::make_unique<journal::Writer>(main_cfg_.td_reponse_location(), response_td_id, false);
}

void TDEngine::on_trading_start(const EventSPtr &event) {
    if (not is_trading_started_) {
        const auto trading_start = event->data<TradingStart>();
        begin_time_ = trading_start.sync_time;

        for (auto &[key, service] : trade_services_) {
            service->start();
        }
        INFRA_LOG_INFO("Trading started at: {}", begin_time_);
        is_trading_started_ = true;
    }
}

void TDEngine::insert_order(const EventSPtr &event) {
    const auto &order_input = event->data<OrderInput>();
    auto td_uid = get_main_cfg().get_td_location_uid();
    uint32_t account_uid = uidutil::to_account_uid(order_input.order_id, td_uid);

    if (trade_services_[account_uid]->get_state() != enums::BrokerState::Ready) {
        // If the trade service is not ready, we should not insert the order.
        // Maybe retry later or notify the user.
        INFRA_LOG_ERROR("Trade service for account {} is not ready.", account_uid);
        return;
    }

    bool success = trade_services_[account_uid]->insert_order(order_input);
    if (not success) {
        // retry and notify me.
    }
}

void TDEngine::cancel_order(const EventSPtr &event) {
    const OrderAction &action = event->data<OrderAction>();
    auto td_uid = get_main_cfg().get_td_location_uid();
    uint32_t account_uid = uidutil::to_account_uid(action.order_id, td_uid);

    if (trade_services_[account_uid]->get_state() != enums::BrokerState::Ready) {
        // If the trade service is not ready, we should not insert the order.
        // Maybe retry later or notify the user.
        INFRA_LOG_ERROR("Trade service for account {} is not ready.", account_uid);
        return;
    }

    bool success = trade_services_[account_uid]->cancel_order(action);
    if (not success) {
        // retry and notify me.
    }
}

void TDEngine::on_account_req(const EventSPtr &event) {
    const auto &req = event->data<AccountReq>();
    auto td_uid = get_main_cfg().get_td_location_uid();
    uint32_t account_uid = uidutil::to_account_uid(req.id, td_uid);
    // std::cout << "Requesting account info for account UID: " << account_uid << std::endl;
    if (trade_services_[account_uid]->get_state() != enums::BrokerState::Ready) {
        // If the trade service is not ready, we should not insert the order.
        // Maybe retry later or notify the user.
        INFRA_LOG_ERROR("Trade service for account {} is not ready.", account_uid);
        return;
    }

    bool success = trade_services_[account_uid]->req_account_info(req);
    if (not success) {
        // retry and notify me.
    }
}

} // namespace btra