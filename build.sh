set -e
mkdir -p build && cd build
cmake .. && make
./test_cpu
if [ -f ./test_cuda ]; then
    ./test_cuda
fi