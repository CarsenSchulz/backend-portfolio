# Lessons Learned – CPP Pipeline Project

This document tracks key problems encountered during development, the investigation process, solutions implemented, and takeaways. Useful both for reference and as interview talking points.

---

## Table of Contents

1. [Event passing and validation issues](#event-passing-and-validation-issues)
2. [Returning pointer vs nullopt / dequeue behavior](#returning-pointer-vs-nullopt--dequeue-behavior)
3. [Multi-threaded processor throughput collapse](#multi-threaded-processor-throughput-collapse)

---

## Event passing and validation issues

**Problem:**  
- Initially attempted to pass events by `const` reference in `enqueue` and `ingest`.  
- Compiler and runtime errors occurred when trying to lock the mutex or move events into the queue.  
- Example: `lock(mtx)` did not compile with a `const` method.

**Investigation:**  
- Checked method signatures and const-correctness.  
- Verified whether moving the event would require a non-const context.  

**Solution:**  
- Removed `const` from methods that needed to lock the mutex or move events.  
- Ensured `Event` objects are moved into the queue rather than copied unnecessarily.

**Impact / Takeaway:**  
- Learned to carefully match **const correctness** with **mutating thread-safe operations**.  
- Key insight: passing by reference is fine for reading, but pushing/moving into a queue requires ownership.

---

## Returning pointer vs nullopt / dequeue behavior

**Problem:**  
- Original `dequeue()` returned a reference to the front of the queue.  
- Could not represent an “empty queue” state safely.  
- Needed to handle blocking and shutdown.

**Investigation:**  
- Considered `std::optional<Event>` but worried about copy costs.  
- Explored returning `nullptr` pointer as a sentinel for empty queue.  

**Solution:**  
- `dequeue()` now returns `const Event*`.  
  - `nullptr` indicates either shutdown or empty queue.  
- `pop()` separated from `dequeue()` to avoid holding the lock twice per event.  
- Processor threads check for `nullptr` and yield if nothing is available.

**Impact / Takeaway:**  
- Learned proper separation of **retrieving vs removing** items in a thread-safe queue.  
- Using pointer as a sentinel avoids unnecessary copies and allows condition variable usage.

---

## Multi-threaded processor throughput collapse

**Problem:**  
- Moving from single-threaded processor to 2 or 4 threads caused **total events processed to drop dramatically**, despite the queue being non-blocking.  

| Threads | Total Events Processed | Peak Queue Size |
|---------|----------------------|----------------|
| 1       | 9,965,000            | 2,341          |
| 2       | 7,751,237            | 803,736        |
| 4       | 2,085,796            | 1,464          |

**Investigation:**  
- Commented out per-instrument stats → throughput did not improve.  
- Realized ingestion thread was **blocked by mutex contention** on the shared queue.  
- Multiple processor threads holding the lock frequently prevented ingestion from enqueueing new events.  

**Solution:**  
- Introduced **sharded queues**: one queue per processor thread.  
- Each processor thread only accesses its own queue → ingestion routes events based on `instrument_id`.  
- Aggregation of stats moved to the end to avoid locking in the hot path.

**Impact / Takeaway:**  
- Learned that **mutex contention is amplified with multiple consumers**, even if each operation is fast.  
- Demonstrates importance of **thread separation and per-thread data structures** for scalability.  
- Validates that **sharded queues + per-thread stats** are higher ROI than jumping to lock-free queues prematurely.

---

### Future sections

- Fault injection testing (ID_MAX / negative prices)  
- Benchmarking & reporting architecture  
- Generator/ingestion thread separation  
- Ring buffer or lock-free queue experiments  

---

