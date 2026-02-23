#include <cstring>
#include <iostream>
#include "lru.h"
#include "vec_lru_emulator.h"

#include <cstdint>

const size_t MAX_CAPACITY = 50;  
const size_t MAX_OPERATIONS = 1000; 

struct FuzzDataReader {
    const uint8_t* data;
    std::size_t size;
    std::size_t pos = 0;

    uint8_t ReadByte() {
        if (pos >= size) return 0;
        return data[pos++];
    }

    uint32_t ReadInt() {
        if (pos + 4 > size) return 0;
        
        uint32_t val;
        std::memcpy(&val, data + pos, sizeof(uint32_t));
        pos += 4;
        return val;
    }

    bool HasData() const { return pos < size; }
};

template<typename KeyType, typename ValueType>
class LRUTest: private LRUCache<KeyType, ValueType>{
public:
    using Node = typename LRUCache<KeyType, ValueType>::Node;

    explicit LRUTest(std::size_t cache_size) 
    : LRUCache<KeyType, ValueType>(cache_size) {}

    using LRUCache<KeyType, ValueType>::put;
    using LRUCache<KeyType, ValueType>::get;
    using LRUCache<KeyType, ValueType>::size;
    using LRUCache<KeyType, ValueType>::clear;
    using LRUCache<KeyType, ValueType>::empty;
    using LRUCache<KeyType, ValueType>::contains;

    const std::list<Node>& get_node_array() const noexcept {
        return this->recency_list_;
    }

    void visualize(){
        auto lst_it = this->recency_list_.begin();

        std::cout << "Oracle cache state: \n";
        while(lst_it != this->recency_list_.end()){
            std::cout << "Key: " << lst_it->key << " Data: " << lst_it->data << std::endl;
            ++lst_it;
        }
    }
};

template<typename KeyType, typename ValueType>
class LRUEmulatorTest: private LRUEmulator<KeyType, ValueType>{
public:
    using Node = typename LRUEmulator<KeyType, ValueType>::Node;

    explicit LRUEmulatorTest(std::size_t cache_size)
    : LRUEmulator<KeyType, ValueType>(cache_size){}

    using LRUEmulator<KeyType, ValueType>::put;
    using LRUEmulator<KeyType, ValueType>::get;
    using LRUEmulator<KeyType, ValueType>::size;
    using LRUEmulator<KeyType, ValueType>::clear;
    using LRUEmulator<KeyType, ValueType>::empty;
    using LRUEmulator<KeyType, ValueType>::contains;

    const std::vector<Node>& get_node_array() const noexcept {
        return this->node_array_;
    }

    void visualize(){
        auto arr_it = this->node_array_.begin();

        std::cout << "Vec cache state: \n";
        while(arr_it != this->node_array_.end()){
            std::cout << "Key: " << arr_it->key << " Data: " << arr_it->data << std::endl;
            ++arr_it;
        }
    }
};

template<typename KeyType, typename ValueType>
bool resent_el_comp(const LRUTest<KeyType, ValueType>& oracle_lru,
                    const LRUEmulatorTest<KeyType, ValueType> naive_lru){

    if(oracle_lru.empty() && naive_lru.empty())
        return true;

    const auto& naive_list = naive_lru.get_node_array();
    const auto& oracle_list = oracle_lru.get_node_array();

    auto naive_it = naive_list.begin();
    auto oracle_it = oracle_list.begin();

    if(naive_it->key != oracle_it->key ||
       naive_it->data != oracle_it->data)
        return false;

    return true;
}


template<typename KeyType, typename ValueType>
bool cache_comp(const LRUTest<KeyType, ValueType>& oracle_lru, 
                const LRUEmulatorTest<KeyType, ValueType>& naive_lru) {
    // 1. Check size
    if(oracle_lru.size() != naive_lru.size())
        return false;

    // 2. Compare content and order
    const auto& naive_list = naive_lru.get_node_array();
    const auto& oracle_list = oracle_lru.get_node_array();
    
    auto naive_it = naive_list.begin();
    auto oracle_it = oracle_list.begin();
    
    while(naive_it != naive_list.end() && oracle_it != oracle_list.end()) {
        if(naive_it->key != oracle_it->key || naive_it->data != oracle_it->data)
            return false;
        
        ++naive_it;
        ++oracle_it;
    }
    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    FuzzDataReader reader{data, size, 0};

    size_t capacity = (reader.ReadByte() % MAX_CAPACITY) + 1;
    std::cout << "Cache capacity: " << capacity << std::endl;

    LRUTest<int, int> oracle_cache(capacity);
    LRUEmulatorTest<int, int> test_cache(capacity);

    // Execute operations
    std::cout << "Test: \n" << std::endl;

    std::size_t op_count = 0;
    while(reader.HasData() && op_count < MAX_OPERATIONS){
        uint8_t opcode = reader.ReadByte();
        uint32_t key   = reader.ReadInt() % 100;
        uint32_t value = reader.ReadInt();

        if(opcode < 100){
            std::cout << "put: key " << key << " data " << value << std::endl;
            oracle_cache.put(key, value);
            test_cache.put(key, value);

            if(!resent_el_comp(oracle_cache, test_cache)){
                std::cerr << "Recent element becomes different for different LRU implementations after put()!\n";
                oracle_cache.visualize();
                test_cache.visualize();
                abort();
            }
        }

        else{
            std::cout << "get: key " << key << std::endl;
            auto res_oracle = oracle_cache.get(key);
            auto res_test   = test_cache.get(key);

            if(res_oracle != res_test){
                std::cerr << "Different values provides from different implementation after get()!\n";
                oracle_cache.visualize();
                test_cache.visualize();
                abort();
            }
        }

        //oracle_cache.visualize();
        //test_cache.visualize();

        if(oracle_cache.size() != test_cache.size()){
            std::cerr << "Oracle and test cache implementations become with different size!\n";
            oracle_cache.visualize();
            test_cache.visualize();
            abort();
        }
    }

    // Verify that both cache implementations contain the same data
    // in the same order
    if(!cache_comp(oracle_cache, test_cache)){
        std::cerr << "Oracle and test cache implementations becomes with different cache buffers\n";
        oracle_cache.visualize();
        test_cache.visualize();
        abort();
    }

    return 0;
}
