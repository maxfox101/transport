#include "input_reader.h"
#include <algorithm>
#include <cassert>
#include <iterator>

namespace {

Coordinates ParseCoordinateData(std::string_view coord_str) {
    static const double nan = std::nan("");
    const auto first_char = coord_str.find_first_not_of(' ');
    const auto comma_pos = coord_str.find(',');
    if (comma_pos == coord_str.npos)
        return {nan, nan};

    const auto second_num_start = coord_str.find_first_not_of(' ', comma_pos + 1);
    if (second_num_start == coord_str.npos)
        return {nan, nan};

    try {
        const double latitude = std::stod(std::string(coord_str.substr(first_char, comma_pos - first_char)));
        const double longitude = std::stod(std::string(coord_str.substr(second_num_start)));
        return {latitude, longitude};
    } catch (...) {
        return {nan, nan};
    }
}

std::string_view TrimWhitespace(std::string_view text) {
    const auto start = text.find_first_not_of(' ');
    if (start == std::string_view::npos)
        return "";
    const auto end = text.find_last_not_of(' ');
    return text.substr(start, end - start + 1);
}

std::vector<std::string_view> SplitByDelimiter(std::string_view text, char delimiter) {
    std::vector<std::string_view> parts;
    size_t current_pos = 0;

    while (current_pos < text.size()) {
        current_pos = text.find_first_not_of(' ', current_pos);
        if (current_pos == std::string_view::npos)
            break;

        const auto delim_pos = text.find(delimiter, current_pos);
        const auto substring = TrimWhitespace(text.substr(current_pos, delim_pos - current_pos));

        if (!substring.empty()) {
            parts.push_back(substring);
        }

        current_pos = delim_pos == std::string_view::npos ? text.size() : delim_pos + 1;
    }

    return parts;
}

std::vector<std::string_view> ProcessRoute(std::string_view route_desc) {
    const bool is_circular = route_desc.find('>') != std::string_view::npos;
    auto stops = SplitByDelimiter(route_desc, is_circular ? '>' : '-');

    if (!is_circular) {
        stops.insert(stops.end(), std::next(stops.rbegin()), stops.rend());
    }

    return stops;
}

std::pair<std::string_view, int> ParseDistance(std::string_view dist_str) {
    size_t m_pos = dist_str.find('m');
    if (m_pos == dist_str.npos) {
        return {"", 0};
    }

    int distance = 0;
    try {
        distance = std::stoi(std::string(dist_str.substr(0, m_pos)));
    } catch (...) {
        return {"", 0};
    }

    std::string_view stop_name = TrimWhitespace(dist_str.substr(m_pos + 1));
    return {stop_name, distance};
}

void ParseAndAddDistances(TransportCatalogue& catalogue, std::string_view stop_name, std::string_view distances_str) {
    const Stop* from = catalogue.FindStop(stop_name);
    if (!from)
        return;

    size_t current_pos = 0;
    while (current_pos < distances_str.size()) {
        size_t comma_pos = distances_str.find(',', current_pos);
        if (comma_pos == distances_str.npos)
            comma_pos = distances_str.size();

        auto part = TrimWhitespace(distances_str.substr(current_pos, comma_pos - current_pos));
        auto [to_name, distance] = ParseDistance(part);

        if (!to_name.empty()) {
            const Stop* to = catalogue.FindStop(to_name);
            if (to) {
                catalogue.SetDistance(from, to, distance);
            }
        }

        current_pos = comma_pos + 1;
    }
}

CommandDescription InterpretCommand(std::string_view command_line) {
    const auto colon_pos = command_line.find(':');
    if (colon_pos == command_line.npos)
        return {};

    const auto cmd_end_pos = command_line.find(' ');
    if (cmd_end_pos >= colon_pos)
        return {};

    const auto id_start_pos = command_line.find_first_not_of(' ', cmd_end_pos);
    if (id_start_pos >= colon_pos)
        return {};

    return {
        std::string(command_line.substr(0, cmd_end_pos)),
        std::string(command_line.substr(id_start_pos, colon_pos - id_start_pos)),
        std::string(command_line.substr(colon_pos + 1))
    };
}

} // namespace

void InputReader::ParseLine(std::string_view line) {
    if (auto cmd = InterpretCommand(line)) {
        commands_.push_back(std::move(cmd));
    }
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
    for (const auto& cmd : commands_) {
        if (cmd.command == "Stop") {
            size_t comma_pos = cmd.description.find(',');
            if (comma_pos == std::string_view::npos)
                continue;

            size_t next_comma = cmd.description.find(',', comma_pos + 1);
            if (next_comma == std::string_view::npos) {
                auto coords = ParseCoordinateData(cmd.description);
                catalogue.AddStop(cmd.id, coords);
            } else {
                auto coords = ParseCoordinateData(cmd.description.substr(0, next_comma));
                catalogue.AddStop(cmd.id, coords);
                ParseAndAddDistances(catalogue, cmd.id, cmd.description.substr(next_comma + 1));
            }
        }
    }

    for (const auto& cmd : commands_) {
        if (cmd.command == "Bus") {
            auto stop_points = ProcessRoute(cmd.description);
            std::deque<const Stop*> stops;
            for (auto stop_name : stop_points) {
                if (auto stop = catalogue.FindStop(stop_name)) {
                    stops.push_back(stop);
                }
            }
            catalogue.AddBus(cmd.id, stops);
        }
    }
}
