#pragma once
#include <unordered_map>
#include <vector>
#include <limits>
#include "Result.h"
#include "GraphView.h"

namespace transport {

    class FloydWarshall {
    public:
        struct AllPairs {
            // map de indice compacto -> id de vertice y viceversa
            std::vector<int> idOf;                      // idx -> vertexId
            std::unordered_map<int, int> idxOf;          // vertexId -> idx
            std::vector<std::vector<double>> dist;      // NxN
            std::vector<std::vector<int>> next;         // NxN (indice del siguiente)

            PathResult path(int srcId, int dstId) const {
                PathResult res; res.algo = "FloydWarshall";
                auto itS = idxOf.find(srcId), itD = idxOf.find(dstId);
                if (itS == idxOf.end() || itD == idxOf.end()) return res;
                int s = itS->second, t = itD->second;
                if (next[s][t] == -1) return res;
                res.reachable = true;
                res.cost = dist[s][t];
                int u = s;
                res.path.push_back(idOf[u]);
                while (u != t) {
                    u = next[u][t];
                    res.path.push_back(idOf[u]);
                }
                return res;
            }
        };

        static AllPairs compute(const Graph& g) {
            // compactar ids
            std::vector<int> ids;
            ids.reserve(g.data().size());
            for (const auto& kv : g.data()) ids.push_back(kv.first);

            int n = (int)ids.size();
            AllPairs ap;
            ap.idOf = ids;
            for (int i = 0; i < n; ++i) ap.idxOf[ids[i]] = i;

            const double INF = std::numeric_limits<double>::infinity();
            ap.dist.assign(n, std::vector<double>(n, INF));
            ap.next.assign(n, std::vector<int>(n, -1));
            for (int i = 0; i < n; ++i) { ap.dist[i][i] = 0.0; ap.next[i][i] = i; }

            // aristas abiertas
            for (int i = 0; i < n; ++i) {
                int u = ids[i];
                forEachOpenNeighbor(g, u, [&](int v, double w) {
                    int j = ap.idxOf.at(v);
                    if (w < ap.dist[i][j]) {
                        ap.dist[i][j] = w;
                        ap.next[i][j] = j;
                    }
                    });
            }

            // floyd
            for (int k = 0; k < n; ++k) {
                for (int i = 0; i < n; ++i) {
                    if (ap.dist[i][k] == INF) continue;
                    for (int j = 0; j < n; ++j) {
                        double nd = ap.dist[i][k] + ap.dist[k][j];
                        if (nd < ap.dist[i][j]) {
                            ap.dist[i][j] = nd;
                            ap.next[i][j] = ap.next[i][k];
                        }
                    }
                }
            }
            return ap;
        }
    };

} // namespace transport
