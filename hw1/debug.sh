# cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug; cmake --build build; ./build/hw1
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_CXX_FLAGS_DEBUG="-fsanitize=undefined,address -fno-sanitize-recover=all -O0 -g"
cmake --build build
./build/hw1