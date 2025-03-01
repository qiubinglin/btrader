// #include "xtptrader.h"

// #include "infra/infra.h"
// #include "version.h"
// #include "xtpconvert.h"

// namespace btra::broker {

// XTPTrader::XTPTrader(const Json::json &cfg) {
//   api_ = XTP::API::TraderApi::CreateTraderApi(config_.client_id, config_.save_path.c_str());
//   api_->RegisterSpi(this);
//   api_->SubscribePublicTopic(XTP_TERT_QUICK);
//   api_->SetSoftwareVersion(SOFTWARE_VERSION);
//   api_->SetSoftwareKey(config_.software_key.c_str());
//   session_id_ = api_->Login(config_.ip.c_str(), config_.port, config_.account_id.c_str(), config_.password.c_str(),
//                             XTP_PROTOCOL_TCP);

//   if (session_id_ > 0) {
//   } else {
//     std::cout << fmt::format("Login failed [{}]: {}", api_->GetApiLastError()->error_id,
//                              api_->GetApiLastError()->error_msg)
//               << std::endl;
//   }
// }

// XTPTrader::~XTPTrader() {
//   if (api_ != nullptr) {
//     api_->Release();
//   }
// }

// bool XTPTrader::insert_order(const OrderInput &input) {
//   // INFRA_LOG_DEBUG("OrderInput: {}", input.to_string());
//   XTPOrderInsertInfo xtp_input = {};
//   // to_xtp(xtp_input, input);

//   // INFRA_LOG_DEBUG("XTPOrderInsertInfo: {}", to_string(xtp_input));
//   // uint64_t order_xtp_id = api_->InsertOrder(&xtp_input, session_id_);
//   // auto success = order_xtp_id != 0;
// }

// bool XTPTrader::cancel_order(const OrderAction &in) {}

// void XTPTrader::OnDisconnected(uint64_t session_id, int reason) {}

// void XTPTrader::OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id) {}

// void XTPTrader::OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id) {}

// void XTPTrader::OnCancelOrderError(XTPOrderCancelInfo *cancel_info, XTPRI *error_info, uint64_t session_id) {}

// void XTPTrader::OnQueryOrder(XTPQueryOrderRsp *order_info, XTPRI *error_info, int request_id, bool is_last,
//                              uint64_t session_id) {}

// void XTPTrader::OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last,
//                              uint64_t session_id) {}

// void XTPTrader::OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last,
//                                 uint64_t session_id) {}

// void XTPTrader::OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last,
//                              uint64_t session_id) {}

// }  // namespace btra::broker