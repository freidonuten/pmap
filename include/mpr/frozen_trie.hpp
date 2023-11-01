#pragma once

#include <string_view>
#include <algorithm>
#include <optional>
#include <cstdint>
#include <vector>
#include <array>
#include <stack>
#include <string>


namespace mpr
{
    class Node
    {
    public:
        static constexpr size_t none = 0;
        static constexpr size_t child_limit = 256; // all possible bytes

        constexpr
        Node() noexcept { std::ranges::fill(children, none); }

        constexpr
        void append(char symbol, size_t index)
        {
            last = children.at(symbol) = index;
            child_count++;
        }

        constexpr
        void terminate()
        { terminal = true; }

        [[nodiscard]] constexpr
        auto has_child() const -> bool
        { return last != Node::none; }

        [[nodiscard]] constexpr
        auto has_child(char symbol) const -> bool
        { return children.at(symbol) != Node::none; }

        [[nodiscard]] constexpr
        auto is_terminated() const -> bool
        { return terminal; }

        [[nodiscard]] constexpr
        auto get_child_id() const -> size_t
        { return last; }

        [[nodiscard]] constexpr
        auto get_child_id(char symbol) const -> size_t
        { return children.at(symbol); }

        [[nodiscard]] constexpr
        auto get_child_count() const -> size_t
        { return child_count; }

        class Iterator
        {
            using wrapee_t = std::array<size_t, child_limit>::iterator;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = size_t;
            using difference_type = ptrdiff_t;
            using pointer = size_t*;
            using reference = size_t&;

            constexpr Iterator() = delete;
            constexpr Iterator(wrapee_t begin, wrapee_t end)
                : begin(begin)
                , current(begin)
                , end(end)
            {
                if (is_incrementable())
                {
                    operator++();
                }
            }

            constexpr
            auto operator==(const Iterator& other) const -> bool
            { return current == other.current; }

            constexpr
            auto operator!=(const Iterator& other) const -> bool
            { return !(*this == other); }

            constexpr
            auto operator*() const -> reference
            { return *current; }

            constexpr
            auto operator->() const -> pointer
            { return &*current; }

            constexpr
            auto operator++() -> Iterator
            {
                ++current;

                while (is_incrementable())
                {
                    ++current;
                }

                return *this;
            }

            constexpr
            auto operator++(int) -> Iterator
            {
                auto tmp = *this;
                operator++();
                return tmp;
            }

            constexpr
            auto operator--() -> Iterator
            {
                --current;

                while (is_decrementable())
                {
                    --current;
                }

                return *this;
            }

            constexpr
            auto operator--(int) -> Iterator
            {
                auto tmp = *this;
                operator--();
                return tmp;
            }

        private:
            constexpr
            auto is_incrementable() -> bool
            { return *current == none && current != end; }

            constexpr
            auto is_decrementable() -> bool
            { return *current == none && current != begin; }

            wrapee_t begin;
            wrapee_t current;
            wrapee_t end;
        };

        [[nodiscard]] constexpr auto begin() noexcept -> Node::Iterator
        { return { children.begin(), children.end() }; }

        [[nodiscard]] constexpr auto end() noexcept -> Node::Iterator
        { return { children.end(), children.end() }; }

    private:
        std::array<size_t, child_limit> children{};
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
        auto has_unique_suffix(std::string_view prefix) const -> bool
        {
            auto* node = search(prefix);

            if (!node)
            {
                return false;
            }

            while (node->get_child_count() == 1)
            {
                node = &node->get_child();
            }

            return node->get_child_count == 0
                && node->is_terminated;
        }

        constexpr
        auto empty() const -> bool
        { return size() == 0; }

        constexpr
        auto size() const -> size_t
        { return WordCount; }

        [[nodiscard]] constexpr auto cbegin() const noexcept { return; }
        [[nodiscard]] constexpr auto cend() const noexcept { return; }

        class Iterator
        {
        public:
            using value_type = std::string;
            using pointer = const std::string*;
            using reference = const std::string&;
            using iterator_category = std::forward_iterator_tag;

            Iterator() = default;
            Iterator(Node& root) { stack.push(&root); }

            auto operator*() const -> reference { return word; }
            auto operator->() const -> pointer { return &word; }

        private:
            void next()
            {
                if (stack.empty())
                {
                    return;
                }

                const auto top = stack.top();
            }

            std::string word = "";
            std::stack<Node*> stack;
        };

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
