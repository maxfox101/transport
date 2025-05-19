#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string_view>

#include "geo.h"

struct Stop {
    std::string name;
    Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<std::string> stops;
    bool is_circular;
};

class TransportCatalogue {
public:
    void AddStop(std::string_view name, Coordinates coords);
    void AddBus(std::string_view name, const std::vector<std::string>& stops, bool is_circular);

    const Stop* FindStop(std::string_view name) const;
    const Bus* FindBus(std::string_view name) const;

    const std::vector<std::string>& GetBusesForStop(std::string_view stop_name) const;
    size_t GetUniqueStopCount(std::string_view bus_name) const;
    double CalculateRouteLength(std::string_view bus_name) const;
    size_t GetStopCount(std::string_view bus_name) const;

private:
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Bus> buses_;
    std::unordered_map<std::string, std::vector<std::string>> stop_to_buses_;
};
