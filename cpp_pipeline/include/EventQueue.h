#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>

#include "Events.h"

class EventQueue {
public:
    explicit EventQueue(size_t capacity);

    bool enqueue(Event&& e);
    std::optional<Event> dequeue(); // Blocks until work arrives or shutdown begins.
    void shutdown();

    size_t size() const;

private:
    std::queue<Event> queue;
    size_t capacity;
    std::atomic<size_t> current_size{0};

    std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;
};
