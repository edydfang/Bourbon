#!/bin/bash

cd ~/build

for dist in books_200M fb_200M lognormal_200M normal_200M uniform_dense_200M uniform_sparse_200M; do
    ./read_cold -f /mnt/db/sosd/${dist}.put -k 16 -v 64 -d /mnt/ssd/db_sosd -m 7 -w > ../evaluation/sosd_${dist}_put.txt
    ./read_cold -f /mnt/db/sosd/${dist}.put -k 16 -v 64 -d /mnt/ssd/db_sosd -m 7 -u -n 10000 -i 5 --change_level_load $* > ../evaluation/sosd_${dist}_llsm.txt
    ./read_cold -f /mnt/db/sosd/${dist}.put -k 16 -v 64 -d /mnt/ssd/db_sosd -m 8 -u -n 10000 -i 5 $* > ../evaluation/sosd_${dist}_baseline.txt
done

cd ~
python3 scripts/collect_results.py 5 > evaluation/expr_sosd.txt
cat evaluation/expr_sosd.txt
