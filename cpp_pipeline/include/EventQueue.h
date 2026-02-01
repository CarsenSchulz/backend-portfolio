#pragma once
#include "events.h"
#include <queue>
#include <optional>

/*
 * EventQueue
 * ------------
 * A non-blocking, single-consumer FIFO queue for Event objects.
 * Guarantees:
 * - FIFO ordering: oldest events are dequeued first
 * - Drop-on-full: enqueue returns false if capacity is reached
 * - Single consumer: only one thread should dequeue events
 * - Ownership: enqueued Event objects are moved into the queue
 * - Non-blocking: enqueue never waits, always returns immediately
 */

struct EventQueue
{
private:
    std::queue<Event> queue;    // Underlying FIFO storage
    const int capacity;         // Maximum number of events allowed in the queue

public:
    // Constructor: sets the maximum capacity of the queue
    explicit EventQueue(int capacity);

    // Enqueue an Event
    // Moves the Event into the queue
    // Returns true if successful, false if queue is full
    bool enqueue(const Event& event);
    
    // Dequeue an Event
    // Moves event into reference param and returns true if available
    // Returns false if the queue is empty
    const Event* dequeue();
};
