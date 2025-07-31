import React, { useState, useEffect } from 'react';
import io from 'socket.io-client';
import axios from 'axios';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';
import { TrendingUp, TrendingDown, Play, Pause, RotateCcw } from 'lucide-react';
import './App.css';

const API_BASE = 'http://localhost:8080/api';
const SOCKET_URL = 'http://localhost:8080';

function App() {
  const [socket, setSocket] = useState(null);
  const [marketData, setMarketData] = useState({});
  const [selectedSymbol, setSelectedSymbol] = useState('AAPL');
  const [orders, setOrders] = useState([]);
  const [isSimulationRunning, setIsSimulationRunning] = useState(false);
  const [priceHistory, setPriceHistory] = useState({});
  
  // 连接WebSocket
  useEffect(() => {
    const newSocket = io(SOCKET_URL);
    setSocket(newSocket);
    
    newSocket.on('connect', () => {
      console.log('Connected to server');
    });
    
    newSocket.on('marketData', (data) => {
      setMarketData(data);
      
      // 更新价格历史
      Object.keys(data).forEach(symbol => {
        const orderBook = data[symbol].orderBook;
        if (orderBook.bids && orderBook.bids.length > 0) {
          const currentPrice = orderBook.bids[0].price;
          const timestamp = new Date().toLocaleTimeString();
          
          setPriceHistory(prev => ({
            ...prev,
            [symbol]: [
              ...(prev[symbol] || []).slice(-49),
              { time: timestamp, price: currentPrice, timestamp: orderBook.timestamp }
            ]
          }));
        }
      });
    });
    
    newSocket.on('orderUpdate', (order) => {
      setOrders(prev => [order, ...prev.slice(0, 19)]);
    });
    
    newSocket.on('orderConfirm', (order) => {
      console.log('Order confirmed:', order);
    });
    
    newSocket.on('orderError', (error) => {
      console.error('Order error:', error);
      alert('订单错误: ' + error.error);
    });
    
    return () => {
      newSocket.close();
    };
  }, []);
  
  // 订单簿组件
  const OrderBook = ({ symbol }) => {
    const data = marketData[symbol];
    if (!data || !data.orderBook) return <div>Loading...</div>;
    
    const { orderBook } = data;
    
    return (
      <div className="order-book">
        <h3>订单簿 - {symbol}</h3>
        <div className="order-book-content">
          <div className="bids">
            <h4 className="buy-header">买盘 (Bids)</h4>
            {orderBook.bids?.slice(0, 8).map((bid, index) => (
              <div key={index} className="order-level bid">
                <span className="price">{bid.price.toFixed(2)}</span>
                <span className="quantity">{bid.quantity}</span>
                <span className="orders">{bid.orders}</span>
              </div>
            ))}
          </div>
          <div className="asks">
            <h4 className="sell-header">卖盘 (Asks)</h4>
            {orderBook.asks?.slice(0, 8).map((ask, index) => (
              <div key={index} className="order-level ask">
                <span className="price">{ask.price.toFixed(2)}</span>
                <span className="quantity">{ask.quantity}</span>
                <span className="orders">{ask.orders}</span>
              </div>
            ))}
          </div>
        </div>
      </div>
    );
  };
  
  // 价格图表组件
  const PriceChart = ({ symbol }) => {
    const data = priceHistory[symbol] || [];
    
    return (
      <div className="price-chart">
        <h3>价格走势 - {symbol}</h3>
        <ResponsiveContainer width="100%" height={300}>
          <LineChart data={data}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="time" />
            <YAxis domain={['dataMin - 1', 'dataMax + 1']} />
            <Tooltip />
            <Line type="monotone" dataKey="price" stroke="#3B82F6" strokeWidth={2} dot={false} />
          </LineChart>
        </ResponsiveContainer>
      </div>
    );
  };
  
  // 成交记录组件
  const TradeHistory = ({ symbol }) => {
    const data = marketData[symbol];
    if (!data || !data.trades) return <div>Loading...</div>;
    
    return (
      <div className="trade-history">
        <h3>成交记录 - {symbol}</h3>
        <div className="trades-list">
          {data.trades.slice(0, 15).map((trade) => (
            <div key={trade.id} className={`trade-item ${trade.side}`}>
              <span className="time">{new Date(trade.timestamp).toLocaleTimeString()}</span>
              <span className="price">{trade.price.toFixed(2)}</span>
              <span className="quantity">{trade.quantity}</span>
              <span className="side">{trade.side === 'buy' ? '买' : '卖'}</span>
            </div>
          ))}
        </div>
      </div>
    );
  };
  
  // 交易面板组件
  const TradingPanel = () => {
    const [orderForm, setOrderForm] = useState({
      symbol: selectedSymbol,
      side: 'buy',
      type: 'market',
      price: '',
      quantity: ''
    });
    
    const handleInputChange = (e) => {
      const { name, value } = e.target;
      setOrderForm(prev => ({ ...prev, [name]: value }));
    };
    
    const submitOrder = async () => {
      if (!orderForm.quantity) {
        alert('请输入数量');
        return;
      }
      
      try {
        const orderData = {
          ...orderForm,
          symbol: selectedSymbol,
          quantity: parseInt(orderForm.quantity),
          price: orderForm.type === 'limit' ? parseFloat(orderForm.price) : 0
        };
        
        const response = await axios.post(`${API_BASE}/orders`, orderData);
        
        if (response.data.success) {
          alert('订单提交成功！订单ID: ' + response.data.orderId);
          setOrderForm({ ...orderForm, quantity: '', price: '' });
        }
      } catch (error) {
        console.error('Submit order error:', error);
        alert('订单提交失败: ' + (error.response?.data?.error || error.message));
      }
    };
    
    return (
      <div className="trading-panel">
        <h3>交易面板</h3>
        <div className="form-group">
          <label>股票代码</label>
          <select value={selectedSymbol} onChange={(e) => setSelectedSymbol(e.target.value)}>
            <option value="AAPL">AAPL</option>
            <option value="GOOGL">GOOGL</option>
            <option value="TSLA">TSLA</option>
          </select>
        </div>
        
        <div className="form-group">
          <label>订单类型</label>
          <select name="type" value={orderForm.type} onChange={handleInputChange}>
            <option value="market">市价单</option>
            <option value="limit">限价单</option>
          </select>
        </div>
        
        <div className="side-buttons">
          <button 
            className={`side-btn buy ${orderForm.side === 'buy' ? 'active' : ''}`}
            onClick={() => setOrderForm(prev => ({ ...prev, side: 'buy' }))}
          >
            买入
          </button>
          <button 
            className={`side-btn sell ${orderForm.side === 'sell' ? 'active' : ''}`}
            onClick={() => setOrderForm(prev => ({ ...prev, side: 'sell' }))}
          >
            卖出
          </button>
        </div>
        
        {orderForm.type === 'limit' && (
          <div className="form-group">
            <label>价格</label>
            <input
              type="number"
              name="price"
              step="0.01"
              value={orderForm.price}
              onChange={handleInputChange}
              placeholder="输入价格"
            />
          </div>
        )}
        
        <div className="form-group">
          <label>数量</label>
          <input
            type="number"
            name="quantity"
            value={orderForm.quantity}
            onChange={handleInputChange}
            placeholder="输入数量"
          />
        </div>
        
        <button className="submit-order" onClick={submitOrder}>
          提交订单
        </button>
      </div>
    );
  };
  
  // 控制面板
  const ControlPanel = () => {
    const startSimulation = async () => {
      try {
        await axios.post(`${API_BASE}/simulation/start`);
        setIsSimulationRunning(true);
      } catch (error) {
        console.error('Start simulation error:', error);
      }
    };
    
    const stopSimulation = async () => {
      try {
        await axios.post(`${API_BASE}/simulation/stop`);
        setIsSimulationRunning(false);
      } catch (error) {
        console.error('Stop simulation error:', error);
      }
    };
    
    const resetData = () => {
      setPriceHistory({});
      setOrders([]);
    };
    
    return (
      <div className="control-panel">
        <h3>控制面板</h3>
        <div className="control-buttons">
          <button 
            onClick={isSimulationRunning ? stopSimulation : startSimulation}
            className={isSimulationRunning ? 'stop' : 'start'}
          >
            {isSimulationRunning ? <Pause /> : <Play />}
            {isSimulationRunning ? '暂停模拟' : '开始模拟'}
          </button>
          <button onClick={resetData} className="reset">
            <RotateCcw />
            重置数据
          </button>
        </div>
      </div>
    );
  };
  
  // 订单历史
  const OrderHistory = () => (
    <div className="order-history">
      <h3>我的订单</h3>
      <div className="orders-list">
        {orders.length === 0 ? (
          <p>暂无订单</p>
        ) : (
          orders.map((order, index) => (
            <div key={index} className={`order-item ${order.side}`}>
              <div className="order-header">
                <span className={`side ${order.side}`}>
                  {order.side === 'buy' ? '买入' : '卖出'}
                </span>
                <span className="symbol">{order.symbol}</span>
              </div>
              <div className="order-details">
                <span>{order.type === 'market' ? '市价' : `限价 ${order.price}`}</span>
                <span>数量: {order.quantity}</span>
              </div>
              {order.orderId && <div className="order-id">ID: {order.orderId}</div>}
            </div>
          ))
        )}
      </div>
    </div>
  );
  
  // 获取当前价格
  const getCurrentPrice = (symbol) => {
    const data = marketData[symbol];
    if (data && data.orderBook && data.orderBook.bids && data.orderBook.bids.length > 0) {
      return data.orderBook.bids[0].price;
    }
    return 0;
  };
  
  return (
    <div className="App">
      <header className="app-header">
        <h1>订单流交易系统</h1>
        <div className="current-price">
          <span className="symbol">{selectedSymbol}</span>
          <span className="price">${getCurrentPrice(selectedSymbol).toFixed(2)}</span>
        </div>
      </header>
      
      <div className="main-content">
        <div className="left-panel">
          <ControlPanel />
          <TradingPanel />
          <OrderHistory />
        </div>
        
        <div className="center-panel">
          <div className="chart-container">
            <PriceChart symbol={selectedSymbol} />
          </div>
          <div className="market-data">
            <OrderBook symbol={selectedSymbol} />
          </div>
        </div>
        
        <div className="right-panel">
          <TradeHistory symbol={selectedSymbol} />
        </div>
      </div>
    </div>
  );
}

export default App;