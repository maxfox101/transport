#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string_view name, Coordinates coords) {
    stops_[std::string(name)] = {std::string(name), coords};
}

void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string>& stops, bool is_circular) {
    buses_[std::string(name)] = {std::string(name), stops, is_circular};

    for (const auto& stop : stops) {
        stop_to_buses_[stop].push_back(std::string(name));
    }
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    auto it = stops_.find(std::string(name));
    return it != stops_.end() ? &it->second : nullptr;
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    auto it = buses_.find(std::string(name));
    return it != buses_.end() ? &it->second : nullptr;
}

const std::vector<std::string>& TransportCatalogue::GetBusesForStop(std::string_view stop_name) const {
    static const std::vector<std::string> empty;
    auto it = stop_to_buses_.find(std::string(stop_name));
    return it != stop_to_buses_.end() ? it->second : empty;
}

size_t TransportCatalogue::GetUniqueStopCount(std::string_view bus_name) const {
    auto it = buses_.find(std::string(bus_name));
    if (it == buses_.end()) return 0;
    
    std::unordered_set<std::string_view> unique_stops;
    for (const auto& stop : it->second.stops) {
        unique_stops.insert(stop);
    }
    return unique_stops.size();
}

double TransportCatalogue::CalculateRouteLength(std::string_view bus_name) const {
    auto it = buses_.find(std::string(bus_name));
    if (it == buses_.end()) return 0.0;
    
    const auto& bus = it->second;
    double total_length = 0.0;
    
    for (size_t i = 0; i < bus.stops.size() - 1; ++i) {
        const auto from = FindStop(bus.stops[i]);
        const auto to = FindStop(bus.stops[i+1]);
        if (from && to) {
            total_length += ComputeDistance(from->coordinates, to->coordinates);
        }
    }
    
    if (!bus.is_circular) {
        for (size_t i = bus.stops.size() - 1; i > 0; --i) {
            const auto from = FindStop(bus.stops[i]);
            const auto to = FindStop(bus.stops[i-1]);
            if (from && to) {
                total_length += ComputeDistance(from->coordinates, to->coordinates);
            }
        }
    }
    
    return total_length;
}

size_t TransportCatalogue::GetStopCount(std::string_view bus_name) const {
    auto it = buses_.find(std::string(bus_name));
    if (it == buses_.end()) return 0;
    
    if (it->second.is_circular) {
        return it->second.stops.size();
    } else {
        return it->second.stops.size() * 2 - 1;
    }
}
