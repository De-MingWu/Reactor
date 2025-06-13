cmake_minimum_required(VERSION 3.28)
project(ReactorServer)

set(CMAKE_CXX_STANDARD 14)

# 添加 include 子目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/base
    ${PROJECT_SOURCE_DIR}/include/net
    ${PROJECT_SOURCE_DIR}/include/http
    ${PROJECT_SOURCE_DIR}/include/service
)

# 添加子目录模块
add_subdirectory(src/base)
add_subdirectory(src/net)
add_subdirectory(src/http)
add_subdirectory(src/service)

# 主程序入口（根据需要添加多个）
add_executable(httptest src/httptest.cpp)
target_link_libraries(httptest base net http service pthread)
