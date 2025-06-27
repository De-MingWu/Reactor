file(GLOB NET_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(service ${NET_SRC})

# 明确依赖 base 和 net
<<<<<<< HEAD
target_link_libraries(service base net http log)
=======
target_link_libraries(service base net http)
>>>>>>> 89bd1dea0bb30cd1ca3ca80a5acfaab6fd895d14
