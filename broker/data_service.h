#pragma once

#include <vector>

#include "core/types.h"
#include "infra/infra.h"
#include "journal/writer.h"

namespace btra::broker {

using DataCustomer = journal::Writer;

FORWARD_DECLARE_UPTR(DataService)
/**
 * @brief Base class to provide data service.
 *
 */
class DataService {
public:
    /**
     * @brief Factory function to create different data service according to institution.
     *
     * @param institution Institution name such as binance.
     * @param backtest_data_type
     * @return DataServiceUPtr
     */
    static DataServiceUPtr create(const std::string &institution);

    virtual ~DataService() = default;

    /**
     * @brief Set the customer object which enjoy the data service.
     *
     * @param w
     */
    void set_customer(DataCustomer *w) { writer_ = w; }

    /**
     * @brief Setup config.
     *
     * @param cfg
     */
    virtual void setup(const Json::json &cfg) = 0;

    /**
     * @brief Start the service
     *
     */
    virtual void start() = 0;

    /**
     * @brief Stop service.
     *
     */
    virtual void stop() = 0;

    /**
     * @brief Get the state of broker.
     *
     * @return enums::BrokerState
     */
    virtual enums::BrokerState get_state() const { return state_; }

    /**
     * @brief Subscribe instruments.
     *
     * @param instrument_keys
     * @return true
     * @return false
     */
    virtual bool subscribe(const std::vector<InstrumentKey> &instrument_keys) = 0;

    /**
     * @brief Subscribe all instruments that adapt to the institution.
     *
     * @return true
     * @return false
     */
    virtual bool subscribe_all() { return false; };

    /**
     * @brief To do: figure out the usage.
     *
     * @param custom_sub
     * @return true
     * @return false
     */
    virtual bool subscribe_custom(const CustomSubscribe &custom_sub) { return subscribe_all(); };

    /**
     * @brief Unsubscribe instruments.
     *
     * @param instrument_keys
     * @return true
     * @return false
     */
    virtual bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) = 0;

protected:
    /* writer_ will consume the data. */
    DataCustomer *writer_ = nullptr;
    enums::BrokerState state_ = enums::BrokerState::DisConnected;
};

} // namespace btra::broker