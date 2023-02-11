#include <vector>
#include <iostream>
#include <functional>
#include <cstring>

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <type_traits>
#include "LinkedList.hpp"



template<typename KeyType, typename ValueType, typename Hash=std::hash<KeyType>, typename KeyEqual=std::equal_to<KeyType>, typename Allocator=std::allocator<std::pair<const KeyType, ValueType> > >
class UnorderedMap {
public:
    using NodeType = std::pair<const KeyType, ValueType>;
    using Iterator = typename List<NodeType, Allocator>::iterator;
    using ConstIterator = typename List<NodeType, Allocator>::const_iterator;

    using ATraits = std::allocator_traits<Allocator>;

    UnorderedMap() : allocator(), nodes(), array(2, nodes.end()) {};

    UnorderedMap(const UnorderedMap& other) : allocator(
            ATraits::select_on_container_copy_construction(other.allocator)),
                                              array(2, nodes.end()) {
        nodes = other.nodes;
        update();
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        nodes = other.nodes;
        if (ATraits::propagate_on_container_copy_assignment::value && allocator != other.allocator) {
            allocator = other.allocator;
        }
        update();
        return *this;
    }

    UnorderedMap& operator=(UnorderedMap&& other) noexcept {
        nodes = std::move(other.nodes);
        array = std::move(other.array);
        if (ATraits::propagate_on_container_move_assignment::value && allocator != other.allocator) {
            allocator = other.allocator;
        }
        return *this;
    }

    UnorderedMap(UnorderedMap&& other)
            : allocator(std::move(other.allocator)),
              nodes(std::move(other.nodes)),
              array(std::move(other.array)) {}

    ~UnorderedMap() = default;

    size_t size() const {
        return nodes.size();
    }

    size_t capacity() const {
        return array.size();
    }

    Iterator begin() {
        return nodes.begin();
    }

    Iterator end() {
        return nodes.end();
    }

    ConstIterator begin() const {
        return nodes.cbegin();
    }

    ConstIterator end() const {
        return nodes.cend();
    }

    ConstIterator cbegin() const {
        return nodes.cbegin();
    }

    ConstIterator cend() const {
        return nodes.cend();
    }

    Iterator find(const KeyType& key) {
        size_t hash = (Hash{}(key)) % array.size();
        if (array[hash] == nodes.end())
            return end();
        for (auto it = array[hash]; it != nodes.end() && Hash{}((*it).first) % array.size() == hash; ++it)
            if (KeyEqual{}(key, it->first))
                return it;
        return end();
    }

    ConstIterator find(const KeyType& key) const {
        size_t hash = (Hash{}(key)) % array.size();
        if (array[hash] == nodes.end())
            return end();
        for (auto it = array[hash]; it != nodes.end() && Hash{}((*it).first) % array.size() == hash; ++it)
            if (KeyEqual{}(key, it->first))
                return it;
        return end();
    }

    Iterator erase(Iterator pos) {
        size_t hash = Hash{}(pos->first) % array.size();
        if (array[hash] == pos) {
            auto it = pos;
            ++it;
            if (it == nodes.end() || Hash{}(it->first) % array.size() != hash)
                array[hash] = nodes.end();
            else
                array[hash] = it;
        }
        return nodes.erase(pos);
    }



    std::pair<Iterator, bool> insert(const NodeType& value) {
        Iterator res = find(value.first);
        if (res != nodes.end()) {
            return {res, false};
        }
        reserve(size() + 1);
        size_t hash = Hash{}(value.first) % array.size();
        NodeType* tmp = std::allocator_traits<Allocator>::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(allocator, tmp, value);
        array[hash] = nodes.insert(array[hash], tmp);
        return {array[hash], true};
    }

    template<class Pair>
    std::pair<Iterator, bool> insert(Pair&& value) {
        Iterator res = find(value.first);
        if (res != nodes.end()) {
            return {res, false};
        }
        reserve(size() + 1);
        size_t hash = Hash{}(value.first) % array.size();
        NodeType* tmp = std::allocator_traits<Allocator>::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(allocator, tmp, std::forward<Pair>(value));
        array[hash] = nodes.insert(array[hash], tmp);
        return {array[hash], true};
    }

    template<typename InpIt>
    void insert(InpIt f, InpIt s) {
        for (auto it = f; it != s; ++it)
            insert(*it);
    }

    ValueType& operator[](const KeyType& key) {
        auto res = find(key);
        if (res != nodes.end())
            return res->second;
        NodeType* tmp = std::allocator_traits<Allocator>::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(allocator, tmp, key, std::move(ValueType()));
        return insert(reinterpret_cast<NodeType&&>(*tmp)).first->second;
    }

    ValueType& operator[](KeyType&& key) {
        auto res = find(key);
        if (res != nodes.end())
            return res->second;
        NodeType* tmp = std::allocator_traits<Allocator>::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(allocator, tmp, std::move(key), std::move(ValueType()));
        return insert(reinterpret_cast<NodeType &&>(*tmp)).first->second;

    }



    void reserve(size_t count) {
        grow(size_t((static_cast<float>(count) + 1.0) / maxLoadFactor));
    }


    float load_factor() {
        return static_cast<float>(size()) / array.size();
    }

    float max_load_factor() {
        return maxLoadFactor;
    }

    void resize(size_t needSize) {
        if (needSize > array.size()){
            array.assign(needSize, nodes.end());
            update();
        } else return;
    }

private:
    Allocator allocator;
    List<NodeType, Allocator> nodes;
    std::vector<typename List<NodeType, Allocator>::iterator> array;

    static constexpr float maxLoadFactor = 0.6;

    void update(){
        std::vector<typename List<NodeType, Allocator>::iterator> tmp;
        tmp.reserve(nodes.size());
        for (auto it = nodes.begin(); it != nodes.end(); ++it)
            tmp.push_back(it);
        if (!nodes.empty()) {
            for (auto it:tmp) {
                size_t hash = Hash{}((*it).first) % array.size();
                auto oldIt = it;
                ++it;
                nodes.splice(array[hash], nodes, oldIt);
                array[hash] = oldIt;
            }
        }
    }

    void grow(size_t needSize) {
        size_t sz = 2;
        while (sz < needSize)
            sz *= 2;
        if (sz <= array.size())
            return;

        array.assign(sz, nodes.end());
        update();
    }

public:
    bool operator==(const UnorderedMap& other) {
        if (nodes.size() != other.nodes.size())
            return false;
        for (auto it = begin(); it != end(); ++it) {
            auto res = other.find(it->first);
            if (it->first != res->first || it->second != it->second)
                return false;
        }
        return true;
    }

    bool operator!=(const UnorderedMap& other) {
        return *this != other;
    }
};