# LRU Cache

This project contains a simple C++ implementation of an LRU (Least Recently Used) cache and a libFuzzer-based test target for stress testing the cache logic.

## Supported Operations

`LRUCache<KeyType, ValueType>` supports:

- `put(key, value)` to insert or update an entry
- `get(key)` to fetch a value and mark it as most recently used
- `contains(key)` to check whether a key exists
- `clear()`, `empty()`, `size()`, and `max_size()` for cache state inspection

The cache stores the most recently used item at the front and evicts the least recently used item when capacity is exceeded.

## Build

Configure and build with CMake:

```bash
cmake -S . -B build
cmake --build build
```

This produces the fuzzing executable `build/lru_fuzz`.

## Fuzzer

The project includes a libFuzzer target in `test/src/lru_fuzz_test.cpp`. It compares the main cache implementation against a simple vector-based emulator using randomized sequences of `put` and `get` operations.

Run the fuzzer with:

```bash
./build/lru_fuzz
```

You can also limit a run, for example:

```bash
./build/lru_fuzz -runs=1000
```
