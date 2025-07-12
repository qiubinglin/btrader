#include "order_manager.h"

#include <QUuid>
#include <algorithm>

OrderManager::OrderManager(QObject* parent) : QObject(parent) {}

OrderManager::~OrderManager() {
    qDeleteAll(orders_);
    orders_.clear();
}

QList<QObject*> OrderManager::get_orders() const {
    QList<QObject*> result;
    for (Order* order : orders_) {
        result.append(order);
    }
    return result;
}

void OrderManager::add_order(const QString& product_name, int quantity, double price, const QString& customer_name) {
    QString order_id = generate_order_id();
    Order* order = new Order(order_id, product_name, quantity, price, customer_name, this);
    orders_.append(order);
    emit orders_changed();
}

void OrderManager::update_order_status(const QString& order_id, const QString& status) {
    auto it = std::find_if(orders_.begin(), orders_.end(),
                           [&order_id](Order* order) { return order->order_id() == order_id; });
    if (it != orders_.end()) {
        (*it)->set_status(status);
        emit orders_changed();
    }
}

void OrderManager::remove_order(const QString& order_id) {
    auto it = std::find_if(orders_.begin(), orders_.end(),
                           [&order_id](Order* order) { return order->order_id() == order_id; });
    if (it != orders_.end()) {
        delete *it;
        orders_.erase(it);
        emit orders_changed();
    }
}

QString OrderManager::generate_order_id() const { return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8); }