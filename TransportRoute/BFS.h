#pragma once
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "Result.h"
#include "GraphView.h"

namespace transport {

    class BFS {
    public:
        static VisitResult traverse(const Graph& g, int start) {
            VisitResult res; res.algo = "BFS";
            if (!g.hasVertex(start)) return res;
            std::unordered_set<int> vis;
            std::queue<int> q;
            vis.insert(start); q.push(start);
            while (!q.empty()) {
                int u = q.front(); q.pop();
                res.order.push_back(u);
                forEachOpenNeighbor(g, u, [&](int v, double) {
                    if (!vis.count(v)) { vis.insert(v); q.push(v); }
                    });
            }
            return res;
        }
    };

} // namespace transport