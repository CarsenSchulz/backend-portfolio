#include "EventQueue.h"
#include "ingestion.h"
#include <iostream>

int main() {
    EventQueue queue(5);
    Ingestion ingestion(queue);

    // feed some demo data
    ingestion.ingest(1, 100.5, 1000);
    ingestion.ingest(2, 200.0, 1001);
    ingestion.ingest(1, 101.0, 999);  // should fail timestamp
    ingestion.ingest(3, -5.0, 1002);  // should fail price

    // dequeue and print events
    while (auto e = queue.dequeue()) {
        std::cout << "Event: " << e->instrument_id << ", " << e->price << ", " << e->timestamp << "\n";
    }
    return 0;
}
