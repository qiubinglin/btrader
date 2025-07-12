#include "order.h"

Order::Order(const QString& order_id, const QString& product_name, int quantity, double price,
             const QString& customer_name, QObject* parent)
    : QObject(parent),
      order_id_(order_id),
      product_name_(product_name),
      quantity_(quantity),
      price_(price),
      customer_name_(customer_name),
      order_time_(QDateTime::currentDateTime()),
      status_("Pending") {}

void Order::set_status(const QString& status) {
    if (status_ != status) {
        status_ = status;
        emit status_changed();
    }
}