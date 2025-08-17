#pragma once

#include <cstdint>
#include <string>

namespace btra::enums {

enum class RunMode : int8_t { LIVE, DATA, REPLAY, BACKTEST, USER_APP };
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
        case RunMode::USER_APP:
            return "user-app";
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
    else if (name == "user-app")
        return RunMode::USER_APP;

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
    All,   ///< 表示全市场
    BSE,   ///< 北交所
    SHFE,  ///< 上期所
    CFFEX, ///< 中金所
    DCE,   ///< 大商所
    CZCE,  ///< 郑商所
    INE,   ///< 上期能源
    SSE,   ///< 上交所
    SZSE,  ///< 深交所
    HKEx ///< 港交所(暂时不支持直连港交所, 港交所行情数据通过深交所和上交所的港股通获取, 市场类型为kSZSE/kSSE)
};

// 证券数据类型
enum class SubscribeDataType : uint64_t {
    All = 0x000000000000,         ///< 订阅全部证券数据类别
    Snapshot = 0x000000000001,    ///< 订阅快照数据类别
    Entrust = 0x000000000002,     ///< 订阅逐笔委托数据
    Transaction = 0x000000000004, ///< 订阅逐笔成交数据
    Tree = 0x000000000008,        ///< 建树行情, 目前只有盛立有
};

// for subscribe
enum class SubscribeInstrumentType : uint64_t {
    All = 0x000000000000,         ///< 订阅全部证券品种类别
    Stock = 0x000000000001,       ///< 订阅股票证券品种类别
    Future = 0x000000000002,      ///< 订阅期货证券品种类别
    Bond = 0x000000000004,        ///< 订阅债券证券品种类别
    StockOption = 0x00000000008,  ///< 订阅期权证券品种类别
    FutureOption = 0x00000000010, ///< 订阅期货期权品种类别
    Fund = 0x0000000000020,       ///< 订阅基金证券品种类别
    Index = 0x000000000040,       ///< 订阅指数证券品种类别
    HKT = 0x000000000080,         ///< 订阅港股通证券品种类别

    /// SubscribeInstrumentType(All | Stock), 虽然没有定义值为3的枚举值, 这个操作不会报错, 会得到一个值为3的枚举值
};

// for trading, different type has different minimum volume, price, accounting rules for making order
enum class InstrumentType : int8_t {
    Unknown,     // 未知
    Stock,       // 股票
    Future,      // 期货
    Bond,        // 债券
    StockOption, // 股票期权
    TechStock,   // 科技股
    Fund,        // 基金
    Index,       // 指数
    Repo,        // 回购
    Warrant,     // 认权证
    Iopt,        // 牛熊证
    Crypto,      // 数字货币
};

enum class ExecType : int8_t { Unknown, Cancel, Trade };

/**
 * @brief Trade side
 *
 */
enum class Side : int8_t {
    Buy,                       // 买入
    Sell,                      // 卖出
    Lock,                      // 锁仓
    Unlock,                    // 解锁
    Exec,                      // 行权
    Drop,                      // 放弃行权
    Purchase,                  // 申购
    Redemption,                // 赎回
    Split,                     // 拆分
    Merge,                     // 合并
    MarginTrade,               // 融资买入
    ShortSell,                 // 融券卖出
    RepayMargin,               // 卖券还款
    RepayStock,                // 买券还券
    CashRepayMargin,           // 现金还款
    StockRepayStock,           // 现券还券
    SurplusStockTransfer,      // 余券划转
    GuaranteeStockTransferIn,  // 担保品转入
    GuaranteeStockTransferOut, // 担保品转出
    Unknown = 99
};

enum class Offset : int8_t { Open, Close, CloseToday, CloseYesterday };

enum class HedgeFlag : int8_t { Speculation, Arbitrage, Hedge, Covered };

enum class PriceType : int8_t {
    Limit, // 限价,证券通用
    Any, // 市价，证券通用，对于股票上海为最优五档剩余撤销，深圳为即时成交剩余撤销，建议客户采用
    FakBest5,    // 上海深圳最优五档即时成交剩余撤销，不需要报价
    ForwardBest, // 深圳本方方最优价格申报, 不需要报价
    ReverseBest, // 上海最优五档即时成交剩余转限价, 深圳对手方最优价格申报，不需要报价
    Fak,         // 深圳即时成交剩余撤销，不需要报价
    Fok,         // 深圳市价全额成交或者撤销，不需要报价
    Unknown
};

