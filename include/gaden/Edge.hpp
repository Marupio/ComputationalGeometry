#pragma once

#include "gaden/Vector3.hpp"

namespace gaden {

class Edge {
    // directed edge u->v on horizon (kept CCW around the perspective point)
    int m_u;
    int m_v;

public:
    Edge(int u, int v):
        m_u(u),
        m_v(v)
    {}

    int u() const { return m_u; }
    int v() const { return m_v; }

    bool operator==(const Edge& o) const { return m_u==o.m_u && m_v==o.m_v; }
};

// Functor for hashing
struct EdgeHash {
    std::size_t operator()(const Edge& e) const noexcept {
        return (e.u() * 1315423911u) ^ (e.v() + 0x9e3779b97f4a7c15ull);
    }
};

} // end namespace gaden