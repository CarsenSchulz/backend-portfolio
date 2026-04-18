# Benchmark History

This file is a lightweight record of benchmark results and methodology notes from the project.
It is not meant to be a perfect lab notebook. The goal is just to keep the main benchmark
milestones and the reasoning behind them in one place.

## Methodology Notes

- Most throughput runs were measured as total events processed in a fixed 5 second window.
- Results usually came from 3-run samples unless otherwise noted.
- Compiler settings mattered a lot. In particular, `-O2` changed throughput enough that build
  flags need to be recorded alongside the results.
- The synthetic generator and validation path both affect the final numbers, so benchmark
  comparisons only really make sense when the pipeline shape stays similar.
- Some experiments below were done to isolate bottlenecks rather than to produce a final version
  of the project.

## Baseline

Original `std::queue`-based version, 1 thread:

- run 1: `7,620,390`
- run 2: `7,826,000`
- run 3: `8,056,485`
- average: `7,834,291.67`

This was the initial single-thread baseline before switching to a bounded ring buffer.

## First Ring Buffer Result

First bounded ring buffer version, 1 thread:

- run 1: `9,198,000`
- run 2: `9,340,000`
- run 3: `9,508,000`
- average: `9,348,666.67`

This was the first clear sign that replacing `std::queue` with a bounded ring buffer was worth it.

## Current Mainline Single-Thread Result

Final single-thread version kept simple for the main project:

- bounded ring buffer
- validation kept
- fixed per-instrument stats
- backpressure instead of dropping

Built with `-O2`, 1 thread:

- run 1: `19,482,394`
- run 2: `18,341,662`
- run 3: `18,272,472`
- average: `18,698,842.67`
- dropped events: `0`

This is the current mainline result I want to present as the primary version of the project.

## Compiler Optimization Comparison

Same final single-thread version, compared with and without `-O2`.

Without `-O2`:

- run 1: `10,465,000`
- run 2: `10,295,000`
- run 3: `10,421,000`
- average: `10,393,666.67`

With `-O2`:

- run 1: `19,482,394`
- run 2: `18,341,662`
- run 3: `18,272,472`
- average: `18,698,842.67`

Takeaway:

- `-O2` added about `8.31M` events over 5 seconds on average.
- Relative gain was about `+79.9%`.
- Compiler optimization had a much bigger impact than expected.

## Mixed-Result Experiments

These were useful for learning, even when they were not the final direction.

### Ring Buffer Plus Local Stats

Average throughput:

- 1 thread: `9,579,666.67`
- 2 threads: `7,658,666.67`
- 4 threads: `2,335,666.67`

Takeaway:

- Helped a little in the 1-thread case.
- Did not really solve the multi-thread scaling problem on its own.

### Sampled Queue Depth

Average throughput:

- 1 thread: `9,635,000`
- 2 threads: `7,608,333.33`
- 4 threads: `2,379,068.33`

Takeaway:

- Sampling queue depth less often helped clean up hot-path overhead.
- The gain was real but not enough to change the overall architecture story.

### Larger Batch Size

Average throughput:

- 1 thread: `9,305,991.67`
- 2 threads: `8,484,229.67`
- 4 threads: `2,374,553.33`

Takeaway:

- Bigger generation batches helped the 2-thread case noticeably.
- It was not a universal win for every configuration.

### Generic Sharded Queues

Average throughput:

- 1 thread: `8,236,666.67`
- 2 threads: `5,323,000`
- 4 threads: `3,203,973.67`

Takeaway:

- Sharding helped the 4-thread case.
- It did not help enough on its own, and it hurt the simpler cases.

## Fastest Multi-Threaded Result

This is the more specialized sharded SPSC path that belongs on a separate branch from the simpler
single-threaded main version.

4-thread averages for the final comparison set:

- generic sharded with `-O2`: `5,594,000`
- ring/local-stats/large-batch with `-O2`: `11,801,240`
- sharded SPSC with `-O2`: `40,571,362`

Sharded SPSC 4-thread runs:

- run 1: `41,260,272`
- run 2: `40,436,544`
- run 3: `40,017,270`
- average: `40,571,362`

Takeaway:

- The specialized sharded SPSC design was much faster.
- The simpler single-thread version is still the better mainline project story because it is
  easier to explain and still performs well.
- The multi-threaded version is worth keeping as its own branch because it shows how far the
  design can scale with a more specialized architecture.
