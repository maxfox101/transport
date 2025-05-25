#include "stat_reader.h"
#include <ostream>

struct Request {
    std::string_view type;
    std::string_view description;
};

Request ParseRequest(std::string_view req) {
    size_t space_pos = req.find(' ');
    std::string_view type = req.substr(0, space_pos);
    std::string_view description = req.substr(space_pos + 1);
    return Request{type, description};
}

void ParseAndPrintStat(const TransportCatalogue& catalogue, std::string_view request,
                      std::ostream& output) {
    Request req = ParseRequest(request);
    output << req.type << " " << req.description << ": ";
    
    if(req.type == "Bus") {
        const Bus *bus = catalogue.FindBus(req.description);
        if(!bus) {
            output << "not found" << std::endl;
            return;    
        }
        BusInfo info = catalogue.GetBusInfo(*bus);
        output << info.stops << " stops on route, " << info.unique_stops << " unique stops, "
                << info.length << " route length" << std::endl;
    }
    
    if(req.type == "Stop") {
        const Stop *stop = catalogue.FindStop(req.description);
        if(!stop) {
            output << "not found" << std::endl;
            return;
        }
        if(stop->buses.empty()) {
            output << "no buses" << std::endl;
            return;
        }
        output << "buses";
        for(auto i : stop->buses) {
            output << " " << i;
        }
        output << std::endl;
    }
}
