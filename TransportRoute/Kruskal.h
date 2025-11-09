#pragma once
#include <vector>
#include <tuple>
#include <algorithm>
#include "DisjointSet.h"
#include "Result.h"
#include "GraphView.h"

namespace transport {

    class Kruskal {
    public:
        static MSTResult mst(const Graph& g) {
            MSTResult res; res.algo = "Kruskal";
            // recolectar aristas abiertas u<v para no duplicar
            std::vector<std::tuple<double, int, int>> edges;
            for (const auto& [u, vec] : g.data()) {
                for (const auto& e : vec) {
                    if (e.closed) continue;
                    if (u < e.to) edges.emplace_back(e.w, u, e.to);
                }
            }
            std::sort(edges.begin(), edges.end(),
                [](auto& a, auto& b) { return std::get<0>(a) < std::get<0>(b); });

            DisjointSet ds;
            for (const auto& [u, _] : g.data()) ds.makeSet(u);

            for (const auto& [w, u, v] : edges) {
                if (ds.unite(u, v)) {
                    res.edges.emplace_back(u, v);
                    res.totalWeight += w;
                }
            }
            return res;
        }
    };

} // namespace transport
