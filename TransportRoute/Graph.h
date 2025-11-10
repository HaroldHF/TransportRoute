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

        bool removeEdge(int u, int v) {
            auto rm = [](std::vector<AdjEdge>& vec, int to) {
                auto it = std::remove_if(vec.begin(), vec.end(), [&](const AdjEdge& e) { return e.to == to; });
                bool changed = (it != vec.end());
                vec.erase(it, vec.end());
                return changed;
                };
            auto itU = adj_.find(u), itV = adj_.find(v);
            bool a = false, b = false;
            if (itU != adj_.end()) a = rm(itU->second, v);
            if (itV != adj_.end()) b = rm(itV->second, u);
            return a || b;
        }

        bool setWeight(int u, int v, double w) {
            bool touched = false;
            auto itU = adj_.find(u), itV = adj_.find(v);
            if (itU != adj_.end()) {
                for (auto& e : itU->second) if (e.to == v) { e.w = w; touched = true; }
            }
            if (itV != adj_.end()) {
                for (auto& e : itV->second) if (e.to == u) { e.w = w; touched = true; }
            }
            return touched;
        }
    };

} // namespace transport