#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string_view name, Coordinates coord) {
    if(stops_.contains(Stop{std::string(name), coord, {}})) {
        return;
    }
    auto [it, done] = stops_.insert(Stop{std::string(name), coord, {}});
    stops_ptr_.insert({it->name, &(*it)});
}

void TransportCatalogue::AddBus(const std::string_view name, const std::deque<const Stop*> &stops_list) {
    if(buses_.contains(Bus{std::string(name), stops_list})) {
        return;
    }
    auto [it, done] = buses_.insert(Bus{std::string(name), stops_list});
    buses_ptr_.insert({it->number, &(*it)});
    for(const Stop *i : stops_list) {
        const_cast<Stop *>(i)->buses.insert(std::string_view(it->number));
    }
}

const Stop* TransportCatalogue::FindStop(std::string_view str) const {
    auto it = stops_ptr_.find(str);
    return it != stops_ptr_.end() ? it->second : nullptr;
}

const Bus* TransportCatalogue::FindBus(std::string_view num) const {
    auto it = buses_ptr_.find(num);
    return it != buses_ptr_.end() ? it->second : nullptr;
}

BusInfo TransportCatalogue::GetBusInfo(const Bus &bus) const {
    int stops = bus.stop_list.size();
    std::unordered_set<std::string_view> unique_stops;
    for(const Stop *i : bus.stop_list) {
        unique_stops.insert(i->name);
    }
    double length = 0;
    Stop prev = *bus.stop_list[0];
    for(size_t i = 1; i < bus.stop_list.size(); ++i) {
        Stop cur = *bus.stop_list[i];
        length += ComputeDistance(prev.coordinates, cur.coordinates);
        prev = cur;
    }
    return BusInfo{stops, static_cast<int>(unique_stops.size()), length};
}
