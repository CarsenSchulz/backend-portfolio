#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "events.h"

class EventQueue {
public:
    explicit EventQueue(size_t capacity);

    bool enqueue(Event&& e);   // Move Event into queue
    const Event* dequeue();    // Blocking pointer return
    void pop();                // Remove last dequeued Event
    void shutdown();           // Stop consumer (not strictly needed here, but safe)

private:
    std::queue<Event> queue;   // Own Events by value
    size_t capacity;

    std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;
};
