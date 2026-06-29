cmake_minimum_required(VERSION 3.20)
project(muduoHTTP)

# C++标准，muduo最低C++11，你用C++17没问题
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 你的muduo源码目录
set(MUDUO_ROOT "/home/xinyu/muduo-master")
# 编译出来的库文件夹
set(MUDUO_LIB ${MUDUO_ROOT}/build/lib)

# 头文件路径：必须填上层目录，才能 #include <muduo/net/xxx>
include_directories(${MUDUO_ROOT})
# 告诉cmake去哪里找muduo静态库
link_directories(${MUDUO_LIB})

# 可执行程序
add_executable(muduoHTTP main.cpp)

# 链接顺序有讲究：muduo_net在前，muduo_base在后，再系统依赖
target_link_libraries(muduoHTTP
        muduo_http
        muduo_net
        muduo_base
        pthread
        boost_system
        boost_thread
        ssl
        crypto
        z
)