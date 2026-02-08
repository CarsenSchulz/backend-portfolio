#pragma once
#include "EventQueue.h"
#include <unordered_map>
#include <cstdint>
#include <atomic>

struct InstrumentStats {
    int64_t count = 0; // Number of events processed
    double min_price = 0; // Minimum price of instrument
    double max_price = 0;  // Maximum price of instrument
    double total_price = 0; //Total price to compute average

    void update(double price) { // Updates stats
        if (count == 0) 
        {
            min_price = max_price = price;
        } 
        else 
        {
            if (price < min_price) min_price = price;
            if (price > max_price) max_price = price;
        }
        total_price += price;
        count++;
    }

    double avg_price() const // Computes average on demand
    {
        return count > 0 ? total_price / count : 0.0;
    }
};

class Processor 
{
    EventQueue& queue; // Reference to queue
    //int64_t total_events = 0; // Num events processed
    std::atomic<int64_t> total_events{0};
    std::unordered_map<int64_t, InstrumentStats> per_instrument; // Map for per-instrument stats
    size_t max_queue_size = 0;
    std::mutex stats_mtx;

public:
    explicit Processor(EventQueue& q) : queue(q) {}

    void run(int duration_seconds); // Main processing loop
    void report(std::ostream& os) const;           // Print metrics
};
