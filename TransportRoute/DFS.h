#pragma once
#include <unordered_set>
#include "Result.h"
#include "GraphView.h"

namespace transport {

    class DFS {
        static void dfs(const Graph& g, int u, std::unordered_set<int>& vis, VisitResult& res) {
            vis.insert(u);
            res.order.push_back(u);
            forEachOpenNeighbor(g, u, [&](int v, double) {
                if (!vis.count(v)) dfs(g, v, vis, res);
                });
        }
    public:
        static VisitResult traverse(const Graph& g, int start) {
            VisitResult res; res.algo = "DFS";
            if (!g.hasVertex(start)) return res;
            std::unordered_set<int> vis;
            dfs(g, start, vis, res);
            return res;
        }
    };

} // namespace transport