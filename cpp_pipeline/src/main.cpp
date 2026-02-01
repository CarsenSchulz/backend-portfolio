#include "EventQueue.h"
#include "processor.h"
#include "ingestion.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

int main() {
    constexpr int QUEUE_CAPACITY = 1000000;
    constexpr int NUM_INSTRUMENTS = 100;  // number of synthetic instruments
    constexpr int ID_MAX = NUM_INSTRUMENTS; // max valid instrument ID
    constexpr int DURATION_SECONDS = 10;

    EventQueue queue(QUEUE_CAPACITY);
    Processor processor(queue);
    Ingestion ingestion(queue);

    // Random generators
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int64_t> instrument_dist(1, NUM_INSTRUMENTS);
    std::uniform_real_distribution<double> price_dist(10.0, 200.0);

    // Launch processor in separate thread
    std::thread processor_thread([&]() {
        processor.run(DURATION_SECONDS);
    });

    // Synthetic event generation loop
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(DURATION_SECONDS);

    int64_t timestamp = 0;
    int dropped_events = 0;
    while (std::chrono::steady_clock::now() < end) {
        int64_t instrument_id = instrument_dist(rng);
        double price = price_dist(rng);
        timestamp++;

        if (!ingestion.ingest(instrument_id, price, timestamp)) {
            dropped_events++;
        }
    }
    queue.shutdown();

    processor_thread.join();

    std::cout << "Dropped events (queue full/validation failure): " << dropped_events << "\n";
    processor.report();

    return 0;
}
