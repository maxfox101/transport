#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <iomanip>

void ParseAndPrintStat(const TransportCatalogue& catalogue,
                       std::string_view request,
                       std::ostream& output) {
    std::istringstream ss{std::string{request}};
    std::string command;

    if (!(ss >> command)) {
        return;
    }

    if (command == "Stop") {
        std::string id;
        std::getline(ss >> std::ws, id);

        auto buses = catalogue.GetBusesForStop(id);
        if (buses.empty()) {
            output << "Stop " << id << ": no buses" << std::endl;
        } else {
            output << "Stop " << id << ": buses";
            for (const auto& bus : buses) {
                output << " " << bus;
            }
            output << std::endl;
        }
    } else if (command == "Bus") {
        std::string id;
        std::getline(ss >> std::ws, id);

        const auto bus = catalogue.FindBus(id);
        if (!bus) {
            output << "Bus " << id << ": not found" << std::endl;
        } else {
            output << "Bus " << id << ": " 
                   << catalogue.GetStopCount(id) << " stops on route, "
                   << catalogue.GetUniqueStopCount(id) << " unique stops, "
                   << std::setprecision(6) << catalogue.CalculateRouteLength(id) << " route length"
                   << std::endl;
        }
    }
}
