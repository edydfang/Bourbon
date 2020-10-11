#!/bin/bash

cd ~

scripts/run_dataset.sh
scripts/run_load_order.sh
scripts/run_request_distribution.sh
scripts/run_ycsb.sh
scripts/run_sosd.sh
