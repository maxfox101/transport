#include "stat_reader.h"
#include <ostream>

struct Request {
    std::string_view type;
    std::string_view description;
};

Request ParseRequest(std::string_view req) {
    const size_t space_pos = req.find(' ');
    return {
        req.substr(0, space_pos),
        space_pos != req.npos ? req.substr(space_pos + 1) : ""
    };
}

void ProcessBusRequest(const TransportCatalogue& catalogue, 
                      std::string_view description,
                      std::ostream& output) {
    const Bus* bus = catalogue.FindBus(description);
    if (!bus) {
        output << "not found\n";
        return;
    }
    const BusInfo info = catalogue.GetBusInfo(*bus);
    output << info.stops << " stops on route, "
           << info.unique_stops << " unique stops, "
           << info.length << " route length\n";
}

void ProcessStopRequest(const TransportCatalogue& catalogue,
                       std::string_view description,
                       std::ostream& output) {
    const Stop* stop = catalogue.FindStop(description);
    if (!stop) {
        output << "not found\n";
        return;
    }
    if (stop->buses.empty()) {
        output << "no buses\n";
        return;
    }
    output << "buses";
    for (const auto& bus : stop->buses) {
        output << " " << bus;
    }
    output << "\n";
}

void ParseAndPrintStat(const TransportCatalogue& catalogue, 
                      std::string_view request,
                      std::ostream& output) {
    const Request req = ParseRequest(request);
    output << req.type << " " << req.description << ": ";
    
    if (req.type == "Bus") {
        ProcessBusRequest(catalogue, req.description, output);
    } 
    else if (req.type == "Stop") {
        ProcessStopRequest(catalogue, req.description, output);
    }
}
