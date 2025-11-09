#pragma once
#include <vector>
#include <unordered_map>

namespace transport {

    class DisjointSet {
        std::unordered_map<int, int> parent;
        std::unordered_map<int, int> rank;
    public:
        void makeSet(int x) { parent[x] = x; rank[x] = 0; }
        int find(int x) {
            if (parent[x] == x) return x;
            return parent[x] = find(parent[x]);
        }
        bool unite(int a, int b) {
            a = find(a); b = find(b);
            if (a == b) return false;
            if (rank[a] < rank[b]) std::swap(a, b);
            parent[b] = a;
            if (rank[a] == rank[b]) rank[a]++;
            return true;
        }
    };

} // namespace transport
