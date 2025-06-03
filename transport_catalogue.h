#pragma once
#include "geo.h"
#include <utility>
#include <string>
#include <string_view>
#include <deque>
#include <unordered_set>
#include <set>
#include <unordered_map>

struct Stop {
    bool operator==(const Stop &v) const {
        return name == v.name && coordinates == v.coordinates;
    }
    std::string name;
    Coordinates coordinates;
    std::set<std::string_view> buses;
};

struct Bus {
    bool operator==(const Bus &v) const {
        return number == v.number && stop_list == v.stop_list;
    }
    std::string number;
    std::deque<const Stop*> stop_list;
};

struct BusInfo {
    int stops;
    int unique_stops;
    double length;
    double curvature;  // новое поле
};

class TransportCatalogue {
public:
    TransportCatalogue() = default;
    void AddStop(const std::string_view name, Coordinates coord);
    void AddBus(const std::string_view name, const std::deque<const Stop*> &stops_list);
    const Stop* FindStop(std::string_view str) const;
    const Bus* FindBus(std::string_view num) const;
    BusInfo GetBusInfo(const Bus &bus) const;

    // Новые методы
    void SetDistance(const Stop* from, const Stop* to, int distance);
    int GetDistance(const Stop* from, const Stop* to) const;

private:
    struct PairPointerHash {
        template <typename T>
        std::size_t operator()(const std::pair<T, T>& p) const {
            return std::hash<void*>()((void*)p.first) ^ std::hash<void*>()((void*)p.second);
        }
    };

    struct StopHash {
        size_t operator() (const Stop &v) const {
            size_t ret = 0;
            for(size_t i = 0; i < v.name.size(); ++i) {
                ret += (v.name[i] - 'A') * std::pow(7, i);
            }
            ret *= 1'000'000'000;
            ret += v.coordinates.lat * 100'000'000 + v.coordinates.lng * 1'000'000'000;
            return ret;
        }
    };

    struct BusHash {
        size_t operator() (const Bus &v) const {
            size_t ret = 0;
            for(size_t i = 0; i < v.number.size(); ++i) {
                ret += (v.number[i] - 'A') * std::pow(7, i);
            }
            return ret;
        }
    };

    std::unordered_set<Stop, StopHash> stops_;
    std::unordered_map<std::string_view, const Stop *> stops_ptr_;
    std::unordered_set<Bus, BusHash> buses_;
    std::unordered_map<std::string_view, const Bus *> buses_ptr_;

    std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairPointerHash> distances_;
};
