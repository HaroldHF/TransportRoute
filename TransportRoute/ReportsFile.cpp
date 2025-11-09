#include "ReportsFile.h"
#include <fstream>

namespace transport {

    bool ReportsFile::appendLine(const std::string& path, const std::string& line) {
        std::ofstream out(path, std::ios::app);
        if (!out) return false;
        out << line << "\n";
        return true;
    }

    bool ReportsFile::appendStationsInOrder(const std::string& path, const std::vector<Station>& ordered) {
        std::ofstream out(path, std::ios::app);
        if (!out) return false;
        out << "Estaciones (in-order): ";
        bool first = true;
        for (const auto& s : ordered) {
            if (!first) out << ", ";
            out << s.id << " " << s.name;
            first = false;
        }
        out << "\n";
        return true;
    }

} // namespace transport
