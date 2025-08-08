#include "coreagent.h"

namespace btra::gui {

CoreAgent::CoreAgent(QObject* parent) : QObject(parent) {}

CoreAgent::~CoreAgent() { StopListening(); }

void CoreAgent::init(const QString& filepath) { corecomm_.init(filepath.toStdString()); }

void CoreAgent::StartListening() {
    corecomm_.start();
    listening_th_ = std::make_unique<WorkThread>();
    listening_th_->setup([this] { this->corecomm_.listening(); });
    listening_th_->start();
}

void CoreAgent::StopListening() {
    /* Stop existing thread */
    if (listening_th_) {
        listening_th_->terminate();
    }
}

} // namespace btra::gui