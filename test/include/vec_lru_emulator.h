#pragma once

#include <stdexcept>
#include <vector>
#include <optional>
#include <algorithm>

template<typename KeyType, typename ValueType>
class LRUEmulator {
protected:
    struct Node {
        KeyType key;
        ValueType data;

        Node(const KeyType& k, const ValueType& v): key(k), data(v) {}
        Node() = default;
    };

    std::size_t max_cache_size_;
    std::vector<Node> node_array_;

public:
    LRUEmulator(std::size_t cache_size) :
        max_cache_size_(cache_size) {
        if(cache_size == 0)
            throw std::invalid_argument("Cache size must be bigger then 0!");

        node_array_.reserve(max_cache_size_);
    }

    void put(const KeyType& key, const ValueType& value) {
        auto vec_it = find_by_key(key);

        // If value not already in cache
        if(vec_it == node_array_.end()){
            node_array_.insert(node_array_.begin(), Node(key, value));

            if (node_array_.size() > max_cache_size_)
                node_array_.pop_back(); 
        }
        
        // If value already in cache
        else{
            Node tmp = *vec_it;
            tmp.data = value; 
            node_array_.erase(vec_it);
            node_array_.insert(node_array_.begin(), tmp);
        }
    }

    std::optional<ValueType> get(const KeyType& key) {
        auto vec_it = find_by_key(key);
        if (vec_it == node_array_.end())
            return std::nullopt;

        ValueType result = vec_it->data;

        Node tmp = *vec_it;
        node_array_.erase(vec_it);
        node_array_.insert(node_array_.begin(), tmp);

        return result;
    }

    void clear() {
        node_array_.clear();
    }

    bool empty () const noexcept {return node_array_.empty();}
    bool contains(const KeyType& key) const {find_by_key(key) == node_array_.end() ? false : true;}
    std::size_t size() const noexcept {return node_array_.size();};
    std::size_t max_size() const noexcept {return max_cache_size_;}

private:
typename std::vector<Node>::iterator find_by_key(const KeyType& target_key){
    return std::find_if(node_array_.begin(), node_array_.end(),
        [&target_key](const Node& node) { return node.key == target_key; });
}

// In find_by_value: fix member name + const ref
typename std::vector<Node>::iterator find_by_value(const ValueType& target_value){
    return std::find_if(node_array_.begin(), node_array_.end(),
        [&target_value](const Node& node) { return node.data == target_value; });
}

typename std::vector<Node>::const_iterator 
find_by_key(const KeyType& target_key) const {
    return std::find_if(node_array_.begin(), node_array_.end(),
        [&target_key](const Node& node) { return node.key == target_key; });
}

typename std::vector<Node>::const_iterator
find_by_value(const ValueType& target_value) const {
    return std::find_if(node_array_.begin(), node_array_.end(),
        [&target_value](const Node& node) { return node.data == target_value; });
}

};
