#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string_view name, Coordinates coord) {
    if (stops_.contains(Stop{std::string(name), coord, {}})) {
        return;
    }
    auto [it, done] = stops_.emplace(Stop{std::string(name), coord, {}});
    stops_ptr_.emplace(it->name, &(*it));
}

void TransportCatalogue::AddBus(const std::string_view name, const std::deque<const Stop*> &stops_list) {
    Bus bus_to_add{std::string(name), stops_list};
    if (buses_.contains(bus_to_add)) {
        return;
    }
    auto [it, done] = buses_.insert(std::move(bus_to_add));
    buses_ptr_.emplace(it->number, &(*it));

    for (const Stop* stop : it->stop_list) {
        const_cast<Stop*>(stop)->buses.insert(std::string_view(it->number));
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

BusInfo TransportCatalogue::GetBusInfo(const Bus& bus) const {
    if (bus.stop_list.empty()) {
        return {};
    }

    int stops = static_cast<int>(bus.stop_list.size());
    std::unordered_set<std::string_view> unique_stops;
    double real_length = 0;
    double geo_length = 0;

    for (size_t i = 1; i < bus.stop_list.size(); ++i) {
        const Stop* a = bus.stop_list[i - 1];
        const Stop* b = bus.stop_list[i];

        unique_stops.insert(a->name);
        unique_stops.insert(b->name);

        real_length += GetDistance(a, b);
        geo_length += ComputeDistance(a->coordinates, b->coordinates);
    }

    double curvature = (geo_length == 0) ? 0 : real_length / geo_length;

    return BusInfo{
        stops,
        static_cast<int>(unique_stops.size()),
        real_length,
        curvature
    };
}

void TransportCatalogue::SetDistance(const Stop* from, const Stop* to, int distance) {
    distances_[{from, to}] = distance;
}

int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
    auto it = distances_.find({from, to});
    if (it != distances_.end()) {
        return it->second;
    }

    it = distances_.find({to, from});
    if (it != distances_.end()) {
        return it->second;
    }

    return ComputeDistance(from->coordinates, to->coordinates);
}