enum class PriceLevel : int8_t {
    Last, // 最新价
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
    UpperLimitPrice, // 涨停价
    LowerLimitPrice, // 跌停价
    Unknown
};

/**
 * @brief Volume condition for order execution
 *
 * Defines how the order volume should be handled during execution.
 */
enum class VolumeCondition : int8_t {
    Any, ///< Execute any available volume (partial fills allowed)
    Min, ///< Execute minimum required volume or cancel
    All  ///< Execute all volume or cancel (FOK - Fill or Kill)
};

/**
 * @brief Time condition for order validity
 *
 * Defines when and how long an order remains valid in the market.
 */
enum class TimeCondition : int8_t {
    IOC, ///< Immediate or Cancel: Execute available volume immediately, cancel remaining
    GFD, ///< Good for Day: Order valid until end of trading day
    GTC, ///< Good Till Cancelled: Order remains valid until manually cancelled
    FOK  ///< Fill or Kill: Must execute all volume or cancel entirely
};

/**
 * @brief Order status enumeration representing the current state of a trading order
 *
 * This enum defines all possible states that an order can be in during its lifecycle,
 * from initial submission to final completion or termination.
 */
enum class OrderStatus : int8_t {
    Unknown,                ///< Order status is unknown or not yet determined
    Submitted,              ///< Order has been submitted to the trading system but not yet processed
    Pending,                ///< Order is pending execution, waiting for market conditions to be met
    Cancelled,              ///< Order has been cancelled by the user or system
    Error,                  ///< Order encountered an error during processing or execution
    Filled,                 ///< Order has been completely filled (all volume executed)
    PartialFilledNotActive, ///< Order was partially filled but is no longer active (remaining volume cancelled)
    PartialFilledActive,    ///< Order was partially filled and remains active for remaining volume
    Lost                    ///< Order was lost due to system failure or network issues
};

/**
 * @brief Currency type enumeration
 *
 * Defines supported currency types for trading and settlement.
 */
enum class CurrencyType : int8_t {
    Unknown = 0, ///< Unknown or unspecified currency
    CNY,         ///< Chinese Yuan (Renminbi)
    HKD,         ///< Hong Kong Dollar
    USD,         ///< US Dollar
    JPY,         ///< Japanese Yen
    GBP,         ///< British Pound Sterling
    EUR,         ///< Euro
    CNH,         ///< Chinese Yuan (Offshore)
    SGD,         ///< Singapore Dollar
    MYR          ///< Malaysian Ringgit
};

/**
 * @brief Basket order status enumeration
 *
 * Represents the current status of a basket order (multiple instruments).
 */
enum class BasketOrderStatus : int8_t {
    Unknown,                ///< Basket order status is unknown
    Pending,                ///< Basket order is pending execution
    PartialFilledNotActive, ///< Partially filled but no longer active
    PartialFilledActive,    ///< Partially filled and still active
    Filled                  ///< All instruments in basket are filled
};

/**
 * @brief Basket order calculation mode
 *
 * Defines how basket order quantities are calculated.
 */
enum class BasketOrderCalculationMode : int8_t {
    Static, ///< Fixed quantities for each instrument
    Dynamic ///< Dynamic quantities based on market conditions
};

/**
 * @brief Basket volume type
 *
 * Defines how volume is specified in basket orders.
 */
enum class BasketVolumeType : int8_t {
    Unknown,   ///< Volume type is unknown or unspecified
    Quantity,  ///< Absolute quantity for each instrument
    Proportion ///< Proportional quantity based on weights
};

/**
 * @brief Basket order type
 *
 * Defines the type of basket order.
 */
enum class BasketType : int8_t {
    Custom, ///< Custom basket with user-defined instruments
    ETF     ///< ETF basket following predefined composition
};

/**
 * @brief Position direction
 *
 * Indicates the direction of a trading position.
 */
enum class Direction : int8_t {
    Long,   ///< Long position (buy and hold)
    Short,  ///< Short position (sell and cover later)
    Unknown ///< Position direction is unknown
};

