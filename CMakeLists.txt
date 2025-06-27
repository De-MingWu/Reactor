cmake_minimum_required(VERSION 3.28)
<<<<<<< HEAD
project(Reactor)

# 设置c++标准
set(CMAKE_CXX_STANDARD 11)

# 查找 nlohmann-json 库的头文件
include_directories(/usr/include)
=======
project(ReactorServer)

set(CMAKE_CXX_STANDARD 14)
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14

# 添加 include 子目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/base
    ${PROJECT_SOURCE_DIR}/include/net
    ${PROJECT_SOURCE_DIR}/include/http
<<<<<<< HEAD
    ${PROJECT_SOURCE_DIR}/include/log
=======
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
    ${PROJECT_SOURCE_DIR}/include/service
)

# 添加子目录模块
add_subdirectory(src/base)
add_subdirectory(src/net)
add_subdirectory(src/http)
<<<<<<< HEAD
add_subdirectory(src/log)
add_subdirectory(src/service)

# 主程序入口（根据需要添加多个）
add_executable(test src/app/main.cpp)
target_link_libraries(test base net http log service pthread)
=======
add_subdirectory(src/service)

# 主程序入口（根据需要添加多个）
add_executable(httptest src/httptest.cpp)
target_link_libraries(httptest base net http service pthread)
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
