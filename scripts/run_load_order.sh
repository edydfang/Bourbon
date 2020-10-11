#!/bin/bash

cd ~/build

# Random vs sequential load
# AR
sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_llsm_seqload.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_llsm_seqload.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_baseline_seqload.txt

sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_llsm_rdmload.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_llsm_rdmload.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_baseline_rdmload.txt

# OSM
sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_llsm_seqload.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_llsm_seqload.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_baseline_seqload.txt

sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_llsm_rdmload.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_llsm_rdmload.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_baseline_rdmload.txt

cd ~
python3 scripts/collect_results.py 2 > evaluation/expr_load_order.txt
cat evaluation/expr_load_order.txt
