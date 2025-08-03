/**
 * @file orderbookmodel.h
 * @brief 订单簿模型头文件
 * @author BTrader Team
 * @date 2024
 * 
 * 该文件定义了订单簿数据模型，用于管理和展示金融交易中的买卖盘数据。
 * 支持实时数据更新、价格计算、深度分析等功能，适用于交易系统的订单簿显示。
 */

#ifndef ORDERBOOKMODEL_H
#define ORDERBOOKMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QDateTime>
#include <QVector>

namespace btra::gui {

/**
 * @brief 订单簿档位数据结构
 * 
 * 包含订单簿中单个价格档位的信息，用于表示买卖盘中的每个价格层级。
 * 该结构体定义了订单簿中每个档位的基本属性，包括价格、挂单量、订单数量等。
 * 
 * 典型用法：
 * @code
 * OrderBookLevel bid(50000.0, 1000, 5, "bid");  // 创建买单档位
 * OrderBookLevel ask(50001.0, 800, 3, "ask");   // 创建卖单档位
 * @endcode
 */
struct OrderBookLevel {
    double price;      ///< 价格档位，表示该档位的价格水平（如：50000.0）
    qint64 volume;     ///< 挂单量，表示该价格档位的总挂单数量（如：1000）
    int orderCount;    ///< 订单数量，表示该价格档位的订单笔数（如：5）
    QString side;      ///< 买卖方向，标识该档位是买单("bid")还是卖单("ask")
    
    /**
     * @brief 默认构造函数
     * 
     * 初始化所有成员变量为0，创建一个空的订单簿档位。
     * 适用于需要后续填充数据的场景。
     */
    OrderBookLevel() : price(0), volume(0), orderCount(0) {}
    
    /**
     * @brief 带参数构造函数
     * @param p 价格档位，该档位的价格水平（必须大于0）
     * @param vol 挂单量，该价格档位的总挂单数量（必须大于等于0）
     * @param count 订单数量，该价格档位的订单笔数（必须大于等于0）
     * @param s 买卖方向，必须是"bid"(买单)或"ask"(卖单)
     * 
     * 创建一个完整的订单簿档位，所有参数都会被验证。
     */
    OrderBookLevel(double p, qint64 vol, int count, const QString& s)
        : price(p), volume(vol), orderCount(count), side(s) {}
};

/**
 * @brief 订单簿模型（OrderBookModel）
 * 
 * 该类继承自QAbstractListModel，用于管理和展示订单簿（买卖盘）数据，适用于Qt的Model/View架构，便于在QML等界面中绑定和显示。
 * 
 * 主要功能包括：
 * - 存储和维护买单（bids）、卖单（asks）以及合并后的订单簿数据
 * - 支持设置和获取最大显示档位数，控制数据量大小
 * - 提供获取最佳买价、最佳卖价、买卖价差、中间价等常用行情指标的方法
 * - 支持QML调用的接口（Q_INVOKABLE），便于前端动态刷新和交互
 * - 支持订单簿的清空、更新、数据获取等操作
 * - 通过信号（signals）通知界面数据变更，便于界面联动刷新
 * - 提供累计成交量、深度百分比等衍生数据的计算
 * 
 * 典型应用场景：Level2盘口、深度图、买卖盘列表等金融行情展示。
 * 
 * 数据流向：外部数据源 -> OrderBookModel -> QML界面显示
 * 
 * 使用示例：
 * @code
 * // 在QML中使用
 * ListView {
 *     model: orderBookModel
 *     delegate: Rectangle {
 *         Text { text: model.price }
 *         Text { text: model.volume }
 *     }
 * }
 * 
 * // 在C++中使用
 * OrderBookModel* model = new OrderBookModel();
 * model->updateOrderBook(bids, asks);
 * double spread = model->getSpread();
 * @endcode
 */
class OrderBookModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief 数据角色枚举
     * 
     * 定义了QML中访问订单簿数据的各种角色，每个角色对应一个数据属性。
     * 这些角色用于在QML中通过model.roleName的方式访问具体数据。
     * 
     * 角色说明：
     * - PriceRole: 档位价格，用于显示价格信息
     * - VolumeRole: 挂单量，用于显示成交量信息
     * - OrderCountRole: 订单数量，用于显示订单笔数
     * - SideRole: 买卖方向，用于区分买单和卖单
     * - IsBidRole/IsAskRole: 布尔值，用于条件判断
     * - CumulativeVolumeRole: 累计成交量，用于深度图显示
     * - DepthPercentageRole: 深度百分比，用于可视化分析
     */
    enum Roles {
        PriceRole = Qt::UserRole + 1,        ///< 价格角色，对应档位价格
        VolumeRole,                          ///< 成交量角色，对应档位挂单量
        OrderCountRole,                      ///< 订单数量角色，对应档位订单笔数
        SideRole,                            ///< 买卖方向角色，对应档位类型
        IsBidRole,                           ///< 是否买单角色，布尔值
        IsAskRole,                           ///< 是否卖单角色，布尔值
        CumulativeVolumeRole,                ///< 累计成交量角色，从该档位开始的累计挂单量
        DepthPercentageRole                  ///< 深度百分比角色，该档位占总深度的百分比
    };

