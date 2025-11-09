#pragma once
#include <string>
#include <vector>
#include "Station.h"

namespace transport {

    class ReportsFile {
    public:
        static bool appendLine(const std::string& path, const std::string& line);
        static bool appendStationsInOrder(const std::string& path, const std::vector<Station>& ordered);
    };

} // namespace transport
