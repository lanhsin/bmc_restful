cmake_minimum_required(VERSION 3.5)

project(cgocommon)

# Build libcgocommon static library

set(    lib_srcs
        src/error.cpp 
        src/general.cpp 
        src/sysfs.cpp 
        src/access.cpp 
        src/chips.cpp 
        src/fan.cpp 
        src/hwmon.cpp
    )

add_library(cgocommon STATIC ${lib_srcs})
target_include_directories(cgocommon PUBLIC include)
target_compile_options(cgocommon PUBLIC -Wall -std=c++11)

# Build cgocommon_test test programe

add_executable(cgocommon_test main.cpp)
target_compile_options(cgocommon_test PUBLIC -Wall -std=c++11)
target_link_options(cgocommon_test PUBLIC)
target_include_directories(cgocommon_test PUBLIC include)
target_link_libraries(cgocommon_test cgocommon -L${CMAKE_CURRENT_SOURCE_DIR}/lib)


