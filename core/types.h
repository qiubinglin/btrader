#pragma once

#include <cstdint>
#include <string>

#include "enums.h"
#include "infra/array.h"

namespace btra {

#define PACK_DATA_BODY static constexpr bool fixed = true;

#define PACK_DATA_BODY2(tag_num)        \
    static constexpr bool fixed = true; \
    static constexpr int tag = MsgTag::tag_num;

#define UNFIXED_DATA_BODY(TagClass)              \
    static constexpr bool fixed = false;         \
    static constexpr int tag = MsgTag::TagClass; \
    TagClass() = default;                        \
    TagClass(const char *, uint32_t);            \
    std::string to_string() const;

static constexpr int INSTRUMENT_ID_LEN = 32;
static constexpr int ACCOUNT_ID_LEN = 32;
static constexpr int PRODUCT_ID_LEN = 128;
static constexpr int DATE_LEN = 9;
static constexpr int EXCHANGE_ID_LEN = 16;
static constexpr int TRAIDNG_PHASE_CODE_LEN = 8;
static constexpr int ERROR_MSG_LEN = 256;
static constexpr int EXTERNAL_ID_LEN = 32;
static constexpr int OPPONENT_SEAT_LEN = 16;
static constexpr int PASSWORD_ID_LEN = 32;
static constexpr int INSTITUTION_ID_LEN = 32;

using VolumeType = double;

struct MsgTag {
    enum Tag {
        PageEnd = 0,
        OrderInput,
        Bar,
        MDSubscribe,
        OrderCancel,
        TradingDay,
        Quote,
        Entrust,
        Transaction,
        OrderActionResp,
        Trade,
        Asset,
        AssetMargin,
        Deregister,
        BrokerStateUpdate,
        TradingStart,
        TradingStop,
        InstrumentKey,
        Instrument,
        Position,
        AccountReq,
        PositionBook,
        Order,
        HistoryOrder,
        HistoryTrade,
        RequestHistoryOrderError,
        RequestHistoryTradeError,
        Termination,  /* Terminate event engine. */
        TAG_MAX_SIZE, /* This must be the last tag which indicates the maximum size of the tag. */
    };
};

template <size_t N>
struct InstrumentDepth {
    static const size_t MAX_SIZE = N;
    uint32_t real_depth_size = 0;
    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID
    infra::Array<double, N> bid_price;                   // 申买价
    infra::Array<double, N> ask_price;                   // 申卖价
    infra::Array<VolumeType, N> bid_volume;              // 申买量
    infra::Array<VolumeType, N> ask_volume;              // 申卖量
};

/**
 * @brief Not use now
 *
 */
struct StrategyStateUpdate { //
    PACK_DATA_BODY
    enums::StrategyState state; //
    int64_t update_time;        //
    std::string info_a;         //
    std::string info_b;         //
    std::string info_c;         //
    std::string value;          //
};

/**
 * @brief Not use now
 *
 */
struct Register { //
    PACK_DATA_BODY
    uint32_t location_uid;    //
    enums::Module category;   //
    enums::RunMode mode;      //
    std::string group;        //
    std::string name;         //
    int32_t pid;              //
    int64_t last_active_time; //
    int64_t checkin_time;     //
};

/**
 * @brief Not use now
 *
 */
struct Deregister {
    UNFIXED_DATA_BODY(Deregister)
    uint32_t location_uid;  //
    enums::Module category; //
    enums::RunMode mode;    //
    std::string group;      //
    std::string name;       //
};

struct BrokerStateUpdate {
    PACK_DATA_BODY2(BrokerStateUpdate)
    uint32_t location_uid;    //
    enums::BrokerState state; //
};

/**
 * @brief Determine directory
 *
 */
struct Location {
    uint32_t location_uid;
    enums::Module category;
    enums::RunMode mode;
    std::string group; /* self-defined group */
    std::string name;  /* self-defined name */
};

struct TradingDay { //
    PACK_DATA_BODY2(TradingDay)
    int64_t timestamp; //
};

/**
 * @brief Re-Synchronize the time
 *
 */
struct TimeReset { //
    PACK_DATA_BODY
    int64_t system_clock_count; //
    int64_t steady_clock_count; //
};

/**
 * @brief The commission of a product
 *
 */
struct Commission {
    PACK_DATA_BODY
    infra::Array<char, PRODUCT_ID_LEN> product_id;   // 品种
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id; // 交易所

