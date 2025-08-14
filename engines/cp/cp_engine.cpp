#include "cp/cp_engine.h"

#include "cp/backtest_subscriber.h"
#include "cp/live_subscriber.h"
#include "strategy/dummy_strategy.h"

namespace btra {

CPEngine::~CPEngine() {
    if (live_subscriber_) {
        delete live_subscriber_;
    }
    if (backtest_subscriber_) {
        delete backtest_subscriber_;
    }

    if (is_backtest()) {
        backtest_dump_.dump_files(main_cfg_.root_path());
    }
}

void CPEngine::react() {
    if (not live_subscriber_) {
        live_subscriber_ = new LiveSubscriber(this);
    }
    events_.filter(is<MsgTag::Termination>).subscribe([this](const EventSPtr &event) {
        auto req_md_dest = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
        auto now_time = infra::time::now_time();
        this->writers_[req_md_dest]->write(now_time, Termination());

        for (auto &[key, writer] : this->writers_) {
            if (key == req_md_dest) {
                continue;
            }
            writer->write(now_time, Termination());
        }
        this->stop();
    });

    events_.filter(is<MsgTag::TradingDay>).subscribe(ON_MEM_OBJ(live_subscriber_, on_trading_day));
    events_.filter(is<MsgTag::Bar>).subscribe(ON_MEM_OBJ(live_subscriber_, on_bar));
    events_.filter(is<MsgTag::Quote>).subscribe(ON_MEM_OBJ(live_subscriber_, on_quote));
    events_.filter(is<MsgTag::Tree>).subscribe(ON_MEM_OBJ(live_subscriber_, on_tree));
    events_.filter(is<MsgTag::Entrust>).subscribe(ON_MEM_OBJ(live_subscriber_, on_entrust));
    events_.filter(is<MsgTag::Transaction>).subscribe(ON_MEM_OBJ(live_subscriber_, on_transaction));
    events_.filter(is<MsgTag::OrderActionError>).subscribe(ON_MEM_OBJ(live_subscriber_, on_order_action_error));
    events_.filter(is<MsgTag::Trade>).subscribe(ON_MEM_OBJ(live_subscriber_, on_trade));
    events_.filter(is<MsgTag::Asset>).subscribe(ON_MEM_OBJ(live_subscriber_, on_asset_sync_reset));
    events_.filter(is<MsgTag::AssetMargin>).subscribe(ON_MEM_OBJ(live_subscriber_, on_asset_margin_sync_reset));
    events_.filter(is<MsgTag::Deregister>).subscribe(ON_MEM_OBJ(live_subscriber_, on_deregister));
    events_.filter(is<MsgTag::BrokerStateUpdate>).subscribe(ON_MEM_OBJ(live_subscriber_, on_broker_state_change));
    events_.filter(over_max_tag).subscribe(ON_MEM_OBJ(live_subscriber_, on_custom_data));
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

    executor_ = strategy::Executor::create(main_cfg_.run_mode(), this);

    if (main_cfg_.run_mode() == enums::RunMode::USER_APP) {
        add_strategy(std::make_shared<strategy::DummyStrategy>());
    } else {
        typedef strategy::Strategy *(*create_strat_func)();
        auto cfg_strats = cfg_["strategy"];
        for (size_t i = 0; i < cfg_strats.size(); ++i) {
            std::string strat_name = cfg_strats[i]["id"].get<std::string>();
            std::string libfile = cfg_strats[i]["lib_name"].get<std::string>();
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
}

void CPEngine::on_active() {
    if (not pre_start_) [[unlikely]] {
        if (main_cfg_.run_mode() == enums::RunMode::LIVE) {
            live_subscriber_->pre_start();
        } else if (main_cfg_.run_mode() == enums::RunMode::BACKTEST) {
            backtest_subscriber_->pre_start();
        }
        INFRA_LOG_CRITICAL("cp pre_start done");
        pre_start_ = true;
    }
}

void CPEngine::add_strategy(strategy::StrategySPtr strat) { strategies_.push_back(strat); }

} // namespace btra