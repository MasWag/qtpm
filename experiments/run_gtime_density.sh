#!/bin/bash

readonly RUN_TIMES=20

readonly date_str=$(date +%m%d%H%M%S)
readonly DATA_DIR=data/density
readonly LOG_DIR=log/$date_str

cd $(dirname $0)

source ./common_run.sh
