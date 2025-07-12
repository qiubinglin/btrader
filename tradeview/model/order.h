#ifndef ORDER_H
#define ORDER_H

#include <QDateTime>
#include <QObject>
#include <QString>

class Order : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString order_id READ order_id CONSTANT)
    Q_PROPERTY(QString product_name READ product_name CONSTANT)
    Q_PROPERTY(int quantity READ quantity CONSTANT)
    Q_PROPERTY(double price READ price CONSTANT)
    Q_PROPERTY(QString customer_name READ customer_name CONSTANT)
    Q_PROPERTY(QDateTime order_time READ order_time CONSTANT)
    Q_PROPERTY(QString status READ status WRITE set_status NOTIFY status_changed)

public:
    explicit Order(const QString& order_id, const QString& product_name, int quantity, double price,
                   const QString& customer_name, QObject* parent = nullptr);

    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

    QString order_id() const { return order_id_; }
    QString product_name() const { return product_name_; }
    int quantity() const { return quantity_; }
    double price() const { return price_; }
    QString customer_name() const { return customer_name_; }
    QDateTime order_time() const { return order_time_; }
    QString status() const { return status_; }
    void set_status(const QString& status);

signals:
    void status_changed();

private:
    const QString order_id_;
    const QString product_name_;
    const int quantity_;
    const double price_;
    const QString customer_name_;
    const QDateTime order_time_;
    QString status_;
};

#endif // ORDER_H