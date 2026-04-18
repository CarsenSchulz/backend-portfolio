#pragma once
#include "EventQueue.h"

#include <array>
#include <cstdint>
#include <ostream>

constexpr int NUM_INSTRUMENTS = 100;
constexpr int ID_MAX = NUM_INSTRUMENTS;

class Ingestion {
    EventQueue& queue;

    std::array<int64_t, ID_MAX> lastTimestamps{};
    std::array<bool, ID_MAX> seen{};
    int validation_fails = 0;
    int dropped_events = 0;

    bool validateInstrument(int64_t instrument_id) const;
    bool validatePrice(double price) const;
    bool validateTimestamp(int64_t instrument_id, int64_t timestamp) const;

public:
    explicit Ingestion(EventQueue& q);

    bool ingest(int64_t instrument_id, double price, int64_t timestamp);
    void report(std::ostream& os) const;
};
