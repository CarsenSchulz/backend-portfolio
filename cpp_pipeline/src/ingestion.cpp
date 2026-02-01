#include "ingestion.h"
#include <cmath>
#include <iostream>


Ingestion::Ingestion(EventQueue& q)
    : queue(q) {}

bool Ingestion::validateInstrument(int64_t instrument_id)
{
    if (instrument_id <= 0 || instrument_id > ID_MAX) return false;
    return true;
}

bool Ingestion::validatePrice(double price)
{
    if (price < 0 || !std::isfinite(price)) return false;
    return true;
}

bool Ingestion::validateTimestamp(int64_t instrument_id, int64_t timestamp)
{
    auto it = lastTimestamps.find(instrument_id);
    if (it != lastTimestamps.end())
    {
        if (timestamp <= it->second) return false;
    }
    return true;
}

bool Ingestion::ingest(int64_t instrument_id, double price, int64_t timestamp)
{
    if (!validateInstrument(instrument_id) || !validatePrice(price) || !validateTimestamp(instrument_id, timestamp))
    {
        return false;
    }

    lastTimestamps[instrument_id] = timestamp;

    Event event(instrument_id, price, timestamp);

    return queue.enqueue(event);
}