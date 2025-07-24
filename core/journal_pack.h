#pragma once

#include "core/journal/reader.h"
#include "core/journal/writer.h"
#include "core/observe_helper.h"
#include "infra/json.h"

namespace btra {

/**
 * @brief JourReadPack is for comminication between btrader and other process.
 *
 */
struct JourReadPack {
    ObserveHelper observe_helper;
    journal::ReaderUPtr reader;

    void init(const Json::json &json);
};

/**
 * @brief JourWritePack is for comminication between btrader and other process.
 *
 */
struct JourWritePack {
    WriterMap writers;

    void init(const Json::json &json);
};

} // namespace btra
