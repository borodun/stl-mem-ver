# Memory versioning project

## Description

Use vs::set, vs::queue, vs::stack as their STL counterparts std::set, std::queue, std::stack,
but with transparent memory versioning under the hood.

And yes they are all built on std::list

## Build

```bash
make
```

## Run

```bash
make run
```

```bash
LD_LIBRARY_PATH=./lib/ ./demo
```
