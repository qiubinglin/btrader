#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <QList>
#include <QObject>

#include "model/order.h"

class OrderManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QList<QObject*> orders READ get_orders NOTIFY orders_changed)

public:
    explicit OrderManager(QObject* parent = nullptr);
    ~OrderManager();

    OrderManager(const OrderManager&) = delete;
    OrderManager& operator=(const OrderManager&) = delete;

    QList<QObject*> get_orders() const;

    Q_INVOKABLE void add_order(const QString& product_name, int quantity, double price, const QString& customer_name);
    Q_INVOKABLE void update_order_status(const QString& order_id, const QString& status);
    Q_INVOKABLE void remove_order(const QString& order_id);

signals:
    void orders_changed();

private:
    QString generate_order_id() const;
    QList<Order*> orders_;
};

#endif // ORDER_MANAGER_H