#include "EventQueue.h"

EventQueue::EventQueue(size_t cap)
    : capacity(cap) {}

bool EventQueue::enqueue(Event&& e) {
    std::unique_lock<std::mutex> lock(mtx);
    if (queue.size() >= capacity) return false;   // Drop if full
    queue.push(std::move(e));                     // Move immutable Event into queue
    cv.notify_one();                              // Wake a waiting consumer
    return true;
}

const Event* EventQueue::dequeue() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&] { return !queue.empty() || stopped; });  // Wait for data or shutdown
    if (queue.empty()) return nullptr;                           // Exit if shutdown
    return &queue.front();                                       // Return pointer to front
}

void EventQueue::pop() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!queue.empty()) queue.pop();       // Remove after processing
}

void EventQueue::shutdown() {
    std::lock_guard<std::mutex> lock(mtx);
    stopped = true;
    cv.notify_all();
}
