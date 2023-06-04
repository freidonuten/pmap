#pragma once

#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <string_view>
#include <optional>
#include <stdexcept>
#include <stack>


namespace mpr
{

template <typename T>
struct map_node
{
    std::optional<T> data;
    std::unordered_map<char, map_node<T>> children;
};

template <typename T>
class prefix_map_inserter
{
public:
    prefix_map_inserter(map_node<T>& node)
        : current(&node)
    {}

    void insert(const char c)
    {
        const auto& [ item, success ] = current->children.insert({ c, {} });
        current = &item->second;
    }

    void set(T&& data)
    {
        current->data.emplace(std::forward<T>(data));
    }

    void step(const char c)
    {
        if (current->children.count(c) == 0)
        {
            throw std::runtime_error("no such element");
        }

        current = &current->children.at(c);
    }

    void step()
    {
        if (current->children.size() != 1)
        {
            throw std::runtime_error("cannot descend, next item not present or ambiguous");
        }

        current = &current->children.begin()->second;
    }

    auto get_node() const -> map_node<T>&
    {
        return *current;
    }

    auto has_data() const -> bool
    {
        return current->data.has_value();
    }

    auto is_terminal() const -> bool
    {
        return current->children.size() == 0;
    }

    auto has_unique_child() const -> bool
    {
        return current->children.size() == 1;
    }

    auto is_unique_prefix() const -> bool
    {
        return has_unique_child() && !has_data();
    }

private:
    map_node<T>* current = nullptr;
};

template <typename T>
class prefix_map_iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    prefix_map_iterator() = default;
    prefix_map_iterator(map_node<value_type>& node)
        : index(0)
    {
        stack.push(&node);
        operator++();
    }

    reference operator*() const
    {
        return *current->data;
    }

    pointer operator->() const
    {
        return &operator*();
    }

    prefix_map_iterator& operator++()
    {
        if (index == -1)
        {
            throw std::runtime_error("incrementing past the end");
        }

        if (stack.empty())
        {
            index = -1;
        }

        // Just a regular DFS on tree-like structure but it breaks
        // on first node containing data next call will catch up
        // where the previous left off due to stack being a part
        // of the iterator state
        while (!stack.empty())
        {
            current = stack.top();
            stack.pop();

            for (auto& [key, val] : current->children)
            {
                stack.push(&val);
                ++index;
            }

            if (current->data)
            {
                break;
            }
        }

        return *this;
    }

    prefix_map_iterator operator++(int)
    {
        auto temp = *this;
        operator++();
        return temp;
    }

    bool operator==(const prefix_map_iterator& other) const
    {
        return index == other.index;
    }

    bool operator!=(const prefix_map_iterator& other) const
    {
        return !operator==(other);
    }

private:
    size_t index = -1;
    map_node<value_type>* current;
    std::stack<map_node<value_type>*> stack;
};

template <typename T>
class prefix_map
{
public:
    using iterator = prefix_map_iterator<T>;
    using const_iterator = prefix_map_iterator<const T>;

    void insert(const std::string_view word, T&& data)
    {
        auto inserter = prefix_map_inserter<T>(root);

        for (const auto c : word)
        {
            inserter.insert(c);
        }

        inserter.set(std::forward<T>(data));
    }

    void remove(const std::string_view word)
    {
        throw std::runtime_error("not implemented");
    }

    auto at(const std::string_view word) -> T&
    {
        auto inserter = prefix_map_inserter<T>(root);

        // iterate word
        for (const auto c : word)
        {
            inserter.step(c);
        }

        // while the word isn't terminated, try to complete it
        while (inserter.is_unique_prefix())
        {
            inserter.step();
        }

        // in case we didn't find a complete word throw
        if (!inserter.has_data())
        {
            throw std::runtime_error("there is no data asociated");
        }

        return inserter.get_node().data.value();
    }

    // Iterator interface
    auto begin() -> iterator { return iterator(root); }
    auto end() -> iterator { return {}; }
    auto cbegin() const -> const_iterator { return iterator(root); }
    auto cend() const -> const_iterator { return {}; }

private:
    map_node<T> root{};
};

}
