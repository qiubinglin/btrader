#pragma once

#include "event.h"
#include "infra/infra.h"

namespace btra {

struct NanoMsgJson : Event {
    explicit NanoMsgJson(const std::string &msg) : binding_(Json::json::parse(msg)), msg_(msg){};

    [[nodiscard]] int64_t gen_time() const override { return get_meta<int64_t>("gen_time", 0); }

    [[nodiscard]] int64_t trigger_time() const override { return get_meta<int64_t>("trigger_time", 0); }

    [[nodiscard]] int32_t msg_type() const override { return get_meta<int32_t>("msg_type", 0); }

    [[nodiscard]] uint32_t source() const override { return get_meta<uint32_t>("source", 0); }

    [[nodiscard]] uint32_t dest() const override { return get_meta<uint32_t>("dest", 0); }

    [[nodiscard]] uint32_t data_length() const override { return binding_.size(); }

    [[nodiscard]] const void *data_address() const override {
        if (binding_.find("data") == binding_.end()) {
            INFRA_LOG_ERROR("nanomsg event has no data {}", msg_);
            return &binding_;
        }
        return &binding_["data"];
    }

    [[nodiscard]] const char *data_as_bytes() const override { return msg_.c_str(); }

    [[nodiscard]] std::string data_as_string() const override { return binding_["data"].dump(); }

    [[nodiscard]] std::string to_string() const override { return msg_; }

private:
    const Json::json binding_;
    const std::string msg_;

    template <typename T> [[nodiscard]] T get_meta(const std::string &name, T default_value) const {
        if (binding_.find(name) == binding_.end()) {
            return default_value;
        }
        T value = binding_[name];
        return value;
    }
};

DECLARE_SPTR(NanoMsgJson)

} // namespace btra