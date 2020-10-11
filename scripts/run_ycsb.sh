#!/bin/bash

cd ~/build

# YCSB A B D F
rm -rf *
cmake ../learned-leveldb -DCMAKE_BUILD_TYPE=RELEASE -DNDEBUG_SWITCH=ON -DLEVEL_SWITCH=ON -DINTERNAL_TIMER_SWITCH=ON
make -j

# AR
for dist in ycsb-a-33-10 ycsb-b-33-10 ycsb-d-33-10 ycsb-f-33-10; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 --change_level_load $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 33000000 -i 5 > ../evaluation/ar_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 8 -u -n 10000 $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 33000000 -i 5 > ../evaluation/ar_baseline_${dist}.txt
done

# OSM
for dist in ycsb-a-20-10 ycsb-b-20-10 ycsb-d-20-10 ycsb-f-20-10; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 --change_level_load $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 20000000 -i 5 > ../evaluation/osm_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 8 -u -n 10000 $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 20000000 -i 5 > ../evaluation/osm_baseline_${dist}.txt
done

# Default traces
for dist in ycsb-a-10-10 ycsb-b-10-10 ycsb-d-10-10 ycsb-f-10-10; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -k 16 -v 64 -d /mnt/ssd/ycsb_default -m 7 -u -n 10000 --change_level_load -i 5 $* --YCSB /mnt/db/ycsb/${dist}.txt > ../evaluation/ycsb_llsm_${dist}.txt
    ./read_cold -k 16 -v 64 -d /mnt/ssd/ycsb_default -m 8 -u -n 10000 --change_level_load -i 5 $* --YCSB /mnt/db/ycsb/${dist}.txt > ../evaluation/ycsb_baseline_${dist}.txt
done

# C
rm -rf *
cmake ../learned-leveldb -DCMAKE_BUILD_TYPE=RELEASE -DNDEBUG_SWITCH=ON
make -j

# AR
for dist in ycsb-c-33-10; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 --change_level_load $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 33000000 -i 5 > ../evaluation/ar_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 --change_level_load $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 33000000 -i 5 > ../evaluation/ar_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 8 -u -n 10000 $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 33000000 -i 5 > ../evaluation/ar_baseline_${dist}.txt
done

# OSM
for dist in ycsb-c-20-10; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 --change_level_load $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 20000000 -i 5 > ../evaluation/osm_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 --change_level_load $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 20000000 -i 5 > ../evaluation/osm_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 8 -u -n 10000 $* --YCSB /mnt/db/ycsb/${dist}.txt --insert 20000000 -i 5 > ../evaluation/osm_baseline_${dist}.txt
done

# Default traces
for dist in ycsb-c-10-10; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -k 16 -v 64 -d /mnt/ssd/ycsb_default -m 7 -u -n 10000 --change_level_load -i 5 $* --YCSB /mnt/db/ycsb/${dist}.txt > ../evaluation/ycsb_llsm_${dist}.txt
    ./read_cold -k 16 -v 64 -d /mnt/ssd/ycsb_default -m 7 -u -n 10000 --change_level_load -i 5 $* --YCSB /mnt/db/ycsb/${dist}.txt > ../evaluation/ycsb_llsm_${dist}.txt
    ./read_cold -k 16 -v 64 -d /mnt/ssd/ycsb_default -m 8 -u -n 10000 --change_level_load -i 5 $* --YCSB /mnt/db/ycsb/${dist}.txt > ../evaluation/ycsb_baseline_${dist}.txt
done

cd ~
python3 scripts/collect_results.py 4 > evaluation/expr_ycsb.txt
cat evaluation/expr_ycsb.txt
