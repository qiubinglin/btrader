#pragma once

#include <memory>

template <typename T>
class Singleton {
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T &instance() { return *instance_ptr(); }

    static std::shared_ptr<T> &instance_ptr() {
        static std::shared_ptr<T> ptr(new T(), Singleton<T>::deleter);
        return ptr;
    }

private:
    static void deleter(T *ptr) {
        delete ptr;
        ptr = nullptr;
    }
};

#define INSTANCE(classname) Singleton<classname>::instance()