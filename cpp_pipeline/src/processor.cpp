#include "processor.h"

#include <chrono>

void Processor::run(int duration_seconds)
{
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(duration_seconds);
    constexpr int kQueueSizeSampleInterval = 1024;

    while (std::chrono::steady_clock::now() < end) {
        auto event = queue.dequeue();
        if (!event) {
            break;
        }

        ++total_events;
        per_instrument[event->instrument_id - 1].update(event->price);

        if ((total_events % kQueueSizeSampleInterval) == 0) {
            const size_t current_size = queue.size();
            if (current_size > max_queue_size) {
                max_queue_size = current_size;
            }
        }
    }
}

void Processor::report(std::ostream& os) const {
    os << "Total events processed: " << total_events << "\n";
    os << "Peak Queue Size: " << max_queue_size << "\n";
    os << "Per-instrument stats:\n";
    for (int instrument_index = 0; instrument_index < ID_MAX; ++instrument_index) {
        const InstrumentStats& stats = per_instrument[instrument_index];
        if (stats.count == 0) {
            continue;
        }

        os << "Instrument " << (instrument_index + 1)
           << " | Count: " << stats.count
           << " | Min: " << stats.min_price
           << " | Max: " << stats.max_price
           << " | Avg: " << stats.avg_price()
           << "\n";
    }
}
