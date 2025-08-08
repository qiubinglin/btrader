#pragma once

#include <QObject>
#include <memory>

#include "control/workthread.h"
#include "core/corecomm.h"

namespace btra::gui {

/**
 * @brief As a QObject, it is a bridge between core and model, also between core and ui.
 *
 */
class CoreAgent : public QObject {
    Q_OBJECT
public:
    explicit CoreAgent(QObject* parent = nullptr);
    ~CoreAgent();

    void init(const QString &filepath);

    void StartListening();
    void StopListening();

    CoreComm& GetCoreComm() { return corecomm_; }

signals:
    void incomming_message();

private:
    CoreComm corecomm_;
    std::unique_ptr<WorkThread> listening_th_;
};

} // namespace btra::gui