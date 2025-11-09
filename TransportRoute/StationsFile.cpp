#include "StationsFile.h"
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
            std::string idStr, name;
            if (std::getline(ss, idStr, ';') && std::getline(ss, name)) {
                int id = std::stoi(idStr);
                out.emplace_back(id, name);
            }
        }
        return out;
    }

    bool StationsFile::save(const std::string& path, const std::vector<Station>& stations) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) return false;
        out << "# id;nombre\n";
        for (const auto& s : stations) out << s.id << ";" << s.name << "\n";
        return true;
    }

    void StationsFile::loadIntoBST(const std::string& path, BST<Station>& bst) {
        for (const auto& s : load(path)) bst.insert(s);
    }

} // namespace transport