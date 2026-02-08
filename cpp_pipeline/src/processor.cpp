#include "processor.h"
#include <chrono>
#include <thread>
#include <iostream>

void Processor::run(int duration_seconds) 
{
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(duration_seconds);

    int processed_since_last_log = 0;

    while (std::chrono::steady_clock::now() < end) {
        const Event* e = queue.dequeue();
        
        if (!e) break; // if e is nullptr, we are in shutdown

        { // --- LOCK BEGIN ---
            std::lock_guard<std::mutex> lock(stats_mtx);
            total_events++;
            per_instrument[e->instrument_id].update(e->price);
        } // --- LOCK END ---

        queue.pop();
        size_t current_size = queue.size(); // this locks briefly
        if (current_size > max_queue_size) max_queue_size = current_size;
    }
}

void Processor::report(std::ostream& os) const {
    os << "Total events processed: " << total_events << "\n";
    os << "Peak Queue Size: " << max_queue_size << "\n";
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
