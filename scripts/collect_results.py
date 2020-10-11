import os
import sys

from os.path import join

ROOT = "evaluation"
LAT_FCTR = 10000*1000*1000
OPS_FCTR = 10000*1000*1000*1000

def read_file(filename):
    lines = []
    with open(filename, 'r') as f:
        for line in f:
            if line.startswith("Timer 13 MEAN"):
                lines.append(line)

    line = lines[-1]
    time = int(line.split(",")[0].split(":")[1])
    return time


def dataset():
    print("Dataset Distibution:\n")
    datasets = ['linear64M', 'segmented1p64M', 'segmented10p64M', 'normal64M', 'ar', 'osm']
    for d in datasets:
        baseline = read_file(join(ROOT, "{}_baseline.txt".format(d)))
        llsm_f = read_file(join(ROOT, "{}_llsm.txt".format(d)))
        llsm_l = read_file(join(ROOT, "{}_llsm_level.txt".format(d)))
        print("{} baseline latency: {:.2f} microseconds".format(d, baseline/LAT_FCTR))
        print("{} llsm (file) latency: {:.2f} microseconds".format(d, llsm_f/LAT_FCTR))
        print("{} llsm (level) latency: {:.2f} microseconds".format(d, llsm_l/LAT_FCTR))
        print("")


def load_order():
    print("Load Order:\n")
    datasets = ['ar', 'osm']
    for d in datasets:
        seq_base = read_file(join(ROOT, "{}_baseline_seqload.txt".format(d)))
        seq_llsm = read_file(join(ROOT, "{}_llsm_seqload.txt".format(d)))
        rdm_base = read_file(join(ROOT, "{}_baseline_rdmload.txt".format(d)))
        rdm_llsm = read_file(join(ROOT, "{}_llsm_rdmload.txt").format(d))
        print("{} baseline seq load latency: {:.2f} microseconds".format(d, seq_base/LAT_FCTR))
        print("{} llsm seq load latency: {:.2f} microseconds".format(d, seq_llsm/LAT_FCTR))
        print("{} baseline random load latency: {:.2f} microseconds".format(d, rdm_base/LAT_FCTR))
        print("{} llsm random load latency: {:.2f} microseconds".format(d, rdm_llsm/LAT_FCTR))
        print("")


def req_dist():
    print("Request Distribution:\n")
    datasets = ['ar', 'osm']
    distributions = ['uniform', 'zipfian', 'sequential', 'hotspot', 'latest', 'exponential']
    for dist in distributions:
        for dat in datasets:
            base = read_file(join(ROOT, "{}_baseline_{}.txt".format(dat, dist)))
            llsm = read_file(join(ROOT, "{}_llsm_{}.txt".format(dat, dist)))
            print("{} baseline under {} latency: {:.2f} microseconds".format(dat, dist, base/LAT_FCTR))
            print("{} llsm under {} latency: {:.2f} microseconds".format(dat, dist, llsm/LAT_FCTR))
            print("")


def ycsb():
    print("YCSB:\n")
    workloads = ['a', 'b', 'c', 'd', 'f']
    datasets = ['ycsb', 'ar', 'osm']
    prefix = [10, 33, 20]
    for w in workloads:
        for (d, p) in zip(datasets, prefix):
            base = read_file(join(ROOT, "{}_baseline_ycsb-{}-{}-10.txt".format(d, w, p)))
            llsm = read_file(join(ROOT, "{}_llsm_ycsb-{}-{}-10.txt".format(d, w, p)))
            print("{} baseline on workload {} throughput: {:.2f} kops/sec".format(d, w, OPS_FCTR/base))
            print("{} llsm on workload {} throughput: {:.2f} kops/sec".format(d, w, OPS_FCTR/llsm))
            print("")


def sosd():
    print("SOSD:\n")
    datasets = ['books_200M', 'fb_200M', 'lognormal_200M', 'normal_200M',
                'uniform_dense_200M', 'uniform_sparse_200M']
    for d in datasets:
        base = read_file(join(ROOT, "sosd_{}_baseline.txt".format(d)))
        llsm = read_file(join(ROOT, "sosd_{}_llsm.txt".format(d)))
        print("{} baseline latency: {:.2f} microseconds".format(d, base/LAT_FCTR))
        print("{} llsm latency: {:.2f} microseconds".format(d, llsm/LAT_FCTR))
        print("")


def main():
    if len(sys.argv) != 2:
        print("Usage: prog expr_num \\in [1-5]")
    expr = int(sys.argv[1])
    if expr == 1:
        dataset()
    elif expr == 2:
        load_order()
    elif expr == 3:
        req_dist()
    elif expr == 4:
        ycsb()
    elif expr == 5:
        sosd()
    print("")


if __name__ == '__main__':
    main()
