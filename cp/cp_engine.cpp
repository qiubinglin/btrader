#include "cp_engine.h"

namespace btra {

void CPEngine::react() {
    events_.filter(is<MsgTag::TradingDay>).subscribe(ON_MEM_FUNC(on_trading_day));
    events_.filter(is<MsgTag::Bar>).subscribe(ON_MEM_FUNC(on_bar));
    events_.filter(is<MsgTag::Quote>).subscribe(ON_MEM_FUNC(on_quote));
    events_.filter(is<MsgTag::Tree>).subscribe(ON_MEM_FUNC(on_tree));
    events_.filter(is<MsgTag::Entrust>).subscribe(ON_MEM_FUNC(on_entrust));
    events_.filter(is<MsgTag::Transaction>).subscribe(ON_MEM_FUNC(on_transaction));
    events_.filter(is<MsgTag::OrderActionError>).subscribe(ON_MEM_FUNC(on_order_action_error));
    events_.filter(is<MsgTag::Trade>).subscribe(ON_MEM_FUNC(on_trade));
    events_.filter(is<MsgTag::Asset>).subscribe(ON_MEM_FUNC(on_asset_sync_reset));
    events_.filter(is<MsgTag::AssetMargin>).subscribe(ON_MEM_FUNC(on_asset_margin_sync_reset));
    events_.filter(is<MsgTag::Deregister>).subscribe(ON_MEM_FUNC(on_deregister));
    events_.filter(is<MsgTag::BrokerStateUpdate>).subscribe(ON_MEM_FUNC(on_broker_state_change));
}

void CPEngine::on_setup() {
    main_cfg_ = MainCfg(cfg_);

    reader_ = std::make_unique<journal::Reader>(false);
    const auto &md_dests = main_cfg_.md_dests();
    for (auto dest : md_dests) {
        reader_->join(main_cfg_.md_location(), dest, begin_time_);
    }
    md_account_count_ = md_dests.size();

    reader_->join(main_cfg_.td_reponse_location(), journal::JIDUtil::build(journal::JIDUtil::TD_RESPONSE), begin_time_);

    const auto &td_dests = main_cfg_.td_dests();
    for (auto dest : td_dests) {
        writers_[dest] = std::make_unique<journal::Writer>(main_cfg_.td_location(), dest, false);
    }

    auto md_req_dest = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
    writers_[md_req_dest] = std::make_unique<journal::Writer>(main_cfg_.md_req_location(), md_req_dest, false);

    executor_ = std::make_shared<strategy::Executor>(this);

    typedef strategy::Strategy *(*create_strat_func)();
    auto cfg_strats = cfg_["strategy"];
    for (size_t i = 0; i < cfg_strats.size(); ++i) {
        std::string strat_name = cfg_strats[i]["id"].get<std::string>();
        std::string libfile = fmt::format("lib{}.so", strat_name);
        std::filesystem::path path = cfg_strats[i]["path"].get<std::string>();
        path /= libfile;

        std::string symbol_name = fmt::format("create_strategy_{}", strat_name);

        // INFRA_LOG_DEBUG("{}, {}", path.string(), symbol_name);

        if (dlhelper_.load(path.string()) == -1) {
            throw std::runtime_error("Load strategy failed!");
        }

        strategy::StrategySPtr strat_sptr =
            strategy::StrategySPtr(dlhelper_.find_symbol<create_strat_func>(static_cast<int>(i), symbol_name)());
        add_strategy(strat_sptr);
    }
}

void CPEngine::on_active() {
    if (not pre_start_) [[unlikely]] {
        pre_start();
        pre_start_ = true;
    }
}

void CPEngine::add_strategy(strategy::StrategySPtr strat) { strategies_.push_back(strat); }

void CPEngine::pre_start() {
    // INFRA_LOG_DEBUG("pre_start");
    TradingStart trading_start;
    trading_start.sync_time = begin_time_;
    auto req_md_dest = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
    auto now_time = infra::time::now_time();
    writers_[req_md_dest]->write(now_time, trading_start);

    for (auto &[key, writer] : writers_) {
        if (key == req_md_dest) {
            continue;
        }
        writers_[key]->write(now_time, trading_start);
        break;
    }

    invoke(&strategy::Strategy::pre_start);
}

void CPEngine::on_trading_day(const EventSPtr &event) {
    if (trading_msg_count_++ != 0) {
        trading_msg_count_ %= md_account_count_;
        return;
    }
    const auto &trading_day = event->data<TradingDay>();
    auto daytime = trading_day.timestamp;
    invoke(&strategy::Strategy::on_trading_day, daytime);
}

void CPEngine::on_bar(const EventSPtr &event) {
    const auto &bar = event->data<Bar>();
    invoke(&strategy::Strategy::on_bar, bar, event->source());
}

void CPEngine::on_quote(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_quote, event->data<Quote>(), event->source());
}

void CPEngine::on_tree(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_tree, event->data<Tree>(), event->source());
}

void CPEngine::on_entrust(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_entrust, event->data<Entrust>(), event->source());
}

void CPEngine::on_transaction(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_transaction, event->data<Transaction>(), event->source());
}

void CPEngine::on_order_action_error(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_order_action_error, event->data<OrderActionError>(), event->source());
}

void CPEngine::on_trade(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_trade, event->data<Trade>(), event->source());
}

void CPEngine::on_position_sync_reset(const EventSPtr &event) {
    PositionBook new_position_book = event->data<PositionBook>();
    invoke(&strategy::Strategy::on_position_sync_reset, PositionBookFn(executor_->book().positions),
           PositionBookFn(new_position_book), event->source());
    executor_->book().positions = new_position_book;
}

void CPEngine::on_asset_sync_reset(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_asset_sync_reset, executor_->book().asset, event->data<Asset>(), event->source());
    executor_->book().asset = event->data<Asset>();
}

void CPEngine::on_asset_margin_sync_reset(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_asset_margin_sync_reset, executor_->book().asset_margin, event->data<AssetMargin>(),
           event->source());
    executor_->book().asset_margin = event->data<AssetMargin>();
}

void CPEngine::on_deregister(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_deregister, event->data<Deregister>(), event->source());
}

void CPEngine::on_broker_state_change(const EventSPtr &event) {
    invoke(&strategy::Strategy::on_broker_state_change, event->data<BrokerStateUpdate>(), event->source());
}

void CPEngine::post_stop() {}

} // namespace btra