#pragma once

#include <cstdint>
#include <string>

namespace btra::enums {

enum class RunMode : int8_t { LIVE, DATA, REPLAY, BACKTEST };
inline std::string get_mode_name(RunMode m) {
  switch (m) {
    case RunMode::LIVE:
      return "live";
    case RunMode::DATA:
      return "data";
    case RunMode::REPLAY:
      return "replay";
    case RunMode::BACKTEST:
      return "backtest";
    default:
      return "live";
  }
}
inline RunMode get_mode_by_name(const std::string &name) {
  if (name == "live")
    return RunMode::LIVE;
  else if (name == "data")
    return RunMode::DATA;
  else if (name == "replay")
    return RunMode::REPLAY;
  else if (name == "backtest")
    return RunMode::BACKTEST;

  return RunMode::LIVE;
}

enum class Module : int8_t { MD, TD, STRATEGY, SYSTEM };
inline std::string get_module_name(Module c) {
  switch (c) {
    case Module::MD:
      return "md";
    case Module::TD:
      return "td";
    case Module::STRATEGY:
      return "strategy";
    case Module::SYSTEM:
    default:
      return "system";
  }
}
inline Module get_module_by_name(const std::string &name) {
  if (name == "md")
    return Module::MD;
  else if (name == "td")
    return Module::TD;
  else if (name == "strategy")
    return Module::STRATEGY;
  else
    return Module::SYSTEM;
}

enum class layout : int8_t { JOURNAL, SQLITE, NANOMSG, LOG };
inline std::string get_layout_name(layout l) {
  switch (l) {
    case layout::JOURNAL:
      return "journal";
    case layout::SQLITE:
      return "db";
    case layout::NANOMSG:
      return "nn";
    case layout::LOG:
    default:
      return "log";
  }
}

// 权限订阅数据类型
enum class MarketType : uint8_t {
  All,    ///< 表示全市场
  BSE,    ///< 北交所
  SHFE,   ///< 上期所
  CFFEX,  ///< 中金所
  DCE,    ///< 大商所
  CZCE,   ///< 郑商所
  INE,    ///< 上期能源
  SSE,    ///< 上交所
  SZSE,   ///< 深交所
  HKEx  ///< 港交所(暂时不支持直连港交所, 港交所行情数据通过深交所和上交所的港股通获取, 市场类型为kSZSE/kSSE)
};

// 证券数据类型
enum class SubscribeDataType : uint64_t {
  All = 0x000000000000,          ///< 订阅全部证券数据类别
  Snapshot = 0x000000000001,     ///< 订阅快照数据类别
  Entrust = 0x000000000002,      ///< 订阅逐笔委托数据
  Transaction = 0x000000000004,  ///< 订阅逐笔成交数据
  Tree = 0x000000000008,         ///< 建树行情, 目前只有盛立有
};

// for subscribe
enum class SubscribeInstrumentType : uint64_t {
  All = 0x000000000000,          ///< 订阅全部证券品种类别
  Stock = 0x000000000001,        ///< 订阅股票证券品种类别
  Future = 0x000000000002,       ///< 订阅期货证券品种类别
  Bond = 0x000000000004,         ///< 订阅债券证券品种类别
  StockOption = 0x00000000008,   ///< 订阅期权证券品种类别
  FutureOption = 0x00000000010,  ///< 订阅期货期权品种类别
  Fund = 0x0000000000020,        ///< 订阅基金证券品种类别
  Index = 0x000000000040,        ///< 订阅指数证券品种类别
  HKT = 0x000000000080,          ///< 订阅港股通证券品种类别

