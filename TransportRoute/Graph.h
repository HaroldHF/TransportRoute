#pragma once
#include <unordered_map>
#include <vector>
#include <limits>
#include <utility>

namespace transport {

    class Graph {
    public:
        struct AdjEdge { int to; double w; bool closed; };
    private:
        // adjacency list: id -> vector of edges
        std::unordered_map<int, std::vector<AdjEdge>> adj_;

    public:
        void clear() { adj_.clear(); }

        void addVertex(int id) {
            (void)adj_[id]; // ensure key exists
        }

        void addEdge(int u, int v, double w, bool closed = false) {
            addVertex(u); addVertex(v);
            adj_[u].push_back({ v,w,closed });
            adj_[v].push_back({ u,w,closed });
        }

        bool setClosed(int u, int v, bool closed) {
            bool touched = false;
            auto itU = adj_.find(u), itV = adj_.find(v);
            if (itU != adj_.end()) {
                for (auto& e : itU->second) if (e.to == v) { e.closed = closed; touched = true; }
            }
            if (itV != adj_.end()) {
                for (auto& e : itV->second) if (e.to == u) { e.closed = closed; touched = true; }
            }
            return touched;
        }

        const std::unordered_map<int, std::vector<AdjEdge>>& data() const { return adj_; }
        bool hasVertex(int id) const { return adj_.count(id) > 0; }
        const std::vector<AdjEdge>& neighbors(int id) const {
            static const std::vector<AdjEdge> empty;
            auto it = adj_.find(id);
            return it == adj_.end() ? empty : it->second;
        }
    };

} // namespace transport