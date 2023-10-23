#pragma once

#include <string_view>
#include <algorithm>
#include <optional>
#include <cstdint>
#include <vector>
#include <array>


namespace mpr
{
    class Node
    {
    public:
        constexpr
        Node() { std::ranges::fill(children, none); }

        constexpr
        void append(char symbol, size_t index)
        {
            last = children[symbol] = index;
            child_count++;
        }

        constexpr
        void terminate()
        { terminal = true; }

        constexpr
        auto has_child() const -> bool
        { return last != Node::none; }

        constexpr
        auto has_child(char symbol) const -> bool
        { return children[symbol] != Node::none; }

        constexpr
        auto is_terminated() const -> bool
        { return terminal; }

        constexpr
        auto get_child_id() const -> size_t
        { return last; }

        constexpr
        auto get_child_id(char c) const -> size_t
        { return children[c]; }

        constexpr
        auto get_child_count() const -> size_t
        { return child_count; }

        static constexpr size_t none = 0;

    private:
        std::array<size_t, 256> children{};
        uint8_t child_count = 0;
        size_t last = none;
        bool terminal = false;
    };

    template <size_t Size>
    class NodePool
    {
    public:
        constexpr
        NodePool(std::array<Node, Size> pool)
            : pool(pool)
        {}

        constexpr
        auto front() const -> const Node&
        { return pool.front(); }

        constexpr
        auto operator[](size_t index) const -> const Node&
        { return pool[index]; }

    private:
        std::array<Node, Size> pool;
    };

    template <size_t WordCount, size_t NodeCount>
    class Trie
    {
    public:
        constexpr
        Trie(NodePool<NodeCount> pool)
            : pool(pool)
        {}

        constexpr
        auto contains_word(std::string_view word) const -> bool
        {
            if (auto node = search(word))
            {
                return node.value()->is_terminated();
            }

            return false;
        }

        constexpr
        auto contains_prefix(std::string_view word) const -> bool
        { return search(word).has_value(); }

        constexpr
        auto empty() const -> bool
        { return pool.front().has_child(); }

        constexpr
        auto size() const -> size_t
        { return WordCount; }

        constexpr auto begin() const { return; }
        constexpr auto end() const { return; }
        constexpr auto cbegin() const { return; }
        constexpr auto cend() const { return; }

    private:
        constexpr
        auto search(std::string_view word) const -> std::optional<const Node*>
        {
            auto* node = &pool.front(); 

            for (const auto c : word)
            {
                if (!node->has_child(c))
                {
                    return std::nullopt;
                }

                node = &pool[node->get_child_id(c)];
            }

            return node;
        }

        NodePool<NodeCount> pool;
    };

    namespace detail
    {
        constexpr
        void pool_insert(std::vector<Node>& pool, std::string_view word)
        {
            auto* current = &pool.front();

            for (auto i = pool.size(); const auto c : word)
            {
                if (current->has_child(c))
                {
                    current = &pool[current->get_child_id(c)];
                    continue;
                }

                current->append(c, i);
                pool.emplace_back();
                current = &pool[i++];
            }

            current->terminate();
        }

        template <typename ...Ts>
        requires (std::convertible_to<Ts, std::string_view> && ...)
        constexpr
        auto make_pool(Ts ...words) -> std::vector<Node>
        {
            auto nodes = std::vector<Node>(1);

            (pool_insert(nodes, words), ...);

            return nodes;
        }

        template <size_t N>
        struct string_literal
        {
            constexpr
            string_literal(const char (&str)[N])
                : str(std::to_array(str))
            {}

            constexpr
            operator std::string_view() const
            { return { str.data(), N - 1 }; }

            std::array<char, N> str;
        };

        template <size_t N, std::ranges::input_range R>
        constexpr
        auto to_array(R&& range)
        {
            using value_type = std::ranges::range_value_t<decltype(range)>;

            auto result = std::array<value_type, N>{};
            std::ranges::copy(range, result.begin());

            return result;
        }
    }

    template <detail::string_literal ...Words>
    consteval
    auto make_trie()
    {
        using namespace detail;

        constexpr auto nodeCount = make_pool(Words...).size();
        const     auto nodeArray = to_array<nodeCount>(make_pool(Words...));

        return Trie<sizeof...(Words), nodeCount>(NodePool(nodeArray));
    }

}
