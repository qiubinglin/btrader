# Build
First install the dependencies.

## Install deps on ubuntu
### Core trading system deps
```
sudo apt install libboost-dev libfmt-dev libssl-dev libspdlog-dev python3-dev pybind11-dev
```

### GUI deps
Install Qt6.8.3:
```
cmake -GNinja       -DCMAKE_BUILD_TYPE=Release       -DQT_BUILD_EXAMPLES=OFF       -DQT_BUILD_TESTS=OFF       -DQT_FEATURE_openssl=ON -DQT_FEATURE_xcb=ON -DQT_FEATURE_xlib=ON -DQT_FEATURE_xkbcommon=ON /path-to-qt-src

cmake --build . --parallel $(nproc)

sudo cmake --install .
```

## Build
Then enter the source directory.
```
mkdir build
cd build
cmake ..
make
```

# Run
Configuration template is `core/main_cfg.tpl.json`. Setup your configuration according to the `configuration_help.md`, then run
```
./btrader --role=master --cfg=/path/to/configuration_file
```

# Requirements

## Thirdparty Libraries
```
nng 1.9.0
rxcpp 4.1.1
fmt
boost
libhv 1.3.3
libssl-dev
spdlog 1.14.1
pybind11-2.13.6
```

## Python libs
* Version python-3.10
```
pandas
matplotlib
mplfinance
jupyter notebook
tushare
```