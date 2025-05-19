#pragma once

#include <cmath>

struct Coordinates {
    double lat;
    double lng;

    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
};

inline double ComputeDistance(Coordinates from, Coordinates to) {
    if (from == to) {
        return 0;
    }

    const double R = 6371000;
    const double deg_to_rad = M_PI / 180.0;

    double lat1 = from.lat * deg_to_rad;
    double lat2 = to.lat * deg_to_rad;
    double delta_lat = (to.lat - from.lat) * deg_to_rad;
    double delta_lng = (to.lng - from.lng) * deg_to_rad;

    double a = std::sin(delta_lat / 2) * std::sin(delta_lat / 2)
               + std::cos(lat1) * std::cos(lat2)
                     * std::sin(delta_lng / 2) * std::sin(delta_lng / 2);

    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    return R * c;
}
