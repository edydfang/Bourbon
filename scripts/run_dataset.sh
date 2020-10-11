#!/bin/bash

cd ~/build

# Datasets (file)
for dist in linear segmented1p segmented10p normal; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/${dist}64M.txt -k 16 -v 64 -d /mnt/ssd/db_ddist_${dist} -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/${dist}64M_llsm.txt
    ./read_cold -f /mnt/db/dataset/${dist}64M.txt -k 16 -v 64 -d /mnt/ssd/db_ddist_${dist} -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/${dist}64M_llsm.txt
    ./read_cold -f /mnt/db/dataset/${dist}64M.txt -k 16 -v 64 -d /mnt/ssd/db_ddist_${dist} -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/${dist}64M_baseline.txt
done
sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_llsm.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_llsm.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/ar_baseline.txt
sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_llsm.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_llsm.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 8 -u -n 10000 -i 5 $* --change_level_load > ../evaluation/osm_baseline.txt


# Datasets (level)
for dist in linear segmented1p segmented10p normal; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/${dist}64M.txt -k 16 -v 64 -d /mnt/ssd/db_ddist_${dist} -m 7 -u -n 10000 -i 5 $* > ../evaluation/${dist}64M_llsm_level.txt
    ./read_cold -f /mnt/db/dataset/${dist}64M.txt -k 16 -v 64 -d /mnt/ssd/db_ddist_${dist} -m 7 -u -n 10000 -i 5 $* > ../evaluation/${dist}64M_llsm_level.txt
done
sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -u -n 10000 -i 5 $* > ../evaluation/ar_llsm_level.txt
./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar -m 7 -u -n 10000 -i 5 $* > ../evaluation/ar_llsm_level.txt
sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -u -n 10000 -i 5 $* > ../evaluation/osm_llsm_level.txt
./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm -m 7 -u -n 10000 -i 5 $* > ../evaluation/osm_llsm_level.txt

cd ~
python3 scripts/collect_results.py 1 > evaluation/expr_dataset.txt
cat evaluation/expr_dataset.txt
