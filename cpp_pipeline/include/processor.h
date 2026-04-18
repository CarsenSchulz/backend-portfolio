#pragma once
#include "EventQueue.h"
#include "Ingestion.h"

#include <array>
#include <cstdint>
#include <ostream>

struct InstrumentStats {
    int64_t count = 0;
    double min_price = 0.0;
    double max_price = 0.0;
    double total_price = 0.0;

    void update(double price) {
        if (count == 0) {
            min_price = max_price = price;
        } else {
            if (price < min_price) min_price = price;
            if (price > max_price) max_price = price;
        }
        total_price += price;
        ++count;
    }

    double avg_price() const {
        return count > 0 ? total_price / count : 0.0;
    }
};

class Processor {
    EventQueue& queue;
    int64_t total_events = 0;
    size_t max_queue_size = 0;
    std::array<InstrumentStats, ID_MAX> per_instrument{};

public:
    explicit Processor(EventQueue& q) : queue(q) {}

    void run(int duration_seconds);
    void report(std::ostream& os) const;
};
