//
// Created by edydfang on 11/1/20.
//
#include <iostream>
#include <fstream>
#include <random>
#include "cxxopts.hpp"
using std::string;
using std::ofstream;
using std::cout;

int gen_dbkey(const string &key_distribution, uint32_t num_keys, uint32_t gap, string output_path) {
  ofstream fd;
  fd.open(output_path);
  if(key_distribution.compare("linear") == 0) {
    for (int i = 0; i < num_keys; ++i)
      fd << i << "\n";
  } else if (key_distribution.compare("segmented1p") == 0 || key_distribution.compare("segmented10p") == 0) {
    int16_t seg_len;
    if (key_distribution.compare("segmented1p") == 0) {
      seg_len = 100;
    }else{
      seg_len = 10;
    }
    uint32_t x = 0;
    for (int i = 0; i < num_keys; ++i) {
      if (i%seg_len == 0){
        x += gap;
      }
      fd << x << "\n";
    }
  } else if (key_distribution.compare("normal") == 0 ) {
    std::normal_distribution<> normal_dis{0,1};
    std::unordered_set<uint32_t> keys;
    std::default_random_engine e1(0), e2(255), e3(0);
    while (keys.size()<num_keys) {
      double new_num = normal_dis(e2);
      if(new_num > 3 || new_num < -3) {
        continue;
      }
      keys.insert(new_num*1e14L + 4e15L);
    }
    for (const auto&elem: keys) {
      fd << elem << "\n";
    }
  }
  fd.close();
  return 0;
}

int main(int argc, char *argv[]) {
  bool db_key;
  string key_distribution, output_path;
  uint32_t num_keys, gap;
  cxxopts::Options commandline_options("bourbon db and trace generator", "bourbon db and trace generator");
  commandline_options.add_options()
      ("k,dbkey", "generate file for db key", cxxopts::value<bool>(db_key)->default_value("true"))
      ("d,keydis", "distribution for the key [linear, segmented1p, segmented10p, normal]", cxxopts::value<string>(key_distribution)->default_value("linear"))
      ("o,output", "path to the output file", cxxopts::value<string>(output_path)->default_value("./output.txt"))
      ("g, gap", "gap,between key segments", cxxopts::value<uint32_t>(gap)->default_value("100"))
  ("n", "number of keys", cxxopts::value<uint32_t>(num_keys)->default_value("1000000"));
  auto result = commandline_options.parse(argc, argv);
  if (result.count("help")) {
    printf("%s", commandline_options.help().c_str());
    exit(0);
  }

  srand(0);
  if(db_key) {
    gen_dbkey(key_distribution, num_keys, gap, output_path);
  }
  //cout << "test\n";
}

