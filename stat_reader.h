#pragma once

#include <string_view>
#include <iostream>

#include "transport_catalogue.h"

void ParseAndPrintStat(const TransportCatalogue& catalogue,
                       std::string_view request,
                       std::ostream& output);
