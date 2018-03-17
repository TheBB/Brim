set(BRIM_TEST_SOURCES
  test.cpp
  lexer.cpp
  parser.cpp
  object-ctor.cpp
  object-tostr.cpp
)

set(BRIM_TEST_TAGS
  lexer
  object-ctor
  object-tostr
  parser
)

add_executable(test-brim ${BRIM_TEST_SOURCES})
target_link_libraries(test-brim brimruntime)
target_include_directories(test-brim PRIVATE "${CMAKE_SOURCE_DIR}/src/lib")

foreach(tag ${BRIM_TEST_TAGS})
  add_test(NAME ${tag} COMMAND $<TARGET_FILE:test-brim> "[${tag}]")
endforeach(tag)
