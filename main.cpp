#include <iostream>
#include <string>
#include <string_view>

#include "input_reader.h"
#include "stat_reader.h"

int main() {
    TransportCatalogue catalogue;

    int base_request_count;
    std::cin >> base_request_count >> std::ws;

    {
        InputReader reader;
        for (int i = 0; i < base_request_count; ++i) {
            std::string line;
            std::getline(std::cin >> std::ws, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    std::cin >> stat_request_count >> std::ws;

    for (int i = 0; i < stat_request_count; ++i) {
        std::string line;
        std::getline(std::cin >> std::ws, line);
        ParseAndPrintStat(catalogue, line, std::cout);
    }

    return 0;
}