    enums::InstrumentType instrument_type; // 合约类型

    enums::CommissionRateMode mode; // 手续费模式(按照交易额或者交易量)

    double open_ratio;        // 开仓费率
    double close_ratio;       // 平仓费率
    double close_today_ratio; // 平今费率

    double min_commission; // 最小手续费
};

struct Instrument {
    PACK_DATA_BODY2(Instrument)
    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID
    enums::InstrumentType instrument_type;               // 合约类型

    infra::Array<int8_t, PRODUCT_ID_LEN> product_id; // 产品ID (品种)

    int32_t contract_multiplier; // 合约乘数
    double price_tick;           // 最小变动价位

    infra::Array<char, DATE_LEN> open_date;   // 上市日
    infra::Array<char, DATE_LEN> create_date; // 创建日
    infra::Array<char, DATE_LEN> expire_date; // 到期日

    int delivery_year;  // 交割年份
    int delivery_month; // 交割月

    bool is_trading;         // 当前是否交易
    bool force_update_ratio; // 两融柜台折算率及保证金率

    double long_margin_ratio;          // 多头保证金率
    double short_margin_ratio;         // 空头保证金率
    double conversion_rate;            // 担保品折扣率
    double exchange_rate;              // 汇率
    enums::CurrencyType currency_type; // 币种
};

struct InstrumentKey { //
    PACK_DATA_BODY2(InstrumentKey)
    uint32_t key;                                        //
    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID
    enums::InstrumentType instrument_type;               // 合约类型
};

/**
 * @brief Figure out the usage. Not use now.
 *
 */
struct CustomSubscribe {
    PACK_DATA_BODY
    int64_t update_time;
    enums::MarketType market_type;
    enums::SubscribeInstrumentType instrument_type;
    enums::SubscribeDataType data_type;
};

/**
 * @brief To be replace by Depth
 *
 */
struct Quote { //
    PACK_DATA_BODY2(Quote)
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    int64_t data_time; // 数据生成时间

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID

    enums::InstrumentType instrument_type; // 合约类型

    double pre_close_price;      // 昨收价
    double pre_settlement_price; // 昨结价

    double last_price; // 最新价
    VolumeType volume; // 数量
    double turnover;   // 成交金额

    double pre_open_interest; // 昨持仓量
    double open_interest;     // 持仓量

    double open_price; // 今开盘
    double high_price; // 最高价
    double low_price;  // 最低价

    double upper_limit_price; // 涨停板价
    double lower_limit_price; // 跌停板价

    double close_price;      // 收盘价
    double settlement_price; // 结算价
    double iopv;             // 基金实时参考净值

    infra::Array<double, 20> bid_price;      // 申买价
    infra::Array<double, 20> ask_price;      // 申卖价
    infra::Array<VolumeType, 20> bid_volume; // 申买量
    infra::Array<VolumeType, 20> ask_volume; // 申卖量
    uint32_t real_depth_size = infra::Array<double, 20>::length;
    infra::Array<char, TRAIDNG_PHASE_CODE_LEN> trading_phase_code;
    // 标的状态, 上交所用四位, 深交所用两位
    //************************************上海现货行情交易状态***************************************************************
    // 该字段为8位字符数组,左起每位表示特定的含义,无定义则填空格。
    // 第0位:‘S’表示启动(开市前)时段,‘C’表示集合竞价时段,‘T’表示连续交易时段,
    // ‘E’表示闭市时段 ,‘P’表示临时停牌,
    // ‘M’表示可恢复交易的熔断(盘中集合竞价;‘N’表示不可恢复交易的熔断(暂停交易至闭市)
    // ‘U’表示收盘集合竞价
    // 第1位:‘0’表示此产品不可正常交易,‘1’表示此产品可正常交易。
    // 第2位:‘0’表示未上市,‘1’表示已上市
    // 第3位:‘0’表示此产品在当前时段不接受进行新订单申报,‘1’ 表示此产品在当前时段可接受进行新订单申报。

