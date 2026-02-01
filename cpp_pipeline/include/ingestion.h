#pragma once
#include "EventQueue.h"
#include <cstdint>
#include <array>

constexpr int NUM_INSTRUMENTS = 100; // total instruments in synthetic load
constexpr int ID_MAX = NUM_INSTRUMENTS; // maximum valid instrument ID

class Ingestion {
    EventQueue& queue; //reference to queue

    std::array<int64_t, ID_MAX> lastTimestamps{}; // Tracks latest timestamp of specific ID, based on index
    std::array<bool, ID_MAX> seen{};  // Tracks if instrument_id has been seen, needed so first entry passes validation

    bool validateInstrument(int64_t instrument_id) const; // Checks if instrument_id is valid
    bool validatePrice(double price) const; // Checks if price is valid
    bool validateTimestamp(int64_t instrument_id, int64_t timestamp) const; // Checks if timestamp is valid

public:
    explicit Ingestion(EventQueue& q); // Constructor
    
    bool ingest(int64_t instrument_id, double price, int64_t timestamp); // Returns true if event was successfully ingested, false if queue full
};
