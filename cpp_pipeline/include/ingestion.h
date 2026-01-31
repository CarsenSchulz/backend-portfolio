#pragma once
#include "events.h"
#include "EventQueue.h"
#include <cstdint>
#include <unordered_map>

// Ingestion handles receiving raw event data, validating it, and sending it to the queue
class Ingestion {
private:
    EventQueue& queue;  // Reference to the main event queue
    std::unordered_map<int64_t, int64_t> lastTimestamps; // Track last timestamp per instrument
    const int64_t ID_MAX = 5; // Maximum ID for intrument

    // Validation helpers
    bool validateInstrument(int64_t instrument_id);
    bool validatePrice(double price);
    bool validateTimestamp(int64_t instrument_id, int64_t timestamp);

public:
    // Constructor: pass the queue to send validated events to
    Ingestion(EventQueue& q);

    // Receive raw inputs, validate, and enqueue if valid
    // Returns true if successfully enqueued, false if invalid or queue full
    bool ingest(int64_t instrument_id, double price, int64_t timestamp);
};
