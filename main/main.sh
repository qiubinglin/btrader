#!/bin/bash

cfg_file=$1

./btrader --role=md --cfg=$cfg_file &
./btrader --role=td --cfg=$cfg_file &
./btrader --role=cp --cfg=$cfg_file