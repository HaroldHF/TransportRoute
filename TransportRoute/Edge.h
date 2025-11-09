#pragma once
namespace transport {

    // Edge between u and v with a weight (time/cost). "closed" marks the segment as unavailable.
    struct Edge {
        int u = -1;
        int v = -1;
        double w = 0.0;
        bool closed = false;

        Edge() = default;
        Edge(int u_, int v_, double w_, bool closed_ = false)
            : u(u_), v(v_), w(w_), closed(closed_) {
        }
    };

} // namespace transport