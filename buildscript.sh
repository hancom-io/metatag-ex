sudo apt-get install libxerces-c-dev 3.2.2
sudo apt-get install libarchive-dev
sudo apt-get install rapidjson-dev
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make