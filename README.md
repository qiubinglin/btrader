# BTrader 🚀

> **High-performance trading research, execution, and visualization toolkit**

[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-blue.svg)](https://github.com/your-repo/btrader)
[![License](https://img.shields.io/badge/license-TBD-yellow.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.8.3-green.svg)](https://www.qt.io/)

BTrader is a comprehensive trading platform built with a high-performance C++ core engine featuring market data processing, strategy execution, and trading operations. Enhanced with a modern Qt6/QML GUI (TradeView) for advanced charting and order flow visualization.

## ✨ Key Features

- **🚀 High Performance**: C++ core engine optimized for speed and efficiency
- **📊 Advanced GUI**: Modern Qt6/QML interface with real-time charts and order flow
- **🔄 Multi-Mode Trading**: Comprehensive backtesting, live trading, and simulation
- **🌐 Cross-Platform**: Linux-first with macOS support via Orbstack/XQuartz
- **🔌 Extensible**: Plugin architecture for custom strategies and data sources

## 🏗️ Architecture Overview

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Market Data  │    │  Strategy      │    │   Trading      │
│     Engine     │◄──►│    Engine      │◄──►│    Engine      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────┐
                    │   Event Bus     │
                    │   & Storage     │
                    └─────────────────┘
                                 │
                    ┌─────────────────┐
                    │  Qt6/QML GUI    │
                    │   (TradeView)   │
                    └─────────────────┘
```

## 📋 Supported Brokers

| Broker/Exchange | Status | Features |
|----------------|---------|----------|
| **Binance** | ✅ Supported | Spot trading, Futures, API integration |

> **Note**: More brokers will be added in future releases. For specific broker requirements, please check our [documentation](docs/) or create an issue.

## 🚀 Quick Start

### Prerequisites

#### System Dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y \
  libboost-dev libfmt-dev libssl-dev libspdlog-dev \
  python3-dev pybind11-dev
```

#### Qt6 Dependencies (6.8.3+)
```bash
# Option 1: Build from source (recommended for latest features)
cmake -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DQT_BUILD_EXAMPLES=OFF \
  -DQT_BUILD_TESTS=OFF \
  -DQT_FEATURE_openssl=ON \
  -DQT_FEATURE_xcb=ON -DQT_FEATURE_xlib=ON -DQT_FEATURE_xkbcommon=ON \
  /path-to-qt-src

cmake --build . --parallel $(nproc)
sudo cmake --install .

# Option 2: Use distribution packages (if sufficiently new)
# sudo apt install qt6-base-dev qt6-declarative-dev
```

> **💡 Tip**: Ensure Qt6 bin directory is on PATH: `export PATH=/opt/Qt/6.8.3/gcc_64/bin:$PATH`

### Build & Install

```bash
# Clone and build
git clone https://github.com/your-repo/btrader.git
cd btrader

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc)
```

### Run Examples

#### Backtesting
```bash
cd build
./btrader --role=master --cfg=./config-tpl/backtest.json
```

#### Simulated Trading
```bash
cd build
./btrader --role=master --cfg=./config-tpl/brokersim.json
```

#### Live Trading
```bash
cd build
./btrader --role=master --cfg=/path/to/your/config.json
```

## 🖥️ GUI (TradeView)

Launch the graphical interface for advanced charting and order management:

```bash
# See tradeview/README.md for detailed setup
cd tradeview
./tradeview
```

**Features:**
- 📈 Real-time candlestick charts with volume
- 📊 Order flow visualization
- 📋 Order book display
- 🎨 Modern, responsive interface

## 📚 Dependencies

### Core Libraries
| Library | Version | Purpose |
|---------|---------|---------|
| **nng** | 1.9.0 | Networking |
| **rxcpp** | 4.1.1 | Reactive programming |
| **fmt** | Latest | String formatting |
| **boost** | Latest | C++ utilities |
| **libhv** | 1.3.3 | HTTP/WebSocket |
| **OpenSSL** | Latest | Cryptography |
| **spdlog** | 1.14.1 | Logging |
| **pybind11** | 2.13.6 | Python bindings |

### Python Dependencies
```bash
pip install pandas matplotlib mplfinance jupyter tushare
```

**Requirements:**
- Python 3.10+

## ⚙️ Configuration

BTrader uses JSON configuration files for different trading modes:

- **Backtesting**: `config-tpl/backtest.json`
- **Simulation**: `config-tpl/brokersim.json`
- **Live Trading**: Custom configuration based on your broker setup

See `configuration_help.md` for detailed parameter documentation.

## 🛠️ Development

### Build Targets
- **Core Engine**: `btrader` binary
- **GUI Application**: `tradeview` (Qt6/QML)

### Performance Tips
- Use Release builds: `-DCMAKE_BUILD_TYPE=Release`
- GPU acceleration: Ensure proper driver support
- Software fallback: `QT_QUICK_BACKEND=software`
- Optimize logging: Disable verbose output in production
- Data handling: Use batched updates for large datasets

## 🔧 Troubleshooting

### Common Issues

#### Qt Display Connection
```bash
# macOS + Orbstack
xhost + 127.0.0.1

# Linux Wayland
export QT_QPA_PLATFORM=xcb
```

#### OpenSSL Features
Rebuild Qt with OpenSSL support:
```bash
cmake -DQT_FEATURE_openssl=ON /path-to-qt-src
```

#### Performance Issues
- Check GPU driver support
- Use software rendering if needed: `export QT_QUICK_BACKEND=software`
- Monitor system resources during large operations

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

**Quick Start:**
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure code follows our style guide
5. Write comments and strings in English
6. Submit a pull request

## 📄 License

**TBD** - License information will be added here.

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/your-repo/btrader/issues)
- **Documentation**: [docs/](docs/)
- **Discussions**: [GitHub Discussions](https://github.com/your-repo/btrader/discussions)

---

<div align="center">

**Made with ❤️ by the BTrader Team**

[![GitHub stars](https://img.shields.io/github/stars/your-repo/btrader?style=social)](https://github.com/your-repo/btrader)
[![GitHub forks](https://img.shields.io/github/forks/your-repo/btrader?style=social)](https://github.com/your-repo/btrader)

</div>