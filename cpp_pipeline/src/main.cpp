#include "EventQueue.h"
#include "processor.h"
#include "ingestion.h"
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

int main() {
    constexpr int QUEUE_CAPACITY = 500000;
    constexpr int DURATION_SECONDS = 5;
    constexpr int NUM_INSTRUMENTS = 100;    
    double MIN_PRICE = 0.0;  
    double MAX_PRICE = 200.0;

    constexpr bool FAULT_INJECTION = true;

    // Variables for generator only — not constants, can change
    int gen_max_id = NUM_INSTRUMENTS;
    double gen_min_price = MIN_PRICE;

    if (FAULT_INJECTION) {
        gen_max_id = 101;      // Some IDs > NUM_INSTRUMENTS → invalid
        gen_min_price = -0.1;  // Some negative prices → invalid
    }

    EventQueue queue(QUEUE_CAPACITY);
    Processor processor(queue);
    Ingestion ingestion(queue);

    // Random generators
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int64_t> instrument_dist(1, gen_max_id);
    std::uniform_real_distribution<double> price_dist(gen_min_price, MAX_PRICE);


    // Launch processor in separate thread
    std::thread processor_thread([&]() {
        processor.run(DURATION_SECONDS);
    });

    // Synthetic event generation loop
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(DURATION_SECONDS);

    int64_t timestamp = 0;
    while (std::chrono::steady_clock::now() < end) {
        int64_t instrument_id = instrument_dist(rng);
        double price = price_dist(rng);
        timestamp++;

        if (!ingestion.ingest(instrument_id, price, timestamp)) {

        }
    }
    queue.shutdown();

    processor_thread.join();

    ingestion.report();
    processor.report();

    return 0;
}
