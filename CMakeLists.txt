cmake_minimum_required(VERSION 3.20)
project(muduoHTTP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MUDUO_ROOT "/home/xinyu/muduo-master")
set(MUDUO_LIB ${MUDUO_ROOT}/build/lib)

include_directories(${MUDUO_ROOT})
include_directories(${PROJECT_SOURCE_DIR}/include)

link_directories(${MUDUO_LIB})

set(SOURCES
        main.cpp
        src/http/HttpRequest.cpp
        src/http/HttpResponse.cpp
        src/http/HttpContext.cpp
        src/http/HttpServer.cpp
        src/router/Router.cpp
)

add_executable(muduoHTTP ${SOURCES})

target_link_libraries(muduoHTTP
        muduo_net
        muduo_base
        pthread
        boost_system
        boost_thread
        ssl
        crypto
        z
)