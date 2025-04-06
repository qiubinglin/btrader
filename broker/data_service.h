#pragma once

#include <vector>

#include "infra/infra.h"
#include "types.h"
#include "writer.h"

namespace btra::broker {

using DataCustomer = journal::Writer;

FORWARD_DECLARE_UPTR(DataService)
class DataService {
public:
    static DataServiceUPtr create(const std::string &institution);
    void set_customer(DataCustomer *w) { writer_ = w; }
    virtual void setup(const Json::json &cfg) = 0;
    virtual void start() = 0;
    virtual void serve() = 0;

    virtual bool subscribe(const std::vector<InstrumentKey> &instrument_keys) = 0;
    virtual bool subscribe_all() { return false; };
    virtual bool subscribe_custom(const CustomSubscribe &custom_sub) { return subscribe_all(); };
    virtual bool unsubscribe(const std::vector<InstrumentKey> &instrument_keys) = 0;

protected:
    DataCustomer *writer_ = nullptr;
};

} // namespace btra::broker