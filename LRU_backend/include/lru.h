#pragma once

#include <iostream>
#include <list>
#include <unordered_map>

template<typename KeyType, typename ValueType>
class LRUCache {
private:
    struct Node{
        KeyType key;
        ValueType data;
    };

    std::size_t max_cache_size_;

    std::list<Node> recency_list_;
    std::unordered_map<KeyType, typename std::list<Node>::iterator> map_;

public:
    explicit LRUCache(std::size_t cache_size):
        max_cache_size_(cache_size) {}

    void get(KeyType key) {

    }

    void put(const KeyType& key, const ValueType& value) {
        auto map_it = map_.find(key);

        if(map_it != map_.end()){
            // Move value in the begin of recency list
            auto list_it = *map_it;
            recency_list_.splice(recency_list_.begin(), recency_list_, list_it);

            // Change iterator in map
            map_it->second = recency_list_.begin();
            
        }
        else{
            recency_list_.push_front(value);

            auto new_val_it = recency_list_.begin();
            map_[key] = new_val_it;
        }

    }

private:
    void evict() {

    }

}