    /**
     * @brief 构造函数
     * @param parent 父对象指针，用于Qt对象树管理，默认为nullptr
     * 
     * 初始化订单簿模型，设置默认的最大档位数和初始状态。
     */
    explicit OrderBookModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    /**
     * @brief 获取模型行数
     * @param parent 父索引，对于列表模型通常为无效索引
     * @return 返回合并后订单簿的总档位数
     * 
     * 该方法被Qt框架调用，用于确定ListView等控件需要显示的行数。
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    /**
     * @brief 获取指定索引和角色的数据
     * @param index 数据索引，包含行号信息
     * @param role 数据角色，对应Roles枚举中的值
     * @return 返回对应的数据值，如果索引无效则返回QVariant()
     * 
     * 该方法被Qt框架调用，用于获取ListView等控件需要显示的具体数据。
     * 根据不同的角色返回不同类型的数据。
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    /**
     * @brief 获取角色名称映射
     * @return 返回角色ID到角色名称的哈希映射
     * 
     * 该方法被Qt框架调用，用于建立角色ID和角色名称之间的映射关系。
     * QML通过这个映射可以访问具体的数据属性。
     */
    QHash<int, QByteArray> roleNames() const override;

    // Public methods - QML可调用的接口
    /**
     * @brief 更新订单簿数据
     * @param bids 买单数据向量，按价格降序排列（最高价在前）
     * @param asks 卖单数据向量，按价格升序排列（最低价在前）
     * 
     * 该方法会触发模型数据更新，并发送相应的信号通知界面刷新。
     * 买单和卖单数据会被分别存储，然后合并生成完整的订单簿视图。
     * 
     * 使用示例：
     * @code
     * QVector<OrderBookLevel> bids, asks;
     * // 填充买单和卖单数据
     * model->updateOrderBook(bids, asks);
     * @endcode
     */
    Q_INVOKABLE void updateOrderBook(const QVector<OrderBookLevel> &bids, const QVector<OrderBookLevel> &asks);
    
    /**
     * @brief 清空订单簿数据
     * 
     * 清空所有买单、卖单和合并后的订单簿数据，并重置最佳价格。
     * 该方法会触发模型重置，并发送dataChanged信号通知界面刷新。
     */
    Q_INVOKABLE void clear();
    
    /**
     * @brief 设置最大显示档位数
     * @param levels 最大档位数，用于控制显示的数据量（必须大于0）
     * 
     * 设置订单簿显示的最大档位数，超过此数量的档位将被忽略。
     * 这有助于控制内存使用和界面性能。
     */
    Q_INVOKABLE void setMaxLevels(int levels);
    
    /**
     * @brief 获取最大显示档位数
     * @return 返回当前设置的最大档位数
     * 
     * 获取当前配置的最大显示档位数，用于了解数据限制。
     */
    Q_INVOKABLE int getMaxLevels() const;
    
    /**
     * @brief 获取最佳买价
     * @return 返回当前最高买价，如果没有买单则返回0
     * 
     * 最佳买价是买单中的最高价格，通常用于计算价差和中间价。
     * 该值会在数据更新时自动重新计算。
     */
    Q_INVOKABLE double getBestBid() const;
    
    /**
     * @brief 获取最佳卖价
     * @return 返回当前最低卖价，如果没有卖单则返回0
     * 
     * 最佳卖价是卖单中的最低价格，通常用于计算价差和中间价。
     * 该值会在数据更新时自动重新计算。
     */
    Q_INVOKABLE double getBestAsk() const;
    
    /**
     * @brief 获取买卖价差
     * @return 返回最佳卖价与最佳买价的差值
     * 
     * 价差是衡量市场流动性的重要指标，价差越小表示流动性越好。
     * 计算公式：spread = bestAsk - bestBid
     */
    Q_INVOKABLE double getSpread() const;
    
    /**
     * @brief 获取中间价
     * @return 返回最佳买价和最佳卖价的平均值
     * 
     * 中间价通常用作参考价格，用于计算相对价格变化。
     * 计算公式：midPrice = (bestBid + bestAsk) / 2
     */
    Q_INVOKABLE double getMidPrice() const;
    
