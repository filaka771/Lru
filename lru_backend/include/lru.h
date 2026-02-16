#pragma once

#include <iostream>

#include <optional>
#include <list>
#include <stdexcept>
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
        max_cache_size_(cache_size) {
        if(cache_size == 0)
            throw std::invalid_argument("Cache size must be bigger then 0!");
    }

    ~LRUCache(){
        delete(map_);
        delete(recency_list_);
    }

    std::optional<ValueType> get(KeyType key) {
        auto map_it = map_.find(key);
        if(map_it == map_.end()){
            return std::nullopt;
        }

        auto list_it = map_it->second;
        recency_list_.splice(recency_list_.begin(), recency_list_, list_it);

        return list_it->data;
    }

    void put(const KeyType& key, const ValueType& value) {
        auto map_it = map_.find(key);

        // Case if value already in cache
        if(map_it != map_.end()) {
            // Move value in the begin of recency list
            auto list_it = map_it->second;
            list_it->data = value;
            recency_list_.splice(recency_list_.begin(), recency_list_, list_it);
        }

        // Case if value not in cache
        else {
            // Add node in list if list smaller then cache size
            if(recency_list_.size() < max_cache_size_) {
                recency_list_.emplace_front(key, value);
                map_[key] = recency_list_.begin();
            }
            // Evict already existed node
            else {
                auto list_it = -- recency_list_.end();
                map_.erase(list_it->key);

                list_it->data = value;
                list_it->key  = key;

                recency_list_.splice(recency_list_.begin(), recency_list_, list_it);
                map_[key] = list_it;
            }
        }

    }

    void clear(){
        map_.clear();
        recency_list_.clear();
    }

    bool empty () const noexcept {return map_.empty();}
    bool contatins(const KeyType& key) const {map_.find(key) == map_.end() ? false : true;}
    std::size_t size() const noexcept {return map_.size();}
    std::size_t max_size() const noexcept{return max_cache_size_;}
}
