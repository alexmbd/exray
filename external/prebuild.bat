@echo off

cd rl\raylib && cmake CMakeLists.txt -Bbuild -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=Release && cd build && ninja