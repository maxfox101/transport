#include "transport_catalogue.h"

void TransportCatalogue::AddStop(const std::string_view name, Coordinates coord) {
    // Проверяем, есть ли уже такая остановка
    if (stops_.contains(Stop{std::string(name), coord, {}})) {
        return;
    }
    
    // Используем emplace вместо insert для создания элемента на месте
    auto [it, done] = stops_.emplace(Stop{std::string(name), coord, {}});
    
    // Также используем emplace для вставки в stops_ptr_
    stops_ptr_.emplace(it->name, &(*it));
}

void TransportCatalogue::AddBus(const std::string_view name, const std::deque<const Stop*> &stops_list) {
    // Создаём временный объект Bus с копированием stops_list
    // (поскольку параметр передаётся по константной ссылке, перемещение невозможно)
    Bus bus_to_add{std::string(name), stops_list};
    
    // Проверяем существование маршрута
    if (buses_.contains(bus_to_add)) {
        return;
    }
    
    // Вставляем с перемещением временного объекта (имя маршрута всё равно можно переместить)
    auto [it, done] = buses_.insert(std::move(bus_to_add));
    
    // Добавляем в указатели
    buses_ptr_.emplace(it->number, &(*it));
    
    // Обновляем информацию об автобусах для каждой остановки
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
    // Количество остановок
    int stops = bus.stop_list.size();
    
    // Уникальные остановки
    std::unordered_set<std::string_view> unique_stops;
    for (const Stop* i : bus.stop_list) {
        unique_stops.insert(i->name);
    }
    
    // Географическая длина маршрута
    double length = 0;
    Stop prev = *bus.stop_list[0];
    for (size_t i = 1; i < bus.stop_list.size(); ++i) {
        Stop cur = *bus.stop_list[i];
        length += ComputeDistance(prev.coordinates, cur.coordinates);
        prev = cur;
    }
    
    return BusInfo{stops, static_cast<int>(unique_stops.size()), length};
}