    //************************************深圳现货行情交易状态***************************************************************
    // 第 0位:‘S’= 启动(开市前)‘O’= 开盘集合竞价‘T’= 连续竞价‘B’= 休市‘C’= 收盘集合竞价‘E’= 已闭市‘H’= 临时停牌‘A’=
    // 盘后交易‘V’=波动性中断 第 1位:‘0’= 正常状态 ‘1’= 全天停牌
};

/**
 * @brief Maybe represent the conditional entrust. Not use now
 *
 */
struct Entrust {
    PACK_DATA_BODY2(Entrust)
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    int64_t data_time; // 数据生成时间

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所代码

    enums::InstrumentType instrument_type; // 合约类型

    double price;                // 委托价格
    VolumeType volume;           // 委托量
    enums::Side side;            // 委托方向
    enums::PriceType price_type; // 订单价格类型（市价、限价、本方最优）

    int64_t main_seq;      // 主序号,
    int64_t seq;           // 子序号,
    int64_t orig_order_no; // 原始订单号 上海为原始订单号, 深圳为索引号
    int64_t biz_index;     // 业务序号
};

/**
 * @brief Represent a transaction happened in the market, not transaction of our order. Not use now.
 *
 */
struct Transaction { //
    PACK_DATA_BODY2(Transaction)
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    int64_t data_time; // 数据生成时间

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所代码

    enums::InstrumentType instrument_type; // 合约类型

    double price;      // 成交价
    VolumeType volume; // 成交量

    int64_t bid_no; // 买方订单号
    int64_t ask_no; // 卖方订单号

    enums::ExecType exec_type; // SZ: 成交标识
    enums::Side side;          // 买卖方向

    int64_t main_seq;  // 主序号
    int64_t seq;       // 子序号
    int64_t biz_index; // 业务序号
};

struct Bar {
    PACK_DATA_BODY2(Bar)
    infra::Array<char, DATE_LEN> trading_day;            // 交易日
    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约代码
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所代码

    enums::InstrumentType instrument_type; // 合约类型

    int64_t start_time{0}; // 开始时间
    int64_t end_time{0};   // 结束时间

    double open;  // 开
    double close; // 收
    double low;   // 低
    double high;  // 高

    VolumeType volume;       // 区间交易量
    VolumeType start_volume; // 初始总交易量

    int32_t tick_count; // 区间有效tick数
};

/**
 * @brief The input for placing an order.
 * This structure is used to send an order request to the exchange.
 */
struct OrderInput {
    PACK_DATA_BODY2(OrderInput)
    uint64_t order_id;  // 订单ID
    uint64_t parent_id; // 母单号

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约代码
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所代码

    enums::InstrumentType instrument_type; // 合约类型

    double limit_price;  // 价格
    double frozen_price; // 冻结价格

    VolumeType volume; // 数量

    bool is_swap;                            // 互换单
    enums::Side side;                        // 买卖方向
    enums::Offset offset;                    // 开平方向
    enums::HedgeFlag hedge_flag;             // 投机套保标识
    enums::PriceType price_type;             // 价格类型
    enums::VolumeCondition volume_condition; // 成交量类型
    enums::TimeCondition time_condition;     // 成交时间类型
    uint64_t block_id;                       // 大宗交易信息id, 非大宗交易则为0

    int64_t insert_time; // 写入时间
};

/**
 * @brief Same as OrderInput, but used for order cancel. It maybe replace OrderInput in future.
 *
 */
struct OrderCancel {
    PACK_DATA_BODY2(OrderCancel)
    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id;
    uint64_t order_id;        // Order id
    uint64_t target_order_id; // Target order id

