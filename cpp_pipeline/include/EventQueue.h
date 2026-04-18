#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <vector>

#include "Events.h"

class EventQueue {
public:
    explicit EventQueue(size_t capacity);

    bool enqueue(Event&& e);
    std::optional<Event> dequeue();
    void shutdown();

    size_t size() const;

private:
    std::vector<std::optional<Event>> buffer;
    const size_t capacity;
    size_t head = 0;
    size_t tail = 0;
    std::atomic<size_t> current_size{0};

    std::mutex mtx;
    std::condition_variable not_empty_cv;
    std::condition_variable not_full_cv;
    bool stopped = false;
};
