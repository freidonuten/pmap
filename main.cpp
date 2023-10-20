#include "prefix_map.hpp"

#include <iostream>


auto main(int argc, char** argv) -> int
{
    constexpr auto x = mpr::make_trie<"test", "xx">();

    std::cout
        << std::boolalpha << x.contains_prefix("te") << "\n"
        << std::boolalpha << x.contains_prefix("tes") << "\n"
        << std::boolalpha << x.contains_prefix("test") << "\n"
        << std::boolalpha << x.contains_prefix("tests") << "\n"
    ;
}
