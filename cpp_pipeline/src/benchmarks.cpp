#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "Benchmarks.h"

namespace {
const std::filesystem::path kBenchmarkDirectory{"benchmarks"};
}

void Benchmarks::ensureFolder() {
    std::filesystem::create_directories(kBenchmarkDirectory);
}

std::string Benchmarks::makeTimestampedFilename() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&t);

    std::ostringstream filename;
    filename << (kBenchmarkDirectory / "run_").string()
             << (local_tm.tm_year + 1900)  // year
             << std::setw(2) << std::setfill('0') << (local_tm.tm_mon + 1)  // month 1-12
             << std::setw(2) << std::setfill('0') << local_tm.tm_mday        // day
             << "_"
             << std::setw(2) << std::setfill('0') << local_tm.tm_hour       // hour
             << std::setw(2) << std::setfill('0') << local_tm.tm_min        // minute
             << std::setw(2) << std::setfill('0') << local_tm.tm_sec        // second
             << ".txt";  // txt file
    return filename.str();
}


void Benchmarks::writeMetrics(const std::string& filename, std::ostream& metrics) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open benchmarks file!");
    }
    out << metrics.rdbuf(); // dump the entire stream content into the file
    out.close();
}

void Benchmarks::writeMetricsWithConsole(const std::string& filename, std::ostringstream& metrics) {
    std::string data = metrics.str();   // copy the contents

    // Print to console
    std::cout << data << std::endl;

    // Write to file
    std::ofstream out(filename);
    if (!out.is_open()) throw std::runtime_error("Failed to open benchmarks file!");
    out << data;
    out.close();
}

