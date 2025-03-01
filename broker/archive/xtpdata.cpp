#include "xtpdata.h"

namespace btra {

// XTPData::~XTPData() {
//   if (api_ != nullptr) {
//     api_->Release();
//   }
// }

// void MarketDataXTP::OnDisconnected(int reason) {
//   SPDLOG_ERROR("disconnected with reason {}", reason);
//   update_broker_state(BrokerState::DisConnected);
// }

// void MarketDataXTP::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last) {
//   if (error_info != nullptr && error_info->error_id != 0) {
//     SPDLOG_ERROR("failed to subscribe level 1, [{}] {}", error_info->error_id, error_info->error_msg);
//   }
// }

// void MarketDataXTP::OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last) {
//   if (error_info != nullptr && error_info->error_id != 0) {
//     SPDLOG_ERROR("failed to subscribe level 2, [{}] {}", error_info->error_id, error_info->error_msg);
//   }
// }

// void MarketDataXTP::OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info) {
//   if (error_info != nullptr && error_info->error_id != 0) {
//     SPDLOG_ERROR("failed to subscribe level 2 all, [{}] {}", error_info->error_id, error_info->error_msg);
//   }
// }

// void MarketDataXTP::OnQueryAllTickers(XTPQSI *ticker_info, XTPRI *error_info, bool is_last) {
//   if (nullptr != error_info && error_info->error_id != 0) {
//     SPDLOG_ERROR("error_id : {} , error_msg : {}", error_info->error_id, error_info->error_msg);
//     return;
//   }

//   if (nullptr == ticker_info) {
//     SPDLOG_ERROR("ticker_info is nullptr");
//     return;
//   }

//   Instrument &instrument = public_wirter_->open_data<Instrument>(0);
//   from_xtp(ticker_info, instrument);
//   public_wirter_->close_data();
//   SPDLOG_TRACE("instrument {}", instrument.to_string());
// }

// void MarketDataXTP::OnDepthMarketData(XTPMD *market_data, int64_t *bid1_qty, int32_t bid1_count, int32_t max_bid1_count,
//                                       int64_t *ask1_qty, int32_t ask1_count, int32_t max_ask1_count) {
//   Quote &quote = public_wirter_->open_data<Quote>(0);
//   from_xtp(*market_data, quote);
//   public_wirter_->close_data();
// }

// void MarketDataXTP::OnTickByTick(XTPTBT *tbt_data) {
//   if (tbt_data->type == XTP_TBT_ENTRUST) {
//     Entrust &entrust = get_writer(level2_tick_band_uid_)->open_data<Entrust>(0);
//     from_xtp(*tbt_data, entrust);
//     get_writer(level2_tick_band_uid_)->close_data();
//   } else if (tbt_data->type == XTP_TBT_TRADE) {
//     Transaction &transaction = get_writer(level2_tick_band_uid_)->open_data<Transaction>(0);
//     from_xtp(*tbt_data, transaction);
//     get_writer(level2_tick_band_uid_)->close_data();
//   }
// }

// void MarketDataXTP::OnQueryAllTickersFullInfo(XTPQFI *ticker_info, XTPRI *error_info, bool is_last) {
//   if (nullptr != error_info && error_info->error_id != 0) {
//     SPDLOG_INFO("error_id : {} , error_msg : {}", error_info->error_id, error_info->error_msg);
//     return;
//   }

//   if (nullptr == ticker_info) {
//     SPDLOG_ERROR("ticker_info is nullptr");
//     return;
//   }

//   Instrument &instrument = public_wirter_->open_data<Instrument>(0);
//   strcpy(instrument.instrument_id, ticker_info->ticker);
//   if (ticker_info->exchange_id == 1) {
//     instrument.exchange_id = EXCHANGE_SSE;
//   } else if (ticker_info->exchange_id == 2) {
//     instrument.exchange_id = EXCHANGE_SZE;
//   }

//   memcpy(instrument.product_id, ticker_info->ticker_name, strlen(ticker_info->ticker_name));
//   instrument.instrument_type = get_instrument_type(instrument.exchange_id, instrument.instrument_id);
//   public_wirter_->close_data();
//   SPDLOG_TRACE("instrument {}", instrument.to_string());
// }

}  // namespace btra