    double price;        // 价格
    VolumeType volume;   // 数量
    int64_t insert_time; // 写入时间
};

/**
 * @brief Response struct for all order actions (OrderInput, OrderCancel)
 *
 */
struct OrderActionResp {
    PACK_DATA_BODY2(OrderActionResp)
    uint64_t order_id;                                     // 订单ID
    infra::Array<char, EXTERNAL_ID_LEN> external_order_id; // Order id in exchange
    uint64_t order_action_id;                              // 订单操作ID
    int32_t error_id{0};                                      // 错误ID
    infra::Array<char, ERROR_MSG_LEN> error_msg;           // 错误信息
    int64_t insert_time;                                   // 写入时间
    enums::BrokerRespType resp_type;
};

/**
 * @brief The order in the exchange.
 *
 */
struct Order {
    PACK_DATA_BODY2(Order)
    uint64_t order_id;                                     // 订单ID
    infra::Array<char, EXTERNAL_ID_LEN> external_order_id; // 柜台订单id
    uint64_t parent_id;                                    // 母单号

    int64_t insert_time; // 订单写入时间
    int64_t update_time; // 订单更新时间

    infra::Array<char, DATE_LEN> trading_day; // 交易日

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID

    enums::InstrumentType instrument_type; // 合约类型

    double limit_price;  // 价格
    double frozen_price; // 冻结价格, 市价单冻结价格为0

    VolumeType volume;      // 数量
    VolumeType volume_left; // 剩余数量

    double tax;        // 税
    double commission; // 手续费

    enums::OrderStatus status; // 订单状态

    int32_t error_id;                            // 错误ID
    infra::Array<char, ERROR_MSG_LEN> error_msg; // 错误信息

    bool is_swap;                            // 互换单
    enums::Side side;                        // 买卖方向
    enums::Offset offset;                    // 开平方向
    enums::HedgeFlag hedge_flag;             // 投机套保标识
    enums::PriceType price_type;             // 价格类型
    enums::VolumeCondition volume_condition; // 成交量类型
    enums::TimeCondition time_condition;     // 成交时间类型
};

struct HistoryOrder { //
    PACK_DATA_BODY2(HistoryOrder)
    uint64_t order_id;                                     // 订单ID
    infra::Array<char, EXTERNAL_ID_LEN> external_order_id; // 柜台订单id, 字符型则hash转换

    int64_t insert_time; // 订单写入时间
    int64_t update_time; // 订单更新时间

    infra::Array<char, DATE_LEN> trading_day; // 交易日

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID

    bool is_last;                     // 是否为本次查询的最后一条记录
    enums::HistoryDataType data_type; // 标记本数据是正常数据, 本页最后一条数据, 全部数据的最后一条

    enums::InstrumentType instrument_type; // 合约类型

    double limit_price;  // 价格
    double frozen_price; // 冻结价格, 市价单冻结价格为0

    VolumeType volume;      // 数量
    VolumeType volume_left; // 剩余数量

    double tax;        // 税
    double commission; // 手续费

    enums::OrderStatus status; // 订单状态

    int32_t error_id;                            // 错误ID
    infra::Array<char, ERROR_MSG_LEN> error_msg; // 错误信息

    bool is_swap; // 互换单

    enums::Side side;                        // 买卖方向
    enums::Offset offset;                    // 开平方向
    enums::HedgeFlag hedge_flag;             // 投机套保标识
    enums::PriceType price_type;             // 价格类型
    enums::VolumeCondition volume_condition; // 成交量类型
    enums::TimeCondition time_condition;     // 成交时间类型
};

/**
 * @brief Represent a trade of our order
 *
 */
struct Trade {
    PACK_DATA_BODY2(Trade)
    uint64_t trade_id; // 成交ID

    uint64_t order_id;                                     // 订单ID
    infra::Array<char, EXTERNAL_ID_LEN> external_order_id; // 柜台订单id
    infra::Array<char, EXTERNAL_ID_LEN> external_trade_id; // 柜台成交编号id

