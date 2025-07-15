#pragma once

#include <stdexcept>

template <typename T, typename... ARGS>
class MemberFuncCallback {
public:
    typedef void (T::*func_type)(ARGS);
    MemberFuncCallback(T& obj, func_type func) : obj_(obj), func_(func) {}

    void operator()(ARGS&&... args) const {
        try {
            (obj_.*func_)(args);
        } catch (const std::exception &e) {
            // error
        }
    }

private:
    T& obj_;
    func_type func_;
};
