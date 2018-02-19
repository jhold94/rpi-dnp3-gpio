cmake_minimum_required (VERSION 2.8)
project (ts-dnp3)
set(TS_DNP3_VERSION 1.0.0)

# other options off-by-default that you can enable
option(WERROR "Set all warnings to errors" OFF)

set(CMAKE_CXX_FLAGS "-fno-tree-cselim -Wall -mcpu=arm9 -std=c++11")
# different release and debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

include_directories(deps/inih)
  
# ---- executable ----
file(GLOB_RECURSE main_SRC src/*.cpp src/*.h)
add_executable(ts-dnp3 deps/inih/ini.c deps/inih/ini.h ${main_SRC} src/GPIOCommandHandler.h src/gpio.cpp src/analog.cpp src/modbusCommandHandler.cpp)
target_link_libraries (ts-dnp3 asiodnp3 asiopal opendnp3 openpal pthread modbus)
