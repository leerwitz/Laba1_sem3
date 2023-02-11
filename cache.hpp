#pragma once
#include <assert.h>
#include <iostream>
#include "Hash_table.hpp"
#include <list>

template<typename KeyType, typename ValueType>
class cache {
private:
    std:: list<KeyType> ItemList;
    UnorderedMap<KeyType, std:: pair<ValueType, typename std:: list<KeyType>::iterator>> ItemMap;
    size_t CacheSize;
    void clean(){
        while (ItemList.size() > CacheSize){
            KeyType back = ItemList.back();
            auto it = ItemMap.find(back);
            ItemMap.erase(it);
            ItemList.pop_back();
        }
    }
public:
    cache(size_t Cache_Size) : CacheSize(Cache_Size){}
    void put(const KeyType& Key,const ValueType& Value){
        auto it = ItemMap.find(Key);
        if (it != ItemMap.end()){
            ItemMap.erase(it);
            ItemList.erase(it->second.second);
        }
        ItemList.push_front(Key);
        ItemMap[Key] = make_pair(Value, ItemList.begin());
        clean();
    }

    ValueType& get(const KeyType& Key){
        assert(ItemMap.find(Key) != ItemMap.end());
        auto it = ItemMap.find(Key);
        return ItemMap[Key].first;
    }
};
