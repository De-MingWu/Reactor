file(GLOB NET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(service ${NET_SRC})

# 明确依赖 base 和 net
target_link_libraries(service base net http)
