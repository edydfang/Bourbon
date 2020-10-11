#!/bin/bash

cd ~/build

# AR
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -w > ../evaluation/ar_put.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -w -l 3 > ../evaluation/ar_random_put.txt

# OSM
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -w > ../evaluation/osm_put.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -w -l 3 > ../evaluation/osm_random_put.txt

# Synthetic
for dist in linear segmented1p segmented10p normal; do
    ./read_cold -f /mnt/db/dataset/${dist}64M.txt -k 16 -v 64 -d /mnt/ssd/db_ddist_${dist} -m 7 -w > ../evaluation/${dist}64M_put_r.txt
done

# YCSB Default tarces
./read_cold -f /mnt/db/dataset/ycsb_default.put -k 16 -v 64 -d /mnt/ssd/ycsb_default -m 7 -w -l 3 > ../evaluation/ycsb_default_put.txt
