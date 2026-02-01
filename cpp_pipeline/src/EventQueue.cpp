#include "EventQueue.h"

EventQueue::EventQueue(int capacity)
    : capacity(capacity) {}

bool EventQueue::enqueue(const Event& event) //single producer / single consumer only
{
    if (queue.size() >= capacity) return false;
    
    else {
        queue.push(event);
        return true;
    }
}

const Event* EventQueue::dequeue()
{
    if (queue.empty()) return nullptr;

    else {
        const Event* e = &queue.front();
        queue.pop();
        return e;
    }
}