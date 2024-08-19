# !bin/bash

max_threads=$(cat /proc/cpuinfo | grep "processor" | wc -l)
cd build
cmake ..
make -j "$max_threads"
make install


