#pragma once
#include "Graph.h"

namespace transport {

    // Pequeño adaptador para iterar vecinos ignorando aristas cerradas
    inline const std::vector<Graph::AdjEdge>& neighborsRaw(const Graph& g, int u) {
        return g.neighbors(u);
    }

    template <typename Fn>
    inline void forEachOpenNeighbor(const Graph& g, int u, Fn fn) {
        for (const auto& e : neighborsRaw(g, u)) {
            if (!e.closed) fn(e.to, e.w);
        }
    }

} // namespace transport