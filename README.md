### BTrader

High-performance trading research and visualization toolkit. Core engine in C++ with real-time data pipelines, plus a Qt6/QML GUI (TradeView) for charts and order flow.

---

## Overview
- Core engine: networking, storage, event bus, models
- GUI (TradeView): candlestick/volume, order flow, order book
- Cross-platform: Linux-first; macOS via Orbstack/XQuartz

## Prerequisites (Ubuntu)
Install system dependencies first.

### Core engine deps
```bash
sudo apt update
sudo apt install -y \
  libboost-dev libfmt-dev libssl-dev libspdlog-dev \
  python3-dev pybind11-dev
```

### GUI deps (Qt 6.8.3)
Build and install Qt 6.8.3 (or install from your distro if sufficiently new). Example from source:
```bash
cmake -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DQT_BUILD_EXAMPLES=OFF \
  -DQT_BUILD_TESTS=OFF \
  -DQT_FEATURE_openssl=ON \
  -DQT_FEATURE_xcb=ON -DQT_FEATURE_xlib=ON -DQT_FEATURE_xkbcommon=ON \
  /path-to-qt-src

cmake --build . --parallel $(nproc)
sudo cmake --install .
```

> Tip: Ensure the installed Qt6 bin directory is on PATH (e.g., `export PATH=/opt/Qt/6.8.3/gcc_64/bin:$PATH`).

## Build
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Build targets
- Core binary: `btrader`
- GUI app: part of TradeView; see `tradeview/README.md` for environment notes

## Run (Core)
Prepare a configuration file from the template `core/main_cfg.tpl.json`. Adjust parameters as documented in `configuration_help.md`.

```bash
./btrader --role=master --cfg=/path/to/config.json
```

## Run (GUI / TradeView)
See `tradeview/README.md` for display backend configuration (X11/XQuartz/Orbstack) and environment variables. Typical environment for Orbstack/macOS is provided there.

## Requirements matrix

### Third-party libraries
```
nng 1.9.0
rxcpp 4.1.1
fmt
boost
libhv 1.3.3
libssl-dev
spdlog 1.14.1
pybind11 2.13.6
```

### Python (for research and notebooks)
- Python 3.10
```bash
pip install pandas matplotlib mplfinance jupyter tushare
```

## Developer notes
- Use Release builds for runtime performance (`-DCMAKE_BUILD_TYPE=Release`).
- GUI performance tips:
  - Ensure GPU/driver support or fall back to software rendering (`QT_QUICK_BACKEND=software`).
  - Disable verbose logging unless needed.
- Large data: prefer batched updates to models.

## Troubleshooting
- Qt cannot connect to display:
  - On macOS + Orbstack: see `tradeview/README.md` (XQuartz, `xhost + 127.0.0.1`).
  - On Linux Wayland: try `QT_QPA_PLATFORM=xcb`.
- Missing OpenSSL features in Qt: rebuild Qt with `-DQT_FEATURE_openssl=ON`.

## License
TBD. Place your project license here.

## Contributing
Issues and PRs are welcome. Please format code consistently and write comments/strings in English.