/**
 * @brief Account type
 *
 * Defines the type of trading account.
 */
enum class AccountType : int8_t {
    Stock,   ///< Stock trading account
    Credit,  ///< Credit/margin trading account
    Future,  ///< Futures trading account
    BackTest ///< Backtesting simulation account
};

/**
 * @brief Commission rate calculation mode
 *
 * Defines how commission is calculated for trades.
 */
enum class CommissionRateMode : int8_t {
    ByAmount, ///< Commission calculated based on trade amount
    ByVolume  ///< Commission calculated based on trade volume
};

/**
 * @brief Ledger category
 *
 * Defines the category of ledger entries.
 */
enum class LedgerCategory : int8_t {
    Account,  ///< Account-level ledger entries
    Strategy, ///< Strategy-level ledger entries
};

/**
 * @brief Broker connection state
 *
 * Represents the current connection and authentication state of a broker.
 */
enum class BrokerState : int8_t {
    Pending = 0,      ///< Initial state, waiting to start
    Idle = 1,         ///< Broker is idle, ready to connect
    DisConnected = 2, ///< Broker is disconnected from exchange
    Connected = 3,    ///< Broker is connected to exchange
    LoggedIn = 4,     ///< Broker is authenticated and logged in
    LoginFailed = 5,  ///< Authentication failed
    Ready = 100       ///< Broker is ready for trading operations
};

/**
 * @brief Historical data type
 *
 * Defines the type of historical data response.
 */
enum class HistoryDataType : int8_t {
    Normal = 0,  ///< Normal historical data record
    PageEnd = 1, ///< End of current data page
    TotalEnd = 2 ///< End of all requested data
};

/**
 * @brief Strategy execution state
 *
 * Represents the current execution state of a trading strategy.
 */
enum class StrategyState : int8_t {
    Normal, ///< Strategy is running normally
    Warn,   ///< Strategy has warnings but continues running
    Error   ///< Strategy has encountered an error and may be stopped
};

/**
 * @brief Journal assembly mode configuration
 *
 * Defines how journal entries are assembled and accessed.
 * Uses bit flags for flexible configuration.
 */
class AssembleMode {
public:
    inline static const uint32_t Channel = 0b00000001; ///< Read only journal of location to dest_id
    inline static const uint32_t Write = 0b00000010;   ///< Read all journal from this location
    inline static const uint32_t Read = 0b00000100;    ///< Read all journal to this dest_id
    inline static const uint32_t Public = 0b00001000;  ///< Read all journal to location::PUBLIC
    inline static const uint32_t Sync = 0b00010000;    ///< Read all journal to location::PUBLIC
    inline static const uint32_t All = 0b00100000;     ///< Read all journal
};

/**
 * @brief Market data type
 *
 * Defines the type of market data being processed.
 */
enum class MDType : int8_t {
    Unknown, ///< Unknown or unspecified market data type
    Kline,   ///< Candlestick/K-line data
    Depth,   ///< Market depth/order book data
    Trade,   ///< Trade execution data
};

/**
 * @brief Broker request type
 *
 * Defines the type of request sent to a broker.
 */
enum class BrokerReqType : uint8_t {
    Unknown = 0, ///< Unknown or unspecified request type
    OrderPlace,  ///< Place a new order
    OrderCancel, ///< Cancel an existing order
    OrderBook,   ///< Request order book information
    OrderState,  ///< Request order state information
    PositionBook ///< Request position book information
};

enum class BrokerRespType : int8_t {
    Unknown = 0, ///< Unknown or unspecified response type
    OrderPlace,
    OrderCancel,
};

/**
 * @brief Convert order side to position direction
 *
 * Maps a trading order side (Buy/Sell) to the corresponding position direction (Long/Short).
 *
 * @param side The order side (Buy or Sell)
 * @return The corresponding position direction (Long for Buy, Short for Sell)
 */
inline Direction side2direction(Side side) {
    Direction ret = Direction::Unknown;
    switch (side) {
        case Side::Buy:
            ret = Direction::Long;
            break;
        case Side::Sell:
            ret = Direction::Short;
            break;
        default:
            break;
    }
    return ret;
}

} // namespace btra::enums