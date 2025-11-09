#pragma once
#include <queue>
#include <unordered_set>
#include "Result.h"
#include "GraphView.h"

namespace transport {

    class Prim {
    public:
        // arranca desde 'start'; si el grafo es desconectado, genera MST del componente
        static MSTResult mst(const Graph& g, int start) {
            MSTResult res; res.algo = "Prim";
            if (!g.hasVertex(start)) return res;

            struct Item { double w; int u; int v; bool operator<(const Item& o) const { return w > o.w; } };
            std::priority_queue<Item> pq;
            std::unordered_set<int> in;

            auto pushEdges = [&](int u) {
                forEachOpenNeighbor(g, u, [&](int v, double w) {
                    if (!in.count(v)) pq.push({ w,u,v });
                    });
                };

            in.insert(start);
            pushEdges(start);

            while (!pq.empty()) {
                auto [w, u, v] = pq.top(); pq.pop();
                if (in.count(v)) continue;
                in.insert(v);
                res.edges.emplace_back(u, v);
                res.totalWeight += w;
                pushEdges(v);
            }
            return res;
        }
    };

} // namespace transport
