#include "mpr/frozen_trie.hpp"

#include <iostream>
#include <vector>
#include <ranges>

auto main(int argc, char** argv) -> int
{
    auto x = mpr::make_trie<"test", "xx">();

    std::cout
        << std::boolalpha << x.contains_prefix("te") << "\n"
        << std::boolalpha << x.contains_prefix("tes") << "\n"
        << std::boolalpha << x.contains_prefix("test") << "\n"
        << std::boolalpha << x.contains_prefix("tests") << "\n"
        << "contains word:\n"
        << std::boolalpha << x.contains_word("") << "\n"
        << std::boolalpha << x.contains_word("t") << "\n"
        << std::boolalpha << x.contains_word("te") << "\n"
        << std::boolalpha << x.contains_word("tes") << "\n"
        << std::boolalpha << x.contains_word("test") << "\n"
        << std::boolalpha << x.contains_word("tests") << "\n"
        << "size: \n"
        << x.size() << "\n";
    ;

    mpr::Node node{};

    node.append('a', 12);
    node.append('h', 11);
    node.append('g', 8);

    for (auto i : node)
    {
        std::cout << i << ", ";
    }
    std::cout << "\n";
}
