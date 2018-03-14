add_subdirectory(lib)

file(GLOB BRIM_SRCS *.cpp)
add_executable(brim ${BRIM_SRCS})
target_link_libraries(brim brimruntime)
target_include_directories(brim PRIVATE lib)
