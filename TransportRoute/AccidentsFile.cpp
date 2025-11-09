#include "AccidentsFile.h"
#include <fstream>
#include <sstream>

namespace transport {
    bool AccidentsFile::apply(const std::string& path, Graph& g) {
        std::ifstream in(path);
        if (!in) return false;
        std::string line; bool any = false;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line); int u, v; double delta;
            if (!(ss >> u >> v >> delta)) continue;

            // ajustar en ambas listas
            auto& du = const_cast<std::vector<Graph::AdjEdge>&>(g.neighbors(u));
            for (auto& e : du) if (e.to == v && !e.closed) { e.w += delta; any = true; }
            auto& dv = const_cast<std::vector<Graph::AdjEdge>&>(g.neighbors(v));
            for (auto& e : dv) if (e.to == u && !e.closed) { e.w += delta; any = true; }
        }
        return any;
    }
}
