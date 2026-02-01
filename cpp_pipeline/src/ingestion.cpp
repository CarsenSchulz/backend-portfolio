#include "ingestion.h"
#include <cmath>

Ingestion::Ingestion(EventQueue& q)
    : queue(q) {}

bool Ingestion::validateInstrument(int64_t instrument_id) const 
{
    return 1 <= instrument_id <= ID_MAX;
}

bool Ingestion::validatePrice(double price) const 
{
    return price >= 0 && std::isfinite(price);
}

bool Ingestion::validateTimestamp(int64_t instrument_id, int64_t timestamp) const 
{
    int idx = instrument_id - 1;
    return !seen[idx] || timestamp > lastTimestamps[idx];
}

bool Ingestion::ingest(int64_t instrument_id, double price, int64_t timestamp) 
{
    if (!validateInstrument(instrument_id) ||
        !validatePrice(price) ||
        !validateTimestamp(instrument_id, timestamp))
    {
        return false; // failed validation
    }

    int idx = instrument_id - 1;
    lastTimestamps[idx] = timestamp;
    seen[idx] = true;

    Event event(instrument_id, price, timestamp);
    return queue.enqueue(event); // copy into queue, drops event if queue full
}
