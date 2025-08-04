#pragma once

#include <QtCore/QThread>
#include <functional>

class WorkThread : private QThread {
public:
    WorkThread() : QThread() {}
    ~WorkThread() { delete m_task; }

    template <typename F, typename... ARGS>
    void setup(F &&f, ARGS &&...args) {
        auto task = std::bind(std::forward<F>(f), std::forward<ARGS>(args)...);
        m_task = new std::function<void()>([task] { task(); });
    }

    void start();
    void terminate();
    void quit();
    bool wait();

private:
    void run() override { (*m_task)(); }

    std::function<void()> *m_task;
};