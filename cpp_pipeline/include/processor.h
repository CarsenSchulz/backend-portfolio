#pragma once
#include "EventQueue.h"
#include <unordered_map>
#include <cstdint>

struct InstrumentStats {
    int64_t count = 0;
    double min_price = 0;
    double max_price = 0;
    double total_price = 0;

    void update(double price) {
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

    double avg_price() const 
    {
        return count > 0 ? total_price / count : 0.0;
    }
};

class Processor 
{
    EventQueue& queue;
    int64_t total_events = 0;
    std::unordered_map<int64_t, InstrumentStats> per_instrument;

public:
    explicit Processor(EventQueue& q) : queue(q) {}

    void run(int duration_seconds); // main processing loop
    void report() const;           // print metrics
};
