#include "./prefix_map.hpp"
#include <iostream>
#include <functional>

auto main(int argc, char** argv) -> int
{
    mpr::prefix_map<std::function<void()>> pmap;

    pmap.insert("a",      [](){ std::cout << "a\n"; });
    pmap.insert("ahoj",   [](){ std::cout << "ahoj\n"; });
    pmap.insert("ahojky", [](){ std::cout << "ahojky\n"; });
    pmap.insert("aha",    [](){ std::cout << "aha\n"; });

    pmap.at("ahoj")();
    pmap.at("ahojky")();
    pmap.at("aha")();
    pmap.at("ahojk")();

    std::cout << "-------\n";

    for (auto& func : pmap)
    {
        func();
    }

    return 0;
}
