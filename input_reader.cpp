#include "input_reader.h"

#include <sstream>
#include <algorithm>
#include <vector>
#include <string_view>
#include <cmath>

using namespace std;

string_view Trim(string_view s) {
    size_t start = s.find_first_not_of(' ');
    if (start == string_view::npos)
        return {};
    size_t end = s.find_last_not_of(' ');
    return s.substr(start, end - start + 1);
}

vector<string_view> Split(string_view str, char delimiter) {
    vector<string_view> result;
    size_t start = 0;

    while (true) {
        size_t end = str.find(delimiter, start);
        if (end == string_view::npos) {
            break;
        }
        if (end > start) {
            result.push_back(str.substr(start, end - start));
        }
        start = end + 1;
    }

    if (start < str.size()) {
        result.push_back(str.substr(start));
    }

    return result;
}

Coordinates ParseCoordinates(string_view str) {
    auto parts = Split(str, ',');
    if (parts.size() != 2) {
        return {NAN, NAN};
    }

    double lat = stod(string(parts[0]));
    double lng = stod(string(parts[1]));

    return {lat, lng};
}

CommandDescription ParseCommandDescription(string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == string_view::npos)
        return {};

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos || space_pos == string_view::npos)
        return {};

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos || not_space == string_view::npos)
        return {};

    return {
        string(line.substr(0, space_pos)),
        string(line.substr(not_space, colon_pos - not_space)),
        string(line.substr(colon_pos + 1))
    };
}

void InputReader::ParseLine(string_view line) {
    auto cmd = ParseCommandDescription(line);
    if (cmd) {
        commands_.push_back(move(cmd));
    }
}

vector<string_view> ParseRoute(string_view route_desc) {
    if (route_desc.find(" > ") != string_view::npos) {
        return Split(route_desc, '>');
    }

    auto stops = Split(route_desc, '-');
    vector<string_view> full_route(stops.begin(), stops.end());

    for (auto it = stops.rbegin() + 1; it != stops.rend(); ++it) {
        full_route.push_back(*it);
    }

    return full_route;
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
    for (const auto& cmd : commands_) {
        if (cmd.command == "Stop") {
            catalogue.AddStop(cmd.id, ParseCoordinates(cmd.description));
        } else if (cmd.command == "Bus") {
            bool is_circular = cmd.description.find('>') != string::npos;
            auto stops = ParseRoute(cmd.description);
            vector<string> stop_names;
            for (auto stop : stops) {
                stop_names.emplace_back(Trim(stop));
            }
            catalogue.AddBus(cmd.id, stop_names, is_circular);
        }
    }
}
