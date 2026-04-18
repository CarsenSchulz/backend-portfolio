#pragma once
#include <filesystem>
#include <ostream>
#include <sstream>
#include <string>

class Benchmarks {
public:
    // Ensure the local benchmarks folder exists for run output files.
    static void ensureFolder();

    // Generate a unique timestamped filename
    static std::string makeTimestampedFilename();

    // Write metrics from a stream into the CSV file
    static void writeMetrics(const std::string& filename, std::ostream& metrics);

    // Optional helper to write metrics and also echo to console
    static void writeMetricsWithConsole(const std::string& filename, std::ostringstream& metrics);
};
