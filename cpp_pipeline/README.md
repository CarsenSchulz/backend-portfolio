# C++ Low-Latency Market Data Pipeline

This project implements a high-performance event processing pipeline in C++, inspired by
real-world market data and low-latency systems. The goal is to explore how architectural choices
impact throughput, latency, and system simplicity.

## Design Overview

The pipeline is intentionally minimal and deterministic:
- Immutable event structures to avoid shared-state mutation
- FIFO queue for predictable ordering
- Single-threaded event processor to eliminate synchronization overhead
- Explicit benchmarking and metrics collection

This design prioritizes clarity and correctness first, with performance improvements layered
on top in a controlled way.

## Key Features

- Immutable event data model
- FIFO queue-based ingestion
- Single-threaded processing loop
- Latency, throughput, and queue utilization measurement
- Fault injection hooks to evaluate system behavior under stress

## Performance Measurement

The system collects and reports:
- Event throughput
- End-to-end latency
- Queue depth and utilization

These metrics are used to reason about system bottlenecks and validate design tradeoffs.

## Planned Optimizations

The current implementation establishes a baseline for correctness and measurement.
Planned enhancements include:
- Lock-free ring buffer for reduced contention
- Multi-threaded processing model
- Asynchronous metrics logging
- Improved memory locality and cache behavior
- Migrated build process to CMake

These changes will allow direct comparison between architectural approaches and their impact
on performance.

## Purpose

This project is not a trading system. It is a focused exploration of low-latency system design,
measurement, and incremental optimization in modern C++.

## Building and Running

### Requirements
- C++17 compatible compiler (GCC or Clang)
- Linux, macOS, or Windows (MinGW / WSL recommended on Windows)

### Build
From the repository root:

```bash
  g++ -std=c++17 \
  -I cpp_pipeline/include \
  cpp_pipeline/src/main.cpp \
  cpp_pipeline/src/ingestion.cpp \
  cpp_pipeline/src/EventQueue.cpp \
  cpp_pipeline/src/processor.cpp \
  cpp_pipeline/src/benchmarks.cpp \
  -o pipeline
```

### Run
```bash
  ./pipeline
```
