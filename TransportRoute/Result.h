#pragma once
#include <vector>
#include <limits>
#include <string>

namespace transport {

    struct PathResult {
        std::vector<int> path;   // secuencia de vertices
        double cost = std::numeric_limits<double>::infinity(); // suma de pesos
        bool reachable = false;
        std::string algo;        // nombre del algoritmo (debug/log)
    };

    struct VisitResult {
        std::vector<int> order;  // orden de visita (BFS/DFS)
        std::string algo;
    };

    struct MSTResult {
        std::vector<std::pair<int, int>> edges; // aristas (u,v) del arbol
        double totalWeight = 0.0;
        std::string algo;
    };

} // namespace transport