const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const cors = require('cors');
const path = require('path');

// 导入C++引擎
let tradingEngine;
try {
    tradingEngine = require('../cpp/build/Release/trading_engine.node');
    tradingEngine.createEngine();
    console.log('C++ Trading Engine loaded successfully');
} catch (error) {
    console.error('Failed to load C++ Trading Engine:', error.message);
    console.log('运行 npm run build:cpp 来编译C++模块');
    process.exit(1);
}

const app = express();
const server = http.createServer(app);
const io = socketIo(server, {
    cors: {
        origin: "http://localhost:3000",
        methods: ["GET", "POST"]
    }
});

app.use(cors());
app.use(express.json());

// RESTful API 路由
app.post('/api/orders', (req, res) => {
    try {
        const { symbol, side, type, price, quantity } = req.body;
        
        if (!symbol || !side || !type || !quantity) {
            return res.status(400).json({ error: 'Missing required fields' });
        }
        
        const order = {
            symbol,
            side,
            type,
            price: price || 0,
            quantity: parseInt(quantity)
        };
        
        const orderId = tradingEngine.addOrder(order);
        
        res.json({
            success: true,
            orderId,
            message: 'Order submitted successfully'
        });
        
        // 广播订单更新
        io.emit('orderUpdate', { orderId, ...order });
        
    } catch (error) {
        console.error('Error adding order:', error);
        res.status(500).json({ error: 'Internal server error' });
    }
});

app.get('/api/orderbook/:symbol', (req, res) => {
    try {
        const { symbol } = req.params;
        const orderBook = tradingEngine.getOrderBook(symbol);
        res.json(orderBook);
    } catch (error) {
        console.error('Error getting order book:', error);
        res.status(500).json({ error: 'Internal server error' });
    }
});

app.get('/api/trades/:symbol', (req, res) => {
    try {
        const { symbol } = req.params;
        const limit = parseInt(req.query.limit) || 100;
        const trades = tradingEngine.getTrades(symbol, limit);
        res.json(trades);
    } catch (error) {
        console.error('Error getting trades:', error);
        res.status(500).json({ error: 'Internal server error' });
    }
});

app.post('/api/simulation/start', (req, res) => {
    try {
        tradingEngine.startSimulation();
        res.json({ success: true, message: 'Simulation started' });
    } catch (error) {
        console.error('Error starting simulation:', error);
        res.status(500).json({ error: 'Internal server error' });
    }
});

app.post('/api/simulation/stop', (req, res) => {
    try {
        tradingEngine.stopSimulation();
        res.json({ success: true, message: 'Simulation stopped' });
    } catch (error) {
        console.error('Error stopping simulation:', error);
        res.status(500).json({ error: 'Internal server error' });
    }
});

// WebSocket 连接处理
io.on('connection', (socket) => {
    console.log('Client connected:', socket.id);
    
    // 发送实时市场数据
    const sendMarketData = () => {
        try {
            const symbols = ['AAPL', 'GOOGL', 'TSLA'];
            const marketData = {};
            
            symbols.forEach(symbol => {
                marketData[symbol] = {
                    orderBook: tradingEngine.getOrderBook(symbol),
                    trades: tradingEngine.getTrades(symbol, 50)
                };
            });
            
            socket.emit('marketData', marketData);
        } catch (error) {
            console.error('Error sending market data:', error);
        }
    };
    
    // 立即发送一次数据
    sendMarketData();
    
    // 每秒发送市场数据更新
    const interval = setInterval(sendMarketData, 1000);
    
    socket.on('disconnect', () => {
        console.log('Client disconnected:', socket.id);
        clearInterval(interval);
    });
    
    // 处理客户端订单
    socket.on('submitOrder', (orderData) => {
        try {
            const orderId = tradingEngine.addOrder(orderData);
            socket.emit('orderConfirm', { orderId, ...orderData });
            io.emit('orderUpdate', { orderId, ...orderData });
        } catch (error) {
            socket.emit('orderError', { error: error.message });
        }
    });
});

const PORT = process.env.PORT || 8080;
server.listen(PORT, () => {
    console.log(`Trading server running on port ${PORT}`);
    console.log(`WebSocket endpoint: ws://localhost:${PORT}`);
    console.log(`REST API: http://localhost:${PORT}/api`);
});