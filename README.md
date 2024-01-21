# Memory versioning project

[Documentation](https://borodun.github.io/stl-mem-ver/)

## Description

Use vs::set, vs::queue, vs::stack as their STL counterparts std::set, std::queue, std::stack,
but with transparent memory versioning under the hood. You need to use vs::thread for creating
threads with versioned variables.

Also starring poor man's AVL vs::tree!!

## Features
* All project requirements fullfilled.
* Library interface fills like STL, at least in most used places.
* Versions-aware iterators
* Initializer list constructors everywhere
  - Goodbye `std::queue{{1,2,3,4}}`
* Modern doxygen documentation on [github pages](https://borodun.github.io/stl-mem-ver/)
* *Godawful mix of codestyles*. Blame C influence, Mom!
* Transparent fokr/join mechanism using vs::thread.
* Library relies on c++20 concepts for faster polymorphism
  - Main goal was to study them and find use for them
* Catch2 modern testing framework.
* Custom user-defined merge strategies.
* A working demo of creating a frequency tree with multiple threads.

## Not imptemented

* ranges and views support
  - no time left due to mismanagement.
* move semantics
  - who in their right mind will move MT-collections?? Although, implementaion is doable.

## Build

```bash
cmake CMakeLists.txt
make
```
It will automatically clone and build the Catch2 testing framework.

Lib is `libmemver.so`.

## Run demo (frequency tree)

```bash
./demo_tree
```

## Run tests

### Run all unit tests

Run through cmake integration:
```bash
make run-tests
```

Or run binary for compact output:
```bash
./tests
```

### Run particular unit tests

List available tests
```
./tests --list-tests
```

Run tests by tag:
```
./tests "[basic]"
```