  /// SubscribeInstrumentType(All | Stock), 虽然没有定义值为3的枚举值, 这个操作不会报错, 会得到一个值为3的枚举值
};

// for trading, different type has different minimum volume, price, accounting rules for making order
enum class InstrumentType : int8_t {
  Unknown,      // 未知
  Stock,        // 股票
  Future,       // 期货
  Bond,         // 债券
  StockOption,  // 股票期权
  TechStock,    // 科技股
  Fund,         // 基金
  Index,        // 指数
  Repo,         // 回购
  Warrant,      // 认权证
  Iopt,         // 牛熊证
  Crypto,       // 数字货币
};

enum class ExecType : int8_t { Unknown, Cancel, Trade };

enum class BsFlag : int8_t { Unknown, Buy, Sell };

/**
 * @brief Trade side
 * 
 */
enum class Side : int8_t {
  Buy,                        // 买入
  Sell,                       // 卖出
  Lock,                       // 锁仓
  Unlock,                     // 解锁
  Exec,                       // 行权
  Drop,                       // 放弃行权
  Purchase,                   // 申购
  Redemption,                 // 赎回
  Split,                      // 拆分
  Merge,                      // 合并
  MarginTrade,                // 融资买入
  ShortSell,                  // 融券卖出
  RepayMargin,                // 卖券还款
  RepayStock,                 // 买券还券
  CashRepayMargin,            // 现金还款
  StockRepayStock,            // 现券还券
  SurplusStockTransfer,       // 余券划转
  GuaranteeStockTransferIn,   // 担保品转入
  GuaranteeStockTransferOut,  // 担保品转出
  Unknown = 99
};

enum class Offset : int8_t { Open, Close, CloseToday, CloseYesterday };

enum class HedgeFlag : int8_t { Speculation, Arbitrage, Hedge, Covered };

enum class OrderActionFlag : int8_t {
  Cancel,
};

enum class PriceType : int8_t {
  Limit,  // 限价,证券通用
  Any,  // 市价，证券通用，对于股票上海为最优五档剩余撤销，深圳为即时成交剩余撤销，建议客户采用
  FakBest5,     // 上海深圳最优五档即时成交剩余撤销，不需要报价
  ForwardBest,  // 深圳本方方最优价格申报, 不需要报价
  ReverseBest,  // 上海最优五档即时成交剩余转限价, 深圳对手方最优价格申报，不需要报价
  Fak,          // 深圳即时成交剩余撤销，不需要报价
  Fok,          // 深圳市价全额成交或者撤销，不需要报价
  Unknown
};

enum class PriceLevel : int8_t {
  Last,  // 最新价
  Sell5,
  Sell4,
  Sell3,
  Sell2,
  Sell1,
  Buy1,
  Buy2,
  Buy3,
  Buy4,
  Buy5,
  UpperLimitPrice,  // 涨停价
  LowerLimitPrice,  // 跌停价
  Unknown
};

enum class VolumeCondition : int8_t { Any, Min, All };

enum class TimeCondition : int8_t {
  IOC, /* 立即成交可用部分，剩余取消 */
  GFD, /* 当日有效 */
  GTC, /* 订单持续有效，直到手动取消 */
  FOK /* 必须全部成交，否则取消 */
};

enum class OrderStatus : int8_t {
  Unknown,
  Submitted,
  Pending,
  Cancelled,
  Error,
  Filled,
  PartialFilledNotActive,
  PartialFilledActive,
  Lost
};

// 币种枚举
enum class CurrencyType : int8_t { Unknown = 0, CNY, HKD, USD, JPY, GBP, EUR, CNH, SGD, MYR };

enum class BasketOrderStatus : int8_t { Unknown, Pending, PartialFilledNotActive, PartialFilledActive, Filled };

enum class BasketOrderCalculationMode : int8_t { Static, Dynamic };

enum class BasketVolumeType : int8_t { Unknown, Quantity, Proportion };

enum class BasketType : int8_t { Custom, ETF };

enum class Direction : int8_t { Long, Short };

enum class AccountType : int8_t { Stock, Credit, Future };

enum class CommissionRateMode : int8_t { ByAmount, ByVolume };

enum class LedgerCategory : int8_t {
  Account,
  Strategy,
};

enum class BrokerState : int8_t {
  Pending = 0,
  Idle = 1,
  DisConnected = 2,
  Connected = 3,
  LoggedIn = 4,
  LoginFailed = 5,
  Ready = 100
};

enum class HistoryDataType : int8_t { Normal = 0, PageEnd = 1, TotalEnd = 2 };

enum class StrategyState : int8_t { Normal, Warn, Error };

class AssembleMode {
 public:
  inline static const uint32_t Channel = 0b00000001;  // read only journal of location to dest_id
  inline static const uint32_t Write = 0b00000010;    // read all journal from this location
  inline static const uint32_t Read = 0b00000100;     // read all journal to this dest_id
  inline static const uint32_t Public = 0b00001000;   // read all journal to location::PUBLIC
  inline static const uint32_t Sync = 0b00010000;     // read all journal to location::PUBLIC
  inline static const uint32_t All = 0b00100000;      // read all journal
};

}  // namespace btra::enums