    /**
     * @brief 获取买单数据
     * @return 返回所有买单数据的QVariantList格式
     * 
     * 返回所有买单档位的数据，按价格降序排列。
     * 返回的数据可以直接在QML中使用。
     */
    Q_INVOKABLE QVariantList getBids() const;
    
    /**
     * @brief 获取卖单数据
     * @return 返回所有卖单数据的QVariantList格式
     * 
     * 返回所有卖单档位的数据，按价格升序排列。
     * 返回的数据可以直接在QML中使用。
     */
    Q_INVOKABLE QVariantList getAsks() const;
    
    /**
     * @brief 获取合并后的订单簿数据
     * @return 返回按价格排序的完整订单簿数据
     * 
     * 返回合并后的完整订单簿数据，包含买单和卖单，按价格排序。
     * 返回的数据可以直接在QML中使用，适用于深度图等显示。
     */
    Q_INVOKABLE QVariantList getOrderBook() const;

signals:
    /**
     * @brief 数据变化信号
     * 
     * 当订单簿数据发生任何变化时发出，用于通知界面刷新。
     * 该信号会被发送到所有连接的槽函数。
     */
    void dataChanged();
    
    /**
     * @brief 订单簿更新信号
     * 
     * 当订单簿数据被更新时发出，用于通知界面重新渲染。
     * 该信号在updateOrderBook方法被调用后发出。
     */
    void orderBookUpdated();
    
    /**
     * @brief 最佳买价变化信号
     * @param price 新的最佳买价
     * 
     * 当最佳买价发生变化时发出，用于实时更新买价显示。
     * 该信号在数据更新导致最佳买价变化时发出。
     */
    void bestBidChanged(double price);
    
    /**
     * @brief 最佳卖价变化信号
     * @param price 新的最佳卖价
     * 
     * 当最佳卖价发生变化时发出，用于实时更新卖价显示。
     * 该信号在数据更新导致最佳卖价变化时发出。
     */
    void bestAskChanged(double price);
    
    /**
     * @brief 价差变化信号
     * @param spread 新的买卖价差
     * 
     * 当买卖价差发生变化时发出，用于实时更新价差显示。
     * 该信号在最佳买价或最佳卖价变化导致价差变化时发出。
     */
    void spreadChanged(double spread);

private:
    QVector<OrderBookLevel> m_bids;          ///< 买单数据容器，存储所有买单档位，按价格降序排列
    QVector<OrderBookLevel> m_asks;          ///< 卖单数据容器，存储所有卖单档位，按价格升序排列
    QVector<OrderBookLevel> m_combinedOrderBook; ///< 合并后的订单簿数据，按价格排序，用于ListView显示
    int m_maxLevels;                         ///< 最大显示档位数，用于控制数据量大小，默认为20
    double m_bestBid;                        ///< 当前最佳买价（最高买价），用于快速访问
    double m_bestAsk;                        ///< 当前最佳卖价（最低卖价），用于快速访问

    /**
     * @brief 更新合并后的订单簿
     * 
     * 将买单和卖单数据合并并按价格排序，生成完整的订单簿视图。
     * 该方法在数据更新时被调用，确保合并数据的正确性。
     * 
     * 合并逻辑：
     * 1. 将买单按价格降序排列
     * 2. 将卖单按价格升序排列
     * 3. 合并两个数组，保持价格排序
     * 4. 限制档位数量不超过m_maxLevels
     */
    void updateCombinedOrderBook();
    
    /**
     * @brief 更新最佳价格
     * 
     * 根据当前买单和卖单数据，计算并更新最佳买价和最佳卖价。
     * 该方法在数据更新时被调用，确保价格信息的准确性。
     * 
     * 计算逻辑：
     * - 最佳买价：买单中的最高价格
     * - 最佳卖价：卖单中的最低价格
     * - 如果没有相应数据，则设置为0
     */
    void updateBestPrices();
    
    /**
     * @brief 计算累计成交量
     * @param index 档位索引，指向m_combinedOrderBook中的位置
     * @return 返回从该档位开始到最高/最低价的累计挂单量
     * 
     * 该方法用于计算深度图中的累计成交量，帮助分析市场深度。
     * 对于买单，累计从该档位到最高价的成交量。
     * 对于卖单，累计从该档位到最低价的成交量。
     */
    qint64 calculateCumulativeVolume(int index) const;
    
    /**
     * @brief 计算深度百分比
     * @param index 档位索引，指向m_combinedOrderBook中的位置
     * @return 返回该档位占总深度的百分比（0.0-100.0）
     * 
     * 该方法用于计算深度图中的百分比，帮助可视化市场深度分布。
     * 百分比基于累计成交量计算，用于显示相对深度。
     */
    double calculateDepthPercentage(int index) const;
};

}

#endif // ORDERBOOKMODEL_H 