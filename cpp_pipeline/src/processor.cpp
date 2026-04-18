#include "processor.h"
#include <chrono>
#include <iostream>

void Processor::run(int duration_seconds) 
{
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(duration_seconds);

    while (std::chrono::steady_clock::now() < end) {
        auto e = queue.dequeue();
        
        if (!e) break; // if e is nullopt, we are in shutdown

        {
            std::lock_guard<std::mutex> lock(stats_mtx);
            total_events++;
            per_instrument[e->instrument_id].update(e->price);
        }

        const size_t current_size = queue.size();
        const size_t previous_peak = max_queue_size.load();
        if (current_size > previous_peak) {
            max_queue_size.store(current_size);
        }
    }
}

void Processor::report(std::ostream& os) const {
    os << "Total events processed: " << total_events << "\n";
    os << "Peak Queue Size: " << max_queue_size.load() << "\n";
    os << "Per-instrument stats:\n";
    for (const auto& [id, stats] : per_instrument) {
        os << "Instrument " << id
           << " | Count: " << stats.count
           << " | Min: " << stats.min_price
           << " | Max: " << stats.max_price
           << " | Avg: " << stats.avg_price()
           << "\n";
    }
}
