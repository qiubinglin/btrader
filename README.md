# Build
First install the dependencies.

On ubuntu:
```
sudo apt install libboost-dev libfmt-dev libssl-dev libspdlog-dev
```

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