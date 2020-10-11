#!/bin/bash

cd ~/build

# Request distributions
for dist in uniform zipfian sequential hotspot latest exponential; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 -i 5 --change_level_load $* --distribution /mnt/db/distribution/${dist}-33-10.txt > ../evaluation/ar_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 7 -u -n 10000 -i 5 --change_level_load $* --distribution /mnt/db/distribution/${dist}-33-10.txt > ../evaluation/ar_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/amazon_reviews.txt -k 16 -v 64 -d /mnt/ssd/db_ar_random -m 8 -u -n 10000 -i 5 $* --distribution /mnt/db/distribution/${dist}-33-10.txt > ../evaluation/ar_baseline_${dist}.txt
done

for dist in uniform zipfian sequential hotspot latest exponential; do
    sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 -i 5 --change_level_load $* --distribution /mnt/db/distribution/${dist}-20-10.txt > ../evaluation/osm_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 7 -u -n 10000 -i 5 --change_level_load $* --distribution /mnt/db/distribution/${dist}-20-10.txt > ../evaluation/osm_llsm_${dist}.txt
    ./read_cold -f /mnt/db/dataset/osm_ny.txt -k 16 -v 64 -d /mnt/ssd/db_osm_random -m 8 -u -n 10000 -i 5 $* --distribution /mnt/db/distribution/${dist}-20-10.txt > ../evaluation/osm_baseline_${dist}.txt
done

cd ~
python3 scripts/collect_results.py 3 > evaluation/expr_request_dist.txt
cat evaluation/expr_request_dist.txt