    int64_t trade_time;                       // 成交时间
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID

    enums::InstrumentType instrument_type; // 合约类型

    enums::Side side;            // 买卖方向
    enums::Offset offset;        // 开平方向
    enums::HedgeFlag hedge_flag; // 投机套保标识

    double price;      // 成交价格
    VolumeType volume; // 成交量

    double tax;        // 税
    double commission; // 手续费
};

struct HistoryTrade { //
    PACK_DATA_BODY2(HistoryTrade)
    uint64_t trade_id; // 成交ID

    uint64_t order_id;                                     // 订单ID
    infra::Array<char, EXTERNAL_ID_LEN> external_order_id; // 柜台订单id
    infra::Array<char, EXTERNAL_ID_LEN> external_trade_id; // 柜台成交编号id

    int64_t trade_time;                       // 成交时间
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID

    bool is_last;                     // 是否为本次查询的最后一条记录
    enums::HistoryDataType data_type; // 标记本数据是正常数据, 本页最后一条数据, 全部数据的最后一条
    bool is_withdraw;                 // 是否是撤单流水

    enums::InstrumentType instrument_type; // 合约类型

    enums::Side side;            // 买卖方向
    enums::Offset offset;        // 开平方向
    enums::HedgeFlag hedge_flag; // 投机套保标识

    double price;                  // 成交价格
    VolumeType volume;             // 成交量
    VolumeType close_today_volume; // 平今日仓量(期货)

    double tax;                                  // 税
    double commission;                           // 手续费
    int32_t error_id;                            // 错误ID
    infra::Array<char, ERROR_MSG_LEN> error_msg; // 错误信息
};

struct Position { //
    PACK_DATA_BODY2(Position)
    int64_t update_time;                      // 更新时间
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id; // 合约ID
    enums::InstrumentType instrument_type;               // 合约类型
    infra::Array<char, EXCHANGE_ID_LEN> exchange_id;     // 交易所ID

    // uint32_t holder_uid;                   //
    // enums::LedgerCategory ledger_category; //

    enums::Direction direction; // 持仓方向

    VolumeType volume; // 数量
    // int64_t yesterday_volume; // 昨仓数量
    // int64_t frozen_total;     // 冻结数量
    // int64_t frozen_yesterday; // 冻结昨仓

    // double last_price; // 最新价

    // double avg_open_price;      // 开仓均价
    double position_cost_price; // 持仓成本价

    // double close_price;     // 收盘价(股票和债券)
    // double pre_close_price; // 昨收价(股票和债券)

    // double settlement_price;     // 结算价(期货)
    // double pre_settlement_price; // 昨结算(期货) ***

    // double margin;       // 保证金(期货)
    // double position_pnl; // 持仓盈亏(期货)
    // double close_pnl;    // 平仓盈亏(期货) ***

    // double realized_pnl;   // 已实现盈亏
    double unrealized_pnl; // 未实现盈亏
};

struct PositionEnd { //
    PACK_DATA_BODY
    uint32_t holder_uid; //
};

struct Asset {
    PACK_DATA_BODY2(Asset)
    int64_t update_time;                      // 更新时间
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    // uint32_t holder_uid;
    // enums::LedgerCategory ledger_category;

    // double initial_equity; // 期初权益
    // double static_equity;  // 静态权益
    // double dynamic_equity; // 动态权益

    // double realized_pnl;   // Realized Profit and Loss
    // double unrealized_pnl; // Unrealized Profit and Loss

    double avail; // 可用资金
    // double market_value; // 市值(股票)

    double margin; // 保证金

    double accumulated_fee; // 累计手续费
    // double intraday_fee;    // 当日手续费

    // double frozen_cash;   // 冻结资金(股票: 买入挂单资金; 期货: 冻结保证金+冻结手续费)
    // double frozen_margin; // 冻结保证金(期货)
    // double frozen_fee;    // 冻结手续费(期货)

