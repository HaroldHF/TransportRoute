#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
#include <limits>
#include "Result.h"
#include "GraphView.h"

namespace transport {

    class Dijkstra {
    public:
        static PathResult shortestPath(const Graph& g, int src, int dst) {
            PathResult res; res.algo = "Dijkstra";
            if (!g.hasVertex(src) || !g.hasVertex(dst)) return res;

            std::unordered_map<int, double> dist;
            std::unordered_map<int, int> parent;

            struct Node { int v; double d; bool operator<(const Node& o) const { return d > o.d; } };
            std::priority_queue<Node> pq;

            dist[src] = 0.0;
            pq.push({ src, 0.0 });

            while (!pq.empty()) {
                auto [u, du] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                if (u == dst) break;
                forEachOpenNeighbor(g, u, [&](int v, double w) {
                    double nd = du + w;
                    if (!dist.count(v) || nd < dist[v]) {
                        dist[v] = nd; parent[v] = u; pq.push({ v, nd });
                    }
                    });
            }

            if (!dist.count(dst)) return res; // unreachable

            // reconstruir camino
            res.reachable = true;
            res.cost = dist[dst];
            for (int cur = dst; ; cur = parent[cur]) {
                res.path.push_back(cur);
                if (cur == src) break;
            }
            std::reverse(res.path.begin(), res.path.end());
            return res;
        }
    };

} // namespace transport
