#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "events.h"
#include <optional>
#include <atomic>

class EventQueue {
public:
    explicit EventQueue(size_t capacity);

    bool enqueue(Event&& e);   // Move Event into queue
    std::optional<Event> dequeue();    // Blocking pointer return
    void shutdown();           // Stop consumer (not strictly needed here, but safe)

    size_t size();

private:
    std::queue<Event> queue;   // Own Events by value
    size_t capacity;
    std::atomic<size_t> current_size{0};

    std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;
};
