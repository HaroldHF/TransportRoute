#include "StationsFile.h"
#include <fstream>
#include <sstream>

namespace transport {

    std::vector<Station> StationsFile::load(const std::string& path) {
        std::ifstream in(path);
        std::vector<Station> out;
        if (!in) return out;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            std::string idStr, name, xStr, yStr;

            // Leer id;nombre;x;y
            if (std::getline(ss, idStr, ';') &&
                std::getline(ss, name, ';') &&
                std::getline(ss, xStr, ';') &&
                std::getline(ss, yStr)) {

                int id = std::stoi(idStr);
                double x = std::stod(xStr);
                double y = std::stod(yStr);
                out.emplace_back(id, name, x, y);
            }
        }
        return out;
    }

    bool StationsFile::save(const std::string& path, const std::vector<Station>& stations) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) return false;
        out << "# id;nombre;x;y\n";
        for (const auto& s : stations) {
            out << s.id << ";" << s.name << ";" << s.x << ";" << s.y << "\n";
        }
        return true;
    }

    void StationsFile::loadIntoBST(const std::string& path, BST<Station>& bst) {
        for (const auto& s : load(path)) bst.insert(s);
    }

} // namespace transport