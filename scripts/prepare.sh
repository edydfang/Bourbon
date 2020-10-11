cd ~

mkdir build
mkdir evaluation
cd ~/build
cmake ../learned-leveldb -DCMAKE_BUILD_TYPE=RELEASE -DNDEBUG_SWITCH=ON
make -j
