#include "EventQueue.h"
#include "Benchmarks.h"
#include "Ingestion.h"
#include "processor.h"

#include <chrono>
#include <random>
#include <sstream>
#include <thread>
#include <vector>

int main() {
    constexpr size_t kQueueCapacity = 1'000'000;
    constexpr int kDurationSeconds = 5;
    constexpr int kProcessorThreads = 1; // Keep the baseline easy to explain in interviews.
    constexpr int kBatchSize = 1'000;
    constexpr double kMinPrice = 0.0;
    constexpr double kMaxPrice = 200.0;
    constexpr bool kFaultInjection = false;

    int generator_max_id = ID_MAX;
    double generator_min_price = kMinPrice;

    if (kFaultInjection) {
        generator_max_id = ID_MAX + 1; // Allow some invalid instrument IDs through the generator.
        generator_min_price = -0.1;    // Allow some invalid negative prices through the generator.
    }

    EventQueue queue(kQueueCapacity);
    Processor processor(queue);
    Ingestion ingestion(queue);

    std::mt19937_64 rng(42);
    std::uniform_int_distribution<int64_t> instrument_dist(1, generator_max_id);
    std::uniform_real_distribution<double> price_dist(generator_min_price, kMaxPrice);

    std::vector<std::thread> threads;
    threads.reserve(kProcessorThreads);

    for (int i = 0; i < kProcessorThreads; ++i) {
        threads.push_back(std::thread([&processor]() {
            processor.run(kDurationSeconds);
        }));
    }

    auto start = std::chrono::steady_clock::now();
    auto end = start + std::chrono::seconds(kDurationSeconds);

    std::vector<Event> batch;
    batch.reserve(kBatchSize);

    int64_t timestamp = 0;
    while (std::chrono::steady_clock::now() < end) {
        batch.clear();
        for (int i = 0; i < kBatchSize; ++i) {
            const int64_t instrument_id = instrument_dist(rng);
            const double price = price_dist(rng);
            batch.emplace_back(instrument_id, price, ++timestamp);
        }

        for (const auto& event : batch) {
            ingestion.ingest(event.instrument_id, event.price, event.timestamp);
        }
    }

    queue.shutdown();

    for (auto& thread : threads) {
        thread.join();
    }

    Benchmarks::ensureFolder();

    std::ostringstream metrics;
    ingestion.report(metrics);
    processor.report(metrics);

    const std::string filename = Benchmarks::makeTimestampedFilename();
    Benchmarks::writeMetricsWithConsole(filename, metrics);

    return 0;
}
