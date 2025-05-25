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
    using namespace std;
    if (from == to) {
        return 0;
    }
    
    const double dr = 3.1415926535 / 180.0;
    const double lat1 = from.lat * dr;
    const double lat2 = to.lat * dr;
    const double dlng = (to.lng - from.lng) * dr;

    return 6371000 * acos(
        sin(lat1) * sin(lat2) + 
        cos(lat1) * cos(lat2) * cos(dlng)
    );
}
