#pragma once
#include "Graph.h"
#include "Result.h"
#include "BFS.h"
#include "DFS.h"
#include "Dijkstra.h"
#include "FloydWarshall.h"
#include "Prim.h"
#include "Kruskal.h"

namespace transport {

    class AlgoFacade {
    public:
        static VisitResult runBFS(const Graph& g, int start) { return BFS::traverse(g, start); }
        static VisitResult runDFS(const Graph& g, int start) { return DFS::traverse(g, start); }
        static PathResult runDijkstra(const Graph& g, int src, int dst) { return Dijkstra::shortestPath(g, src, dst); }

        // Floyd: computar una vez y reusar (UI puede cachear)
        static FloydWarshall::AllPairs computeFloyd(const Graph& g) { return FloydWarshall::compute(g); }
        static PathResult runFloyd(const FloydWarshall::AllPairs& ap, int src, int dst) { return ap.path(src, dst); }

        static MSTResult runPrim(const Graph& g, int start) { return Prim::mst(g, start); }
        static MSTResult runKruskal(const Graph& g) { return Kruskal::mst(g); }
    };

} // namespace transport
