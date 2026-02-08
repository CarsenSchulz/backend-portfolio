#include "EventQueue.h"
#include "processor.h"
#include "ingestion.h"
#include "benchmarks.h"

#include <thread>
#include <chrono>
#include <random>
#include <vector>

#include <sstream>

int main() {
    constexpr int QUEUE_CAPACITY = 1E6;
    constexpr int DURATION_SECONDS = 5;
    constexpr int NUM_INSTRUMENTS = 100;    
    double MIN_PRICE = 0.0;  
    double MAX_PRICE = 200.0;

    constexpr bool FAULT_INJECTION = false;

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

    //spawn and start threads
    const int NUM_THREADS = 2;
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        threads.push_back(std::thread([&processor]() {
            processor.run(DURATION_SECONDS);
        }));
    }

    // Synthetic event generation loop
    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(DURATION_SECONDS);

    constexpr int BATCH_SIZE = 1000;

    std::vector<Event> batch;
    batch.reserve(BATCH_SIZE);

    int64_t timestamp = 0;
    while (std::chrono::steady_clock::now() < end) {
        // Generate a batch
        batch.clear();
        for (int i = 0; i < BATCH_SIZE; i++) {
            int64_t instrument_id = instrument_dist(rng);
            double price = price_dist(rng);
            timestamp++;
            batch.emplace_back(instrument_id, price, timestamp);
        }

        // Validate + enqueue batch
        for (auto& e : batch) {
            ingestion.ingest(e.instrument_id, e.price, e.timestamp);
        }
    }
    queue.shutdown();

    //join all processor threads
    for (auto& t : threads) t.join();

    // Ensure benchmarks folder exists
    Benchmarks::ensureFolder();

    // Collect metrics from ingestion and processor into a string stream
    std::ostringstream metrics;
    ingestion.report(metrics);   // must accept std::ostream& now
    processor.report(metrics);   // must accept std::ostream& now

    // Generate timestamped filename
    std::string filename = Benchmarks::makeTimestampedFilename();

    // Write metrics to file and echo to console
    Benchmarks::writeMetricsWithConsole(filename, metrics);

    return 0;
}
