#pragma once

#include "core/journal/reader.h"
#include "core/journal/writer.h"
#include "core/observe_helper.h"
#include "infra/json.h"

namespace btra {

/**
 * @brief JourCommData is for comminication between btrader and other process.
 *
 */
struct JourCommData {
    ObserveHelper observe_helper;
    journal::ReaderUPtr reader;
    WriterMap writers;
    unsigned md_account_count;

    journal::JourIndicator interrupt_sender;

    void init(const Json::json &json);
};

} // namespace btra
