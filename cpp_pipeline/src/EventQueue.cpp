#include "EventQueue.h"

EventQueue::EventQueue(size_t cap)
    : capacity(cap) {}

bool EventQueue::enqueue(Event&& e) {
    std::unique_lock<std::mutex> lock(mtx);
    if (queue.size() >= capacity) return false;   // Drop if full
    queue.push(std::move(e));                     // Move immutable Event into queue
    current_size++;
    cv.notify_one();                              // Wake a waiting consumer
    return true;
}

std::optional<Event> EventQueue::dequeue() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]{ return !queue.empty() || stopped; });

    if (queue.empty()) return std::nullopt;  // shutdown

    Event e = std::move(queue.front());
    queue.pop();
    current_size--;
    return e;                               // Return pointer to front
}


void EventQueue::shutdown() {
    std::lock_guard<std::mutex> lock(mtx);
    stopped = true;
    cv.notify_all();
}

size_t EventQueue::size() {
    return current_size.load();
}
