#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.

set -e
CURRENT_DIR=$(dirname "$0")

echo "add curl patch..."
cp -f $CURRENT_DIR/lib/curl_config_lite.h $CURRENT_DIR/lib/curl_config.h
