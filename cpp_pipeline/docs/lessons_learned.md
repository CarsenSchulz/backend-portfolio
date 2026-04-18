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

## Ring buffer throughput improvement

**Problem:**  
- The original queue implementation was simple and correct, but it was still using `std::queue` on the hottest path in the whole program.  
- Even in the single-threaded version, every event still had to go through enqueue/dequeue plus queue bookkeeping, so small overhead there added up fast.  

**What I changed:**  
- Swapped the queue storage to a bounded ring buffer instead of `std::queue`.  
- Kept the pipeline shape mostly the same so it was still easy to explain.  
- Also cleaned up the processor side a bit by using fixed instrument slots instead of a more general container.  

**Why it helped:**  
- The ring buffer keeps storage fixed and reuses the same memory instead of relying on a more general queue structure.  
- That made the hot path cheaper and more predictable.  
- Since the queue sits between generation and processing, even a small improvement there had a pretty big effect on total throughput.  

**Takeaway:**  
- This was a good example of a simple systems change with a measurable payoff.  
- I did not need a more complicated architecture to get a big gain here, just a better data structure for the workload.  

## Compiler optimization note

Rebuilding the same single-threaded version with `-O2` gave a much bigger throughput jump than I expected. It was a good reminder that compiler settings can change the results a lot, and benchmarking without optimization can give a pretty misleading picture.  

## AI-assisted cleanup and review workflow

**What I did:**  
- Used Codex for a cleanup pass near the end of the project.  
- Mainly used it for readability, repo cleanup, and checking for small inconsistencies.  
- Reviewed the diffs before keeping anything.  

**Where it helped:**  
- Helped fix the build/output path mismatch so the repo layout matched the README.  
- Helped clean up naming, comments, and small things that were making the code harder to explain.  
- Good for catching low-risk cleanup items faster than doing a full manual pass.  

**Takeaway:**  
- Codex was useful for cleanup and review speed, but I still had to check the changes and make sure they matched what I wanted.  
- It saved time on polish, but I still need to be able to explain every change myself.  

