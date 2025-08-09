#include "coreagent.h"

#include <QDebug>
#include <qtmetamacros.h>

#include <stdexcept>

namespace btra::gui {

CoreAgent::CoreAgent(QObject* parent) : QObject(parent) {}

CoreAgent::~CoreAgent() { StopListening(); }

void CoreAgent::init(const QString& filepath) {
    try {
        corecomm_.init(filepath.toStdString());
    } catch (const std::runtime_error& e) {
        qDebug() << "CoreAgent::init error: " << e.what();
    }
}

void CoreAgent::StartListening() {
    corecomm_.start();
    listening_th_ = std::make_unique<WorkThread>();
    listening_th_->setup([this] {
        while (this->corecomm_.get_status() >= 0) {
            this->corecomm_.wait_msg();
            emit this->incomming_message();
        }
    });
    listening_th_->start();
}

void CoreAgent::StopListening() {
    /* Stop existing thread */
    if (listening_th_) {
        corecomm_.terminate();
        listening_th_->wait();
    }
}

} // namespace btra::gui