enable_testing()
add_executable(test main.cpp)
target_link_libraries(test PRIVATE frozen_trie)
add_test(NAME test COMMAND test)