    // double position_pnl; // 持仓盈亏(期货)
    // double close_pnl;    // 平仓盈亏(期货)
};

struct AssetMargin {
    PACK_DATA_BODY2(AssetMargin)
    int64_t update_time;                      // 更新时间
    infra::Array<char, DATE_LEN> trading_day; // 交易日

    uint32_t holder_uid;                   //
    enums::LedgerCategory ledger_category; //

    double total_asset;  // 总资产
    double avail_margin; // 可用保证金
    double cash_margin;  // 融资占用保证金
    double short_margin; // 融券占用保证金
    double margin;       // 总占用保证金

    double cash_debt;  // 融资负债
    double short_cash; // 融券卖出金额

    double short_market_value;  // 融券卖出证券市值
    double margin_market_value; // 融资买入证券市值
    double margin_interest;     // 融资融券利息
    double settlement;          // 融资融券清算资金

    double credit;           // 信贷额度
    double collateral_ratio; // 担保比例
};

struct OrderStat { //
    PACK_DATA_BODY
    uint64_t order_id;       //
    int64_t md_time;         //
    int64_t input_time;      //
    int64_t insert_time;     //
    int64_t ack_time;        //
    int64_t trade_time;      //
    double total_price;      //
    VolumeType total_volume; //
    double avg_price;        //
};

struct BasketOrder { //
    PACK_DATA_BODY
    uint64_t order_id;  // 篮子单uid
    uint64_t parent_id; // 母单号

    int64_t insert_time; // 下单时间
    int64_t update_time; // 更新时间

    enums::Side side;              // 买卖方向
    enums::PriceType price_type;   // 价格类型
    enums::PriceLevel price_level; // 价格级别
    double price_offset;           // 价格偏移量

    VolumeType volume;      // 成交量
    VolumeType volume_left; // 剩余数量

    enums::BasketOrderStatus status; // 订单状态

    uint32_t source_id; // 下单方
    uint32_t dest_id;   // 下单账户

    enums::BasketOrderCalculationMode calculation_mode; // 计算方式
};

struct RequestHistoryOrder { //
    PACK_DATA_BODY
    uint64_t trigger_time; // 触发时间
    uint32_t query_num;    // 请求查询的数量
};

struct RequestHistoryTrade { //
    uint64_t trigger_time;   // 触发时间
    uint32_t query_num;      // 请求查询的数量
};

struct RequestHistoryOrderError { //
    PACK_DATA_BODY2(RequestHistoryOrderError)
    int32_t error_id;                            // 错误ID
    infra::Array<char, ERROR_MSG_LEN> error_msg; // 错误信息
    int64_t trigger_time;                        // 写入时间
};

struct RequestHistoryTradeError { //
    PACK_DATA_BODY2(RequestHistoryTradeError)
    int32_t error_id;                            // 错误ID
    infra::Array<char, ERROR_MSG_LEN> error_msg; // 错误信息
    int64_t trigger_time;                        // 写入时间
};

struct MDSubscribe {
    UNFIXED_DATA_BODY(MDSubscribe)
    uint32_t id = 0;
    std::vector<InstrumentKey> instrument_keys;
};

struct TradingStart {
    PACK_DATA_BODY2(TradingStart)
    int64_t sync_time;
};

struct TradingStop {
    PACK_DATA_BODY2(TradingStop)
    int64_t sync_time;
};

struct AccountReq {
    PACK_DATA_BODY2(AccountReq)

    enum ReqType {
        Status,
        OrderBook,
        Order,
        PositionBook,
    };
    uint64_t id;
    ReqType type;
    infra::Array<char, INSTRUMENT_ID_LEN> instrument_id;
    uint64_t target_id;  /* target id you request */
    int64_t insert_time; /* Maybe useless */
};

struct Termination {
    PACK_DATA_BODY2(Termination)
};

struct TDID {
    std::string institution;
    std::string account;
};

void order_from_input(const OrderInput &input, Order &order);
void trade_from_orderinput(const OrderInput &input, Trade &trade);

} // namespace btra