# Memory versioning project

## Description

Use vs::set, vs::queue, vs::stack as their STL counterparts std::set, std::queue, std::stack,
but with transparent memory versioning under the hood.

Also starring poor man's AVL vs::tree!!

## Features
* All project requirements fullfilled.
* Library interface fills like STL, at least in most used places.
* Versions-aware iterators
* Initializer list constructors everywhere
  - Goodbye `std::queue{{1,2,3,4}}`
* Modern doxygen documentation on github pages
* *Godawful mix of codestyles*. Blame C influence, Mom!
* Transparent fokr/join mechanism using vs::thread.

## Not imptemented

* Nice testing framework
  - it is a mess now. A working mess, though.
* ranges and views support
  - no time left due to mismanagement.
* move semantics
  - who in their right mind will move MT-collections?? Although, implementaion is doable.
* rewrite from legacy iterators to c++20 concepts
  - we use stl containers under the hood, and they use legacy iterators still.

## Build

```bash
cmake CMakeLists.txt
make
```
It will automatically clone and build the Catch2 testing framework.

Lib is `libmemver.so`.

## Run tests

### Run all tests

Run through cmake integration:
```bash
make run-tests
```

Or run binary for compact output:
```bash
./tests
```

### Run particular tests

List available tests
```
./tests --list-tests
```

Run tests by tag:
```
./tests "[basic]"
```
