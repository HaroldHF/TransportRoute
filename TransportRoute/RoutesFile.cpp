#include "RoutesFile.h"
#include <fstream>
#include <sstream>

namespace transport {

    bool RoutesFile::load(const std::string& path, Graph& g) {
        std::ifstream in(path);
        if (!in) return false;
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            int u, v; double w;
            if (ss >> u >> v >> w) g.addEdge(u, v, w, false);
        }
        return true;
    }

    // Nota: guardado basico (solo recorre el mapa y vuelca aristas; puede duplicar u-v / v-u).
    // Para evitar duplicados, en el futuro podrias normalizar u<v antes de escribir.
    bool RoutesFile::save(const std::string& path, const Graph& g) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) return false;
        out << "# u v peso\n";
        for (const auto& [u, vec] : g.data()) {
            for (const auto& e : vec) {
                out << u << " " << e.to << " " << e.w << "\n";
            }
        }
        return true;
    }

    bool ClosuresFile::applyClosures(const std::string& path, Graph& g) {
        std::ifstream in(path);
        if (!in) return false;
        std::string line;
        bool any = false;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            int u, v; if (ss >> u >> v) { any = g.setClosed(u, v, true) || any; }
        }
        return any;
    }

} // namespace transport
