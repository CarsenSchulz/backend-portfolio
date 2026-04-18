#include "EventQueue.h"

EventQueue::EventQueue(size_t cap)
    : buffer(cap), capacity(cap) {}

bool EventQueue::enqueue(Event&& e) {
    std::unique_lock<std::mutex> lock(mtx);
    while (current_size.load() >= capacity && !stopped) {
        not_full_cv.wait(lock);
    }
    if (stopped) {
        return false;
    }

    buffer[tail].emplace(std::move(e));
    tail = (tail + 1) % capacity;
    current_size++;
    not_empty_cv.notify_one();
    return true;
}

std::optional<Event> EventQueue::dequeue() {
    std::unique_lock<std::mutex> lock(mtx);
    while (current_size.load() == 0 && !stopped) {
        not_empty_cv.wait(lock);
    }

    if (current_size.load() == 0) {
        return std::nullopt;
    }

    std::optional<Event> event = std::move(buffer[head]);
    buffer[head].reset();
    head = (head + 1) % capacity;
    current_size--;
    not_full_cv.notify_one();
    return event;
}

void EventQueue::shutdown() {
    std::lock_guard<std::mutex> lock(mtx);
    stopped = true;
    not_empty_cv.notify_all();
    not_full_cv.notify_all();
}

size_t EventQueue::size() const {
    return current_size.load();
}
