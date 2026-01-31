#include "EventQueue.h"
#include <utility>

EventQueue::EventQueue(int capacity)
    : capacity(capacity) {}

bool EventQueue::enqueue(Event&& event) //single producer / single consumer only
{
    if (queue.size() >= capacity) return false;
    
    else {
        queue.push(std::move(event));
        return true;
    }
}

std::optional<Event> EventQueue::dequeue()
{
    if (queue.empty()) return std::nullopt;

    else {
        Event event = std::move(queue.front());
        queue.pop();
        return event;
    }
}