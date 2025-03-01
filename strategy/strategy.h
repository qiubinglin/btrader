#pragma once

#include "executor.h"
#include "jlocation.h"
#include "positionbook.h"
#include "types.h"

namespace btra::strategy {

class Strategy {
 public:
  virtual ~Strategy() = default;

  virtual void pre_start(ExecutorSPtr &executor) {}
  virtual void post_start(ExecutorSPtr &executor) {}

  virtual void pre_stop(ExecutorSPtr &executor) {}
  virtual void post_stop(ExecutorSPtr &executor) {}

  /**
   * @brief Callback on trading day update, from md
   *
   * @param executor
   * @param daytime
   */
  virtual void on_trading_day(ExecutorSPtr &executor, int64_t daytime) {}

  /**
   * @brief Callback on quote update, from td
   *
   * @param executor
   * @param quote
   * @param source
   */
  virtual void on_quote(ExecutorSPtr &executor, const Quote &quote, JID source) {}

  /**
   * @brief Callback on tree update, from md?
   *
   * @param executor
   * @param tree
   * @param source
   */
  virtual void on_tree(ExecutorSPtr &executor, const Tree &tree, JID source) {}

  /**
   * @brief Callback on bar update, from md
   *
   * @param executor
   * @param bar
   * @param source
   */
  virtual void on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) {}

  /**
   * @brief Callback on entrust update, from td
   *
   * @param executor
   * @param entrust
   * @param source
   */
  virtual void on_entrust(ExecutorSPtr &executor, const Entrust &entrust, JID source) {}

  /**
   * @brief Callback on transaction update, from td
   *
   * @param executor
   * @param transaction
   * @param source
   */
  virtual void on_transaction(ExecutorSPtr &executor, const Transaction &transaction, JID source) {}

  /**
   * @brief Callback on order update???
   *
   * @param executor
   * @param order
   * @param source
   */
  virtual void on_order(ExecutorSPtr &executor, const Order &order, JID source) {}

  /**
   * @brief Callback on order action error update, from td
   *
   * @param executor
   * @param error
   * @param source
   */
  virtual void on_order_action_error(ExecutorSPtr &executor, const OrderActionError &error, JID source) {}

  /**
   * @brief Callback on trade update, from td
   *
   * @param executor
   * @param trade
   * @param source
   */
  virtual void on_trade(ExecutorSPtr &executor, const Trade &trade, JID source) {}

  /**
   * @brief Callback on history order update
   *
   * @param executor
   * @param history_order
   * @param source
   */
  virtual void on_history_order(ExecutorSPtr &executor, const HistoryOrder &history_order, JID source) {}

  /**
   * @brief Callback on history trade update
   *
   * @param executor
   * @param history_trade
   * @param source
   */
  virtual void on_history_trade(ExecutorSPtr &executor, const HistoryTrade &history_trade, JID source) {}

  /**
   * @brief 历史订单查询报错回调
   *
   * @param executor
   * @param error
   * @param source
   */
  virtual void on_req_history_order_error(ExecutorSPtr &executor, const RequestHistoryOrderError &error, JID source) {}

  /**
   * @brief 历史成交查询报错回调
   *
   * @param executor
   * @param error
   * @param source
   */
  virtual void on_req_history_trade_error(ExecutorSPtr &executor, const RequestHistoryTradeError &error, JID source) {}

  /**
   * @brief 同步柜台资金持仓信息回调
   *
   * @param executor
   * @param old_book
   * @param new_book
   */
  virtual void on_position_sync_reset(ExecutorSPtr &executor, const PositionBook &old_book,
                                      const PositionBook &new_book) {}

  /**
   * @brief 同步柜台资金信息回调
   *
   * @param executor
   * @param old_asset
   * @param new_asset
   */
  virtual void on_asset_sync_reset(ExecutorSPtr &executor, const Asset &old_asset, const Asset &new_asset) {}

  /**
   * @brief 同步柜台资金信息回调
   *
   * @param executor
   * @param old_asset_margin
   * @param new_asset_margin
   */
  virtual void on_asset_margin_sync_reset(ExecutorSPtr &executor, const AssetMargin &old_asset_margin,
                                          const AssetMargin &new_asset_margin) {}

  /**
   * @brief 断开回调
   *
   * @param executor
   * @param deregister
   * @param source
   */
  virtual void on_deregister(ExecutorSPtr &executor, const Deregister &deregister, JID source) {}

  /**
   * @brief 客户端状态变化回调
   *
   * @param executor
   * @param broker_state_update
   * @param source
   */
  virtual void on_broker_state_change(ExecutorSPtr &executor, const BrokerStateUpdate &broker_state_update,
                                      JID source) {}

  /**
   * @brief 自定义数据回调, 如果数据的msg_type不在AllTypes, 则会通过此函数响应
   * @param executor
   * @param msg_type 数据类型
   * @param data  自定义数据, 以字节数组表示
   * @param length 自定义数据的字节数
   * @param source 数据来源
   */
  virtual void on_custom_data(ExecutorSPtr &executor, uint32_t msg_type, const std::vector<uint8_t> &data,
                              uint32_t length, JID source) {}
};
DECLARE_SPTR(Strategy)

}  // namespace btra::strategy

#define REGISTER_STRATEGY(strat) \
  extern "C" btra::strategy::Strategy *create_strategy_##strat() { return new strat(); }
