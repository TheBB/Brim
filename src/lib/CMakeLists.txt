file(GLOB LIBBRIM_SRCS *.cpp)

add_library(brimruntime SHARED ${LIBBRIM_SRCS})
target_include_directories(brimruntime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(brimruntime PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
